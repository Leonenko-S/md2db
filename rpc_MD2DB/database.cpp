#include <database.h>

DBconnection::DBconnection(std::string conninfo) : pgConn(conninfo.c_str()) {
  // TODO: remove default constructors from this and PGConnection classes
}

bool DBconnection::TableExist(const std::string& table) {
  std::string request = "SELECT COUNT (datetime) FROM " + table + ";";

  auto res = PQexec(pgConn.connection().get(), request.c_str());
  auto status = PQresultStatus(res);

  if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
    throw std::runtime_error(
        "something wrong with query? database or table in it");
  return true;
}
std::string DBconnection::getMin(const std::string& table) {
  std::string request = "SELECT MIN (datetime) FROM " + table + " limit 1;";

  auto res = PQexec(pgConn.connection().get(), request.c_str());
  auto status = PQresultStatus(res);

  if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
    throw std::runtime_error(
        "something wrong with query? database or table in it");
  return PQgetvalue(res, 0, 0);
}

std::string DBconnection::getMax(const std::string& table) {
  std::string request = "SELECT MAX (datetime) FROM " + table + " Limit 1;";

  auto res = PQexec(pgConn.connection().get(), request.c_str());
  auto status = PQresultStatus(res);

  if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
    throw std::runtime_error(
        "something wrong with query? database or table in it");
  return PQgetvalue(res, 0, 0);
}

std::string DBconnection::getLastClose(const std::string& table) {
  std::string lastDateTime = getMax(table);
  std::string request = "SELECT close FROM " + table + " WHERE datetime='" +
                        lastDateTime + "' Limit 1;";

  auto res = PQexec(pgConn.connection().get(), request.c_str());
  auto status = PQresultStatus(res);

  if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
    throw std::runtime_error(
        "something wrong with query? database or table in it");

  return PQgetvalue(res, 0, 0);
}

std::vector<std::vector<std::string>> DBconnection::getBars(
    const std::string& table, const std::string& datetime_from,
    const std::string& datetime_to) {
  // WHERE datetime BETWEEN '2007-02-07' AND '2007-02-15';
  std::string request = "SELECT * FROM " + table + " WHERE datetime>'" +
                        datetime_from + "' AND datetime<='" + datetime_to +
                        "' ;";

  auto res = PQexec(pgConn.connection().get(), request.c_str());
  auto status = PQresultStatus(res);

  if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
    throw std::runtime_error(
        "something wrong with query, database or table in it");
  ;

  std::vector<std::vector<std::string>> result(atoi(PQcmdTuples(res)),
                                               std::vector<std::string>(5, ""));
  for (int i = 0; i < atoi(PQcmdTuples(res)); i++) {
    result[i][0] = PQgetvalue(res, i, 0);
    result[i][1] += PQgetvalue(res, i, 1);
    result[i][2] += PQgetvalue(res, i, 2);
    result[i][3] = PQgetvalue(res, i, 3);
    result[i][4] = PQgetvalue(res, i, 4);
  }
  return result;
}

void DBconnection::addToDB(const std::string& table,
                           const std::string& datetime, const std::string& open,
                           const std::string& high, const std::string& low,
                           const std::string& close) {
  std::string request = "INSERT INTO " + table + " VALUES ('" + datetime +
                        "','" + open + "','" + high + "','" + low + "','" +
                        close + "') ON CONFLICT (datetime) DO UPDATE SET ";
  request += "open='" + open + "', high='" + high + "', low='" + low +
             "', close='" + close + "';";
  if (requestCount < 1000) {
    requestCollector += request;
    requestCount++;
  } else {
    PQsendQuery(pgConn.connection().get(), requestCollector.c_str());
    //  auto res = PQgetResult(pgConn.connection().get());
    //  std::cout << PQresStatus(PQresultStatus(res)) << std::endl << std::endl;
    while (PQgetResult(pgConn.connection().get()) != nullptr) {
    }
    requestCollector.clear();
    requestCount = 0;
  }
}
