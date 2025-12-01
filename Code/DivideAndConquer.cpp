/*****************************************************************************
*                       Divide and Conquer Convex Hull code                  *
*****************************************************************************/
// Original Code can be found here >> https://www.geeksforgeeks.org/cpp/convex-hull-algorithm-in-cpp/#
// int Main () was only major change, Added aditional code to read from and produce
// a file with outputs and operation count.
// This code is being used for research purposes only. Any orignal comments
// made on unchanged code are untouched. Added comments are to the side or in boxes
// To run code open Cmd in folder, type g++ DivideAndConquer.cpp, run executable.
// 
// Orininal code notes are below:
// C++ program to to find convex
// hull of a given set of points using divide and conquer.

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
using namespace std;

struct Point {
    double x, y;
};

// Global operation counter
int op_counter = 0;

// Orientation test: 0=collinear, 1=clockwise, -1=counterclockwise
int orientation(const Point &a, const Point &b, const Point &c) {
    double val = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
    op_counter++;
    if (fabs(val) < 1e-9) return 0;
    return (val > 0 ? 1 : -1);
}

// Brute-force hull for <=5 points
vector<Point> bruteHull(vector<Point> &pts) {
    int n = pts.size();
    if (n <= 1) return pts;
    vector<Point> hull;

    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            int pos = 0, neg = 0;
            for (int k = 0; k < n; k++) {
                int o = orientation(pts[i], pts[j], pts[k]);
                if (o > 0) pos++;
                else if (o < 0) neg++;
            }
            if (pos == 0 || neg == 0) {
                hull.push_back(pts[i]);
                hull.push_back(pts[j]);
            }
        }
    }

    // Remove duplicates
    sort(hull.begin(), hull.end(), [](Point a, Point b){ return a.x < b.x || (a.x==b.x && a.y < b.y); });
    hull.erase(unique(hull.begin(), hull.end(), [](Point a, Point b){ return a.x==b.x && a.y==b.y; }), hull.end());
    
    // Sort counterclockwise
    Point center = {0,0};
    for(auto &p:hull) { center.x += p.x; center.y += p.y; }
    center.x /= hull.size(); center.y /= hull.size();
    sort(hull.begin(), hull.end(), [&](Point a, Point b){
        return atan2(a.y-center.y, a.x-center.x) < atan2(b.y-center.y, b.x-center.x);
    });

    return hull;
}

// Merge two convex hulls using upper and lower tangents
vector<Point> mergeHulls(const vector<Point> &left, const vector<Point> &right) {
    int nL = left.size(), nR = right.size();

    // Rightmost of left, leftmost of right
    int iL = max_element(left.begin(), left.end(), [](Point a, Point b){ return a.x < b.x; }) - left.begin();
    int iR = min_element(right.begin(), right.end(), [](Point a, Point b){ return a.x < b.x; }) - right.begin();

    // Upper tangent
    int uL=iL, uR=iR;
    bool done=false;
    while(!done){
        done=true;
        while(orientation(right[uR], left[uL], left[(uL+1)%nL])==1) uL=(uL+1)%nL;
        while(orientation(left[uL], right[uR], right[(uR-1+nR)%nR])==-1) { uR=(uR-1+nR)%nR; done=false; }
    }

    // Lower tangent
    int lL=iL, lR=iR;
    done=false;
    while(!done){
        done=true;
        while(orientation(left[lL], right[lR], right[(lR+1)%nR])==1) lR=(lR+1)%nR;
        while(orientation(right[lR], left[lL], left[(lL-1+nL)%nL])==-1) { lL=(lL-1+nL)%nL; done=false; }
    }

    // Collect points along merged hull
    vector<Point> merged;
    int idx=uL;
    merged.push_back(left[idx]);
    while(idx!=lL){ idx=(idx+1)%nL; merged.push_back(left[idx]); }
    idx=lR;
    merged.push_back(right[idx]);
    while(idx!=uR){ idx=(idx+1)%nR; merged.push_back(right[idx]); }

    return merged;
}

// Divide-and-conquer convex hull
vector<Point> divideHull(vector<Point> &pts) {
    if(pts.size() <= 5) return bruteHull(pts);
    sort(pts.begin(), pts.end(), [](Point a, Point b){ return a.x < b.x; });
    vector<Point> left(pts.begin(), pts.begin() + pts.size()/2);
    vector<Point> right(pts.begin() + pts.size()/2, pts.end());
    vector<Point> leftHull = divideHull(left);
    vector<Point> rightHull = divideHull(right);
    return mergeHulls(leftHull, rightHull);
}

// Read points from CSV
vector<Point> readPoints(const string &filename) {
    ifstream fin(filename);
    vector<Point> pts;
    if(!fin){ cerr << "Cannot open file: " << filename << endl; return pts; }
    double x, y; char comma;
    while(fin >> x >> comma >> y) pts.push_back({x,y});
    return pts;
}

int main() {
    string filename;
    cout << "Enter input .txt file: ";
    cin >> filename;
    vector<Point> points = readPoints(filename);
    if(points.empty()) return 1;

    auto start = chrono::high_resolution_clock::now();
    vector<Point> hull = divideHull(points);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end-start;
    string outFile = "Divide_Results_" + filename;
    ofstream fout(outFile);
    fout << "Hull computation time: " << elapsed.count() << " seconds\n";
    fout << "Operations: " << op_counter << "\n";
    fout << "Hull points:\n";
    for(auto &p:hull) fout << p.x << "," << p.y << "\n";
    fout.close();
    cout << "\nResults saved to: " << outFile << endl;
    return 0;
}