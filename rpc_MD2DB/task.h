#include <map>

#include "rapidjson/document.h"

struct Task {
  enum class TaskType { file, realtime, get };
  std::map<std::string, const TaskType> strTsk = {
      {"file", TaskType::file},
      {"realtime", TaskType::realtime},
      {"get", TaskType::get}};

  TaskType type;
  std::string filename;                     // f
  std::string currency_pair;                // frg
  std::string timeframe;                    // frg
  std::string timestamp;                    // r
  std::string& timestamp_from = timestamp;  // g
  std::string timestamp_to;                 // g

  std::string open;   // r
  std::string high;   // r
  std::string low;    // r
  std::string close;  // r

  Task() = delete;
  Task(std::string jsonStr) {
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    if (!doc.IsObject()) throw std::runtime_error("json is not an object");
    if (!doc.HasMember("type"))
      throw std::runtime_error("missing fields in json");

    type = strTsk[doc["type"].GetString()];
    switch (type) {
      case TaskType::file: {
        static const char* members[] = {"filename ", "currency_pair",
                                        "timeframe"};
        for (auto i = 0u; i < sizeof(members) / sizeof(members[0]); i++)
          if (!doc.HasMember(members[i]))
            throw std::runtime_error("missing fields in json");

        filename = doc["filename"].GetString();
        currency_pair = doc["currency_pair"].GetString();
        timeframe = doc["timeframe"].GetString();

        break;
      }
      case TaskType::realtime: {
        static const char* members[] = {
            "currency_pair", "timeframe", "timestamp", "open",
            "high",          "low",       "close"};
        for (auto i = 0u; i < sizeof(members) / sizeof(members[0]); i++)
          if (!doc.HasMember(members[i]))
            throw std::runtime_error("missing fields in json");

        currency_pair = doc["currency_pair"].GetString();
        timeframe = doc["timeframe"].GetString();
        timestamp = doc["timestamp"].GetString();
        open = doc["open"].GetString();
        high = doc["high"].GetString();
        low = doc["low"].GetString();
        close = doc["close"].GetString();
        break;
      }
      case TaskType::get: {
        static const char* members[] = {"currency_pair", "timeframe",
                                        "timestamp_from", "timestamp_to"};
        for (auto i = 0u; i < sizeof(members) / sizeof(members[0]); i++)
          if (!doc.HasMember(members[i]))
            throw std::runtime_error("missing fields in json");
        currency_pair = doc["currency_pair"].GetString();
        timeframe = doc["timeframe"].GetString();
        timestamp_from = doc["timestamp_from"].GetString();
        timestamp_to = doc["timestamp_to"].GetString();
        break;
      }
        // default: throw std::runtime_error("wrong type of task");
    }
  }
};
