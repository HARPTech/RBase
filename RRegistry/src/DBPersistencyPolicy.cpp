#include <RComm/LiteComm.hpp>
#include <RRegistry/DBPersistencyPolicy.hpp>
#include <RRegistry/Detail.hpp>
#include <RRegistry/Entries.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <ctime>
#include <functional>
#include <iostream>
#include <sqlite3.h>
#include <sstream>

// Link with correct target glibc.
// https://stackoverflow.com/questions/2856438/how-can-i-link-to-a-specific-glibc-version
__asm__(".symver realpath,realpath@GLIBC_3.4.21");

struct Sqlite3StmtDeleter
{
  int operator()(sqlite3_stmt* stmt) { return sqlite3_finalize(stmt); }
};

using std::cout;
using std::endl;

namespace lrt {
namespace rregistry {

inline int
setNthParamFromLData(sqlite3_stmt* StmtPtr,
                     int n,
                     Type type,
                     const rcomm::LiteCommData& lData)
{
  switch(type) {
    case Type::Int8:
      return sqlite3_bind_int(StmtPtr, n, lData.Int8);
      break;
    case Type::Int16:
      return sqlite3_bind_int(StmtPtr, n, lData.Int16);
      break;
    case Type::Int32:
      return sqlite3_bind_int(StmtPtr, n, lData.Int32);
      break;
    case Type::Int64:
      return sqlite3_bind_int64(StmtPtr, n, lData.Int64);
      break;
    case Type::Uint8:
      return sqlite3_bind_int(StmtPtr, n, lData.Uint8);
      break;
    case Type::Uint16:
      return sqlite3_bind_int(StmtPtr, n, lData.Uint16);
      break;
    case Type::Uint32:
      return sqlite3_bind_int64(StmtPtr, n, lData.Uint32);
      break;
    case Type::Uint64:
      return sqlite3_bind_int64(StmtPtr, n, lData.Uint64);
      break;
    case Type::Float:
      return sqlite3_bind_double(StmtPtr, n, lData.Float);
      break;
    case Type::Double:
      return sqlite3_bind_double(StmtPtr, n, lData.Double);
      break;
    case Type::Bool:
      return sqlite3_bind_int(StmtPtr, n, lData.Bool);
      break;
    case Type::String:
      break;
    default:
      break;
  }
  return SQLITE_OK;
}

DBPersistencyPolicy::DBPersistencyPolicy(int trigger)
  : m_trigger(trigger)
  , m_db(nullptr, sqlite3_close)
{
  m_stmtInsertSet.resize(static_cast<size_t>(rregistry::Type::_COUNT));
  m_controlStatements.resize(2);
}
DBPersistencyPolicy::~DBPersistencyPolicy()
{
  cout << "[DBPersistencyPolicy] Stopping DBPersistencyPolicy." << endl;
  stop();
}

void
DBPersistencyPolicy::start(std::string dbFile)
{
  if(dbFile == "") {
    // New files should be named after the current datetime and reside in
    // ~/public_html for easy access.
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[100];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(
      buffer, sizeof(buffer), "RegistryDump_%Y-%m-%d_%H-%M-%S.db", timeinfo);
    std::string homedir = getenv("HOME");

    dbFile = homedir + "/public_html/" + std::string(buffer);
  }
  m_dbFile = dbFile;

  // Start the worker.
  m_stop = false;
  m_workerThread = std::thread(std::bind(&DBPersistencyPolicy::run, this));
}
void
DBPersistencyPolicy::stop()
{
  if(m_running) {
    cout << "[DBPersistencyPolicy] Trying to stop internal worker thread."
         << endl;
    {
      std::lock_guard<std::mutex> lock(m_queueMutex);
      m_stop = true;
    }
    m_queueFullCondition.notify_all();
    m_workerThread.join();
  }
}
void
DBPersistencyPolicy::enable(const std::string& dbFile)
{
  if(!m_running)
    start(dbFile);
}
void
DBPersistencyPolicy::disable()
{
  if(m_running)
    stop();
}
void
DBPersistencyPolicy::push(uint16_t clientId,
                          rregistry::Type type,
                          uint16_t property,
                          rcomm::LiteCommData data)
{
  if(m_stop)
    return;
  Record record;
  record.timeSinceStart = Clock::now() - m_startTime;
  record.clientId = clientId;
  record.registryType = type;
  record.property = property;
  record.data = data;

  std::lock_guard<std::mutex> lock(m_queueMutex);
  m_recordsQueue.push(record);

  if(++m_triggerCounter > m_trigger) {
    m_queueFullCondition.notify_one();
    m_triggerCounter = 0;
  }
}

void
DBPersistencyPolicy::run()
{
  m_running = true;
  cout << "[DBPersistencyPolicy] Worker started. Generating Database." << endl;
  {
    int rc = 0;
    sqlite3* db = nullptr;
    char* err = nullptr;

    rc = sqlite3_open(m_dbFile.c_str(), &db);
    if(rc) {
      cout << "[DBPersistencyPolicy] Could not open database file \""
           << m_dbFile << "\"! Error: " << sqlite3_errmsg(db) << endl;
      sqlite3_close(db);
      return;
    }
    m_db.reset(db);

    {
      std::string statement = "CREATE TABLE IF NOT EXISTS general (key TEXT "
                              "PRIMARY KEY, value TEXT)";

      rc = sqlite3_exec(
        db,
        statement.c_str(),
        [](void* data, int columns, char** values, char** names) { return 0; },
        nullptr,
        &err);
      if(rc) {
        cout << "[DBPersistencyPolicy] Could not create table general! Error: "
             << err << endl;
      }
    }
    for(size_t i = 0; i < static_cast<size_t>(rregistry::Type::_COUNT); ++i) {
      auto detail = rregistry::GetTypeDetail(static_cast<rregistry::Type>(i));
      std::string name = detail.name;
      std::string typeCategory = detail.sqlType;

      std::string statement = "CREATE TABLE IF NOT EXISTS sets" + name +
                              " ( clientId INTEGER, property INTEGER,  "
                              "timestamp BIGINT, value " +
                              typeCategory + ");";

      rc = sqlite3_exec(
        db,
        statement.c_str(),
        [](void* data, int columns, char** values, char** names) { return 0; },
        nullptr,
        &err);
      if(rc) {
        cout << "[DBPersistencyPolicy] Could not create table sets" << name
             << "! Error: " << err << endl;
      }
    }

    // Create prepared statements for every possible type.
    for(size_t i = 0; i < static_cast<size_t>(rregistry::Type::_COUNT); ++i) {
      std::string statement = "INSERT INTO sets";
      // Add the name of the current type for the correct table.
      statement +=
        rregistry::GetTypeDetail(static_cast<rregistry::Type>(i)).name;

      // Add the rest of the insert statement. The type is not needed because
      // that information is contained in the table.
      statement +=
        " (clientId, property, timestamp, value) VALUES (?1, ?2, ?3, "
        "?4);";

      m_stmtInsertSet[i] = prepareStatement(statement);
    }

    m_controlStatements[0] = prepareStatement("BEGIN TRANSACTION;");
    m_controlStatements[1] = prepareStatement("COMMIT;");

    if(err)
      sqlite3_free(err);
  }
  {
    // Insert general data.
    auto addProperty =
      prepareStatement("INSERT INTO general (key, value) VALUES (?1, ?2)");
    auto StmtPtr = addProperty.get();

    // Registry variables.
    beginTransaction();
    for(std::size_t type = 0; type < static_cast<std::size_t>(Type::_COUNT);
        ++type) {
      for(std::size_t property = 0;
          property < GetEntryCount(static_cast<Type>(type));
          ++property) {
        // Type::Name - ID

        std::string key = GetTypeDetail(static_cast<Type>(type)).name;
        key += "::";
        key += GetNameOfEntryClass(static_cast<Type>(type), property);
        std::string value;

        sqlite3_bind_text(
          StmtPtr, 1, key.c_str(), key.length(), SQLITE_TRANSIENT);

        value = std::to_string(property);
        sqlite3_bind_text(
          StmtPtr, 2, value.c_str(), value.length(), SQLITE_TRANSIENT);
        sqlite3_step(StmtPtr);
        sqlite3_reset(StmtPtr);
      }
    }
    {
      // TODO: Client list
    }
    commit();
  }
  while(!m_stop) {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_queueFullCondition.wait(lock);
    // After waiting for a notification, new values should be written into the
    // database.
    beginTransaction();
    while(!m_recordsQueue.empty()) {
      auto record = m_recordsQueue.front();
      m_recordsQueue.pop();

      insertRecord(record);
    }
    commit();
  }
  m_running = false;
  cout << "[DBPersistencyPolicy] Worker ended." << endl;
}
void
DBPersistencyPolicy::insertRecord(const Record& record)
{
  auto StmtPtr =
    m_stmtInsertSet[static_cast<size_t>(record.registryType)].get();

  sqlite3_bind_int(StmtPtr, 1, record.clientId);
  sqlite3_bind_int(StmtPtr, 2, record.property);
  sqlite3_bind_int64(
    StmtPtr,
    3,
    std::chrono::duration_cast<std::chrono::nanoseconds>(record.timeSinceStart)
      .count());

  setNthParamFromLData(StmtPtr, 4, record.registryType, record.data);

  sqlite3_step(StmtPtr);
  sqlite3_reset(StmtPtr);
}
void
DBPersistencyPolicy::beginTransaction()
{
  auto StmtPtr = m_controlStatements[0].get();
  sqlite3_step(StmtPtr);
  sqlite3_reset(StmtPtr);
}
void
DBPersistencyPolicy::commit()
{
  auto StmtPtr = m_controlStatements[1].get();
  sqlite3_step(StmtPtr);
  sqlite3_reset(StmtPtr);
}

DBPersistencyPolicy::Sqlite3StmtPtr
DBPersistencyPolicy::prepareStatement(const std::string& statement)
{
  sqlite3_stmt* stmt;
  sqlite3_prepare(m_db.get(), statement.c_str(), -1, &stmt, nullptr);

  Sqlite3StmtPtr ptr = Sqlite3StmtPtr(stmt);

  if(!stmt) {
    cout << "[DBPersistencyPolicy] Could not compile SQL-Statement: \""
         << statement << "\". Error: " << sqlite3_errmsg(m_db.get()) << endl;
    ptr.reset();
  }

  return ptr;
}
}
}
