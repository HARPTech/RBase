#ifndef LRT_RREGISTRY_DBCONSISTENCYPOLICY_HPP
#define LRT_RREGISTRY_DBCONSISTENCYPOLICY_HPP

#include "PersistencyPolicy.hpp"
#include <RComm/LiteComm.hpp>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "Entries.hpp"
#include "Registry.hpp"
#include "TypeConverter.hpp"

struct sqlite3;
struct sqlite3_stmt;
struct Sqlite3StmtDeleter;

namespace lrt {
namespace rcomm {
struct LiteCommMessage;
}
namespace rregistry {

/**
 * @brief Implements persistent saving of registry data into a SQLite database.
 *
 * Per default, all database files are captured in ~/public_html with the
 * current time as filename. Capturing can be activated by enable() and
 * disable() and is integrated into the LiteCommAdapter to be activated when
 * Bool::PERS_ENABLE is activated.
 *
 * The database can later be queried with commands like this (in SQL):
 *  SELECT general.key,setsInt16.timestamp,setsInt16.value FROM setsInt16
 *      JOIN general ON general.value = setsInt16.property
 *    WHERE substr(general.key, 0, 6) = "Int16";
 */
class DBPersistencyPolicy : public PersistencyPolicy
{
  public:
  DBPersistencyPolicy(std::shared_ptr<Registry> registry, int trigger = 50);
  virtual ~DBPersistencyPolicy();

  void start(std::string dbFile = "");
  void stop();

  virtual void push(uint16_t clientId,
                    rregistry::Type type,
                    uint16_t property) override;
  virtual void enable(const std::string& dbFile = "") override;
  virtual void disable() override;

#define LRT_RREGISTRY_DBPERSISTENCYPOLICY_RECORD_UNION(CLASS) \
  LRT_RREGISTRY_TYPE_CONVERTER_NATIVE(CLASS) data_##CLASS;
#define LRT_RREGISTRY_DBPERSISTENCYPOLICY_RECORD_SWITCH(CLASS) \
  case Type::CLASS:                                            \
    data_##CLASS = record.data_##CLASS;                        \
    break;

  using Clock = std::chrono::high_resolution_clock;
  struct Record
  {
    Record()
    {
      clientId = 0;
      registryType = rregistry::Type::Int8;
      property = 0;
      data_Uint64 = 0;
    }
    Record(const Record& record)
    {
      timeSinceStart = record.timeSinceStart;
      clientId = record.clientId;
      property = record.property;
      registryType = record.registryType;
      switch(registryType) {
        LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
          LRT_RREGISTRY_DBPERSISTENCYPOLICY_RECORD_SWITCH)
        default:
          break;
      }
    }
    ~Record() {}

    Clock::duration timeSinceStart;
    uint16_t clientId;
    rregistry::Type registryType;
    uint16_t property;
    union
    {
      LRT_RREGISTRY_CPPTYPELIST_HELPER_INCLUDE_STRING(
        LRT_RREGISTRY_DBPERSISTENCYPOLICY_RECORD_UNION)
    };
  };

  private:
  using Sqlite3Ptr = std::unique_ptr<::sqlite3, int (*)(::sqlite3*)>;
  using Sqlite3StmtPtr = std::unique_ptr<::sqlite3_stmt, ::Sqlite3StmtDeleter>;

  void run();
  void insertRecord(const Record& record);
  Sqlite3StmtPtr prepareStatement(const std::string& statement);

  void beginTransaction();
  void commit();

  std::condition_variable m_queueFullCondition;
  std::mutex m_queueMutex;

  Clock::time_point m_startTime = Clock::now();

  std::queue<Record> m_recordsQueue;
  bool m_running = false;
  bool m_stop = true;

  int m_trigger;
  int m_triggerCounter = 0;

  Sqlite3Ptr m_db;

  std::vector<Sqlite3StmtPtr> m_stmtInsertSet;
  std::vector<Sqlite3StmtPtr> m_controlStatements;
  std::shared_ptr<Registry> m_registry;

  std::thread m_workerThread;

  std::string m_dbFile;
};
}
}

#endif
