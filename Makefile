#Makefile for our library

PROG = run
CC = g++
CPPFLAGS = -std=c++1z -Wall -pthread -DDYNLOAD_CJANGO
CPPFLAGS += -I./lib/simplefilewatcher/include/FileWatcher
# FIXME: user only need to include <cjango.hpp>
CPPFLAGS += -I./app/
CPPFLAGS += -ferror-limit=5
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CPPFLAGS += -g -DDEBUG
else
    #CPPFLAGS=-DNDEBUG
endif
# CPPFLAGS = -std=c++1z -Wall
SFCDIR=lib/simplefilewatcher/build/intermediate/Debug
OBJS = app/app.o app/main.o routing/router.o
TESTOBJS = app/app.o routing/router.o \
  $(SFCDIR)/FileWatcher.o \
  $(SFCDIR)/FileWatcherOSX.o \
  $(SFCDIR)/FileWatcherLinux.o

HTTPPARSEROBJ = http_parser/http_request_parser.o\
	http_parser/http_request.o \
	http_parser/http_request_body_parser.o \
	http_parser/url_encoded_form_parser.o \
	http_parser/http_request_line.o \
	http_parser/http_stream_reader.o \
	http_parser/http_response.o \
	app/libhttp_response.so \
	app/libhttp_request.so

TESTOBJS += $(HTTPPARSEROBJ)
OBJS += $(HTTPPARSEROBJ)
testrun : $(TESTOBJS) test/testrun.cpp
	$(CC) $(CPPFLAGS) -o testrun $(TESTOBJS) test/testrun.cpp
$(PROG) : $(OBJS)
	$(CC) $(CPPFLAGS) -o $(PROG) $(OBJS)
main.o :
	$(CC) $(CPPFLAGS) -c app/main.cpp
app.o :
	$(CC) $(CPPFLAGS) -c app/app.cpp
router.o : routing/router.cpp routing/router.hpp
	$(CC) $(CPPFLAGS) -c routing/router.cpp

app/libhttp_response.so: http_parser/http_response.o
	$(CC) $(CPPFLAGS) -shared -o app/libhttp_response.so http_parser/http_response.o
app/libhttp_request.so: http_parser/http_request.o
	$(CC) $(CPPFLAGS) -shared -o app/libhttp_request.so http_parser/http_request.o

clean:
	rm -f core $(PROG) $(OBJS) $(HTTPPARSERCPP)
