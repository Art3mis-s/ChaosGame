#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

using namespace sf;
using namespace std;

int main()
{
    RenderWindow window(VideoMode(1920, 1080), "Chaos Game");
    srand(time(NULL));

    vector<Vector2f> vertices;
    vector<Vector2f> points;

    // Load font for on-screen text
    sf::Font font;
    if (!font.loadFromFile("Times.ttc")) {
        std::cerr << "Warning: failed to load Times.ttc. On-screen text will not appear.\n";
    }
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(18);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(10.f, 10.f);

    // sEXTRA CREDIT SHAPE SELECTION
    size_t requiredVertices = 3; // default

    cout << "Choose a shape:\n";
    cout << "1. Triangle (3 vertices)\n";
    cout << "2. Square   (4 vertices)\n";
    cout << "3. Pentagon (5 vertices)\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    if (choice == 2)
        requiredVertices = 4;
    else if (choice == 3)
        requiredVertices = 5;
    else
        requiredVertices = 3;

    cout << "You must click " << requiredVertices
         << " times to set the vertices.\n";
    cout << "Then click once more to set the starting point.\n";

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed &&
                event.mouseButton.button == Mouse::Left)
            {
                float mx = event.mouseButton.x;
                float my = event.mouseButton.y;

                // First N clicks → vertices
                if (vertices.size() < requiredVertices)
                {
                    vertices.push_back(Vector2f(mx, my));
                }
                // Next click → starting point
                else if (points.size() == 0)
                {
                    points.push_back(Vector2f(mx, my));
                }
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();

        // Chaos Game Logic
        if (points.size() > 0 && !vertices.empty())
        {
            int r = rand() % static_cast<int>(requiredVertices);

            Vector2f last = points.back();
            Vector2f target = vertices[r];

            Vector2f next(
                (last.x + target.x) / 2.f,
                (last.y + target.y) / 2.f
            );

            points.push_back(next);
        }

        // Drawing
        window.clear(Color::Black);

        // Draw vertices
        for (auto &v : vertices)
        {
            RectangleShape r(Vector2f(10, 10));
            r.setFillColor(Color::Blue);
            r.setPosition(v);
            window.draw(r);
        }

        // Draw points
        for (auto &p : points)
        {
            RectangleShape pixel(Vector2f(2, 2));
            pixel.setFillColor(Color::White);
            pixel.setPosition(p);
            window.draw(pixel);
        }

        // Update and draw on-screen info panel (top-left)
        if (font.getInfo().family.size() > 0) {
            std::ostringstream oss;
            std::string shapeName = "Triangle";
            if (requiredVertices == 4) shapeName = "Square";
            else if (requiredVertices == 5) shapeName = "Pentagon";

            oss << "Shape: " << shapeName << " (" << requiredVertices << ")\n";
            oss << "Vertices: " << vertices.size() << " / " << requiredVertices << "\n";
            oss << "Starting point: " << (points.size() > 0 ? "set" : "not set") << "\n";
            oss << "Left-click to add points. Press Esc to quit.";

            infoText.setString(oss.str());
            window.draw(infoText);
        }

        window.display();
    }

    return 0;
}
