CXX ?= g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# vrp: main.o
# 	$(CXX) $(CXXFLAGS) main.o -o vrp
pdp_xgabon00: objs/main.o objs/solver.o objs/task.o objs/config.o objs/vrp_helpers.o
	$(CXX) $(CXXFLAGS) objs/main.o objs/solver.o objs/task.o objs/config.o objs/vrp_helpers.o -o pdp_xgabon00

objs/main.o: src/main.cc
	$(CXX) $(CXXFLAGS) -c src/main.cc -o objs/main.o

objs/solver.o: src/solver.cc
	$(CXX) $(CXXFLAGS) -c src/solver.cc -o objs/solver.o

objs/task.o: src/task.cc
	$(CXX) $(CXXFLAGS) -c src/task.cc -o objs/task.o

objs/config.o: src/config.cc
	$(CXX) $(CXXFLAGS) -c src/config.cc -o objs/config.o

objs/vrp_helpers.o: src/vrp_helpers.cc
	$(CXX) $(CXXFLAGS) -c src/vrp_helpers.cc -o objs/vrp_helpers.o

run:
	./pdp_xgabon00

clean:
	rm -f pdp_xgabon00 