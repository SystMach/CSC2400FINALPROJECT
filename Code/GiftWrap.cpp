/*****************************************************************************
*               Jarvis's march (Gift Wrapping) Convex Hull code              *
*****************************************************************************/
// Original code can be found here >> https://www.sanfoundry.com/cpp-program-implement-jarvis-march-find-convex-hull/
// int Main () was only major change, struct Point was changed from int to double
// Added a file write to the main output as well as counter to void Convex Hull to save results
// This code is being used for research purposes only. Major changes to the code include
// being able to read the coordinates from a file and change those cordinates into an array
// of the size needed to complete the code. Original comments made on unchanged
// code are untouched. Added comments are to the side or in boxes.
// To run code open Cmd in folder, type g++ GiftWrap.cpp, run executable.
// giftwrap_fixed.cpp
// 
// Original codes notes are below 
// A C++ program to find convex hull of a set of points
// Refer http://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
// for explanation of orientation()

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

int op_counter = 0;

struct Point {
    double x, y;
};

// orientation using double, returns 0 collinear, 1 clockwise, 2 counterclockwise
int orientation(const Point &p, const Point &q, const Point &r) {
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    op_counter++;
    const double EPS = 1e-12;
    if (fabs(val) < EPS) return 0;
    return (val > 0) ? 1 : 2;
}

// Jarvis March (Gift Wrapping)
// points: vector of input points, n = points.size()
// outputs the hull indices in 'next' (size n, -1 if not on hull)
void convexHull(const vector<Point> &points, vector<int> &next, const string &in_filename) {
    int n = (int)points.size();
    next.assign(n, -1);

    if (n < 3) return;

    // Start timing the hull algorithm
    auto start = chrono::high_resolution_clock::now();

    // Find leftmost point index
    int l = 0;
    for (int i = 1; i < n; ++i) {
        op_counter++;
        if (points[i].x < points[l].x) l = i;
    }

    int p = l;
    do {
        int q = (p + 1) % n;
        for (int i = 0; i < n; ++i) {
            // choose the most counterclockwise point
            if (orientation(points[p], points[i], points[q]) == 2) {
                q = i;
            }
            // note: orientation increments op_counter
        }
        next[p] = q;
        p = q;
    } while (p != l);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    // prepare output file
    string outname = "Gift_Results_" + in_filename;
    ofstream outputFile(outname);
    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outname << "\n";
        return;
    }

    // compute hull size
    int hullSize = 0;
    for (int i = 0; i < n; ++i) if (next[i] != -1) ++hullSize;

    // Print results to console & file
    cout << hullSize << " point hull size.\n";
    cout << "The Algorithm made " << op_counter << " operations.\n";
    cout << "The size of the array was " << n << ".\n";
    cout << "Hull computation time: " << fixed << setprecision(6) << elapsed.count() << " seconds\n";
    cout << "The points in the convex hull are:\n";

    outputFile << hullSize << " point hull size.\n";
    outputFile << "The Algorithm made " << op_counter << " operations.\n";
    outputFile << "The size of the array was " << n << ".\n";
    outputFile << "Hull computation time: " << fixed << setprecision(6) << elapsed.count() << " seconds\n";
    outputFile << "The points in the convex hull are:\n";

    for (int i = 0; i < n; ++i) {
        if (next[i] != -1) {
            cout << "(" << points[i].x << ", " << points[i].y << ")\n";
            outputFile << "(" << points[i].x << ", " << points[i].y << ")\n";
        }
    }

    outputFile.close();
}

// read CSV-like file robustly: handles BOM, \r, spaces, multiple points per line
vector<Point> read_points_robust(const string &filename) {
    ifstream inputFile(filename, ios::binary);
    vector<Point> pts;
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open file: " << filename << "\n";
        return pts;
    }

    string line;
    bool firstLine = true;
    while (std::getline(inputFile, line)) {
        // Remove possible UTF-8 BOM on first line
        if (firstLine) {
            firstLine = false;
            if (line.size() >= 3 &&
                (unsigned char)line[0] == 0xEF &&
                (unsigned char)line[1] == 0xBB &&
                (unsigned char)line[2] == 0xBF) {
                line = line.substr(3);
            }
        }
        // Normalize: remove carriage returns
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());

        // If line contains multiple points without newlines (malformed), split by spaces or semicolons
        // But primary expectation: entries are x,y pairs possibly separated by whitespace
        // We'll parse tokens separated by whitespace, comma, or semicolon
        string token;
        std::istringstream ss(line);
        // We'll manually parse the line looking for numbers separated by comma
        // Approach: iterate, find substrings of the form number,number
        size_t pos = 0;
        while (pos < line.size()) {
            // skip whitespace
            while (pos < line.size() && isspace((unsigned char)line[pos])) ++pos;
            if (pos >= line.size()) break;

            // find next comma
            size_t comma_pos = line.find(',', pos);
            if (comma_pos == string::npos) break; // no more pairs on this line

            string sx = line.substr(pos, comma_pos - pos);
            // find end of y (next whitespace or end)
            size_t endpos = comma_pos + 1;
            while (endpos < line.size() && !isspace((unsigned char)line[endpos]) && line[endpos] != ',' && line[endpos] != ';') ++endpos;
            string sy = line.substr(comma_pos + 1, endpos - (comma_pos + 1));

            // trim spaces
            auto trim = [](string &s){
                // left
                while (!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin());
                // right
                while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
            };
            trim(sx); trim(sy);
            if (!sx.empty() && !sy.empty()) {
                try {
                    double x = stod(sx);
                    double y = stod(sy);
                    pts.push_back({x, y});
                } catch (...) {
                    cerr << "Warning: skipped unparsable token pair: '" << sx << "','" << sy << "'\n";
                }
            }
            pos = endpos;
            // skip separators
            while (pos < line.size() && (isspace((unsigned char)line[pos]) || line[pos]==';' || line[pos]==',')) ++pos;
        }
    }

    inputFile.close();
    return pts;
}

int main() {
    string infile;
    cout << "Please enter a file name, please include .txt in the file name.\n";
    getline(cin, infile);

    // read points robustly
    vector<Point> pts = read_points_robust(infile);
    if (pts.empty()) {
        cerr << "No points read from file. Exiting.\n";
        return 1;
    }

    // use vector<int> for next pointers
    vector<int> next;
    convexHull(pts, next, infile);

    return 0;
}