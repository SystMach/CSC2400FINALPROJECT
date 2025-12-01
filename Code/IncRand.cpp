// To run code open Cmd in folder, type g++ IncRand.cpp, run executable.

#include <algorithm>    // for sort, shuffle
#include <unordered_set> // for unordered_set
#include <random>       // for random_device, default_random_engine
#include <vector>       // for std::vector
#include <string>       // for std::string
#include <iostream>     // for std::cout, std::cin
#include <fstream>
#include <chrono>
using namespace std;

static const double EPS = 1e-9;
int op_counter = 0;

struct Point {
    double x, y;

    bool operator==(const Point &other) const {
        return fabs(x - other.x) < EPS && fabs(y - other.y) < EPS;
    }
};

struct PointHash {
    size_t operator()(const Point &p) const {
        // Scale doubles to integers to stabilize hashing
        long long xi = llround(p.x * 1e9);
        long long yi = llround(p.y * 1e9);
        return std::hash<long long>()(xi) ^ (std::hash<long long>()(yi) << 1);
    }
};

double cross(const Point &O, const Point &A, const Point &B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

bool lexLess(const Point &a, const Point &b) {
    if (fabs(a.x - b.x) > EPS) return a.x < b.x;
    return a.y < b.y - EPS;
}

// Andrew’s monotone chain convex hull (returns CCW hull without duplicate last point)
vector<Point> monotoneChain(vector<Point> pts) {
    if (pts.size() <= 1) return pts;
    sort(pts.begin(), pts.end(), lexLess);
    // Remove near-duplicates post-sort
    vector<Point> uniq;
    uniq.reserve(pts.size());
    for (auto &p : pts) {
        if (uniq.empty() || !(p == uniq.back())) uniq.push_back(p);
    }
    pts.swap(uniq);
    int n = (int)pts.size();
    if (n <= 1) return pts;

    vector<Point> lower, upper;
    for (int i = 0; i < n; ++i) {
        while (lower.size() >= 2 && cross(lower[lower.size()-2], lower.back(), pts[i]) <= EPS)
            lower.pop_back();
        lower.push_back(pts[i]);
    }
    for (int i = n - 1; i >= 0; --i) {
        while (upper.size() >= 2 && cross(upper[upper.size()-2], upper.back(), pts[i]) <= EPS)
            upper.pop_back();
        upper.push_back(pts[i]);
    }
    // Concatenate, removing duplicate endpoints
    lower.pop_back();
    upper.pop_back();
    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}

// Point-in-convex-polygon test for CCW hull; returns true if inside or on boundary
bool insideConvexHullCCW(const vector<Point> &hull, const Point &p) {
    int n = (int)hull.size();
    if (n < 3) return false;
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        if (cross(hull[i], hull[j], p) < -EPS) return false;
    }
    return true;
}

// Robust incremental hull: when p is outside, rebuild hull using monotone chain on hull+{p}
vector<Point> incrementalHull(vector<Point> pts) {
    // Remove duplicates using hash
    unordered_set<Point, PointHash> seen;
    vector<Point> uniquePts;
    uniquePts.reserve(pts.size());
    for (auto &p : pts) {
        if (seen.find(p) == seen.end()) {
            seen.insert(p);
            uniquePts.push_back(p);
        }
        op_counter++;
    }

    // Randomize order
    shuffle(uniquePts.begin(), uniquePts.end(), default_random_engine(random_device{}()));

    if (uniquePts.size() <= 2) {
        // Return unique points as-is (degenerate hull)
        return uniquePts;
    }

    // Initialize with 3 non-collinear points; if collinear, monotone chain will handle later
    vector<Point> init{uniquePts[0], uniquePts[1], uniquePts[2]};
    // Ensure CCW orientation for the initial triangle when non-collinear
    if (fabs(cross(init[0], init[1], init[2])) > EPS && cross(init[0], init[1], init[2]) < 0)
        swap(init[1], init[2]);

    vector<Point> hull = monotoneChain(init);

    for (size_t k = 3; k < uniquePts.size(); ++k) {
        Point p = uniquePts[k];
        // Skip if already on hull (approx)
        bool onHull = false;
        for (const auto &q : hull) {
            if (p == q) { onHull = true; break; }
            op_counter++;
        }
        if (onHull) continue;

        if (insideConvexHullCCW(hull, p)) continue;

        // Rebuild hull from current hull points plus p (guarantees correctness)
        vector<Point> expanded = hull;
        expanded.push_back(p);
        hull = monotoneChain(expanded);
    }
    return hull;
}

int main() {
    ios::sync_with_stdio(false);
    //cin.tie(nullptr);

    std::string filename;

    std::cout << "File Name: " << std::flush;
    std::cin >> filename;
    
    // variables for counting number of lines/ number of points
    std::string line;
    int n = 0;

    ifstream fin(filename);
    if (!fin) {
        cerr << "Failed to open coordinates.txt\n";
        return 1;
    }
    
    while (std::getline(fin, line)) {
        n++;
    }

    fin.clear(); // Clear EOF flag
    fin.seekg(0); // Move read pointer to the beginning

    vector<Point> pts;
    pts.reserve(n);
    for (int i = 0; i < n; ++i) {
        string line;
        fin >> line; // expects "x,y"
        size_t comma = line.find(',');
        if (comma == string::npos) {
            cerr << "Invalid line: " << line << "\n";
            continue;
        }
        Point p;
        p.x = stod(line.substr(0, comma));
        p.y = stod(line.substr(comma + 1));
        pts.push_back(p);
    }
    fin.close();

    auto start = chrono::high_resolution_clock::now();

    // Compute incremental hull with robust fallback
    vector<Point> hull = incrementalHull(pts);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    string file = "IncRandom_Results_" + filename;

    ofstream fout(file);
    if (!fout) {
        cerr << "Failed to open hull.txt for writing\n";
        return 1;
    }

    fout << hull.size() << " point hull size. \n" <<
        "The Algorithm made " << op_counter << " operations.\n" <<
        "The size of the array was " << n << ". \n" <<
        "Hull computation time: " << elapsed.count() << " seconds\n" <<
        "The points in the convex hull are: \n";
    for (auto &p : hull) {
        // Output in a stable order (sorted CCW by monotone chain output)
        fout << "(" << p.x << " " << p.y << ")\n";
    }
    fout.close();

    return 0;
}
