// Implementation for Problem 116 - GreedySnake
#ifndef SRC_HPP
#define SRC_HPP

#include <iostream>
#include <deque>
#include <utility>
#include <vector>

struct Map;
enum class instruction { UP, DOWN, LEFT, RIGHT, NONE };

bool is_food(Map* map, int x, int y);
bool is_wall(Map* map, int x, int y);
void eat_food(Map* map, int x, int y);
int get_height(Map* map);
int get_width(Map* map);

struct Snake {
  std::deque<std::pair<int,int>> body;
  instruction dir = instruction::NONE;

  void initialize(int x, int y, instruction ins){
    body.clear();
    body.emplace_front(x, y);
    dir = ins;
  }
  int get_length(){
    return static_cast<int>(body.size());
  }
  bool move(Map* map, instruction ins){
    if (ins != instruction::NONE) {
      // Reject reverse moves
      if (!body.empty()){
        auto head = body.front();
        int hx = head.first, hy = head.second;
        int nx = hx, ny = hy;
        switch(dir){
          case instruction::UP: nx = hx-1; break;
          case instruction::DOWN: nx = hx+1; break;
          case instruction::LEFT: ny = hy-1; break;
          case instruction::RIGHT: ny = hy+1; break;
          default: break;
        }
        // Calculate opposite direction of current dir
      }
      // Update direction if not reversing
      if (!( (dir==instruction::UP && ins==instruction::DOWN) ||
             (dir==instruction::DOWN && ins==instruction::UP) ||
             (dir==instruction::LEFT && ins==instruction::RIGHT) ||
             (dir==instruction::RIGHT && ins==instruction::LEFT) )){
        dir = ins;
      } else {
        // Reverse move is immediate death
        return false;
      }
    }
    if (dir == instruction::NONE) return false;
    auto head = body.front();
    int x = head.first, y = head.second;
    switch(dir){
      case instruction::UP: x--; break;
      case instruction::DOWN: x++; break;
      case instruction::LEFT: y--; break;
      case instruction::RIGHT: y++; break;
      default: break;
    }
    // Check boundary via map helpers
    if (x < 0 || y < 0 || x >= get_height(map) || y >= get_width(map)) return false;
    // Check wall
    if (is_wall(map, x, y)) return false;
    // Check self collision (body contains positions; tail moves unless eating food)
    bool will_eat = is_food(map, x, y);
    // If not eating, the tail will move: remove last before self-collision check
    if (!will_eat && !body.empty()){
      // simulate tail pop for collision check
      auto tail = body.back();
      // Check collision excluding tail if it moves away
      for (size_t i=0;i+1<body.size();++i){
        if (body[i].first==x && body[i].second==y) return false;
      }
    } else {
      // Eating: check against all current body cells
      for (auto &p: body){
        if (p.first==x && p.second==y) return false;
      }
    }
    // Perform move
    body.emplace_front(x, y);
    if (will_eat){
      eat_food(map, x, y);
    } else {
      body.pop_back();
    }
    return true;
  }
  std::pair<int, std::pair<int,int>*> get_snake(){
    int n = static_cast<int>(body.size());
    auto *arr = new std::pair<int,int>[n];
    for (int i=0;i<n;++i) arr[i]=body[i];
    return {n, arr};
  }
};

const int MaxWidth = 20;
struct Map{
  bool wall[MaxWidth][MaxWidth]{};
  bool food[MaxWidth][MaxWidth]{};
  int width = 0, height = 0;
  int get_height(){ return height; }
  int get_width(){ return width; }
  bool is_food(int x, int y){ return food[x][y]; }
  void eat_food(int x, int y){ food[x][y]=false; }
  bool is_wall(int x, int y){ return wall[x][y]; }

  void initialize(Snake *snake){
    char str[MaxWidth+5];
    int head_x = -1 , head_y = -1;
    instruction ins = instruction::NONE;
    std::cin >> height >> width;
    for (int i=0;i<height;i++){
      std::cin >> str;
      for (int j=0;j<width;j++){
        char c = str[j];
        if (c=='#') wall[i][j]=true;
        else wall[i][j]=false;
        if (c=='*') food[i][j]=true; else food[i][j]=false;
        if (c=='@'){ head_x=i; head_y=j; }
      }
    }
    std::cin >> str;
    switch(str[0]){
      case 'U': ins = instruction::UP; break;
      case 'D': ins = instruction::DOWN; break;
      case 'L': ins = instruction::LEFT; break;
      case 'R': ins = instruction::RIGHT; break;
      default: ins = instruction::NONE; break;
    }
    snake->initialize(head_x, head_y, ins);
  }
  void print(Snake *snake){
    auto snake_body = snake->get_snake();
    int n = snake_body.first;
    std::vector<std::string> grid(height, std::string(width, '.'));
    for (int i=0;i<height;i++){
      for (int j=0;j<width;j++){
        if (wall[i][j]) grid[i][j] = '#';
        else if (food[i][j]) grid[i][j] = '*';
      }
    }
    if (n>0){
      grid[snake_body.second[0].first][snake_body.second[0].second] = '@';
      for (int k=1;k<n;k++){
        grid[snake_body.second[k].first][snake_body.second[k].second] = 'o';
      }
    }
    for (int i=0;i<height;i++){
      std::cout << grid[i] << std::endl;
    }
    delete [] snake_body.second;
  }
};

struct Game{
  Map *map;
  Snake *snake;
  int score;
  int round;
  void initialize(){
    map = new Map();
    snake = new Snake();
    map->initialize(snake);
    score = 0;
    round = 0;
  }
  bool step()
  {
    char str[MaxWidth];
    std::cin >> str;
    instruction ins;
    switch(str[0]){
      case 'U':
        ins = instruction::UP;
        break;
      case 'D':
        ins = instruction::DOWN;
        break;
      case 'L':
        ins = instruction::LEFT;
        break;
      case 'R':
        ins = instruction::RIGHT;
        break;
      default:
        ins = instruction::NONE;
    }
    if(snake->move(map, ins)){
      score++;
    }else{
      return false;
    }
    return true;
  }
  void print(){
    std::cout<< "Round: " << round << std::endl;
    map->print(snake);
  }
  void play()
  {
    while(step()){
      round++;
      print();
    }
    score += snake->get_length() ;
    std::cout << "Game Over" << std::endl;
    std::cout << "Score: " << score << std::endl;
  }
};

#endif
