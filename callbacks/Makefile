#Makefile for app library

# PROG = run
CC = g++
CPPFLAGS = -std=c++1z -Wall -DDEBUG -DDYNLOAD_CJANGO
# CPPFLAGS = -std=c++1z -Wall
OBJS = mycallback.o mycallback.so

# $(PROG) : $(OBJS)
# 	$(CC) $(CPPFLAGS) -o $(PROG) $(OBJS)
all: $(OBJS)
mycallback.o : mycallback.cpp
	$(CC) $(CPPFLAGS) -fpic -c mycallback.cpp
mycallback.so : mycallback.o
	$(CC) $(CPPFLAGS) -shared -o mycallback.so mycallback.o
	# pic (position-independent code)
clean:
	rm -f $(OBJS)
