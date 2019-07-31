#include <iostream>
#include <vector>
#include <limits>
#include "fitting.hpp"

using namespace std;

vector<Polyline> strokeOrientation(vector<Polyline> cluster)
{
  double max_length = 0;
  size_t max_i = 0;
  vector<size_t> undefined_orient;
  vector<size_t> defined_orient;
  //最長のPolylineを求める
  for (size_t i = 0; i < cluster.size(); ++i) {
    undefined_orient.push_back(cluster.size() - 1 - i);
    double target_len = cluster.at(i).getLength();
    if(max_length < target_len) {
      max_length = target_len;
      max_i = i;
    }
  }
  //最長のPolylineに向きを統一する
  // cluster.at(max_i).setDirection(true);
  undefined_orient = eraseObj(undefined_orient, max_i);
  defined_orient.push_back(max_i);
  // size_t debug;
  while(undefined_orient.size() > 0) {
    double min_len = std::numeric_limits<double>::infinity();
    size_t min_i = 0, min_j = 0;
    for(size_t i : undefined_orient) {
      for(size_t j : defined_orient) {
        double len = calcPolylineDistance(cluster.at(i),cluster.at(j));
        if (len < min_len) {
          min_len = len;
          min_i = i;
          min_j = j;
        }
      }
    }
    if(min_i == 0 && min_j == 0) { //もし残ったPolylineたちが外れなら消し去る
      for(size_t i : undefined_orient) {//undefined_orient は降順になっているのでうまく消えるはず、、、
        cluster.erase(cluster.begin() + i);
      }
      break;
    } else {
      Point i_mid_tan = cluster.at(min_i).getMidTangent();
      Point j_mid_tan = cluster.at(min_j).getMidTangent();
      // std::cout << min_i;
      // i_mid_tan.printPoint();
      // std::cout << min_j;
      // j_mid_tan.printPoint();
      // cout << endl;
      if(calcDot(i_mid_tan, j_mid_tan) < 0) {
        std::cout << "reversed" << std::endl;
        cluster.at(min_i).reverse();
      }
      undefined_orient = eraseObj(undefined_orient, min_i);
      defined_orient.push_back(min_i);
    }
  }
  return cluster;
}
