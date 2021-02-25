#ifndef _SNAKE_CLASSES
#define _SNAKE_CLASSES

#include <vector>


struct Point {
    int x, y;

    Point(std::pair<int, int> position);
    bool operator==(Point p);
};



class GameField {
public:
    GameField(int height, int width);

    int at(Point position) const;
    int at(int x, int y) const;

    void set(Point position, int value);
    void set(int x, int y, int value);

    void reset();

    int get_height() const;
    int get_width() const;

private:
    int height_, width_;
    std::vector<std::vector<int>> matrix_;
};



class Game {
public:
    GameField field;

    Game(int height, int width);

    void NewGame();

    int sec_passed();

    bool is_over() const;

    void GameOver();

private:
    int timer_;
    bool is_over_;
};



class Snake {
public:
    enum Direction {right, up, left, down};

    Direction direction;

    Snake(int x, int y);

    Point position();

    void Grow();
    int get_length() const;

    void Move(const GameField &field);

    void Teleport(int x, int y);
    void Teleport(Point position);

    void UpdateDirection(Direction dir);

private:
    int x_, y_;
    int length_;
};



class PortalPair;

class Fruit {
public:
    Fruit(int x, int y);

    Point position() const;

    void GenerateNewOne(const GameField &field, const PortalPair portal_pair);

private:
    int x_, y_;
};



class PortalPair {
public:
    enum PortalState {snake_in = 0, active = -1, inactive = -2};

    PortalPair(int x1, int y1, int x2, int y2);

    Point  first_position() const;
    Point second_position() const;

    void GenerateNewPair(const GameField &field, const Fruit fruit);

private:
    int x1_, y1_,
        x2_, y2_;
};

#endif
