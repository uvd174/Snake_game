#include <SFML/Graphics.hpp>
#include <string>
#include <random>


enum Direction {right, up, left, down};

enum PortalState {snake_in = 0, active = -1, inactive = -2};

struct Snake {
    Snake(int x_, int y_) : x(x_), y(y_), length(3), direction(right) {}
    int x, y;
    int length;
    Direction direction;

    void Move(int field_width, int field_height) {
        switch (direction) {
            case right:
                y = (y + 1) % field_width;
                break;

            case up:
                x = (x == 0) ? (field_height - 1) : (x - 1);
                break;

            case left:
                y = (y == 0) ? (field_width - 1) : (y - 1);
                break;

            case down:
                x = (x + 1) % field_height;
                break;
        }
    }

    void UpdateDirection(Direction dir) {
        switch (dir) {
            case right:
                direction = (direction == left) ? direction : dir;
                break;

            case up:
                direction = (direction == down) ? direction : dir;
                break;

            case left:
                direction = (direction == right) ? direction : dir;
                break;

            case down:
                direction = (direction == up) ? direction : dir;
                break;
        }
    }
};


struct PortalPair;

struct Fruit {
    Fruit(int x_, int y_) : x(x_), y(y_) {}
    int x, y;
    void GenerateNewOne(int game_matr[15][30], const int field_width,
                        const int field_height, const PortalPair portal_pair);
};


struct PortalPair {
    PortalPair(int x1_, int y1_, int x2_, int y2_) : x1(x1_), y1(y1_),
                                                     x2(x2_), y2(y2_) {}
    int x1, y1, x2, y2;

    void GenerateNewPair(int game_matr[15][30], const int field_width,
                                                const int field_height,
                                                const Fruit fruit) {
        while (true) {
            x1 = rand() % field_height;
            y1 = rand() % field_width;
            x2 = rand() % field_height;
            y2 = rand() % field_width;
            if (game_matr[x1][y1] == 0 &&
                game_matr[x2][y2] == 0 &&
                !(x1 == x2 && y1 == y2) &&
                !(x1 == fruit.x && y1 == fruit.y) &&
                !(x2 == fruit.x && y2 == fruit.y))
                return;
        }
    }
};

void Fruit::GenerateNewOne(int game_matr[15][30],
                           const int field_width,
                           const int field_height,
                           const PortalPair portal_pair) {
    while (true) {
        x = rand() % field_height;
        y = rand() % field_width;
        if (game_matr[x][y] == 0 &&
            !(x == portal_pair.x1 && y == portal_pair.y1) &&
            !(x == portal_pair.x2 && y == portal_pair.y2))
            return;
    }
}


inline void UpdateTimer(sf::Text &timer, int time_in_sec) {
    int const SEC_IN_MIN = 60;
    std::string screen_timer = "";
    screen_timer += std::to_string((time_in_sec / SEC_IN_MIN) / 10);
    screen_timer += std::to_string((time_in_sec / SEC_IN_MIN) % 10);
    screen_timer += ":";
    screen_timer += std::to_string((time_in_sec % SEC_IN_MIN) / 10);
    screen_timer += std::to_string((time_in_sec % SEC_IN_MIN) % 10);
    timer.setString(screen_timer);
}


int main() {
    srand(time(NULL));

    int const WINDOW_WIDTH = 1920;
    int const WINDOW_HEIGHT = 1080;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                            "Snake", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);

    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(255, 255, 255));


    sf::Clock timer_clock;
    sf::Clock snake_clock;
    sf::Clock portal_clock;

    int portal_clock_threshold = 5;


    sf::Font snake_font;
    if (!snake_font.loadFromFile("resources/sansation.ttf")) {
        return EXIT_FAILURE;
    }


    int timer = 0;
    sf::Text timer_text;
    timer_text.setFont(snake_font);
    timer_text.setCharacterSize(WINDOW_WIDTH / 20);
    timer_text.setPosition(WINDOW_WIDTH / 5, 0);
    timer_text.setFillColor(sf::Color::Black);
    timer_text.setString("00:00");
    timer_text.move(-timer_text.getLocalBounds().width / 2, 0);


    sf::Text score_text;
    score_text.setFont(snake_font);
    score_text.setCharacterSize(WINDOW_WIDTH / 20);
    score_text.setPosition(WINDOW_WIDTH * 3.5 / 5, 0);
    score_text.setFillColor(sf::Color::Black);
    score_text.setString("Score: 0");


    int const FIELD_HEIGHT = 15;
    int const FIELD_WIDTH = 30;

    int game_matrix[FIELD_HEIGHT][FIELD_WIDTH] = {0};
    game_matrix[7][1] = 1;
    game_matrix[7][2] = 2;
    game_matrix[7][3] = 3;

    Snake snake = Snake(7, 3);
    Direction user_direction = snake.direction;

    Fruit fruit = Fruit(7, 27);

    PortalPair portal_pair = PortalPair(7, 10, 7, 20);
    int portal_state = active;

    bool game_over = false;


    sf::Texture tile_texture;
    sf::Texture snake_tile_texture;
    sf::Texture fruit_tile_texture;
    sf::Texture portal_tile_texture;
    sf::Texture portal_snake_tile_texture;

    if (!tile_texture.loadFromFile("resources/tile.png")) {
        return EXIT_FAILURE;
    }
    if (!snake_tile_texture.loadFromFile("resources/snake_tile.png")) {
        return EXIT_FAILURE;
    }
    if (!fruit_tile_texture.loadFromFile("resources/fruit_tile.png")) {
        return EXIT_FAILURE;
    }
    if (!portal_tile_texture.loadFromFile("resources/portal_tile.png")) {
        return EXIT_FAILURE;
    }
    if (!portal_snake_tile_texture.loadFromFile("resources/portal_snake_tile.png")) {
        return EXIT_FAILURE;
    }

    int const TEXTURE_WIDTH = tile_texture.getSize().x;
    int const TEXTURE_HEIGHT = tile_texture.getSize().y;

    tile_texture.setRepeated(true);


    sf::Sprite playground;
    playground.setTexture(tile_texture);

    int const PLAYGROUND_WIDTH  = TEXTURE_WIDTH  * FIELD_WIDTH;
    int const PLAYGROUND_HEIGHT = TEXTURE_HEIGHT * FIELD_HEIGHT;

    playground.setTextureRect(sf::IntRect(0, 0, PLAYGROUND_WIDTH,
                                                PLAYGROUND_HEIGHT));
    playground.setPosition((WINDOW_WIDTH - PLAYGROUND_WIDTH) / 2,
                            timer_text.getLocalBounds().height * 2);


    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed: {
                    switch (event.key.code) {
                        case sf::Keyboard::W:
                            user_direction = up;
                            break;
                        case sf::Keyboard::A:
                            user_direction = left;
                            break;
                        case sf::Keyboard::S:
                            user_direction = down;
                            break;
                        case sf::Keyboard::D:
                            user_direction = right;
                            break;
                        case sf::Keyboard::Up:
                            user_direction = up;
                            break;
                        case sf::Keyboard::Left:
                            user_direction = left;
                            break;
                        case sf::Keyboard::Down:
                            user_direction = down;
                            break;
                        case sf::Keyboard::Right:
                            user_direction = right;
                            break;
                        case sf::Keyboard::Space:
                            if (game_over) {
                                for (int i = 0; i < FIELD_HEIGHT; ++i)
                                    for (int j = 0; j < FIELD_WIDTH; ++j)
                                        game_matrix[i][j] = 0;
                                game_matrix[7][1] = 1;
                                game_matrix[7][2] = 2;
                                game_matrix[7][3] = 3;

                                snake = Snake(7, 3);
                                snake_clock.restart();
                                user_direction = snake.direction;

                                fruit = Fruit(7, 27);

                                portal_pair = PortalPair(7, 10, 7, 20);
                                portal_state = active;

                                timer = 0;
                                timer_clock.restart();
                                timer_text.setString("00:00");

                                score_text.setString("Score: 0");

                                game_over = false;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                }

                default:
                    break;
            }
        }

        window.clear();

        window.draw(background);
        window.draw(playground);
        if (!game_over &&
            timer_clock.getElapsedTime().asMilliseconds() > 1000) {

            timer_clock.restart();
            UpdateTimer(timer_text, ++timer);
        }

        if (!game_over &&
            snake_clock.getElapsedTime().asMilliseconds() > 100) {

            snake_clock.restart();

            snake.UpdateDirection(user_direction);
            snake.Move(FIELD_WIDTH, FIELD_HEIGHT);
            if (snake.x == fruit.x && snake.y == fruit.y) {
                snake.length++;
                score_text.setString("Score: " +
                                     std::to_string(snake.length - 3));
                fruit.GenerateNewOne(game_matrix, FIELD_WIDTH, FIELD_HEIGHT,
                                     portal_pair);
            } else {
                for (int i = 0; i < FIELD_HEIGHT; ++i)
                    for (int j = 0; j < FIELD_WIDTH; ++j) {
                        game_matrix[i][j] = std::max(0, game_matrix[i][j] - 1);
                    }
            }
            if (game_matrix[snake.x][snake.y] > 0) {
                game_over = true;
            }

            if (portal_state == inactive &&
                portal_clock.getElapsedTime()
                            .asSeconds() > portal_clock_threshold) {
                portal_state = active;
                portal_pair.GenerateNewPair(game_matrix, FIELD_WIDTH,
                                                         FIELD_HEIGHT, fruit);
            }

            if (portal_state >= snake_in ) {
                portal_state++;
            }

            if (portal_state == snake.length) {
                portal_pair = PortalPair(-1, -1, -1, -1);
                portal_state = inactive;
                portal_clock.restart();
                portal_clock_threshold = 5 + rand() % 6;
            }

            if (snake.x == portal_pair.x1 && snake.y == portal_pair.y1) {
                snake.x = portal_pair.x2;
                snake.y = portal_pair.y2;
                portal_state = snake_in;
            } else if (snake.x == portal_pair.x2 &&
                       snake.y == portal_pair.y2) {
                snake.x = portal_pair.x1;
                snake.y = portal_pair.y1;
                portal_state = snake_in;
            }
            game_matrix[snake.x][snake.y] = snake.length;
        }

        for (int i = 0; i < FIELD_HEIGHT; ++i)
            for (int j = 0; j < FIELD_WIDTH; ++j) {
                if (game_matrix[i][j] > 0) {
                    sf::Sprite snake_tile;
                    snake_tile.setTexture(snake_tile_texture);
                    snake_tile.setPosition(playground.getPosition());
                    snake_tile.move(TEXTURE_HEIGHT * j, TEXTURE_WIDTH * i);
                    window.draw(snake_tile);
                }
            }

        sf::Sprite fruit_tile;
        fruit_tile.setTexture(fruit_tile_texture);
        fruit_tile.setPosition(playground.getPosition());
        fruit_tile.move(TEXTURE_HEIGHT * fruit.y, TEXTURE_WIDTH * fruit.x);
        window.draw(fruit_tile);

        if (portal_state != inactive) {
            sf::Sprite portal_tile1, portal_tile2;
            if (game_matrix[portal_pair.x1][portal_pair.y1] == 0) {
                portal_tile1.setTexture(portal_tile_texture);
            } else {
                portal_tile1.setTexture(portal_snake_tile_texture);
            }
            if (game_matrix[portal_pair.x2][portal_pair.y2] == 0) {
                portal_tile2.setTexture(portal_tile_texture);
            } else {
                portal_tile2.setTexture(portal_snake_tile_texture);
            }
            portal_tile1.setPosition(playground.getPosition());
            portal_tile2.setPosition(playground.getPosition());
            portal_tile1.move(TEXTURE_HEIGHT * portal_pair.y1,
                              TEXTURE_WIDTH  * portal_pair.x1);
            portal_tile2.move(TEXTURE_HEIGHT * portal_pair.y2,
                              TEXTURE_WIDTH  * portal_pair.x2);
            window.draw(portal_tile1);
            window.draw(portal_tile2);
        }


        window.draw(timer_text);
        window.draw(score_text);

        if (game_over) {
            int const GAME_OVER_WIDTH = 700;
            int const GAME_OVER_HEIGHT = 250;
            sf::RectangleShape game_over_box(sf::Vector2f(GAME_OVER_WIDTH,
                                                       GAME_OVER_HEIGHT));
            game_over_box.setFillColor(sf::Color(255, 255, 255));
            game_over_box.setPosition(playground.getPosition());
            game_over_box.move((PLAYGROUND_WIDTH  - GAME_OVER_WIDTH)  / 2,
                                   (PLAYGROUND_HEIGHT - GAME_OVER_HEIGHT) / 2);

            sf::Text game_over_text;
            game_over_text.setFont(snake_font);
            game_over_text.setCharacterSize(WINDOW_WIDTH / 20);
            game_over_text.setPosition(game_over_box.getPosition());
            game_over_text.move(GAME_OVER_WIDTH / 2, 0);
            game_over_text.setFillColor(sf::Color::Black);
            game_over_text.setString("Game Over!");
            game_over_text.move(-game_over_text.getLocalBounds().width / 2, 0);

            sf::Text restart_text;
            restart_text.setFont(snake_font);
            restart_text.setCharacterSize(WINDOW_WIDTH / 40);
            restart_text.setPosition(game_over_box.getPosition());
            restart_text.move(GAME_OVER_WIDTH / 2,
                              game_over_text.getLocalBounds().height * 2);
            restart_text.setFillColor(sf::Color::Black);
            restart_text.setString("Press 'Space' To Restart");
            restart_text.move(-restart_text.getLocalBounds().width / 2, 0);

            window.draw(game_over_box);
            window.draw(game_over_text);
            window.draw(restart_text);
        }

        window.display();
    }

    return EXIT_SUCCESS;
}
