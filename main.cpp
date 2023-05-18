#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#define COL 30
#define ROWS 10

#define PI 3.141

struct Bat
{
    sf::RectangleShape bat;
    sf::Vector2f pos;
    float batSizeX { 100.f };
    float batSizeY { 30.f };
    float speed { 300.f };
    bool moveLeft { false }, moveRight { false };
};

struct Ball
{
    sf::CircleShape ball;
    float radius { 10.f };
    float speed { 60.f };
    float velX { 1.f }, velY { 1.f };
    float ballPosX, ballPosY;
};

struct Tile
{
    sf::RectangleShape tile[COL][ROWS];
    float tileSize { 20.f };
    float tileDis { 10.f };
    bool removed[COL][ROWS] { false };
};

// Create the bat and spawn it
void createBat(Bat* bat, sf::Window& window)
{
    sf::Vector2u winSize = window.getSize();
    bat->bat.setFillColor(sf::Color::Red);
    bat->bat.setSize(sf::Vector2f(bat->batSizeX, bat->batSizeY));
    bat->pos.x = winSize.x / 2;
    bat->pos.y = 830.f;
    bat->bat.setPosition(bat->pos.x, bat->pos.y);
}

// Move and update the bat
void updateBat(Bat* bat, float elapsedTime)
{
    if (!bat->moveLeft)
    {
        bat->pos.x += bat->speed * elapsedTime;
    }
    if (!bat->moveRight)
    {
        bat->pos.x -= bat->speed * elapsedTime;
    }
    bat->bat.setPosition(bat->pos);
}

// Draw the bat
void drawBat(Bat* bat, sf::RenderWindow& window)
{
    window.draw(bat->bat);
}

// Create the ball and spawn it
void createBall(Ball* ball, sf::Window& window)
{
    sf::Vector2u winSize = window.getSize();
    ball->ball.setFillColor(sf::Color::White);
    ball->ball.setRadius(ball->radius);
    ball->ballPosX = winSize.x / 2 - ball->radius;
    ball->ballPosY = (winSize.y / 2 - ball->radius) + 120;
    ball->ball.setPosition(ball->ballPosX, ball->ballPosY);
}

// Move and update the ball
void updateBall(Ball* ball, float elapsedTime)
{
    //ball->ballPosX += ball->velX * ball->speed * elapsedTime;
    //ball->ballPosY += ball->velY * ball->speed * elapsedTime;
    //ball->ball.setPosition(ball->ballPosX, ball->ballPosY);

    float dropBall { ((3 * PI) / 2) * ball->velY * ball->speed * elapsedTime };
    ball->ballPosX += ball->velX * ball->speed * elapsedTime;
    ball->ballPosY += dropBall;

    ball->ball.setPosition(ball->ballPosX, ball->ballPosY);
}

// Draw the ball
void drawBall(Ball* ball, sf::RenderWindow& window)
{
    window.draw(ball->ball);
}

// Create the tiles and spawn them
void createTiles(Tile* tile, sf::Window& window)
{
    sf::Vector2u winSize { window.getSize() };
    const float x { (winSize.x - ((COL * tile->tileSize) + ((COL - 1) * tile->tileDis))) / 2.f };
    const float y { (winSize.y - ((ROWS * tile->tileSize) + ((ROWS - 1) * tile->tileDis))) / 2.f - 250.f };
    for (std::size_t i { 0 }; i < COL; i++)
    {
        for (std::size_t j { 0 }; j < ROWS; j++)
        {
            tile->tile[i][j].setFillColor(sf::Color::Green);
            tile->tile[i][j].setSize(sf::Vector2f(tile->tileSize, tile->tileSize));
            tile->tile[i][j].setPosition(x + i * (tile->tileSize + tile->tileDis), y + j * (tile->tileSize + tile->tileDis));
        }
    }
}

// Draw the tiles
void drawTiles(Tile* tile, sf::RenderWindow& window)
{
    for (std::size_t i { 0 }; i < COL; i++)
    {
        for (std::size_t j { 0 }; j < ROWS; j++)
        {
            if (!tile->removed[i][j])
            {
                window.draw(tile->tile[i][j]);
            }
        }
    }
}

// All functions for Collisions
// Collide between Ball and Screen sides
bool isCollideSides(Ball* ball, sf::Window& window)
{
    sf::Vector2u winSize = window.getSize();
    // Left
    if (ball->ball.getPosition().x < 0)
    {
        std::cout << "Collide with Left side of screen" << '\n';
        ball->velX = -ball->velX;
    }
    // Right
    if (ball->ball.getPosition().x + ball->radius * 2 > winSize.x)
    {
        std::cout << "Collide with Right side of screen" << '\n';
        ball->velX = -ball->velX;
    }
    // Top
    if (ball->ball.getPosition().y < 0)
    {
        std::cout << "Collide with Top side of screen" << '\n';
        ball->velY = -ball->velY;
    }
    // Bottom
    if (ball->ball.getPosition().y + ball->radius * 2 > winSize.y)
    {
        std::cout << "Collide with Bottom side of screen" << '\n';
        ball->velY = -ball->velY;
    }
}

// Collide between Ball and Bat
bool isCollideBat(Ball* ball, Bat* bat)
{
    if (ball->ball.getGlobalBounds().intersects(bat->bat.getGlobalBounds()))
    {
        std::cout << "Ball collided with Bat" << '\n';
        ball->velY = -ball->velY;
    }
}

// Collide between Ball and Tiles
bool isCollideTiles(Ball* ball, Tile* tile)
{
    for (std::size_t i { 0 }; i < COL; i++)
    {
        for (std::size_t j { 0 }; j < ROWS; j++)
        {
            sf::FloatRect ballB { ball->ball.getGlobalBounds() };
            sf::FloatRect tileB { tile->tile[i][j].getGlobalBounds() };
            if (!tile->removed[i][j] && ball->ball.getGlobalBounds().intersects(tile->tile[i][j].getGlobalBounds()))
            {
                // Left side of Tile
                if (ballB.left > tileB.left
                    && ballB.left + ballB.width > tileB.left + tileB.width
                    && ballB.top < tileB.top + tileB.height
                    && ballB.top + tileB.height > tileB.top)
                {
                    tile->removed[i][j] = true;
                    ball->velX = -ball->velX;
                }

                // Right side of Tile
                if (ballB.left < tileB.left
                    && ballB.left + ballB.width < tileB.left + tileB.width
                    && ballB.top < tileB.top + tileB.height
                    && ballB.top + tileB.height > tileB.top)
                {
                    tile->removed[i][j] = true;
                    ball->velX = -ball->velX;
                }

                // Top side of Tile
                if (ballB.top > tileB.top
                    && ballB.top + ballB.height > tileB.top + tileB.height
                    && ballB.left < tileB.left + tileB.width
                    && ballB.left + tileB.width > tileB.left)
                {
                    tile->removed[i][j] = true;
                    ball->velY = -ball->velY;
                }

                // Bottom side of Tile
                if (ballB.top < tileB.top
                    && ballB.top + ballB.height < tileB.top + tileB.height
                    && ballB.left < tileB.left + tileB.width
                    && ballB.left + tileB.width > tileB.left)
                {
                    tile->removed[i][j] = true;
                    ball->velY = -ball->velY;
                }
            }
        }
    }
}

int main()
{
    Bat bat;
    Ball ball;
    Tile tile;

    using kb = sf::Keyboard;

    std::string title { "SFML  FPS: " };
    const unsigned int WIDTH { 1200 };
    const unsigned int HEIGHT { 900 };
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), title);
    window.setVerticalSyncEnabled(true);

    // Spawn the bat
    createBat(&bat, window);

    // Spawn the ball
    createBall(&ball, window);

    // Spawn the tiles
    createTiles(&tile, window);

    sf::Clock clock;

    double starting { 0.0001 };
    while (window.isOpen())
    {
        double ending { clock.getElapsedTime().asSeconds() };
        double dt { ending - starting };
        starting = ending;
        std::string FPS { std::to_string(int(1 / dt)) };
        window.setTitle(title + FPS);

        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close the program if press X button
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            // Move the bat
            // if press A & D keys
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == kb::A)
                {
                    bat.moveLeft = true;
                }
                else if (event.key.code == kb::D)
                {
                    bat.moveRight = true;
                }
            }
            // Stop the bat
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == kb::A)
                {
                    bat.moveLeft = false;
                }
                else if (event.key.code == kb::D)
                {
                    bat.moveRight = false;
                }
            }
        }

        // Input
        // Close the program if press Escape key
        if (kb::isKeyPressed(kb::Escape))
        {
            window.close();
        }

        // Update
        sf::Time deltaTime = clock.restart();
        float dtAsSeconds = deltaTime.asSeconds();
        updateBat(&bat, dtAsSeconds); // update the bat
        updateBall(&ball, dtAsSeconds); // update the ball
        isCollideSides(&ball, window); // check collision between ball and screen sides
        isCollideBat(&ball, &bat); // check collision between ball and bat
        isCollideTiles(&ball, &tile); // check collision between ball and tiles

        // Draw
        window.clear();
        drawBat(&bat, window); // draw the bat
        drawBall(&ball, window); // draw the ball
        drawTiles(&tile, window); // draw the tiles
        window.display();
    }

    return 0;
}
