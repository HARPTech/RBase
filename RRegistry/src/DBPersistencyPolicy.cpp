#include <RComm/LiteComm.hpp>
#include <RRegistry/DBPersistencyPolicy.hpp>
#include <RRegistry/Detail.hpp>
#include <RRegistry/Entries.hpp>
#include <RRegistry/TypeConverter.hpp>
#include <functional>
#include <iostream>
#include <sqlite3.h>

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
  stop();
}

void
DBPersistencyPolicy::start(const std::string& dbFile)
{
  int rc = 0;
  sqlite3* db = nullptr;
  char* err = nullptr;

  rc = sqlite3_open(dbFile.c_str(), &db);
  if(rc) {
    cout << "[DBConsistencyPolicy] Could not open database file \"" << dbFile
         << "\"! Error: " << sqlite3_errmsg(db) << endl;
    sqlite3_close(db);
    return;
  }
  m_db.reset(db);

  {
    std::string statement =
      "CREATE TABLE IF NOT EXISTS general (key VARCHAR(20) "
      "PRIMARY KEY, value TEXT)";

    rc = sqlite3_exec(
      db,
      statement.c_str(),
      [](void* data, int columns, char** values, char** names) { return 0; },
      nullptr,
      &err);
    if(rc) {
      cout << "[DBConsistencyPolicy] Could not create table general! Error: "
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
      cout << "[DBConsistencyPolicy] Could not create table sets" << name
           << "! Error: " << err << endl;
    }
  }

  // Create prepared statements for every possible type.
  for(size_t i = 0; i < static_cast<size_t>(rregistry::Type::_COUNT); ++i) {
    std::string statement = "INSERT INTO sets";
    // Add the name of the current type for the correct table.
    statement += rregistry::GetTypeDetail(static_cast<rregistry::Type>(i)).name;

    // Add the rest of the insert statement. The type is not needed because
    // that information is contained in the table.
    statement += " (clientId, property, timestamp, value) VALUES (?1, ?2, ?3, "
                 "?4);";

    m_stmtInsertSet[i] = prepareStatement(statement);
  }

  m_controlStatements[0] = prepareStatement("BEGIN TRANSACTION;");
  m_controlStatements[1] = prepareStatement("COMMIT;");

  if(err)
    sqlite3_free(err);

  // Start the worker.
  m_workerThread = std::thread(std::bind(&DBPersistencyPolicy::run, this));
}
void
DBPersistencyPolicy::stop()
{
  std::unique_lock<std::mutex> lock(m_queueMutex);
  m_stop = true;
  m_queueFullCondition.notify_one();
  m_queueFullCondition.wait(lock, [this]() { return m_running == false; });
}
void
DBPersistencyPolicy::enable(const std::string &dbFile)
{
}
void
DBPersistencyPolicy::disable()
{
}
void
DBPersistencyPolicy::push(uint16_t clientId,
                          rregistry::Type type,
                          uint16_t property,
                          rcomm::LiteCommData data)
{
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
  cout << "[DBConsistencyPolicy] Worker started." << endl;
  m_running = true;
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
  cout << "[DBConsistencyPolicy] Worker ended." << endl;
  m_running = false;
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
    cout << "[DBConsistencyPolicy] Could not compile SQL-Statement: \""
         << statement << "\". Error: " << sqlite3_errmsg(m_db.get()) << endl;
    ptr.reset();
  }

  return ptr;
}
}
}
