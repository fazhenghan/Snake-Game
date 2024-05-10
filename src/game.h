#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include <vector>
#include <memory>

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  Snake snake;
  std::vector<std::shared_ptr<SDL_Point>> food;
  std::vector<std::shared_ptr<SDL_Point>> posion;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  bool OpenCell(int x, int y);
  bool VectorOccupies(std::vector<std::shared_ptr<SDL_Point>> group, int x, int y);
  void PlaceItem(std::vector<std::shared_ptr<SDL_Point>> &items);
  /* void PlacePosion(); */
  void RemoveItem(std::vector<std::shared_ptr<SDL_Point>> &items, int x, int y);
  void Update();
};

#endif
