#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <limits>
#include "polyline.hpp"

using namespace std;


//座標を表すクラス
Point::Point() : x(0.0), y(0.0)
{}

Point::Point(double x, double y):x(x),y(y)
{}

void Point::reverse()
{
  this->x = -x;
  this->y = -y;
}

void Point::printPoint()
{
  std::cout << "L " << x << " " << y << "  ";
}
// end class Point

void Point::normalize()
{
  double sqr = sqrt(this->x * this->x + this->y * this->y);
  this->x /= sqr;
  this->y /= sqr;
}

double calcPointDistance(Point a, Point b)
{
  return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

double calcDot(Point a, Point b) //a,bをベクトルとした時の内積を取る
{
  return a.x * b.x + a.y * b.y;
}

vector<Point> doubles_to_points (vector<double> v) {//v:座標の羅列　しかもpair化してない=長さ2n
  vector<Point> points = {};
  for(size_t i = 0; i < v.size() / 2; ++i) {
    Point p(v.at(2*i), v.at(2*i+1));
    points.push_back(p);
  }
  return points;
}

//class Polyline
Polyline::Polyline() {}

Polyline::Polyline(vector<Point> v) {
  if(v.size() == 0) {
    Polyline();
  } else {
    this->m_points = v;
    int size = static_cast<int>(v.size());
    std::vector<double> each_point_length = {};
    //initialize m_length m_tangents
    this->m_tangents.push_back(calcTangent(v.at(0),v.at(1)));
    for(int i = 1; i < size; ++i) {
      this->m_length += calcPointDistance(v.at(i-1),v.at(i));
      each_point_length.push_back(this->m_length);
      if(i == size - 1) {
        this->m_tangents.push_back(calcTangent(v.at(i-1),v.at(i)));
      } else {
        this->m_tangents.push_back(calcTangent(v.at(i-1),v.at(i+1)));
      }
    }
    //initialize m_mid_point
    for(int i = 1; i < size; ++i) {
      if(each_point_length.at(i) > this->m_length / 2) {
        this->m_mid_point = i - 1;
        break;
      }
    }
    //initialize m_centor_points
    Point mid_point = this->m_points.at(this->m_mid_point);
    for(int i = 0; i < size; ++i) {
      Point target_point = this->m_points.at(i);
      double inner_product = calcDot(mid_point, target_point);
      if(abs(inner_product) >= cos(M_PI / 3)) { //角度(内積の絶対値)が60°以内
        this->m_centor_points.push_back(i);
      }
    }
  }
}

Polyline::Polyline(vector<double> v) : Polyline(doubles_to_points(v)){//座標の羅列　しかもpair化してない=長さ2n
}


//setter
void Polyline::setDirection (bool b) {
  this->m_direcion = b;
}


//getter
Point Polyline::getMidPoint() {
  return this->m_points.at(this->m_mid_point);
}

Point Polyline::getMidTangent() {
  return this->m_tangents.at(this->m_mid_point);
}

vector<Point> Polyline::getPoints() {
  return this->m_points;
}

double Polyline::getLength() {
  return this->m_length;
}

vector<Point> Polyline::getTangents() {
  return this->m_tangents;
}

vector<int> Polyline::getCentorPoints() {
  return this->m_centor_points;
}

bool Polyline::getDirection() {
  return this->m_direcion;
}

//print
void Polyline::printPoints() {
  for(Point i : this->m_points) {
    i.printPoint();
  }
  std::cout << std::endl;
}

//function
tuple<double,int,int> Polyline::calcPolylineDistance(Polyline b) { //Polyline bとの距離
  double mids_angle = calcDot(getMidTangent(), b.getMidTangent());
  double min = std::numeric_limits<double>::infinity();
  int a_point = 0, b_point = 0;
  if (abs(mids_angle) >= cos(M_PI / 3)) { //中点の接線どうしの角度が60°より大きいなら角度は無限(つまりif文の処理はしない)
    std::vector<int> b_centor_points = b.getCentorPoints();
    std::vector<Point> b_points = b.getPoints();
    for (int i : this->m_centor_points){
      for (int j : b_centor_points) {
        double dist = calcPointDistance(this->m_points.at(i), b_points.at(j));
        if (dist < min){
          min = dist;
          a_point = i;
          b_point = j;
        }
      }
    }
  }
  return make_tuple(min, a_point, b_point);
}

Point Polyline::calcTangent(/*Point x, */Point l, Point r) { //3点連続点l,x,rにおいてxでの接線の傾きを中心差分で求める(x不要)
  if(l.x == r.x) { //縦につながっていそうな場合->y座標から求める
    // double k = (r.x - l.x) / (r.y - l.y); // k = 0では？
    int sign = (r.y - l.y) > 0 ? 1 : -1;
    // double inv_sqrt_k = sign * sqrt(1 + pow(k));
    // Point tangent(k / inv_sqrt_k, 1 / inv_sqrt_k);
    Point tangent(0, sign * 1.0);
    return tangent;
  } else { //通常通りに求める
    double k = (r.y - l.y) / (r.x - l.x);
    int sign = (r.x - l.x) > 0 ? 1 : -1;
    double inv_sqrt_k = sign * sqrt(1 + pow(k, 2));
    Point tangent(1 / inv_sqrt_k, k / inv_sqrt_k);
    return tangent;
  }
}

void Polyline::reverse () { //Polylineの向きを変える
  int size = static_cast<int>(this->m_points.size());
  for(int i = 0; i < size; ++i) {
    this->m_tangents.at(i).reverse();
  }
  for(int i = 0; i < (size + 1) / 2; ++i) {//ex) size = 13なら0~5 <-> 12~7を対応
    Point back = this->m_points.at(size - 1 - i);
    this->m_points.at(size - 1 - i) = this->m_points.at(i);
    this->m_points.at(i) = back;
    back = this->m_tangents.at(size - 1 - i);
    this->m_tangents.at(size - 1 - i) = this->m_tangents.at(i);
    this->m_tangents.at(i) = back;
  }
  for(size_t i = 0; i < this->m_centor_points.size(); ++i) {
    this->m_centor_points.at(i) = size - 1 - this->m_centor_points.at(i); //位置は後ろから順になっただけ
  }
}

double calcPolylineDistance(Polyline a, Polyline b)
{
  return get<0>(a.calcPolylineDistance(b));
}
