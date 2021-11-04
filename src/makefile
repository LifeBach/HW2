CC = g++
CFLAGS = -std=c++11 -Ofast
HEADERS = headers.h parser.h fm_core.h
SOURCE = main.cpp parser.cpp fm_core.cpp
OBJECTS = main.o parser.o fm_core.o
TARGET = ../bin/hw2
$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
main.o : $(SOURCE) $(HEADERS)
	$(CC) $(CFLAGS) -c main.cpp
parser.o : $(SOURCE) $(HEADERS)
	$(CC) $(CFLAGS) -c parser.cpp
fm_core.o : $(SOURCE) $(HEADERS)
	$(CC) $(CFLAGS) -c fm_core.cpp
clean :
	rm $(TARGET) $(OBJECTS)