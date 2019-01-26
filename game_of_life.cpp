#include <iostream>
#include <vector>
#include "game_of_life.h"

GameOfLife::GameOfLife (std::vector<std::vector<bool> > sg) : grid(sg) {}

GameOfLife::~GameOfLife () {}

void GameOfLife::Run ()
{
  for (auto column : grid){
    for (auto row : column){
      std::cout << "/* message */" << '\n';
    }
  }
}

int main () {
  // Populate seed grid
  std::vector<std::vector<bool> > seedGrid;

  GameOfLife gol(seedGrid);
  gol.Run();

  return 0;
}
