#Makefile for our library

PROG = run
CC = g++
CPPFLAGS = -std=c++1z -Wall -DDEBUG -pthread
# CPPFLAGS = -std=c++1z -Wall
OBJS = app/app.o app/main.o routing/router.o http_parser/http_request_parser.o
TESTOBJS = app/app.o test/testrun.cpp routing/router.o http_parser/http_request_parser.o
HTTPPARSEROBJS =

testrun : $(TESTOBJS)
	$(CC) $(CPPFLAGS) -o testrun $(TESTOBJS)
$(PROG) : $(OBJS)
	$(CC) $(CPPFLAGS) -o $(PROG) $(OBJS)
main.o :
	$(CC) $(CPPFLAGS) -c app/main.cpp
app.o :
	$(CC) $(CPPFLAGS) -c app/app.cpp
router.o :
	$(CC) $(CPPFLAGS) -c routing/router.cpp
http_request_parser.o:
	$(CC) $(CPPFLAGS) -c http_parser/http_request_parser.cpp
http_request.o:
	$(CC) $(CPPFLAGS) -c http_parser/http_request.cpp

clean:
	rm -f core $(PROG) $(OBJS)
