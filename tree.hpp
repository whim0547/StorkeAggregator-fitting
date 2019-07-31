#pragma once

#include <vector>
#include <tuple>
#include "polyline.hpp"

using namespace std;

// class Vertex
// {
// public:
//   Vertex();
//   Vertex(Point p);
//   void addEdge(Vertex* v);
//
//   Point point;
//   vector<Vertex*> edge = {};
// };

class Tree
{
public:
  Tree();
  Tree(vector<Polyline> lines);

  // vector<Vertex*> getEdge(Vertex v);

  // vector<Vertex> vertex;
  vector<Point> vertex = {};
  vector<tuple<Point,Point>> edge = {}; //有向辺
};

//最小全域木
Tree prim(Tree t);

//最小全域有向木
Tree edmonds(Tree t, Point a);

Point kernel(tuple<Point, Point, double> p0, vector<tuple<Point, Point, double>> Sp);

void printTree(Tree t, string file_name);
