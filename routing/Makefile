all: router

CXX=g++
CXXFLAGS=-std=c++1z -Wall

router.o: router.cc router.hpp
	$(CXX) $(CXXFLAGS) router.cc -c -o router.o

# unit test
test_router: router.cc router.hpp
	$(CXX) $(CXXFLAGS) router.o test_router.cpp -o test_router