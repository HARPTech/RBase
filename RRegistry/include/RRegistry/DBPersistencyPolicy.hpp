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

struct sqlite3;
struct sqlite3_stmt;
struct Sqlite3StmtDeleter;

namespace lrt {
namespace rcomm {
struct LiteCommMessage;
}
namespace rregistry {
class DBPersistencyPolicy : public PersistencyPolicy
{
  public:
  DBPersistencyPolicy(int trigger = 50);
  virtual ~DBPersistencyPolicy();

  void start(const std::string& dbFile = "./registry.db");
  void stop();

  virtual void push(uint16_t clientId,
                    rregistry::Type type,
                    uint16_t property,
                    rcomm::LiteCommData data) override;

  virtual void enable(const std::string &dbFile) override;
  virtual void disable() override;

  private:
  using Clock = std::chrono::high_resolution_clock;
  using Sqlite3Ptr = std::unique_ptr<::sqlite3, int (*)(::sqlite3*)>;
  using Sqlite3StmtPtr = std::unique_ptr<::sqlite3_stmt, ::Sqlite3StmtDeleter>;
  struct Record
  {
    Clock::duration timeSinceStart;
    uint16_t clientId;
    rregistry::Type registryType;
    uint16_t property;
    rcomm::LiteCommData data;
  };

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
  bool m_stop = false;

  int m_trigger;
  int m_triggerCounter = 0;

  Sqlite3Ptr m_db;

  std::vector<Sqlite3StmtPtr> m_stmtInsertSet;
  std::vector<Sqlite3StmtPtr> m_controlStatements;

  std::thread m_workerThread;
};
}
}

#endif
