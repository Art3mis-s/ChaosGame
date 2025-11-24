#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace sf;
using namespace std;

int main()
{
    // Window
    RenderWindow window(VideoMode(1920, 1080), "Chaos Game");

    srand(time(NULL));

    vector<Vector2f> vertices; // store the 3 vertices
    vector<Vector2f> points;   // fourth and random clicks
    // Loading the font
    Font font;
    font.loadFromFile("Times.ttc");

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            // close window
            if (event.type == Event::Closed)
                window.close();

            // mouse click
            if (event.type == Event::MouseButtonPressed &&
                event.mouseButton.button == Mouse::Left)
            {
                float mx = event.mouseButton.x;
                float my = event.mouseButton.y;

                // First 3 clicks → collect vertices
                if (vertices.size() < 3)
                {
                    vertices.push_back(Vector2f(mx, my));
                }
                // 4th click → starting point
                else if (points.size() == 0)
                {
                    points.push_back(Vector2f(mx, my));
                }
            }
        }

        // Escape closes window
        if (Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();

        /*
        Update - Chaos Game Logic
        */
        if (points.size() > 0)
        {
            // pick random vertex index 0,1,2
            int r = rand() % 3;

            // last generated point
            Vector2f last = points.back();

            // chosen vertex
            Vector2f target = vertices[r];

            // midpoint formula:
            // newX = (lastX + vertexX) / 2
            // newY = (lastY + vertexY) / 2
            Vector2f next(
                (last.x + target.x) / 2.f,
                (last.y + target.y) / 2.f
            );

            points.push_back(next);
        }

        /*
        Drawing
        */
        window.clear(Color::Black);

        // Draw vertices as blue squares
        for (auto &v : vertices)
        {
            RectangleShape r(Vector2f(10, 10));
            r.setFillColor(Color::Blue);
            r.setPosition(v);
            window.draw(r);
        }

        // Draw all chaos game points (white pixels)
        for (auto &p : points)
        {
            RectangleShape pixel(Vector2f(2, 2));
            pixel.setFillColor(Color::White);
            pixel.setPosition(p);
            window.draw(pixel);
        }

        window.display();
    }

    return 0;
}
