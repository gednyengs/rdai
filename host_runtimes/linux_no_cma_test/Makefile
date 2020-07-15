CXX				:= g++
CXXFLAGS		:= -std=c++17 -I../../rdai_api -I../../host_runtimes/linux_no_cma

SRCs			:= $(wildcard *.cpp) $(wildcard ../../host_runtimes/linux_no_cma/*.cpp)

all: $(SRCs)
	$(CXX) $(CXXFLAGS) $^ -o program

clean:
	rm -rf program *.o
