#include "fitting.hpp"


Tree makeDirectedTree (vector<tuple<Point, Point, double>> Sp)
{
  Tree dt;
  for(tuple<Point, Point, double> xt : Sp) {
    Point p = get<0>(xt);
    Point t = get<1>(xt);
    double h = get<2>(xt);
    dt.vertex.push_back(p);
    for(tuple<Point, Point, double> xt2 : Sp) {
      Point p2 = get<0>(xt2);
      Point t2 = get<1>(xt2);
      double dist = calcPointDistance(p, p2);
      if(dist == 0 || dist > h) {
        continue;
      } else {
        if (calcDot(t, t2) < 0) {
          continue;
        } else {
          //p,p2の接線の平均ベクトルとp->p2の方向ベクトルの内積が負ならp<-p2, 正ならp->p2方向
          Point pp2(p2.x - p.x, p2.y - p.y);
          Point tt2(t.x + t2.x, t.y + t2.y);
          if (calcDot(pp2, tt2) >= 0) {
            dt.edge.push_back(make_tuple(p,p2));
          }
          // これ以降はp2が上のfor文で選ばれてpが下のfor文で選ばれれば同じ
          //} else {
          //   dt.edge.push_back(make_tuple(p2,p));
          // }
        }
      }
    }
  }
  return dt;
}

tuple<Point, Point,double> search (Point p, vector<tuple<Point, Point,double>> Sp)
{
  for(tuple<Point, Point,double> sp : Sp) {
    if(get<0>(sp) == p) {
      return sp;
    }
  }
  Point none;
  return make_tuple(none, none, 0);
}


tuple<double, vector<tuple<Point, Point, double>>> max_root_loop(Tree t, vector<tuple<Point, Point, double>> Sp, Point from)
{
  vector<tuple<Point, Point, double>> max_path = {};
  double max = 0;
  for(tuple<Point, Point> e : t.edge) {
    if(get<0>(e) == from) {
      Point to = get<1>(e);
      // cout << from .x << " " << from.y << " " << to.x << " " << to.y << endl;
      // to.x = get<1>(e).x;
      // to.y = get<1>(e).y;
      tuple<double, vector<tuple<Point, Point, double>>> to_max = max_root_loop(t, Sp, to);
      double to_max_dist = get<0>(to_max);
      vector<tuple<Point, Point, double>> to_max_path = get<1>(to_max);
      double dist = calcPointDistance(from, to) + to_max_dist;
      // cout << dist << endl;
      if(max < dist) {
        to_max_path.insert(to_max_path.begin(), search(from, Sp));
        max_path = to_max_path;
        max = dist;
      }
    }
  }
  return make_tuple(max, max_path);
}

//最大パスとそのルートを求める
tuple<double, vector<tuple<Point, Point, double>>> max_root(Tree t, vector<tuple<Point, Point, double>> Sp)
{
  double max = 0;
  vector<tuple<Point, Point,double>> max_path;
  for(Point start : t.vertex) {
    tuple<double, vector<tuple<Point, Point, double>>> max_root = max_root_loop(t, Sp, start);
    double max_root_dist = get<0>(max_root);
    // cout << max_root_dist << " " << max << endl;
    if (max < max_root_dist) {
      max = max_root_dist;
      max_path = get<1>(max_root);
    }
  }
  return make_tuple(max, max_path);
}

vector<tuple<Point, Point, double>> updateTangent(vector<tuple<Point, Point, double>> Sp)
{
  vector<tuple<Point, Point, double>> new_Sp = {};
  for(tuple<Point, Point, double> p0 : Sp) {
    Point new_t = kernel(p0, Sp);
    new_Sp.push_back(make_tuple(get<0>(p0), new_t, get<2>(p0)));
  }
  return new_Sp;
}

vector<tuple<Point, Point, double>> polylineExtraction(vector<tuple<Point, Point, double>> Sp)
{
  vector<tuple<Point, Point, double>> new_Sp = updateTangent(Sp);
  Tree t = makeDirectedTree(new_Sp);
  double max_path = 0;
  vector<tuple<Point, Point,double>> max_path_points;
  printTree(t, "debug/Tree.svg");
  int counter = 0;
  for(Point p : t.vertex) {
    Tree msdt = edmonds(t, p); //pを根とする最小有向全域木
    string file_tree_name = "debug/Tree" + std::to_string(counter) + ".svg";
    printTree(msdt, file_tree_name);
    // tuple<double, vector<tuple<Point, Point, double>>> path = max_root(msdt, new_Sp);
    // cout << msdt.edge.size() << endl;
    tuple<double, vector<tuple<Point, Point, double>>> path = max_root_loop(msdt, new_Sp, p);
    // cout << get<0>(path) << " " << max_path << endl;
    cout << counter << " : " << get<0>(path) << " " << max_path << endl;
    ++counter;
    // cout << get<1>(path).size() << endl;
    if(get<0>(path) > max_path) {
      max_path = get<0>(path);
      max_path_points = get<1>(path);
    }
  }
  return max_path_points;
}
