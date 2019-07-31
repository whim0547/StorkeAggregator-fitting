#include "fitting.hpp"

using namespace std;

vector<Point> tangentOptimization(vector<tuple<Point, Point, double>> Sp, double lambda = 1e-3)
{
  vector<tuple<Point, Point, double>> Sp0 = Sp;
  // double lambda = 1e-4;
  size_t size = Sp.size();
  // vector<tuple<double, Point, Point>> old_c = {};
  // 繰り返し最小二乗法を 3 loop
  for(int loop = 0; loop < 3; ++loop) {
    // vector<tuple<double, Point, Point>> c = {};
    // vector<array<double, 3>> A = {};
    vector<tuple<double, Point>> PQ = {}; // AをTDMAアルゴリズムに適用した結果
    // vector<Point> b = {};
    Point old_c_2_div_1;
    Point zero;
    double old_a_r = 0;
    double old_p;
    Point old_q;
    for(size_t i = 0; i < size; ++i) {
      // Objective Functionの定数値 : 分母c1, カーネルc2, 初期位置 get<0>(Sp0.at(i))を取得
      double c1 = (i == size - 1) ? 0 : calcPointDistance(get<0>(Sp.at(i + 1)), get<0>(Sp.at(i)));
      Point c2 = kernel(Sp.at(i), Sp);
      // std::cout << loop << " : " << i << endl;
      // c2.printPoint();
      Point c3 = get<0>(Sp0.at(i));
      // c.push_back(make_tuple(c1, c2, c3));
      // Ax = b (x, bはベクトル)の三重対角行列Aとbの各行成分
      double a_l = old_a_r;
      double a_r = (i == size - 1) ? 0 : -2 / (c1 * c1);
      double a_c = 2 * lambda - old_a_r - a_r;
      old_a_r = a_r;
      Point c_2_div_1 = i == size - 1 ? zero : (c2 * (-2 / c1));
      Point b_i = c3 * (2 * lambda) - old_c_2_div_1 + c_2_div_1;
      old_c_2_div_1 = c_2_div_1;
      // std::cout << loop << " : " << i << " a: " << a_l << ", " << a_c << ", " << a_r << " b: ";
      // b_i.printPoint();
      // std::cout << " c1 : " << c1 << " c2 : ";
      // c2.printPoint();

      // Ax = b の各行をTDMAアルゴリズムに適用
      // ??イテレータ破壊の可能性
      // double p = -a_r / (a_c + (i == 0 ? 0 : a_l * get<0>(PQ.back())));
      // Point q = (i == 0) ? b_i / a_c : (b_i - get<1>(PQ.back()) * a_l) / (a_c + a_l * get<0>(PQ.back()));
      double p = -a_r / (a_c + (i == 0 ? 0 : a_l * old_p));
      Point q = (i == 0) ? b_i / a_c : (b_i - old_q * a_l) / (a_c + a_l * old_p);
      old_p = p;
      old_q = q;
      // std::cout << loop << " : " << i << " P: " << p << " Q: ";
      // q.printPoint();
      // cout << endl;
      PQ.push_back(make_tuple(p, q));
    }

    Point old_point;
    for(int i = static_cast<int>(size) - 1; i >= 0; --i) {
      size_t i2 =  static_cast<size_t>(i);
      // ?? イテレータ破壊
      // Point p = (i2 == size - 1) ? get<1>(PQ.at(i2)) : (get<0>(Sp.at(i2 + 1)) * get<0>(PQ.at(i2)) + get<1>(PQ.at(i2)));
      Point p = (i2 == size - 1) ? get<1>(PQ.at(i2)) : (old_point * get<0>(PQ.at(i2)) + get<1>(PQ.at(i2)));
      old_point = p;
      // std::cout << loop << " : " << i2;
      // p.printPoint();
      // cout << " : ";
      get<1>(PQ.at(i2)).printPoint();
      cout << endl;
      Sp.at(i2) = make_tuple(p, get<1>(Sp.at(i2)), get<2>(Sp.at(i2)));
      // std::cout << loop << " : " << i2;
      // get<0>(Sp.at(i2)).printPoint();
      // cout << endl;
    }
  }

  vector<Point> polyline = {};
  // for(tuple<Point, Point, double> p : Sp) {
  for(size_t i = 0; i < size; ++i) {
    tuple<Point, Point, double> p = Sp.at(i);
    polyline.push_back(get<0>(p));
    // get<0>(p).printPoint();
    // cout << endl;
  }
  return polyline;
}
