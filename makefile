HEADERS = game_of_life.h
CPPFLAGS = -std=c++11
LDLIBS = -lncurses

default: game_of_life

game_of_life.o: game_of_life.cpp $(HEADERS)
	g++ $(CPPFLAGS) -c game_of_life.cpp -o game_of_life.o $(LDLIBS)

game_of_life: game_of_life.o
	g++ $(CPPFLAGS) game_of_life.o -o game_of_life $(LDLIBS)

clean:
	-rm -f game_of_life.o
	-rm -f game_of_life
