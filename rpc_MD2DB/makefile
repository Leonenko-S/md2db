LIBS          = -L/home/sergiy/Documents/cppExercises/RabbitMQproject/AMQP/lib -lboost_system -lamqpcpp -lpthread -ldl -L/usr/include/boost -lboost_date_time -lpq   
INCPATH       = -I../../MD2DB_RPC/rpc_MD2DB -I. -I../../AMQP/include 
all: rpc_MD2DB

rpc_MD2DB: main.o csv.o database.o tasksexecutor.o
	g++ main.o csv.o database.o tasksexecutor.o -o rpc_MD2DB $(LIBS)

main.o: main.cpp
	g++ -c  $(INCPATH) main.cpp 

csv.o: csv.cpp
	g++ -c  $(INCPATH) csv.cpp 


database.o: database.cpp
	g++ -c  $(INCPATH) database.cpp 


tasksexecutor.o: tasksexecutor.cpp
	g++ -c  $(INCPATH) tasksexecutor.cpp 
	
clean:
	rm -rf *.o rpc_MD2DB