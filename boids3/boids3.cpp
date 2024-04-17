// Hello :) just play around with the values and see what happens, I suggest to start with the boids being visible. Good luck!
// My laptop can hanlde ~200 of them before slowing down, so don't go overboard
// When manually resizing it does tend to go a bit haywire :P

#include <iostream>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <vector>

const float radius = 1.0f; //Circle radius 0.0 by default, hides the circles.
const auto circleColor = sf::Color::Cyan;

const float vMax = 1.0f;
const float vMin = 0.2f;
const float cohesionDistance = 500.0f;
const float separationDistance = 50.0f;
const float maxForce = .5f;
const float borderDistance = 100.0f; // Distance from window border to start avoiding

const int spawnCount = 3; // Amount of entities spawned per click (does not function likely due to similar starting values (randomness not being random))
const double pi = 3.14159265539f;
const float repulsionDistance = 70.0f; // Distance for repulsion behavior
const float repulsionForceFactor = 3.0f; // Strength of repulsion force


const int windowSizeX = 1200;
const int windowSizeY = 600;
const int gridSizeX = 20;
const int gridSizeY = gridSizeX * windowSizeY/windowSizeX;
const int pixelSize = 1; //arbitrary number, at 1 it's a grid higher it becomes smaller
//const int maxCircleCount = 3; //Pixel brightness is at maximum
//std::vector<std::vector<int> > grid(gridSizeX, std::vector<int>(gridSizeY, 0));

struct MovingCircle {
    sf::CircleShape circle;
    sf::Vector2f position;
    sf::Vector2f velocity;
};

sf::Vector2f limitVectorLength(const sf::Vector2f& vec, float maxLength)
{
    float length = std::sqrt(std::pow(vec.x, 2) + std::pow(vec.y, 2));
    if (length > maxLength && length > 0.0f)
    {
        return vec / length * maxLength;
    }

    return vec;
}

// Calculates the repulsion between the different entities (prevent them from clumping together, becoming one)
sf::Vector2f repulsion(const MovingCircle& currentCircle, const std::vector<MovingCircle>& circles) {
    sf::Vector2f repulsionForce;

    for (const auto& circle : circles) {
        float distance = std::sqrt(std::pow(circle.position.x - currentCircle.position.x, 2) +
            std::pow(circle.position.y - currentCircle.position.y, 2));

        if (&circle != &currentCircle && distance < repulsionDistance) {
            sf::Vector2f diff = currentCircle.position - circle.position;
            diff /= distance; 
            repulsionForce += diff * repulsionForceFactor;
        }
    }

    repulsionForce = limitVectorLength(repulsionForce, maxForce);

    return repulsionForce;
}

// Makes it so that the entities have a tendency to align their direction with the direction of their 'neighbours'
sf::Vector2f alignment(const MovingCircle& currentCircle, const std::vector<MovingCircle>& circles) {
    sf::Vector2f avgVelocity;
    int count = 0;

    for (const auto& circle : circles) {
        float distance = std::sqrt(std::pow(circle.position.x - currentCircle.position.x, 2) +
            std::pow(circle.position.y - currentCircle.position.y, 2));

        if (&circle != &currentCircle && distance < cohesionDistance) {
            avgVelocity += circle.velocity;
            count++;
        }
    }

    if (count > 0) {
        avgVelocity /= static_cast<float>(count);
        avgVelocity = limitVectorLength(avgVelocity, vMax);
        sf::Vector2f alignmentForce = avgVelocity - currentCircle.velocity;
        alignmentForce = limitVectorLength(alignmentForce, maxForce);
        return alignmentForce;
    }

    return sf::Vector2f(1.f, 1.f); // No alignment force (basically just prime it)
}

// Gives them a tendency to go towards their neighbours
sf::Vector2f cohesion(const MovingCircle& currentCircle, const std::vector<MovingCircle>& circles)
{
    sf::Vector2f steeringForce;
    int count = 0;

    for (const auto& circle : circles) 
    {
        float distance = std::sqrt(std::pow(circle.position.x - currentCircle.position.x, 2) + std::pow(circle.position.y - currentCircle.position.y, 2));

        if (&circle != &currentCircle && distance < cohesionDistance)
        {
            steeringForce += circle.position;
            count++;
        }
    }
    if (count > 0) {
        steeringForce /= static_cast<float>(count);
        steeringForce -= currentCircle.position;
        steeringForce = limitVectorLength(steeringForce, maxForce);
    }

    return steeringForce;
}

// you don't want them to fly on the screen :)
sf::Vector2f borderAvoidance(const MovingCircle& currentCircle, const sf::Vector2f& windowSize) {
    sf::Vector2f steeringForce;

    if (currentCircle.position.x < borderDistance) {
        steeringForce.x = borderDistance - currentCircle.position.x; // Move right
    }
    else if (currentCircle.position.x > windowSize.x - borderDistance) {
        steeringForce.x = windowSize.x - borderDistance - currentCircle.position.x; // Move left
    }

    if (currentCircle.position.y < borderDistance) {
        steeringForce.y = borderDistance - currentCircle.position.y; // Move down
    }
    else if (currentCircle.position.y > windowSize.y - borderDistance) {
        steeringForce.y = windowSize.y - borderDistance - currentCircle.position.y; // Move up
    }

    return steeringForce;
}

// 
sf::Vector2f separation(const MovingCircle& currentCircle, const std::vector<MovingCircle>& circles, const sf::Vector2f& windowSize)
{
    sf::Vector2f steeringForce;

    for (const auto& circle : circles) {
        float distance = std::sqrt(std::pow(circle.position.x - currentCircle.position.x, 2) +
            std::pow(circle.position.y - currentCircle.position.y, 2));

        if (&circle != &currentCircle && distance < separationDistance) {
            sf::Vector2f diff = currentCircle.position - circle.position;
            diff /= distance; // Normalize
            steeringForce += diff;
        }
    }
    
    sf::Vector2f borderForce = borderAvoidance(currentCircle, windowSize);
    steeringForce += borderForce;

    steeringForce = limitVectorLength(steeringForce, maxForce);

    return steeringForce;
}

// Counts the amount of boids active
void updateActiveBoidsText(sf::Text& text, const std::vector<MovingCircle>& circles) {
    int activeBoids = circles.size();
    text.setString("Active Boids: " + std::to_string(activeBoids));
}


int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(1200, 600), "Boids FTW", sf::Style::Close|sf::Style::Titlebar|sf::Style::Resize);
    window.setFramerateLimit(60);
    //boid.setFillColor(sf::Color::Green);
    //boid.setOrigin(20.0f, 20.0f);

    std::vector<MovingCircle> circles;
    bool circleSpawned = false; // Has a circle spawned?

    float grid[gridSizeX][gridSizeY];

    // active boids counter
    sf::Font font;
    if (!font.loadFromFile("COMICSANS.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }
    sf::Text activeBoidsText;
    activeBoidsText.setFont(font);
    activeBoidsText.setCharacterSize(24);
    activeBoidsText.setFillColor(sf::Color::White);
    activeBoidsText.setPosition(10.f, 10.f); // Where the top left corner of the text is placed

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && !circleSpawned)
            {
                for (int i = 0; i < spawnCount; ++i) 
                {
                    MovingCircle movingCircle;
                    movingCircle.circle.setRadius(radius);
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x + i, event.mouseButton.y + i)); // Add i to prevent clumping at spawn
                    movingCircle.circle.setPosition(mousePos);
                    movingCircle.circle.setFillColor(circleColor);

                    float speed = vMin + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (vMax - vMin))); // Rand speed
                    float angle = static_cast<float>((rand() + i )% 360); // Rand angle degrees
                    float radians = angle * pi / 180.f; // Radian conversion

                    movingCircle.velocity = sf::Vector2f(speed * std::cos(radians), speed * std::sin(radians));
                    movingCircle.position = mousePos;
                    circles.push_back(movingCircle);
                }

                circleSpawned = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                circleSpawned = false;
            }
        }

        // for the hidden grid, Ideally this would be outside the main loop
        for (int i = 0; i < gridSizeX; i++){
            for (int j = 0; j < gridSizeY; j++){
                grid[i][j] = 0;
            }
        }


        float proximity = 0.f;

        for (const auto& movingCircle : circles) // count boids on each grid intersection
        {
            int gridX = static_cast<int>(movingCircle.position.x / (window.getSize().x / gridSizeX));
            int gridY = static_cast<int>(movingCircle.position.y / (window.getSize().y / gridSizeY));

            proximity = (((window.getSize().x / gridSizeX * gridX) + (windowSizeX / gridSizeX / 2) - movingCircle.position.x) / (window.getSize().x / gridSizeX) + ((window.getSize().y / gridSizeY * gridY) + (windowSizeX / gridSizeX / 2)- movingCircle.position.y) / (window.getSize().y / gridSizeY)) *-1 / 2;
            
            if (gridX >= 0 && gridX < gridSizeX && gridY >= 0 && gridY < gridSizeY) {
                grid[gridX][gridY] += proximity;
            }
        }

        

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

        // this is the part that actually runs the code
        for (auto& movingCircle : circles) 
        {
            sf::Vector2f cohesionForce = cohesion(movingCircle, circles);
            sf::Vector2f separationForce = separation(movingCircle, circles, windowSize);

            sf::Vector2f alignmentForce = alignment(movingCircle, circles);
            movingCircle.velocity += alignmentForce;

            sf::Vector2f repulsionForce = repulsion(movingCircle, circles);
            movingCircle.velocity += repulsionForce;

            movingCircle.velocity += cohesionForce + separationForce;
            movingCircle.velocity = limitVectorLength(movingCircle.velocity, vMax);

            movingCircle.position += movingCircle.velocity;
            movingCircle.circle.setPosition(movingCircle.position);
            window.draw(movingCircle.circle);

            updateActiveBoidsText(activeBoidsText, circles);
            window.draw(activeBoidsText);
            
        }

        window.display();
    }

    return 0;
}

// It's a mess I know, next step is to make a header file for it and make sure that everything is rougly the same style.