#Makefile for app library

# PROG = run
CC = g++
CPPFLAGS = -std=c++1z -Wall -DDEBUG -DCJANGO_DYNLOAD -Wno-return-type-c-linkage
CPPFLAGS += -I./../app/
CPPFLAGS += -L./../app/
CPPFLAGS += -lhttp_response -lhttp_request
CPPFLAGS += -ferror-limit=5
# CPPFLAGS = -std=c++1z -Wall
OBJS = mycallback.o mycallback2.o mycallback2.so mycallback.so demo.o demo.so

# $(PROG) : $(OBJS)
# 	$(CC) $(CPPFLAGS) -o $(PROG) $(OBJS)
all: $(OBJS)
mycallback.o : mycallback.cpp
	$(CC) $(CPPFLAGS) -fPIC   -o $@ -c $<
mycallback2.o : mycallback2.cpp
	$(CC) $(CPPFLAGS) -fPIC   -o $@ -c $<
demo.o : demo.cpp
	$(CC) $(CPPFLAGS) -fPIC   -o $@ -c $<
demo.so : demo.o
	$(CC) $(CPPFLAGS) -shared -o $@ demo.o
mycallback.so : mycallback.o
	$(CC) $(CPPFLAGS) -shared -o $@ mycallback.o
mycallback2.so : mycallback2.o
	$(CC) $(CPPFLAGS) -shared -o $@ mycallback2.o
	# pic (position-independent code)
clean:
	rm -f $(OBJS)
