#include <limits>
#include <fstream>
#include <cmath>
#include "tree.hpp"

// class Vertex
// Vertex::Vertex(){}
//
// Vertex::Vertex(Point p) : point(p) {}
//
// void Vertex::addEdge(Vertex* v)
// {
//   this->edge.push_back(v);
// }

//class Tree
Tree::Tree(){}

Tree::Tree(vector<Polyline> lines)
{
  for(Polyline l : lines) {
    for(Point p : l.getPoints()) {
      if(find(this->vertex, p)) continue;
      this->vertex.push_back(p);
    }
  }
}

// vector<Vertex> Tree::getEdge(Vertex v)
// {
//   return this->vertex.
// }

Tree prim(Tree t)
{
  vector<Point> elected = {};
  vector<Point> nonelected = t.vertex;
  // Point begin = *(nonelected.begin());
  // nonelected = eraseObj(nonelected, begin);
  // elected.push_back(begin);
  Point begin = nonelected.front();
  nonelected.erase(nonelected.begin());
  elected.push_back(begin);
  while(nonelected.size()) {
    // std::cout << nonelected.size() << std::endl;
    double min_dist = std::numeric_limits<double>::infinity();
    Point nearest_p;
    Point nearest_q;
    for(Point p : nonelected) {
      // std::cout << "p" << std::endl;
      for(Point q : elected) {
        double dist = calcPointDistance(p, q);
        if(min_dist > dist) {
          nearest_p = p;
          nearest_q = q;
          min_dist = dist;
        }
      }
    }
    elected.push_back(nearest_p);
    nonelected = eraseObj(nonelected, nearest_p);
    tuple<Point, Point> pq = make_tuple(nearest_p, nearest_q);
    tuple<Point, Point> qp = make_tuple(nearest_q, nearest_p);
    t.edge.push_back(pq);
    t.edge.push_back(qp);
  }
  // std::cout << t.edge.size() << std::endl;
  return t;
}




// 最小有向全域木 deletes は縮約後の点の座標の最低値(負)が入る。 縮約情報はdelete_to_points
Tree edmonds(vector<Point> V, vector<tuple<Point, Point, double>> cost_edges, Point p) {
  Tree t;
  vector<Point> used = {};
  vector<tuple<Point, Point, double>> copy = cost_edges;
  double contraction = -1; //縮約した回数(の負にしたもの)->Point(負の値)でうまくマーク
  vector<Point> cycle= {}; //縮約した点たち

  auto getCost = [] (/*Point s, */Point t,  vector<tuple<Point, Point, double>> costs) {
    double min = std::numeric_limits<double>::infinity();
    for(tuple<Point, Point, double> cost : costs) {
      if (/*s == get<0>(cost) && */t == get<1>(cost) && get<2>(cost) < min) {
        min = get<2>(cost);
      }
    }
    return min;
  };

  for(Point v : V) {
    if (find(used, v)) continue;
    if (v == p) continue;
    vector<Point> passed = {};

    // 最小コスト枝
    while(true) {
      tuple<Point, Point, double> min_cost_edge = make_tuple(v, v, std::numeric_limits<double>::infinity());
      for(tuple<Point, Point, double> e : cost_edges) {
        // Point s = get<0>(e);
        Point t = get<1>(e);
        double cost = get<2>(e);
        if(t == v && cost < get<2>(min_cost_edge)) {
          min_cost_edge = e;
        }
      }

      if(v.x > 0) passed.push_back(v);
      // vに入る点がなかった
      if (get<2>(min_cost_edge) == std::numeric_limits<double>::infinity()) {
          for(Point pass : passed) {
            used.push_back(pass);
          }
          break;
      }

      Point u = get<0>(min_cost_edge);
      // vが縮約したやつ
      if (v.x < 0) {
        double min = std::numeric_limits<double>::infinity();
        Point minP;
        for(tuple<Point, Point, double> e : cost_edges) {
          Point s = get<0>(e);
          Point t = get<1>(e);
          double cost = get<2>(e);
          if(s == u && find(cycle, t)) {
            double kari_cost = cost - getCost(t, cost_edges);
            if (kari_cost < min) {
              min = kari_cost;
              minP.x = t.x;
              minP.y = t.y;
            }
          }
        }
        v.x = minP.x;
        v.y = minP.y;

        //cycle内の不要になった辺を一つ削除
        for(size_t i = 0; i < t.edge.size(); ++i) {
          tuple<Point, Point> e = t.edge.at(i);
          if (find(cycle, get<0>(e)) && get<1>(e) == v) {
            t.edge.erase(t.edge.begin() + i);
          }
        }
        cost_edges = copy;
      }

      t.edge.push_back(make_tuple(u,v));

      if(u == p || find(used, u)) {//すでにそれ以上は通っている
        for(Point pass : passed) {
          used.push_back(pass);
        }
        break;
      } else {
        if(find(passed, u)) { //Cycleがあった
          //cycleを検出->縮約
          cycle = {};//縮約したもの
          Point new_p(contraction, 0.0);
          bool flag = false;
          for(Point c : passed) {
            if(flag) {
              cycle.push_back(c);
            }
            if(c == u) {
              cycle.push_back(c);
              flag = true;
            }
          }
          //コスト変更
          vector<tuple<Point, Point, double>> new_cost_edges = cost_edges;
          for(tuple<Point, Point, double> e : cost_edges) {
            Point s = get<0>(e);
            Point t = get<1>(e);
            double cost = get<2>(e);
            if(!find(cycle, t)) {
              if(find(cycle, s)) { // cycle -> t
                new_cost_edges.push_back(make_tuple(new_p, t, cost));
              }
            } else {
              if(!find(cycle, s)) { // s -> cycle
                double new_cost = cost - getCost(t, new_cost_edges);
                new_cost_edges.push_back(make_tuple(s, new_p, new_cost));
              }
            }
          }
          cost_edges = new_cost_edges;
          v = new_p;
          // vector<tuple<Point, Point, double>> new_cost_edges = cost_edges;
          // for(tuple<Point, Point, double> e : cost_edges) {
          //   Point s = get<0>(e);
          //   Point t = get<1>(e);
          //   double cost = get<2>(e);
          //   if(!find(cycle, t)) {
          //     if(find(cycle, s)) { // cycle -> t
          //       eraseObj(new_cost_edges, e);
          //       new_cost_edges.push_back(make_tuple(new_p, t, cost));
          //     }
          //   } else {
          //     if(!find(cycle, s)) { // s -> cycle
          //       eraseObj(new_cost_edges, e);
          //       double new_cost = cost - getCost(t, new_cost_edges);
          //       new_cost_edges.push_back(make_tuple(s, new_p, new_cost));
          //     }
          //   }
          // }
          // cost_edges = new_cost_edges;
          // passed.push_back(new_p);
        } else {
          v = u;
        }
      }
    }
  }

  // //縮約解除
  // for(size_t count = t.edge.size() - 1; count >= 0; --count) {
  //   tuple<Point, Point> e = t.edge.at(i);
  //   Point s = get<0>(e);
  //   Point t = get<1>(e);
  //   if(s.x < 0 && t.x < 0) {
  //     if(s.x > t.x) { //tの方が新しい
  //       while(s.x > t.x) {
  //         for(tuple<double, vector<Point>> deleted : delete_to_points) {
  //           if(get<0>(deleted) == t.x) {
  //
  //           }
  //         }
  //       }
  //     } else {
  //
  //     }
  //   }
  //   if(t.x < 0) {
  //
  //   }
  // }
  t.vertex = used;
  // cout << t.edge.size() << endl;
  return t;
}


// Tree edmonds(vector<Point> V, vector<tuple<Point, Point, double>> cost_edges, Point p, double deletes, vector<tuple<Point, vector<Point>>> delete_to_points)
// {
//   Tree t();
//   t.vertex.push_back(p);
//   // vector<Point> passed = {}; //すでに通った木
//   for(Point v : V) {
//     // if(v == p) continue;
//     //vがすでに計算済み
//     if(find(t.vertex, v)) continue;
//
//     tuple<Point, Point, double> min_cost_edge = make_tuple(v, v, std::numeric_limits<double>::infinity());
//
//     for(tuple<Point, Point, double> e : cost_edges) {
//       Point s = get<0>(e);
//       Point t = get<1>(e);
//       double cost = get<2>(e);
//       if(t != v) {
//         continue;
//       }
//       if(cost < get<2>(min_cost_edge)) {
//         min_cost_edge = e;
//       }
//     }
//
//     // vに入る点がなかった
//     if (get<2>(min_cost_edge) == std::numeric_limits<double>::infinity()) {
//         continue;
//     }
//
//     t.vertex.push_back(v); //すでに通った点
//
//     t.edge.push_back(make_tuple(get<0>(min_cost_edge), get<1>(min_cost_edge)));
//   }
// }

// 最小有向全域木
Tree edmonds(Tree t, Point a)
{
  vector<tuple<Point, Point, double>> cost_edges = {};
  for(tuple<Point, Point> e : t.edge) {
    Point p = get<0>(e);
    Point q = get<1>(e);
    double cost = calcPointDistance(p, q);
    cost_edges.push_back(make_tuple(p, q, cost));
  }
  return edmonds(t.vertex, cost_edges, a);
}

Point kernel(tuple<Point, Point, double> p0, vector<tuple<Point, Point, double>> Sp)
{
  double alpha = 0.6;
  double beta = std::cos(M_PI / 3);
  double ah = get<2>(p0) * alpha;
  Point p0_p = get<0>(p0);
  Point p0_t = get<1>(p0);
  Point new_t;
  for(tuple<Point, Point, double> p : Sp) {
    Point p_p = get<0>(p);
    // if (p0_p == p_p) continue;
    Point p_t = get<1>(p);
    double dist = calcPointDistance(p0_p, p_p);
    if (dist >= ah) continue;
    if (calcDot(p0_t, p_t) <= beta) continue;
    new_t = new_t + p_t * std::exp(-std::pow(dist / ah, 2));
  }
  new_t.normalize();
  return new_t;
}

void printTree(Tree t, string file_name)
{
  std::ofstream file_tree(file_name);
/*
  string input_str = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";
  input_str += "\n";
  input_str += "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
  input_str += "<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" preserveAspectRatio=\"xMidYMid meet\" viewBox=\"0 0 640 640\" width=\"640\" height=\"640\"><defs>";
*/
  string input_str = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
  input_str += "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
  input_str += "<svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" preserveAspectRatio=\"xMidYMid meet\" viewBox=\"0 0 700 700\" width=\"1000\" height=\"1000\">\n";
  input_str += "\t<defs>\n";
  input_str += "\t\t<marker id=\"mu_mh\" markerUnits=\"userSpaceOnUse\" markerWidth=\"3\" markerHeight=\"3\" viewBox=\"0 0 10 10\" refX=\"5\" refY=\"5\" orient=\"auto\">\n";
  input_str += "\t\t\t<polygon points=\"0,0 5,5 0,10 10,5\" fill=\"red\"/>\n";
  input_str += "\t\t</marker>\n";
  input_str += "\t</defs>\n";
  input_str += "\t<g font-size=\"5\" fill=\"black\">\n";
  int counter = 0;
  for(Point p : t.vertex) {
    input_str += "\t\t<text x = \"" + to_string(p.x) + "\" y=\"" + to_string(p.y) + "\">" + to_string(counter) + "</text>\n";
    ++counter;
  }
  input_str += "\t</g>\n";
  input_str += "\t<g stroke=\"orange\" stroke-width=\"0.24\" marker-end=\"url(#mu_mh)\">\n";
  for(tuple<Point, Point> e : t.edge) {
    input_str += "\t\t<line x1=\"" + to_string(get<0>(e).x) + "\" y1=\"" + to_string(get<0>(e).y) + "\" x2=\"" + to_string(get<1>(e).x) + "\" y2=\"" + to_string(get<1>(e).y)+ "\"/>";
  }
  input_str += "\n\t</g>\n</svg>";

  file_tree << input_str;
  file_tree.close();
}
