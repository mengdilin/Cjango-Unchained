#Makefile for our library

PROG = run
CC = g++
CPPFLAGS = -std=c++1z -Wall -pthread
CPPFLAGS += -I./app/
CPPFLAGS += -ferror-limit=5
DEBUG ?= 1
CJANGO_DYNLOAD ?= 1
ifeq ($(DEBUG), 1)
    CPPFLAGS += -g -DDEBUG
else
    #CPPFLAGS=-DNDEBUG
endif

ifeq ($(CJANGO_DYNLOAD), 1)
    CPPFLAGS +=  -DCJANGO_DYNLOAD -I./lib/simplefilewatcher/include/FileWatcher
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

HTTPPARSERCPP = http_parser/http_request_parser.o\
	http_parser/http_request.o \
	http_parser/http_request_body_parser.o \
	http_parser/url_encoded_form_parser.o \
	http_parser/http_request_line.o \
	http_parser/http_stream_reader.o \
	http_parser/http_response.o
TESTOBJS += $(HTTPPARSERCPP)


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
http_request_parser.o:
	$(CC) $(CPPFLAGS) -c http_parser/http_request_parser.cpp
http_request.o:
	$(CC) $(CPPFLAGS) -c http_parser/http_request.cpp
http_request_line.o:
	$(CC) $(CPPFLAGS) -c http_parser/http_request_line.cpp
http_request_body_parser.o:
	$(CC) $(CPPFLAGS) -c http_parser/http_request_body_parser.cpp
url_encoded_form_parser.o:
	$(CC) $(CPPFLAGS) -c http_parser/url_encoded_form_parser.cpp
http_stream_reader.o:
	$(CC) $(CPPFLAGS) -c http_parser/http_stream_reader.cpp
http_response.o:
	$(CC) $(CPPFLAGS) -c http_parser/http_response.cpp
clean:
	rm -f core $(PROG) $(OBJS) $(HTTPPARSERCPP)
