#pragma once

#include <vector>
#include <tuple>
#include "polyline.hpp"
#include "tree.hpp"

using namespace std;

vector<Polyline> strokeOrientation(vector<Polyline> cluster);
// vector<tuple<Point, Point, double>> MLS(Tree t);
vector<tuple<Point, Point, double>> MLS(vector<Polyline> ps);
vector<tuple<Point, Point, double>> polylineExtraction(vector<tuple<Point, Point, double>> Sp);
vector<Point> tangentOptimization(vector<tuple<Point, Point, double>> Sp, double lambda);
void fitting();

//utility
string pathToStr(vector<Point> path);
// vector<tuple<Point, Point, double>> pointMerge(vector<tuple<Point, Point, double>> Sp);
void pointNeighberMerge(vector<tuple<Point, Point, double>>* Sp);
