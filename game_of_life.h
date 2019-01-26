// classes example
#include <vector>

class GameOfLife {
  public:
    GameOfLife(std::vector<std::vector<bool> > sg);
    ~GameOfLife();

    void Run();

  private:
    std::vector<std::vector<bool> > grid;

};
