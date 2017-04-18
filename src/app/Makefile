#Makefile for app library

PROG = run
CC = g++
CPPFLAGS = -std=c++1z -Wall -DDEBUG
# CPPFLAGS = -std=c++1z -Wall
OBJS = app.o main.o

$(PROG) : $(OBJS)
	$(CC) $(CPPFLAGS) -o $(PROG) $(OBJS)
main.o :
	$(CC) $(CPPFLAGS) -c main.cpp
app.o :
	$(CC) $(CPPFLAGS) -c app.cpp
clean:
	rm -f core $(PROG) $(OBJS)
