#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <limits>

using namespace std;

template<typename T>
vector<T> eraseObj(vector<T> v, T t)
{
  int count = 0;
  for(T p : v) {
    if (p == t) {
      v.erase(v.begin() + count);
      break;
    }
    ++count;
  }
  return v;
}

template<typename T>
bool find(vector<T> v, T t)
{
  for(T p : v) {
    if (p == t) {
      return true;
    }
  }
  return false;
}



class Point
{
public:
  Point();

  Point(double x, double y);

  void reverse();

  void printPoint();

  void normalize();

  bool operator==(const Point& b) const
  {
    return (this->x == b.x && this->y == b.y);
  }

  Point operator+(const Point& b)
  {
    this->x += b.x;
    this->y += b.y;
    return *this;
  }

  Point operator-(const Point& b)
  {
    this->x -= b.x;
    this->y -= b.y;
    return *this;
  }

  Point operator*(const double b)
  {
    this->x *= b;
    this->y *= b;
    return *this;
  }

  Point operator/(const double b)
  {
    this->x /= b;
    this->y /= b;
    return *this;
  }

  double x;
  double y;
};

double calcPointDistance(Point a, Point b);
double calcDot(Point a, Point b); //a,bをベクトルとした時の内積を取る

vector<Point> doubles_to_points (vector<double> v);//v:座標の羅列　しかもpair化してない=長さ2n

class Polyline
{
public:
  Polyline();

  Polyline(vector<Point> v);

  Polyline(vector<double> v);//座標の羅列　しかもpair化してない=長さ2n


  //setter
  void setDirection (bool b);


  //getter
  Point getMidPoint(); //中点を得る

  Point getMidTangent();

  vector<Point> getPoints();

  double getLength();

  vector<Point> getTangents();

  vector<int> getCentorPoints();

  bool getDirection();

  //print
  void printPoints();

  //function
  tuple<double,int,int> calcPolylineDistance(Polyline b); //Polyline bとの距離

  Point calcTangent(/*Point x, */Point l, Point r); //3点連続点l,x,rにおいてxでの接線の傾きを中心差分で求める(x不要)

  void reverse (); //Polylineの向きを変える

private:
  vector<Point> m_points = {}; //Polylineを構築する点群
  vector<Point> m_tangents = {}; //各点での接線ベクトル
  double m_length = 0; //Polylineの長さ
  int m_mid_point; //中点
  vector<int> m_centor_points = {}; //m_mid_pointの接線との角度が60°以内の接線を持つ点のm_pointsでの位置
  bool m_direcion = false;
};

double calcPolylineDistance(Polyline a, Polyline b); //線a,bの距離を求める
