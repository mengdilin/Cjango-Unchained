CC = g++
CPPFLAGS = -std=c++1z -Wall -Wno-return-type-c-linkage
CPPFLAGS += -DDEBUG -DCJANGO_DYNLOAD
CPPFLAGS += -I./../app/
CPPFLAGS += -ferror-limit=5
LDFLAGS = -L./../app/ -lhttp_response -lhttp_request
OBJS = mycallback.o mycallback2.o hello.o mycallback2.so mycallback.so hello.so

# $(PROG) : $(OBJS)
# 	$(CC) $(CPPFLAGS) -o $(PROG) $(OBJS)
all: $(OBJS)

# Suffixes to apply Make's Suffix Rules
.SUFFIXES: .cpp .o .so

# Suffix Rule 1: create *.o from corresponding *.cpp
.cpp.o:
	$(CC) $(CPPFLAGS) -fPIC -c $<

# Suffix Rule 2: create a shared object *.so from the corresponding *.o object file
.o.so:
	$(CC) $(CPPFLAGS) $(LDFLAGS) -shared -o $@ $^

clean:
	rm -f $(OBJS)
