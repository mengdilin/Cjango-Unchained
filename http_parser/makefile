CC=gcc
CXX=g++
RM=rm -f
LDFLAGS=-O2 -g -std=c++1z -Wall
CPPFLAGS += -I./../app/
SRCS=main.cpp http_request.cpp \
	url_encoded_form_parser.cpp \
	http_request_line.cpp \
	http_request_parser.cpp \
	http_request_body_parser.cpp\
	http_stream_reader.cpp \
	http_response.cpp
OBJS=$(subst .cc,.o,$(SRCS))
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CPPFLAGS += -g -DDEBUG
endif
all: main

main: $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

clean:
	rm *.o main
