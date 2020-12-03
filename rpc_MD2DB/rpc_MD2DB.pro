TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH  +=/home/sergiy/Documents/cppExercises/RabbitMQproject/AMQP/include
LIBS += \
  -L/home/sergiy/Documents/cppExercises/RabbitMQproject/AMQP/lib \
  -lboost_system -lamqpcpp -lpthread -ldl \
  -L/usr/include/boost -lboost_date_time  \
  -lpq

SOURCES += \
        csv.cpp \
        database.cpp \
        main.cpp \
        tasksexecutor.cpp

HEADERS += \
  csv.h \
  database.h \
  server.h \
  task.h \
  tasksexecutor.h
