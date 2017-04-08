#Makefile for app library

# PROG = run
CC = g++
CPPFLAGS = -std=c++1z -Wall -DDEBUG -DDYNLOAD_CJANGO -Wno-return-type-c-linkage
# CPPFLAGS = -std=c++1z -Wall
OBJS = mycallback.o mycallback2.o mycallback2.so mycallback.so

# $(PROG) : $(OBJS)
# 	$(CC) $(CPPFLAGS) -o $(PROG) $(OBJS)
all: $(OBJS)
mycallback.o : mycallback.cpp
	$(CC) $(CPPFLAGS) -fPIC   -o $@ -c $<
mycallback2.o : mycallback2.cpp
	$(CC) $(CPPFLAGS) -fPIC   -o $@ -c $<
mycallback.so : mycallback.o
	$(CC) $(CPPFLAGS) -shared -o $@ mycallback.o
mycallback2.so : mycallback2.o
	$(CC) $(CPPFLAGS) -shared -o $@ mycallback2.o
	# pic (position-independent code)
clean:
	rm -f $(OBJS)
