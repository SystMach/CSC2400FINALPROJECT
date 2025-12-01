
// To run code open Cmd in folder, type g++ brute_hull.cpp, run executable.

//#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

struct Point {
    double x, y;
};

// ----------------------
// Load points from CSV
// ----------------------
std::vector<Point> load_points(const std::string& filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        double x, y;
        char comma;
        ss >> x >> comma >> y;
        points.push_back({x, y});
    }
    return points;
}

// ----------------------
// Brute-force convex hull
// ----------------------
double cross(const Point& p1, const Point& p2, const Point& p3) {
    return (p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x);
}

std::vector<std::pair<Point, Point>> brute_force_convex_hull(const std::vector<Point>& points) {
    std::vector<std::pair<Point, Point>> edges;
    int n = points.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            bool left = false, right = false;
            for (int k = 0; k < n; ++k) {
                if (k == i || k == j) continue;
                double cp = cross(points[i], points[j], points[k]);
                if (cp > 0) left = true;
                if (cp < 0) right = true;
                if (left && right) break;
            }
            if (!(left && right)) edges.push_back({points[i], points[j]});
        }
    }
    return edges;
}

/*/ ----------------------
// Optional visualization
// ----------------------
void visualize(const std::vector<Point>& points, const std::vector<std::pair<Point, Point>>& edges, const std::string& filename) {
    sf::RenderTexture texture;
    const int size = 600;
    texture.create(size, size);
    texture.clear(sf::Color::White);

    // Find max X/Y for scaling
    double maxX = 0, maxY = 0;
    for (auto& p : points) {
        if (p.x > maxX) maxX = p.x;
        if (p.y > maxY) maxY = p.y;
    }
    double scaleX = size / maxX;
    double scaleY = size / maxY;

    // Draw points
    for (auto& p : points) {
        sf::CircleShape circle(2);
        circle.setFillColor(sf::Color::Blue);
        circle.setPosition(p.x * scaleX, size - p.y * scaleY);
        texture.draw(circle);
    }

    // Draw edges
    for (auto& e : edges) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(e.first.x * scaleX, size - e.first.y * scaleY), sf::Color::Red),
            sf::Vertex(sf::Vector2f(e.second.x * scaleX, size - e.second.y * scaleY), sf::Color::Red)
        };
        texture.draw(line, 2, sf::Lines);
    }

    texture.display();
    sf::Texture result = texture.getTexture();
    sf::Image screenshot = result.copyToImage();
    screenshot.saveToFile(filename + "_hull.png");
    std::cout << "Saved PNG: " << filename + "_hull.png\n";
}*/

// ----------------------
// Main
// ----------------------
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./hull <points_file> [visualize=1]\n";
        return 1;
    }

    std::string filename = argv[1];
    bool visualize_flag = (argc >= 3 && std::string(argv[2]) == "1");

    auto points = load_points(filename);

    if (points.empty()) {
        std::cout << "No points loaded.\n";
        return 1;
    }

    // --------- TIMING ---------
    auto start = std::chrono::high_resolution_clock::now();
    auto edges = brute_force_convex_hull(points);
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    std::cout << "Hull computation time: " << elapsed << " seconds\n";

    /*/ Optional visualization (AFTER RUNTIME ALREADY MEASURED FOR ALGORITHM)
    if (visualize_flag) {
        visualize(points, edges, filename);
    }*/

    return 0;
}