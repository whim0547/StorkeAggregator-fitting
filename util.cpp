#include <cmath>
#include "fitting.hpp"

using namespace std;

string pathToStr(vector<Point> path)
{
  // string init = "<defs><path d=\"";
  // string id="a1Pxnpq6Lu";
  // string final = "\" id=\""+id+"\"></path>"
  //              + "</defs><g><g><g><g><use xlink:href=\""
  //              + id
  //              + "\" opacity=\"1\" fill-opacity=\"0\" stroke=\"#000000\" stroke-width=\"1\" stroke-opacity=\"1\"></use></g></g></g></g>";
  string init = "<polyline fill=\"none\" stroke=\"green\" stroke-width=\"3\" points=\"";
  string final = "\" />";
  string points = "";
  int i = 0;
  for(Point p : path) {
    if (i == 0) {
      points += to_string(p.x) + "," + to_string(p.y);
      ++i;
    } else {
      points += " " + to_string(p.x) + "," + to_string(p.y);
    }

  }
  return init + points + final;
}


// ソートしながら入れて行く
template<typename T>
void downerSortInsert(vector<T> *v, T t)
{
  for(size_t i = 0; i < v->size(); ++i) {
    if(t < v->at(i)) {
      v->insert(v->begin() + i, t);
      return;
    }
  }
  v->push_back(t);
  return;
}

//近傍点をマージする
//隣接点にしよ、、、？
void pointNeighberMerge(vector<tuple<Point, Point, double>>* Sp)
{
  // double min_dist = 3e-3;
  double min_dist = 1;
  bool diff = true;
  size_t start = 0;
  while (diff) {
    size_t size = Sp->size();
    for(size_t i = start; i < size - 1; ++i) {
      Point p = get<0>(Sp->at(i));
      Point next = get<0>(Sp->at(i+1));
      double dist = calcPointDistance(p, next);
      if(dist < min_dist) {
        Point new_p = (p + next) / 2;
        Point new_t = (get<1>(Sp->at(i)) + get<1>(Sp->at(i+1))) / 2;
        double new_h = max(get<2>(Sp->at(i)), get<2>(Sp->at(i+1)));
        Sp->at(i) = make_tuple(new_p, new_t, new_h);
        Sp->erase(Sp->begin() + i + 1);
        break;
      }
      ++start;
    }
    diff = false;
  }
  // return Sp;
}

//近傍点ver 未完成
// vector<tuple<Point, Point, double>> pointMerge(vector<tuple<Point, Point, double>> Sp)
// {
//   double min_dist = 3e-3;
//   vector<vector<size_t>> backet = {};
//  //近接点探し
//   // for(size_t i = 0; i < Sp.size() - 1; ++i) { //最後の点はやらなくてよい
//   for(size_t i = 0; i < Sp.size(); ++i) {
//     vector<int> near = {};
//     Point pi = get<0>(Sp.at(i));
//     // for(size_t k = i; k < Sp.size(); ++k) {
//     for(size_t k = 0; k < Sp.size(); ++k) {
//       if(i == k) continue;
//       Point pk = get<0>(Sp.at(k));
//       double dist = calcPointDistance(pi, pk);
//       if(dist < min_dist) {
//         near.push_back(k);
//       }
//     }
//     backet.push_back(near);
//   }
//
//   if (backet.empty()) return Sp;
//
//   vector<size_t> destroy = {};
// // ここから下が未完成
//   for(size_t i = 0; i < Sp.size() - 1; ++i) {
//     if(!find(destroy, i)) continue;
//     Point sum_p = get<0>(Sp.at(i));
//     Point sum_t = get<1>(Sp.at(i));
//     double max_h = get<2>(Sp.at(i));
//
//     count = 1;
//     vector<size_t> b = backet.at(i);
//     for(size_t k : b) {
//       // if(!backet.at(k).empty()) continue;
//       if(!find(destroy, k)) continue;
//       sum_p = sum_p + get<0>(Sp.at(k));
//       sum_t = sum_t + get<1>(Sp.at(k));
//       max_h = max_h > get<2>(Sp.at(k)) ? max_h : get<2>(Sp.at(k));
//       downerSortInsert(&destroy, k);
//       ++count;
//     }
//     if (count > 1) {
//       Sp.at(i) = make_tuple(sum_p / count, sum_t / count, get<2>(Sp.at(i)));
//     }
//   }
//
//
//   for(int d : destroy) {
//     Sp.erase(v.begin() + d);
//   }
//   return Sp;
// }
