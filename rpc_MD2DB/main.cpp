
//#include <database.h>
#include <server.h>
#include <task.h>

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "tasksexecutor.h"

using namespace std;

std::string ExecuteRequest(std::string requestJSON) {
  rapidjson::Document doc;
  doc.SetObject();
  rapidjson::Document::AllocatorType& alloc = doc.GetAllocator();

  rapidjson::Value val;
  val.SetBool(true);

  try {
    Task T(requestJSON);
    TasksExecutor TEX;

    switch (T.type) {
      case Task::TaskType::file: {
        TEX.FillHistory(T.filename, T.timeframe, T.currency_pair);
        break;
      }
      case Task::TaskType::realtime: {
        TEX.FillNew(T.timeframe, T.currency_pair, T.timestamp,
                    Bar(T.open, T.high, T.low, T.close));
      }
      case Task::TaskType::get: {
        rapidjson::Value array_val;
        array_val.SetArray();

        string tableName = T.currency_pair;
        // tableName += "_";
        // tableName += T.timeframe;
        vector<vector<string>> resArr =
            TEX.getBars(tableName, T.timestamp_from, T.timestamp_to);

        for (vector<string>& s : resArr) {
          rapidjson::Value currentObj_val;
          currentObj_val.SetObject();
          rapidjson::Value currentString_val;

          currentString_val.SetString(s[0].data(), s[0].size(), alloc);
          currentObj_val.AddMember("Timestamp", currentString_val, alloc);
          currentString_val.SetString(s[1].data(), s[1].size(), alloc);
          currentObj_val.AddMember("Open", currentString_val, alloc);
          currentString_val.SetString(s[2].data(), s[2].size(), alloc);
          currentObj_val.AddMember("High", currentString_val, alloc);
          currentString_val.SetString(s[3].data(), s[3].size(), alloc);
          currentObj_val.AddMember("Low", currentString_val, alloc);
          currentString_val.SetString(s[4].data(), s[4].size(), alloc);
          currentObj_val.AddMember("Close", currentString_val, alloc);

          array_val.PushBack(currentObj_val, alloc);
        }

        doc.AddMember("result_array", array_val, alloc);
      }
    }
  } catch (...) {
    val.SetBool(false);
  }
  doc.AddMember("succes", val, alloc);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return buffer.GetString();
}

DBconnection TasksExecutor::dbCon(
    "hostaddr =127.0.0.1 port=5432 dbname=testdb user=postgres "
    "password=postgres");

int main() {
  Server server("amqp://guest:guest@localhost/", ExecuteRequest);
  server.run();
  return 0;
}
