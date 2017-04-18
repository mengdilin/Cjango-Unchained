CC = g++
CPPFLAGS = -std=c++1z -Wall -Wno-return-type-c-linkage
CPPFLAGS += -DDEBUG -DCJANGO_DYNLOAD
CPPFLAGS += -I./../src/app/ -I./../src/lib/
#CPPFLAGS += -ferror-limit=5   # doesn't work on Linux
LDFLAGS = -L./../src/app/ -lhttp_response -lhttp_request

# read callbacks/*.cpp files into a variable SOURCES
SOURCES = $(wildcard *.cpp)
# substitute .cpp by .so
OBJS = $(SOURCES:.cpp=.so) $(SOURCES:.cpp=.o)
OBJDIRS = $(SOURCES:.cpp=.so.dSYM)

# When you execute "make" (i.e. make with no arugment),
# this "all" target or all *.so files will be built.
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
	rm -r $(OBJDIRS)
	# FIXME: rm OBJDIRS error
