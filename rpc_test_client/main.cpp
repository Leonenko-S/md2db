#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>

#include <boost/asio.hpp>
#include <iostream>

int main() {
  const std::string correlation = std::to_string(random());

  boost::asio::io_service service;
  AMQP::LibBoostAsioHandler handler(service);

  AMQP::TcpConnection connection(
      &handler, AMQP::Address("amqp://guest:guest@localhost/"));
  AMQP::TcpChannel channel(&connection);

  auto receiveCallback = [&](const AMQP::Message &msgReceive, uint64_t, bool) {
    if (msgReceive.correlationID() != correlation) return;

    std::cout << " Answer:"
              << std::string(msgReceive.body(), msgReceive.bodySize())
              << std::endl;
    // service.stop();
  };

  AMQP::QueueCallback sendTaskCallback = [&](const std::string &name, int,
                                             int) {
    channel.bindQueue("MD2DB_Tasks_EX", name, "Result");
    std::string msgSend =
        "{\"type\": "
        "\"get\",\"currency_pair\":\"TMP_CUR\",\"timeframe\":\"M1\","
        "\"timestamp_from\":\"10.10.2018 "
        "10:10:00\",\"timestamp_to\":\"10.10.2018 10:20:30\"}";
    // TODO: msg to JSON converting here, or just hardcode JSON

    AMQP::Envelope env(msgSend.c_str(), msgSend.size());
    env.setCorrelationID(correlation);
    env.setReplyTo("Result");
    channel.publish("MD2DB_Tasks_EX", "Task", env);
    std::cout << " Request:" << msgSend << std::endl;
    channel.consume(name, AMQP::noack).onReceived(receiveCallback);  // Result
  };

  channel.declareExchange("MD2DB_Tasks_EX", AMQP::direct);
  channel.declareQueue(AMQP::exclusive).onSuccess(sendTaskCallback);

  service.run();
  return 0;
}
