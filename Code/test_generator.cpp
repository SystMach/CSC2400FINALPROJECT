// To run code open Cmd in folder, type g++ test_generator.cpp, run executable.

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <random>
#include <string>
#include <iomanip>

struct Point {
    double x, y;
};

// -----------------------
// Random Generators
// -----------------------
std::mt19937 rng(std::random_device{}());

double random_uniform(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

double random_gaussian(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(rng);
}

// -----------------------
// Distribution Functions
// -----------------------
std::vector<Point> uniform_random(int n, double xmin=0, double xmax=500, double ymin=0, double ymax=500) {
    std::vector<Point> pts;
    for(int i=0; i<n; ++i)
        pts.push_back({random_uniform(xmin, xmax), random_uniform(ymin, ymax)});
    return pts;
}

std::vector<Point> gaussian_clusters(int n, int clusters=5, double spread=25, double size=500) {
    std::vector<Point> centers;
    for(int i=0; i<clusters; ++i)
        centers.push_back({random_uniform(0, size), random_uniform(0, size)});

    std::vector<Point> pts;
    for(int i=0; i<n; ++i) {
        int c_idx = static_cast<int>(random_uniform(0, clusters-0.0001));
        Point c = centers[c_idx];
        pts.push_back({random_gaussian(c.x, spread), random_gaussian(c.y, spread)});
    }
    return pts;
}

std::vector<Point> circle_points(int n, double radius=200, double cx=250, double cy=250, double noise=5) {
    std::vector<Point> pts;
    for(int i=0; i<n; ++i) {
        double angle = random_uniform(0, 2*M_PI);
        double r = radius + random_uniform(-noise, noise);
        pts.push_back({cx + r*std::cos(angle), cy + r*std::sin(angle)});
    }
    return pts;
}

std::vector<Point> grid_points(int n, double size=500) {
    int step = std::sqrt(n);
    std::vector<Point> pts;
    for(int i=0; i<step; ++i)
        for(int j=0; j<step; ++j) {
            pts.push_back({i*(size/step), j*(size/step)});
            if(pts.size() == n) return pts;
        }
    return pts;
}

std::vector<Point> square_boundary(int n, double size=500) {
    std::vector<Point> pts;
    int per = n/4;
    for(int i=0; i<per; ++i) pts.push_back({i*(size/per), 0});
    for(int i=0; i<per; ++i) pts.push_back({size, i*(size/per)});
    for(int i=0; i<per; ++i) pts.push_back({size - i*(size/per), size});
    for(int i=0; i<per; ++i) pts.push_back({0, size - i*(size/per)});
    return std::vector<Point>(pts.begin(), pts.begin() + n);
}

std::vector<Point> star_shape(int n, double cx=250, double cy=250, double inner=80, double outer=200, int points=5) {
    std::vector<Point> pts;
    for(int i=0; i<n; ++i) {
        double angle = (double)i / n * 2*M_PI;
        double r = ((i*points)%2 == 0) ? outer : inner;
        pts.push_back({cx + r*std::cos(angle), cy + r*std::sin(angle)});
    }
    return pts;
}

// -----------------------
// Master Function
// -----------------------
std::vector<Point> generate(int n, const std::string& mode) {
    if(mode == "uniform") return uniform_random(n);
    else if(mode == "gaussian") return gaussian_clusters(n);
    else if(mode == "circle") return circle_points(n);
    else if(mode == "grid") return grid_points(n);
    else if(mode == "square") return square_boundary(n);
    else if(mode == "star") return star_shape(n);
    else throw std::runtime_error("Invalid distribution mode");
}

// -----------------------
// Save Points to File (Linux-friendly)
// -----------------------
void write_points(const std::vector<Point>& pts, const std::string& filename) {
    std::ofstream out(filename, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }

    for (const auto& p : pts)
        out << std::fixed << std::setprecision(1) << p.x << "," << p.y << "\n"; // always \n

    out.close();
    std::cout << "\n✔ File created: " << filename << "\n";
    std::cout << "✔ Wrote " << pts.size() << " points.\n\n";
}

// -----------------------
// Main
// -----------------------
int main() {
    std::cout << "=== Point Generator ===\n";
    std::cout << "Choose distribution type:\n";
    std::cout << "1 - Uniform random\n2 - Gaussian clusters\n3 - Circle / ring\n4 - Grid\n5 - Square\n6 - Star\n";

    std::string choice;
    std::cout << "\nEnter choice (1-6): ";
    std::getline(std::cin, choice);

    std::string mode_map[7] = {"", "uniform","gaussian","circle","grid","square","star"};
    int choice_num = std::stoi(choice);
    if(choice_num < 1 || choice_num > 6) {
        std::cout << "Invalid choice. Exiting.\n";
        return 1;
    }
    std::string mode = mode_map[choice_num];

    int n;
    std::cout << "How many points do you want? ";
    std::cin >> n;

    std::string filename;
    std::cout << "Enter output filename (e.g., points.txt): ";
    std::cin >> filename;
    if(filename.size() < 4 || filename.substr(filename.size()-4) != ".txt") filename += ".txt";

    std::vector<Point> pts = generate(n, mode);
    write_points(pts, filename);

    return 0;
}