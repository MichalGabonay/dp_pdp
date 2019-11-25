CXX ?= g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

vrp: main.o utilities.o
	$(CXX) $(CXXFLAGS) main.o utilities.o -o vrp

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

utilities.o: utilities.cpp
	$(CXX) $(CXXFLAGS) -c utilities.cpp

run:
	./vrp

clean:
	rm -f vrp