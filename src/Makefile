#Makefile for our library

PROG = manage
CC = g++
CPPFLAGS = -std=c++1z -Wall -pthread -MMD -MP
CPPFLAGS += -I./app/ -I./lib/
OVERALLCPP = $(CPPFLAGS)
OVERALLCPP += -L./app/ -lhttp_response -lhttp_request

#CPPFLAGS += -ferror-limit=5   # doesn't work on Linux
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
# OBJS = app/app.o app/main.o routing/router.o
OBJS = app/msocket.o app/app.o routing/router.o
TESTOBJS = app/msocket.o app/app.o routing/router.o app/logger.o \
  $(SFCDIR)/FileWatcher.o \
  $(SFCDIR)/FileWatcherOSX.o \
  $(SFCDIR)/FileWatcherLinux.o

UNITTESTDIR=tests
UNITTESTS= $(UNITTESTDIR)/test-Router.o
UNITTESTS= $(UNITTESTDIR)/test-http-request-parser.o
UNITTESTEXE=$(UNITTESTDIR)/tests

HTTPPARSEROBJ = http_parser/http_request_parser.o\
	http_parser/http_request_body_parser.o \
	http_parser/url_encoded_form_parser.o \
	http_parser/http_request_line.o \
	http_parser/http_stream_reader.o
HTTPPARSERLIB = app/libhttp_response.so \
	app/libhttp_request.so


TESTOBJS += $(HTTPPARSEROBJ)
OBJS += $(HTTPPARSEROBJ)
# substitute .o suffix with .d
DEPENDS   = $(TESTOBJS:.o=.d)

# $@ : target file name  ( left side of ':')
# $^ : source file names (right side of ':')
# Note: $< is only the first source file

# -ldl is needed for Linux
$(PROG) : $(TESTOBJS) $(HTTPPARSERLIB) app/manage.cpp
	$(CC) $(OVERALLCPP) -o $@ $^ -ldl
# main.o : app/main.cpp
# 	$(CC) $(CPPFLAGS) -c $^
msocket.o : app/msocket.cpp
	$(CC) $(CPPFLAGS) -c $^
app.o : app/app.cpp
	$(CC) $(CPPFLAGS) -c $^
router.o : routing/router.cpp
	$(CC) $(CPPFLAGS) -c $^
logger.o: app/logger.cpp
	$(CC) $(CPPFLAGS) -fPIC -c $^

# Needed for Linux (/usr/bin/ld: http_parser/http_response.o: relocation R_X86_64_32 against `__pthread_key_create' can not be used when making a shared object; recompile with -fPIC)
http_parser/http_request.o: http_parser/http_request.cpp http_parser/http_session.cpp
	$(CC) $(CPPFLAGS) -fPIC -c http_parser/http_request.cpp -o $@
http_parser/http_response.o: http_parser/http_response.cpp
	$(CC) $(CPPFLAGS) -fPIC -c http_parser/http_response.cpp -o $@

# http_request.cpp uses _SPDLOG which calls cjango_logger global variable
# Without defining that variable, linker returns error. Thus app/logger.o
app/libhttp_response.so: http_parser/http_request.o http_parser/http_session.o http_parser/http_response.o app/logger.o
	$(CC) $(CPPFLAGS) -shared -o $@ $^
app/libhttp_request.so: http_parser/http_request.o http_parser/http_session.o app/logger.o
	$(CC) $(CPPFLAGS) -shared -o $@ $^

lib/simplefilewatcher/build/intermediate/Debug/FileWatcher.o:
	make -C lib/simplefilewatcher/build/gmake
lib/simplefilewatcher/build/intermediate/Debug/FileWatcherOSX.o:
	make -C lib/simplefilewatcher/build/gmake
lib/simplefilewatcher/build/intermediate/Debug/FileWatcherLinux.o:
	make -C lib/simplefilewatcher/build/gmake

test: $(UNITTESTS) $(TESTOBJS) $(HTTPPARSERLIB)
	$(CC) $(OVERALLCPP) $(UNITTESTDIR)/test-main.cpp -c -o $(UNITTESTDIR)/test-main.o
	$(CC) $(OVERALLCPP) $(UNITTESTDIR)/test-main.o $(UNITTESTS) $(TESTOBJS) -o $(UNITTESTEXE)
test-run:
	$(UNITTESTEXE) -r compact

clean:
	rm -f core $(PROG) $(OBJS) $(HTTPPARSERCPP) $(DEPENDS) $(UNITTESTS) $(UNITTESTEXE)

# By -MMD -MP flags, dependency between source files are written in *.d files
# These *.d files are included here in order to auto-recompile when header files change
-include $(DEPENDS)
