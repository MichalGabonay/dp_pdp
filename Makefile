CXX ?= g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

vrp: objs/main.o objs/utilities.o
	$(CXX) $(CXXFLAGS) objs/main.o objs/utilities.o -o vrp

objs/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o objs/main.o

objs/utilities.o: src/utilities.cpp
	$(CXX) $(CXXFLAGS) -c src/utilities.cpp -o objs/utilities.o

run:
	./vrp

clean:
	rm -f vrp 