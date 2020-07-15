CXX				:= g++
CXXFLAGS		:= -std=c++17 -I../../rdai_api -I../../host_runtimes/linux_no_cma/include

SRCs			:= $(wildcard *.cpp) $(wildcard ../../host_runtimes/linux_no_cma/src/*.cpp)

all: $(SRCs)
	$(CXX) $(CXXFLAGS) $^ -o program

clean:
	rm -rf program *.o
