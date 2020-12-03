#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>

#include <boost/asio.hpp>

std::string now() {
  using namespace std;
  using namespace chrono;

  const auto now = system_clock::now();
  const auto nowAsTimeT = system_clock::to_time_t(now);
  const auto nowMs = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

  stringstream nowSs;
  nowSs << put_time(localtime(&nowAsTimeT), "%d %b %Y %T") << '.'
        << setfill('0') << setw(3) << nowMs.count();

  return nowSs.str();
}
class Logger {
  const std::string EXCHANGE = "MD2DB_Tasks_EX";
  boost::asio::io_service service;
  AMQP::LibBoostAsioHandler handler;

  AMQP::TcpConnection connection;
  mutable AMQP::TcpChannel channel;

  AMQP::MessageCallback receiveCallBack;
  AMQP::QueueCallback callback;
  std::ofstream logfile;

 public:
  Logger(std::string rabbitMQconnectionString,
         std::string filename = "tasks_log.txt")
      : handler(service),
        connection(&handler, AMQP::Address(rabbitMQconnectionString)),
        channel(&connection),
        logfile(filename, ios::app)

  {
    if (!logfile.is_open()) throw std::runtime_error("Can't open log file");

    channel.setQos(1);
    receiveCallBack = [&](const AMQP::Message &message, uint64_t deliveryTag,
                          bool) {
      const std::string body(message.body(), message.bodySize());
      logfile << now() << ":" << body << std::endl;
      channel.ack(deliveryTag);
    };

    callback = [&](const std::string &name, int, int) {
      channel.bindQueue(EXCHANGE, name, "Task");
      channel.bindQueue(EXCHANGE, name, "Result");
      channel.consume(name).onReceived(receiveCallBack);
    };

    channel.declareExchange(EXCHANGE, AMQP::direct);
    channel.declareQueue(AMQP::exclusive).onSuccess(callback);
    // queue for incoming logs;
  }

  ~Logger() { logfile.close(); }
  int run() { return service.run(); }
};

int main() {
  Logger logger("amqp://guest:guest@localhost/");
  logger.run();
}
