CC=g++
CXX=g++
RM=rm -f
LDFLAGS=-std=c++1z -Wall -pthread -MMD -MP
CPPFLAGS += -I./../app/
OBJS = http_request.o \
	http_session.o \
	http_response.o \
	libhttp_response.so \
	libhttp_request.so
all: $(OBJS)

libhttp_response.so: http_request.o http_session.o http_response.o
	$(CC) $(CPPFLAGS) -shared -o $@ $^
libhttp_request.so: http_request.o http_session.o
	$(CC) $(CPPFLAGS) -shared -o $@ $^

# Needed for Linux (/usr/bin/ld: http_parser/http_response.o: relocation R_X86_64_32 against `__pthread_key_create' can not be used when making a shared object; recompile with -fPIC)
http_request.o: http_request.cpp http_session.cpp
	$(CC) $(CPPFLAGS) -fPIC -c http_request.cpp -o $@
http_response.o: http_response.cpp
	$(CC) $(CPPFLAGS) -fPIC -c http_response.cpp -o $@
http_session.o: http_session.cpp
	$(CC) $(CPPFLAGS) -fPIC -c http_session.cpp -o $@

clean:
	rm *.o main *.so
