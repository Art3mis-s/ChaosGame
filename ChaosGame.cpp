#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <sstream>

using namespace sf;
using namespace std;

int main()
{
    RenderWindow window(VideoMode(1920, 1080), "Chaos Game");
    srand(time(NULL));

    vector<Vector2f> vertices;
    vector<Vector2f> points;
    // Parallel color arrays
    vector<sf::Color> vertexColors;
    // track which vertex was chosen to create each point (-1 for starting point)
    vector<int> pointOwners;

    // Load font for on-screen text
    sf::Font font;
    bool fontLoaded = font.loadFromFile("Times.ttc");
    if (!fontLoaded) {
        std::cerr << "Warning: failed to load Times.ttc. On-screen text will not appear.\n";
    }
    sf::Text infoText;
    if (fontLoaded) infoText.setFont(font);
    infoText.setCharacterSize(18);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(10.f, 10.f);

    // EXTRA CREDIT SHAPE SELECTION
    size_t requiredVertices = 3; // default

    cout << "Choose a shape:\n";
    cout << "1. Triangle (3 vertices)\n";
    cout << "2. Square   (4 vertices)\n";
    cout << "3. Pentagon (5 vertices)\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    if (choice == 2) {
        requiredVertices = 4;
    } else if (choice == 3) {
        requiredVertices = 5;
    } else {
        requiredVertices = 3;
    }

    cout << "You must click " << requiredVertices
         << " times to set the vertices.\n";
    cout << "Then click once more to set the starting point.\n";

    int lastChosen = -1; // used to prevent immediate repetitions
    const int pointsPerFrame = 2000; // generate many points per frame for fast rendering
    bool running = true; // toggle generation with Space
    bool noRepeat = false; // when true, avoid selecting same vertex twice in a row
    int colorMode = 1; // 0 = white, 1 = per-vertex, 2 = iteration gradient (default to per-vertex)

    auto hsvToRgb = [](float h, float s, float v) {
        // h in [0,360), s,v in [0,1]
        float C = v * s;
        float X = C * (1 - std::fabs(std::fmod(h / 60.0f, 2) - 1));
        float m = v - C;
        float r = 0, g = 0, b = 0;
        if (h < 60) { r = C; g = X; b = 0; }
        else if (h < 120) { r = X; g = C; b = 0; }
        else if (h < 180) { r = 0; g = C; b = X; }
        else if (h < 240) { r = 0; g = X; b = C; }
        else if (h < 300) { r = X; g = 0; b = C; }
        else { r = C; g = 0; b = X; }
        sf::Uint8 R = static_cast<sf::Uint8>((r + m) * 255);
        sf::Uint8 G = static_cast<sf::Uint8>((g + m) * 255);
        sf::Uint8 B = static_cast<sf::Uint8>((b + m) * 255);
        return sf::Color(R, G, B);
    };

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
                    // assign a random color to this vertex
                    sf::Color vc = sf::Color(rand()%256, rand()%256, rand()%256);
                    vertexColors.push_back(vc);
                }
                // Next click → starting point
                else if (points.size() == 0)
                {
                    points.push_back(Vector2f(mx, my));
                    // starting point owner
                    pointOwners.push_back(-1);
                }
            }
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Space) {
                    running = !running; // pause / resume
                }
                if (event.key.code == Keyboard::Escape) {
                    window.close();
                }
                if (event.key.code == Keyboard::R) {
                    noRepeat = !noRepeat; // toggle no-repeat rule
                }
                if (event.key.code == Keyboard::C) {
                    colorMode = (colorMode + 1) % 3; // cycle color mode
                }
            }
        }


        // Chaos Game Logic — generate many points per frame for quicker visual
        if (running && points.size() > 0 && !vertices.empty())
        {
            for (int i = 0; i < pointsPerFrame; ++i) {
                int r = rand() % static_cast<int>(requiredVertices);

                // avoid immediate repetition of the same vertex (optional)
                int attempts = 0;
                if (noRepeat) {
                    while (r == lastChosen && attempts < 10) {
                        r = rand() % static_cast<int>(requiredVertices);
                        ++attempts;
                    }
                }
                lastChosen = r;

                Vector2f last = points.back();
                Vector2f target = vertices[r];

                Vector2f next(
                    (last.x + target.x) / 2.f,
                    (last.y + target.y) / 2.f
                );

                points.push_back(next);
                pointOwners.push_back(r);
            }
        }

        // Drawing
        window.clear(Color::Black);

        // Draw vertices (centered markers)
        for (size_t vi = 0; vi < vertices.size(); ++vi)
        {
            auto &v = vertices[vi];
            RectangleShape r(Vector2f(10, 10));
            sf::Color vc = (vi < vertexColors.size() ? vertexColors[vi] : Color::Blue);
            r.setFillColor(vc);
            r.setOrigin(5.f, 5.f);
            r.setPosition(v);
            window.draw(r);
        }

        // Draw points (small centered pixels) — color computed from owner or iteration
        for (size_t i = 0; i < points.size(); ++i)
        {
            auto &p = points[i];
            RectangleShape pixel(Vector2f(2, 2));
            sf::Color pc = Color::White;
            int owner = (i < pointOwners.size() ? pointOwners[i] : -1);
            if (colorMode == 1 && owner >= 0 && owner < (int)vertexColors.size()) {
                pc = vertexColors[owner];
            } else if (colorMode == 2) {
                float hue = std::fmod(static_cast<float>(i) * 0.5f, 360.0f);
                pc = hsvToRgb(hue, 1.0f, 1.0f);
            }
            pixel.setFillColor(pc);
            pixel.setOrigin(1.f, 1.f);
            pixel.setPosition(p);
            window.draw(pixel);
        }

        // Highlight starting point (first point) if present
        if (!points.empty()) {
            CircleShape startMarker(4.f);
            startMarker.setFillColor(Color::Red);
            startMarker.setOrigin(4.f, 4.f);
            startMarker.setPosition(points.front());
            window.draw(startMarker);
        }

        // Update and draw on-screen info panel (top-left)
        if (fontLoaded) {
            std::ostringstream oss;
            std::string shapeName = "Triangle";
            if (requiredVertices == 4) shapeName = "Square";
            else if (requiredVertices == 5) shapeName = "Pentagon";

            oss << "Shape: " << shapeName << " (" << requiredVertices << ")\n";
            oss << "Vertices: " << vertices.size() << " / " << requiredVertices << "\n";
            oss << "Starting point: " << (points.size() > 0 ? "set" : "not set") << "\n";
            std::string colorModeName = "White";
            if (colorMode == 1) colorModeName = "Per-vertex";
            else if (colorMode == 2) colorModeName = "Iteration Gradient";

            oss << "Left-click to add points. Space: pause/resume. C: change color (" << colorModeName << ") Esc: quit.";

            infoText.setString(oss.str());
            window.draw(infoText);
        }

        window.display();
    }

    return 0;
}
