#include <iostream>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>

const float radius = 1.0f; //Circle radius 0.0 by default, hides the circles.
const sf::Color circleColor = sf::Color::Cyan;

const int gridSizeX = 20;
const int gridSizeY = 10;
const int pixelSize = 1; //arbitrary number, at 1 it's a grid higher it becomes smaller


int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 600), "Boids FTW", sf::Style::Close|sf::Style::Titlebar|sf::Style::None);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("COMICSANS.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }
    sf::Text proximityText;
    proximityText.setFont(font);
    proximityText.setCharacterSize(24);
    proximityText.setFillColor(sf::Color::White);
    proximityText.setPosition(10.f, 10.f); // Where the top left corner of the text is placed

    float grid[gridSizeX][gridSizeY];

    sf::CircleShape movingCircle;
    movingCircle.setRadius(radius);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)); // Add i to prevent clumping at spawn
                movingCircle.setPosition(mousePos);
                movingCircle.setFillColor(circleColor);
            }
        }

        float proximity = 0.f;

        int gridX = static_cast<int>(movingCircle.getPosition().x / (window.getSize().x / gridSizeX));
        int gridY = static_cast<int>(movingCircle.getPosition().y / (window.getSize().y / gridSizeY));
        

        proximity = (((window.getSize().x / gridSizeX * gridX) - movingCircle.getPosition().x) / (window.getSize().x / gridSizeX) + ((window.getSize().y / gridSizeY * gridY) - movingCircle.getPosition().y) / (window.getSize().y / gridSizeY)) *-1 / 2;
        //proximity = ((window.getSize().x / gridSizeX * gridX - movingCircle.getPosition().x) / (window.getSize().x / gridSizeX) + (window.getSize().y / gridSizeY * gridY - movingCircle.getPosition().y) / (window.getSize().y / gridSizeY)) *-1 / 2;
        if (gridX >= 0 && gridX < gridSizeX && gridY >= 0 && gridY < gridSizeY) {
            grid[gridX][gridY] = 1 - proximity;
            //std::cout<<std::to_string(gridX) + " x " + std::to_string(gridY) + " x " + std::to_string(proximity) + "\n";
        }

        //float transparency = -proximity/window.getSize().x/gridSizeX;
        proximityText.setString(std::to_string(movingCircle.getPosition().x) + " x " +std::to_string(grid[gridX][gridY]));

        // Refreshes the window
        window.clear(sf::Color::Black);

        // This drawes the visible grid squares, Should take it out of the main loop 
        sf::Vector2f windowSize(window.getSize().x, window.getSize().y);

        for (int x = 0; x < gridSizeX; ++x) {
            for (int y = 0; y < gridSizeY; ++y) {
                float transparency = grid[x][y];// Transparency based on the maxCircleCount (Amount of 'boids' is a square)
                transparency = std::max(0.0f, transparency); // Ensure transparency is between 0 and 1
                sf::RectangleShape square(sf::Vector2f(windowSize.x / gridSizeX / pixelSize , windowSize.y / gridSizeY / pixelSize));
                //sf::CircleShape square(windowSize.x / gridSizeX / pixelSize); // Pixel size
                square.setPosition(x * (windowSize.x / gridSizeX), y * (windowSize.y / gridSizeY)); // Places the pixels in the grid
                //square.setOrigin(-windowSize.x / gridSizeX / 4, -windowSize.y / gridSizeY / 4); // Places the 'pixels' in the middle of the grid - bullshit, we are detecting at the intersections, not in the center
                square.setFillColor(sf::Color(255, 255, 255, (static_cast<sf::Uint8>(transparency*255))));
                square.setOutlineColor(sf::Color(100,0,100));
                square.setOutlineThickness(1);

                sf::Text proximityText;
                proximityText.setString(std::to_string(transparency));
                proximityText.setPosition(x * (windowSize.x / gridSizeX), y * (windowSize.y / gridSizeY));
                proximityText.setFont(font);
                proximityText.setCharacterSize(10);
                proximityText.setFillColor(sf::Color::White);


                window.draw(proximityText);
                window.draw(square);
            }
        }

        window.draw(movingCircle); 
        window.draw(proximityText);
        window.display();
    
    }

    return 0;

}