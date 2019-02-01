#include <ncurses.h>
#include <vector>

struct Coordinates {
  int x;
  int y;
  Coordinates() {}
  Coordinates(int xVal, int yVal) : x(xVal), y(yVal) {}
};

class GameOfLife {
public:
  explicit GameOfLife(int lc);
  ~GameOfLife();

  void Generation();
  void UpdateDisplay();

private:
  void Randomise();
  bool CheckNeighbours(int x, int y);

  std::vector<std::vector<bool>> grid;

  unsigned generationCount = 0;
  unsigned livingCellCount = 0;
  unsigned startLiveCells; // Amount of live cells on start

  // Coordinates of where to display information
  Coordinates titleCoords;
  Coordinates liveCellCoords;
  Coordinates generationCoords;

  std::vector<Coordinates> neighbourCoords;

  WINDOW *window; // Parent window
  // Child windows
  WINDOW *infoWindow;
  WINDOW *gameWindow;
};
