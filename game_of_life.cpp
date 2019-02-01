#include "game_of_life.h"
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <signal.h>
#include <unistd.h>

#define DEFAULT_GENERATION_MS 500
#define DEFAULT_LIVE_START 800

#define X_PADDING 2
#define Y_PADDING 4

GameOfLife::GameOfLife(int lc) : startLiveCells(lc) {
  // Initialise the displays
  window = initscr();
  start_color();
  noecho();
  curs_set(false);
  // Get parent width, height values to build children
  int x, y;
  getmaxyx(window, y, x);
  infoWindow = subwin(window, 8, x, 0, 0);
  box(infoWindow, ACS_VLINE, ACS_HLINE);

  gameWindow = subwin(window, y - 8, x, 8, 0);

  // Set the grid to the initial window size, minus some spaces for padding
  getmaxyx(gameWindow, y, x);
  grid = std::vector<std::vector<bool>>(y - 2, std::vector<bool>(x - 2, false));

  // Set all the information/statistic x,y coordinates
  titleCoords = Coordinates(X_PADDING, Y_PADDING);
  liveCellCoords = Coordinates(X_PADDING + 3, Y_PADDING);
  generationCoords = Coordinates(X_PADDING + 4, Y_PADDING);

  neighbourCoords.push_back(Coordinates(-1, -1));
  neighbourCoords.push_back(Coordinates(-1, 0));
  neighbourCoords.push_back(Coordinates(-1, 1));
  neighbourCoords.push_back(Coordinates(0, -1));
  neighbourCoords.push_back(Coordinates(0, 1));
  neighbourCoords.push_back(Coordinates(1, -1));
  neighbourCoords.push_back(Coordinates(1, 0));
  neighbourCoords.push_back(Coordinates(1, 1));

  // Setup all the colour pairs
  init_pair(1, COLOR_BLACK, COLOR_RED);
  init_pair(2, COLOR_BLACK, COLOR_GREEN);
  init_pair(3, COLOR_BLACK, COLOR_YELLOW);
  init_pair(4, COLOR_BLACK, COLOR_BLUE);
  init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(6, COLOR_BLACK, COLOR_CYAN);
  init_pair(7, COLOR_BLACK, COLOR_WHITE);

  // Randomly populate the grid with live cells
  Randomise();
}

GameOfLife::~GameOfLife() {
  delwin(window);
  delwin(infoWindow);
  delwin(gameWindow);
}
void GameOfLife::Generation() {
  // Check the status of every cell in the grids neighbours
  for (int i = 0; i < grid.size(); ++i) {
    for (int j = 0; j < grid.at(i).size(); ++j) {
      // Check the neighbouring cells to determine this cells status
      grid.at(i).at(j) = CheckNeighbours(i, j);
    }
  }
  ++generationCount;
}

void GameOfLife::UpdateDisplay() {
  // Clear previous state
  wclear(gameWindow);

  // Initalise rng's range to the amount of colour pairs
  std::mt19937 mt;
  mt.seed(std::random_device()());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1, 7);

  for (int i = 0; i < grid.size(); ++i) {
    for (int j = 0; j < grid.at(i).size(); ++j) {
      if (grid.at(i).at(j)) {
        // Display live cell with a random colour
        wattron(gameWindow, COLOR_PAIR(dist(mt)));
        mvwprintw(gameWindow, i, j, " ");
      }
    }
  }
  wrefresh(gameWindow);

  // Now update the information display

  // Count how many live cells there are
  unsigned tempCount = 0;
  for (int i = 0; i < grid.size(); ++i) {
    tempCount += std::count_if(grid.at(i).begin(), grid.at(i).end(),
                               [&](bool x) { return x; });
  }
  livingCellCount = tempCount;

  // Clear the old statistics
  wclear(infoWindow);
  box(infoWindow, ACS_VLINE, ACS_HLINE);

  mvwprintw(infoWindow, titleCoords.x, titleCoords.y, "Conway's Game of Life");
  mvwprintw(infoWindow, liveCellCoords.x, liveCellCoords.y, "Live cells: %d",
            livingCellCount);
  mvwprintw(infoWindow, generationCoords.x, generationCoords.y,
            "Generation iteration: %d", generationCount);

  wrefresh(infoWindow);
}

void GameOfLife::Randomise() {
  // Randomly populate live cells in the grid
  std::mt19937 mt;
  mt.seed(std::random_device()());
  std::uniform_int_distribution<std::mt19937::result_type> distCol(
      0, grid.size() - 1);
  std::uniform_int_distribution<std::mt19937::result_type> distRow(
      0, grid.at(0).size() - 1);

  for (int i = 0; i < startLiveCells; ++i) {
    grid.at(distCol(mt)).at(distRow(mt)) = true;
  }
}

bool GameOfLife::CheckNeighbours(int i, int j) {
  // Check how many neighbours the cell has
  unsigned neighbours = 0;
  for (auto neighbour : neighbourCoords) {
    try {
      if (grid.at(i + neighbour.x).at(j + neighbour.y)) {
        ++neighbours;
      }
    } catch (const std::out_of_range &oor) {
      // The exception was thrown due to the cell being on the edge of the grid.
      // Don't need to handle, just check next neighbour
      continue;
    }
  }

  bool ret;
  // Live cell scenarios
  if (grid.at(i).at(j)) {
    // Scenario: Underpopulation
    if (neighbours < 2) {
      ret = false;
    }
    // Scenario: Overcrowding
    else if (neighbours > 3) {
      ret = false;
    }
    // Scenario: Survival
    else if (neighbours == 2 || neighbours == 3) {
      ret = true;
    }
  }
  // Empty cell scenarios
  else {
    // Scenario: Creation of Life
    if (neighbours == 3) {
      ret = true;
    }
  }

  return ret;
}

static volatile sig_atomic_t quitFlag = 0;
void sig_handler(int sig) { quitFlag = 1; }

int main(int argc, char *argv[]) {
  int opt;
  int generationMs = DEFAULT_GENERATION_MS;
  int liveCells = DEFAULT_LIVE_START;

  while ((opt = getopt(argc, argv, "l:s:h")) != -1) {
    switch (opt) {
    case 's':
      try {
        generationMs = std::stoi(optarg);
      } catch (const std::invalid_argument &ia) {
        std::cerr << "Error: Please provide -s argument in integer format"
                  << std::endl;
        return -1;
      }
      break;
    case 'l':
      try {
        liveCells = std::stoi(optarg);
      } catch (const std::invalid_argument &ia) {
        std::cerr << "Error: Please provide -l argument in integer format"
                  << std::endl;
        return -1;
      }
      break;
    case 'h':
    default:
      std::cerr
          << "Usage: game_of_life [-l] [-s] [-h]  " << std::endl
          << "    -l" << std::endl
          << "        The number of randomly populated live cells on "
             "startup. (If not supplied default parameter is used)"
          << std::endl
          << "    -s" << std::endl
          << "        The number of seconds between each generation. (If not "
             "supplied default parameter is used)"
          << std::endl
          << "    -h" << std::endl
          << "        Print this help message" << std::endl;
      return 0;
    }
  }

  // Setup game
  GameOfLife gameOfLife(liveCells);

  // Setup signal handlers
  signal(SIGSTOP, sig_handler);
  signal(SIGABRT, sig_handler);
  signal(SIGTERM, sig_handler);

  while (!quitFlag) {
    gameOfLife.UpdateDisplay();
    usleep(1000 * generationMs);
    gameOfLife.Generation();
  }

  return 0;
}
