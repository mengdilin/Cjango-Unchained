CC=gcc
CXX=g++
RM=rm -f
LDFLAGS=-O2 -g

SRCS=main.cpp http_request.cpp \
	url_encoded_form_parser.cpp \
	http_request_line.cpp \
	http_request_parser.cpp \
	http_request_body_parser.cpp\
	http_stream_reader.cpp \
	http_response.cpp
OBJS=$(subst .cc,.o,$(SRCS))

all: main

main: $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

clean:
	rm *.o main
