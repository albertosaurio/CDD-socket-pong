CXX=g++
CXXFLAGS= -std=c++11 -Wall -Wextra -pthread
EXPORT=-o pongGame
all:
	$(CXX) $(CXXFLAGS) main.cpp game/Menu.cpp  $(EXPORT)

clean:
	rm *.o pongGame