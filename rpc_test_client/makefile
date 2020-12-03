LIBS          = -L/home/sergiy/Documents/cppExercises/RabbitMQproject/AMQP/lib -lboost_system -lamqpcpp -lpthread -ldl -L/usr/include/boost -lboost_date_time -lpq   
INCPATH       = -I../../MD2DB_RPC/rpc_test_client -I. -I../../AMQP/include
all: rpc_test_client

rpc_test_client: main.o 
	g++ main.o -o rpc_test_client $(LIBS)

main.o: main.cpp
	g++ -c  $(INCPATH) main.cpp 
	
clean:
	rm -rf *.o rpc_test_client