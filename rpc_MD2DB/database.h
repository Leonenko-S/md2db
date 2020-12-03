#include <postgresql/libpq-fe.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class PGConnection {
 private:
  // std::string host = "localhost";
  // std::string port = "5432";
  // std::string name = "testdb";
  // std::string user = "postgres";
  // std::string pass = "postgres";

  std::shared_ptr<PGconn> ptr_PGconn;

 public:
  PGConnection() = delete;
  PGConnection(PGConnection &) = delete;
  PGConnection(PGConnection &&) = delete;
  PGConnection &operator=(PGConnection &) = delete;
  PGConnection &operator=(PGConnection &&) = delete;

  PGConnection(const char *conninfo) {
    // ptr_PGconn.reset(PQsetdbLogin(host.c_str(), port.c_str(), nullptr,
    // nullptr,
    //                              name.c_str(), user.c_str(), pass.c_str()),
    //                 &PQfinish);

    ptr_PGconn.reset(PQconnectdb(conninfo), &PQfinish);

    if (PQstatus(ptr_PGconn.get()) != CONNECTION_OK &&
        PQsetnonblocking(ptr_PGconn.get(), 1) != 0) {
      throw std::runtime_error(PQerrorMessage(ptr_PGconn.get()));
    }
  }

  std::shared_ptr<PGconn> connection() const { return ptr_PGconn; }
};

class DBconnection {
 private:
  PGConnection pgConn;
  std::string requestCollector;
  int requestCount = 0;

 public:
  DBconnection(std::string conninfo);
  DBconnection() = delete;
  DBconnection(DBconnection &) = delete;
  DBconnection(DBconnection &&) = delete;
  DBconnection &operator=(DBconnection &) = delete;
  DBconnection &operator=(DBconnection &&) = delete;

  bool TableExist(const std::string &table);

  std::string getMin(const std::string &table);

  std::string getMax(const std::string &table);

  std::string getLastClose(const std::string &table);
  std::vector<std::vector<std::string>> getBars(const std::string &table,
                                                const std::string &datetime_fom,
                                                const std::string &datetime_to);

  void addToDB(const std::string &table, const std::string &datetime,
               const std::string &open, const std::string &high,
               const std::string &low, const std::string &close);
};
