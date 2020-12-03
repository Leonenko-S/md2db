LIBS          = -L/home/sergiy/Documents/cppExercises/RabbitMQproject/AMQP/lib -lboost_system -lamqpcpp -lpthread -ldl -L/usr/include/boost -lboost_date_time -lpq   
INCPATH       = -I../../MD2DB_RPC/logger -I. -I../../AMQP/include 
all: logger

logger: main.o 
	g++ main.o -o logger $(LIBS)

main.o: main.cpp
	g++ -c  $(INCPATH) main.cpp 
	
clean:
	rm -rf *.o logger