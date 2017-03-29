#Makefile for our library

PROG = run
CC = g++
CPPFLAGS = -std=c++1z -Wall -DDEBUG
# CPPFLAGS = -std=c++1z -Wall
OBJS = app/app.o app/main.o routing/router.o
TESTOBJS = app/app.o test/testrun.cpp routing/router.o

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
clean:
	rm -f core $(PROG) $(OBJS)
