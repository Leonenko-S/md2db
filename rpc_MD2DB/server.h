#include <amqpcpp.h>
#include <amqpcpp/libboostasio.h>

#include <boost/asio.hpp>

class Server {
  const std::string EXCHANGE = "MD2DB_Tasks_EX";
  boost::asio::io_service service;
  AMQP::LibBoostAsioHandler handler;

  AMQP::TcpConnection connection;
  mutable AMQP::TcpChannel channel;

  AMQP::MessageCallback receiveCallBack;
  AMQP::QueueCallback callback;

 public:
  Server(std::string rabbitMQconnectionString,
         std::string (*requestExecution)(std::string) = nullptr)
      : handler(service),
        connection(&handler, AMQP::Address(rabbitMQconnectionString)),
        channel(&connection) {
    channel.setQos(1);

    receiveCallBack = [&, requestExecution](const AMQP::Message &message,
                                            uint64_t deliveryTag, bool) {
      const std::string body(message.body(), message.bodySize());
      // incoming  JSON HERE;
      std::string result =
          (nullptr == requestExecution) ? "" : requestExecution(body);
      AMQP::Envelope env(result.c_str(), result.size());
      env.setCorrelationID(message.correlationID());

      channel.publish(EXCHANGE, message.replyTo(), env);
      channel.ack(deliveryTag);
    };

    callback = [&](const std::string &name, int, int) {
      channel.bindQueue(EXCHANGE, name, "Task");
      channel.consume(name).onReceived(receiveCallBack);
    };

    channel.declareExchange(EXCHANGE, AMQP::direct);
    channel.declareQueue(AMQP::exclusive).onSuccess(callback);
    // queue for incoming tasks
  }

  int run() { return service.run(); }
};
