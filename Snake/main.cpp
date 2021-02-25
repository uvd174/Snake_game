#include <SFML/Graphics.hpp>
#include <string>
#include <random>

#include "SnakeClasses.hpp"


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

    Game game(FIELD_HEIGHT, FIELD_WIDTH);
    game.NewGame();

    Snake snake = Snake(7, 3);
    Snake::Direction user_direction = snake.direction;

    Fruit fruit = Fruit(7, 27);

    PortalPair portal_pair = PortalPair(7, 10, 7, 20);
    int portal_state = PortalPair::active;


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
                            user_direction = Snake::up;
                            break;
                        case sf::Keyboard::A:
                            user_direction = Snake::left;
                            break;
                        case sf::Keyboard::S:
                            user_direction = Snake::down;
                            break;
                        case sf::Keyboard::D:
                            user_direction = Snake::right;
                            break;
                        case sf::Keyboard::Up:
                            user_direction = Snake::up;
                            break;
                        case sf::Keyboard::Left:
                            user_direction = Snake::left;
                            break;
                        case sf::Keyboard::Down:
                            user_direction = Snake::down;
                            break;
                        case sf::Keyboard::Right:
                            user_direction = Snake::right;
                            break;
                        case sf::Keyboard::Space:
                            if (game.is_over()) {
                                game.NewGame();

                                snake = Snake(7, 3);
                                snake_clock.restart();
                                user_direction = snake.direction;

                                fruit = Fruit(7, 27);

                                portal_pair = PortalPair(7, 10, 7, 20);
                                portal_state = PortalPair::active;

                                timer_clock.restart();
                                timer_text.setString("00:00");

                                score_text.setString("Score: 0");
                            }
                            break;
                        case sf::Keyboard::Escape:
                            window.close();
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
        if (!game.is_over() &&
            timer_clock.getElapsedTime().asMilliseconds() > 1000) {

            timer_clock.restart();
            UpdateTimer(timer_text, game.sec_passed());
        }

        if (!game.is_over() &&
            snake_clock.getElapsedTime().asMilliseconds() > 100) {

            snake_clock.restart();

            snake.UpdateDirection(user_direction);
            snake.Move(game.field);

            if (game.field.at(snake.position()) > 0) {
                game.GameOver();
            }

            if (snake.position() == fruit.position()) {
                snake.Grow();
                score_text.setString("Score: " +
                                     std::to_string(snake.get_length() - 3));
                game.field.set(snake.position(), snake.get_length());
                fruit.GenerateNewOne(game.field, portal_pair);
            } else {
                for (int i = 0; i < game.field.get_height(); ++i)
                    for (int j = 0; j < game.field.get_width(); ++j) {
                        game.field.set(i, j, std::max(0, game.field.at(i, j) - 1));
                    }
                game.field.set(snake.position(), snake.get_length());
            }

            if (portal_state == PortalPair::inactive &&
                portal_clock.getElapsedTime()
                            .asSeconds() > portal_clock_threshold) {
                portal_state = PortalPair::active;
                portal_pair.GenerateNewPair(game.field, fruit);
            }

            if (portal_state >= PortalPair::snake_in ) {
                portal_state++;
            }

            if (portal_state == snake.get_length()) {
                portal_pair = PortalPair(-1, -1, -1, -1);
                portal_state = PortalPair::inactive;
                portal_clock.restart();
                portal_clock_threshold = 5 + std::rand() % 6;
            }

            if (snake.position() == portal_pair.first_position()) {
                snake.Teleport(portal_pair.second_position());
                portal_state = PortalPair::snake_in;

            } else if (snake.position() == portal_pair.second_position()) {
                snake.Teleport(portal_pair.first_position());
                portal_state = PortalPair::snake_in;
            }
        }

        for (int i = 0; i < game.field.get_height(); ++i)
            for (int j = 0; j < game.field.get_width(); ++j) {
                if (game.field.at(i, j) > 0) {
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
        fruit_tile.move(TEXTURE_HEIGHT * fruit.position().y,
                        TEXTURE_WIDTH * fruit.position().x);
        window.draw(fruit_tile);

        if (portal_state != PortalPair::inactive) {
            sf::Sprite portal_tile1, portal_tile2;
            if (game.field.at(portal_pair.first_position()) == 0) {
                portal_tile1.setTexture(portal_tile_texture);
            } else {
                portal_tile1.setTexture(portal_snake_tile_texture);
            }
            if (game.field.at(portal_pair.second_position()) == 0) {
                portal_tile2.setTexture(portal_tile_texture);
            } else {
                portal_tile2.setTexture(portal_snake_tile_texture);
            }
            portal_tile1.setPosition(playground.getPosition());
            portal_tile2.setPosition(playground.getPosition());
            portal_tile1.move(TEXTURE_HEIGHT * portal_pair.first_position().y,
                              TEXTURE_WIDTH  * portal_pair.first_position().x);
            portal_tile2.move(TEXTURE_HEIGHT * portal_pair.second_position().y,
                              TEXTURE_WIDTH  * portal_pair.second_position().x);
            window.draw(portal_tile1);
            window.draw(portal_tile2);
        }


        window.draw(timer_text);
        window.draw(score_text);

        if (game.is_over()) {
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
