#include <random>
#include <vector>

#include "SnakeClasses.hpp"


Point::Point(std::pair<int, int> position) : x(position.first ),
                                             y(position.second) {}

bool Point::operator==(Point p) {
    return this->x == p.x && this->y == p.y;
}



GameField::GameField(int height, int width) : height_(height), width_(width) {
    matrix_.resize(height_);
    for (int i = 0; i < height_; ++i) {
        matrix_[i].resize(width_);
    }
}

int GameField::at(Point position) const {
    return matrix_[position.x][position.y];
}

int GameField::at(int x, int y) const {
    return matrix_[x][y];
}

void GameField::set(Point position, int value) {
    matrix_[position.x][position.y] = value;
}

void GameField::set(int x, int y, int value) {
    matrix_[x][y] = value;
}

void GameField::reset() {
    for (int i = 0; i < height_; ++i)
        for (int j = 0; j < width_; ++j)
            matrix_[i][j] = 0;
}

int GameField::get_height() const {
    return height_;
}

int GameField::get_width() const {
    return width_;
}



Game::Game(int height, int width) : field(GameField(height, width)),
                                    timer_(0), is_over_(false) {}

void Game::NewGame() {
    field.reset();

    field.set(7, 1, 1);
    field.set(7, 2, 2);
    field.set(7, 3, 3);

    timer_ = 0;
    is_over_ = false;
}

int Game::sec_passed() {
    return ++timer_;
}

bool Game::is_over() const {
    return is_over_;
}

void Game::GameOver() {
    is_over_ = true;
}



Snake::Snake(int x, int y) : direction(right), x_(x), y_(y), length_(3) {}

void Snake::Move(const GameField &field) {
    switch (direction) {
        case right:
            y_ = (y_ + 1) % field.get_width();
            break;

        case up:
            x_ = (x_ == 0) ? (field.get_height() - 1) : (x_ - 1);
            break;

        case left:
            y_ = (y_ == 0) ? (field.get_width() - 1) : (y_ - 1);
            break;

        case down:
            x_ = (x_ + 1) % field.get_height();
            break;
    }
}

void Snake::Teleport(int x, int y) {
    x_ = x;
    y_ = y;
}
void Snake::Teleport(Point position) {
    x_ = position.x;
    y_ = position.y;
}

Point Snake::position(){
    return std::make_pair(x_, y_);
}

void Snake::Grow() {
    length_++;
}

int Snake::get_length() const {
    return length_;
}

void Snake::UpdateDirection(Direction new_direction) {
    switch (new_direction) {
        case right:
            direction = (direction == left) ? direction : new_direction;
            break;

        case up:
            direction = (direction == down) ? direction : new_direction;
            break;

        case left:
            direction = (direction == right) ? direction : new_direction;
            break;

        case down:
            direction = (direction == up) ? direction : new_direction;
            break;
    }
}



Fruit::Fruit(int x, int y) : x_(x), y_(y) {}

Point Fruit::position() const {
    return std::make_pair(x_, y_);
}

void Fruit::GenerateNewOne(const GameField &field,
                           const PortalPair portal_pair) {
    while (true) {
        x_ = std::rand() % field.get_height();
        y_ = std::rand() % field.get_width();
        if (field.at(position()) == 0 &&
            !(position() ==  portal_pair.first_position()) &&
            !(position() == portal_pair.second_position()))
            return;
    }
}



PortalPair::PortalPair(int x1, int y1,
                       int x2, int y2) : x1_(x1), y1_(y1),
                                         x2_(x2), y2_(y2) {}

Point PortalPair::first_position() const {
    return std::make_pair(x1_, y1_);
}

Point PortalPair::second_position() const {
    return std::make_pair(x2_, y2_);
}

void PortalPair::GenerateNewPair(const GameField &field, const Fruit fruit) {
    while (true) {
        x1_ = std::rand() % field.get_height();
        y1_ = std::rand() % field.get_width();
        x2_ = std::rand() % field.get_height();
        y2_ = std::rand() % field.get_width();
        if (field.at(first_position()) == 0 &&
            field.at(second_position()) == 0 &&
            !(first_position() == second_position()) &&
            !(first_position() == fruit.position()) &&
            !(second_position() == fruit.position()))
            return;
    }
}
