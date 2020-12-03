#include "tasksexecutor.h"

#include <fstream>
//#include  <string>
TasksExecutor::TasksExecutor() {}
using namespace std;

const boost::posix_time::ptime TasksExecutor::StringToPtime(
    const string &zeitstempel, const string &formatstring) {
  static stringstream ss;
  static boost::posix_time::time_input_facet *input_facet = NULL;
  if (!input_facet) {
    input_facet = new boost::posix_time::time_input_facet(1);
    ss.imbue(locale(locale(), input_facet));
  }

  input_facet->format(formatstring.c_str());

  ss.str(zeitstempel);
  boost::posix_time::ptime timestamp;

  ss >> timestamp;
  ss.clear();
  return timestamp;
}
std::string TasksExecutor::PtimeToString(const boost::posix_time::ptime &now,
                                         const string &formatstring) {
  static std::locale loc(std::cout.getloc(), new boost::posix_time::time_facet(
                                                 formatstring.c_str()));

  std::stringstream ss;
  ss.imbue(loc);
  ss << now;
  return ss.str();
}

void TasksExecutor::FillTheTimeGaps(const TFMap &from, TFMap &to,
                                    ptime startTime, ptime endTime,
                                    int stepInMinutes, string lastClose) {
  time_iterator titr(startTime, minutes(stepInMinutes));
  while (titr <= endTime) {
    if (from.count(*titr) == 0) {
      Bar C(lastClose, lastClose, lastClose, lastClose);
      to[*titr] = C;
    } else {
      to[*titr] = from.at(*titr);
      lastClose = from.at(*titr).Close;
    }
    ++titr;
  }
}

void TasksExecutor::FillHistory(string fileName, string timeframe,
                                string tableName) {
  if (!dbCon.TableExist(tableName)) {
    cout << "Something wrong with database or table" << endl;
    return;  // -1;
  }

  ptime dbMIN;  // not_a_date_time by defaul
  ptime dbMAX;
  ptime fileMIN;
  ptime fileMAX;

  try {
    dbMIN = time_from_string(dbCon.getMin(tableName));
    dbMAX = time_from_string(dbCon.getMax(tableName));
  } catch (...) {
    // table exist,but empty
    dbMIN = not_a_date_time;
    dbMAX = not_a_date_time;
  }

  TFMap from_file_less;
  TFMap already_in_DB;
  TFMap from_file_more;

  std::ifstream file(fileName);

  for (auto &row : CSVRange(file)) {
    ptime currentTime = StringToPtime(row[0], "%d.%m.%Y %H:%M:%S");

    if (fileMIN.is_not_a_date_time()) {
      fileMIN = currentTime;
      if (dbMIN.is_not_a_date_time()) dbMIN = fileMIN;
      if (dbMAX.is_not_a_date_time()) dbMAX = fileMIN;
    }

    Bar Tmp(row[1], row[2], row[3], row[4]);

    if (currentTime < dbMIN)
      from_file_less[currentTime] = Tmp;
    else if (currentTime >= dbMAX)
      from_file_more[currentTime] = Tmp;
    else
      already_in_DB[currentTime] = Tmp;
    // if(currentTime>fileMax)
    fileMAX = currentTime;
  }
  FillTheTimeGaps(from_file_less, already_in_DB, fileMIN, dbMIN,
                  TimeframeInMinutes[timeframe]);
  FillTheTimeGaps(from_file_more, already_in_DB, dbMAX, fileMAX,
                  TimeframeInMinutes[timeframe], dbCon.getLastClose(tableName));

  for (auto i : already_in_DB) {
    dbCon.addToDB(tableName, PtimeToString(i.first, "%Y-%m-%d %H:%M:%S"),
                  i.second.Open, i.second.High, i.second.Low, i.second.Close);
  }
}

void TasksExecutor::FillNew(string timeframe, string tableName, string datetime,
                            Bar OHLC) {
  if (!dbCon.TableExist(tableName)) {
    cout << "Something wrong with database or table" << endl;
    return;  // -1;
  }

  ptime dbMAX;
  ptime currentTime = StringToPtime(datetime, "%d.%m.%Y %H:%M:%S");

  try {
    dbMAX = time_from_string(dbCon.getMax(tableName));
  } catch (...) {
    // table exist,but empty
    dbMAX = not_a_date_time;
  }

  TFMap already_in_DB;
  TFMap from_file_more;

  if (currentTime >= dbMAX)
    from_file_more[currentTime] = OHLC;
  else
    already_in_DB[currentTime] = OHLC;

  FillTheTimeGaps(from_file_more, already_in_DB, dbMAX, currentTime,
                  TimeframeInMinutes[timeframe], dbCon.getLastClose(tableName));

  for (auto i : already_in_DB) {
    dbCon.addToDB(tableName, PtimeToString(i.first, "%Y-%m-%d %H:%M:%S"),
                  i.second.Open, i.second.High, i.second.Low, i.second.Close);
  }
}

vector<vector<string>> TasksExecutor::getBars(std::string tableName,
                                              std::string datetime_from,
                                              std::string datetime_to) {
  ptime from = StringToPtime(datetime_from, "%d.%m.%Y %H:%M:%S");
  ptime to = StringToPtime(datetime_to, "%d.%m.%Y %H:%M:%S");
  if (from > to) swap(from, to);  // maybe this swap couldn't work
  return dbCon.getBars(tableName, PtimeToString(from, "%Y-%m-%d %H:%M:%S"),
                       PtimeToString(to, "%Y-%m-%d %H:%M:%S"));
}
