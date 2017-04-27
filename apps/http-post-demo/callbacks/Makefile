CC = g++
CPPFLAGS = -std=c++1z -Wall -Wno-return-type-c-linkage
CPPFLAGS += -DDEBUG -DCJANGO_DYNLOAD
CPPFLAGS += -I./../../../src/app/ -I./../../../src/lib/
#CPPFLAGS += -ferror-limit=5   # doesn't work on Linux
LDFLAGS = -L./../../../src/http_parser/ -lhttp_response -lhttp_request -lsqlite3
#LDFLAGS += -L./../Jinja2CppLight/build -lJinja2CppLight
#LDFLAGS += -L./../mstch/build/src -lmstch
# read callbacks/*.cpp files into a variable SOURCES
SOURCES = $(wildcard *.cpp)
# substitute .cpp by .so
OBJS = $(SOURCES:.cpp=.so) $(SOURCES:.cpp=.o)
OBJDIRS = $(SOURCES:.cpp=.so.dSYM)

# When you execute "make" (i.e. make with no arugment),
# this "all" target or all *.so files will be built.
all: $(OBJS)

packages:
	sudo apt-get install sqlite3 libsqlite3-dev
.PHONY: packages
# Suffixes to apply Make's Suffix Rules
.SUFFIXES: .cpp .o .so

# Suffix Rule 1: create *.o from corresponding *.cpp
.cpp.o:
	$(CC) $(CPPFLAGS) -fPIC -c $<
	touch ../json/urls.json

# Suffix Rule 2: create a shared object *.so from the corresponding *.o object file
.o.so:
	$(CC) $(CPPFLAGS) -shared -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS)
	rm -r $(OBJDIRS)
	# FIXME: rm OBJDIRS error
