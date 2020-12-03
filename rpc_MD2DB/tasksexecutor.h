#ifndef TASKSEXECUTOR_H
#define TASKSEXECUTOR_H

#include <csv.h>
#include <database.h>

#include "boost/date_time/date_facet.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"  //include all types plus i/o

struct Bar {
  std::string Open;
  std::string High;
  std::string Low;
  std::string Close;
  Bar() = default;
  Bar(std::string open, std::string high, std::string low, std::string close)
      : Open(open), High(high), Low(low), Close(close) {}
};
// using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

using TFMap = std::map<ptime, Bar>;

class TasksExecutor {
 private:
  void FillTheTimeGaps(const TFMap &from, TFMap &to, ptime startTime,
                       ptime endTime, int stepInMinutes,
                       std::string lastClose = "");
  std::string PtimeToString(const boost::posix_time::ptime &now,
                            const std::string &formatstring);
  const boost::posix_time::ptime StringToPtime(const std::string &zeitstempel,
                                               const std::string &formatstring);

 public:
  std::map<std::string, const int> TimeframeInMinutes = {
      {"M1", 1},    {"M5", 5},    {"M15", 15},   {"M30", 30},   {"H1", 60},
      {"H12", 720}, {"D1", 1440}, {"W1", 10080}, {"MN1", 43800}  // TODO:check
                                                                 // it
                                                                 // for truth
  };

  static DBconnection dbCon;  //(tableName);

  TasksExecutor();
  void FillHistory(std::string fileName, std::string timeframe,
                   std::string tableName);
  void FillNew(std::string timeframe, std::string tableName,
               std::string datetime, Bar OHLC);

  std::vector<std::vector<std::string>> getBars(std::string tableName,
                                                std::string datetime_from,
                                                std::string datetime_to);
};

#endif  // TASKSEXECUTOR_H
