#include "game.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  for(int i = 0; i < 5; i++){
     PlaceItem(food);
     PlaceItem(posion);
  }
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food, posion);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

/**
 * @brief Given an x & y position returns true if that cell is not filled by the snake, food, or posion.
 *
 * @param x integer value of the x coordinate on grid
 * @param y integer value of the y coordinate on grid
 * @return bool true if grid position is open.
 */
bool Game::OpenCell(int x, int y) {
  // Check if snake is in cell
  if ( snake.SnakeCell(x, y)) {
    return false;
  }
  // Check if there is a food or posion in cell
  return !Game::VectorOccupies(food, x, y) && !Game::VectorOccupies(posion, x, y);
}

/**
 * @brief Checks if a vector of SDL_Points occupies a given position.
 *
 * @param group Vector SDL_Point either food or posion
 * @param x integer value of the x coordinate on grid
 * @param y integer value of the x coordinate on grid
 * @return bool true if given vectory has SDL_Point at given location.
 */
bool Game::VectorOccupies(std::vector<std::shared_ptr<SDL_Point>> group, int x, int y){
  for( auto peice : group ){
    if ( peice->x == x && peice->y == y){
      return true;
    }
  }
  return false;
}

/**
 * @brief Given a vector add a new SDL_Point to it using random x & y coordinates
 *
 * @param items A reference to a vector of SDL_Points. Either the food or posion vector.
 */
void Game::PlaceItem(std::vector<std::shared_ptr<SDL_Point>> &items){
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (Game::OpenCell(x, y)) {
      SDL_Point p {x, y};
      items.emplace_back(std::make_shared<SDL_Point>(p));
      return;
    }
  }
}

/**
 * @brief Removes a SDL_Point from a given vector.
 *
 * @param items reference to the vector to remove from
 * @param x int x coordinate to be used to find correct point to remove
 * @param y int y coordinate to be used to find correct point to remove
 */
void Game::RemoveItem(std::vector<std::shared_ptr<SDL_Point>> &items, int x, int y){
  items.erase(std::remove_if(items.begin(), items.end(), 
        [x, y](std::shared_ptr<SDL_Point> p){ return p->x == x && p->y == y; }
        ), items.end());
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if ( VectorOccupies(food, new_x, new_y)) {
    score++;
    PlaceItem(food);
    RemoveItem(food, new_x, new_y);
    snake.GrowBody();
    snake.speed += 0.02;
  }
  if ( VectorOccupies(posion, new_x, new_y)) {
    score--;
    PlaceItem(posion);
    RemoveItem(posion, new_x, new_y);
    snake.ShrinkBody();
    snake.speed -= 0.02;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }
