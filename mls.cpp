#include <Eigen/Dense>
#include <cmath>
#include "fitting.hpp"

using namespace Eigen;
using namespace std;

vector<Point> neighber(Point p, Tree t)
{
  vector<Point> neighbers = {};
  for(tuple<Point, Point> e : t.edge) {
    if(get<0>(e) == p) {
      neighbers.push_back(get<1>(e));
    }
  }
  return neighbers;
}


// see Lee 2000 MLS
void Collect1(Point p, Point p_star, vector<Point>* A, double h, Tree t)
{
  A->push_back(p);
  vector<Point> neighbers = neighber(p,t);
  // std::cout << neighbers.size() << std::endl;
  for(Point q : neighbers) {
    if(!(find(*A,q)) && calcPointDistance(p_star, q) < h) {
      Collect1(q, p_star, A, h, t);
    }
  }
}

//重み付き最小２乗法
// https://jp.mathworks.com/help/curvefit/least-squares-fitting.html
VectorXd weightedLeastSquares(MatrixXd x, VectorXd y, MatrixXd w)
{
  // MatrixXd w = MatrixXd::Zero(data_size, data_size);
  // for(int i = 0; i < data_size; ++i) {
  //   w(i, i) = x(i, 0)
  // }
  MatrixXd XWX = x.transpose() * w * x;
  MatrixXd XWy = x.transpose() * w * y;
  return XWX.colPivHouseholderQr().solve(XWy);
  // return XWX.fullPivLu().solve(XWy);
  // return XWX.partialPivLu().solve(XWy); //fullPivLuが重い場合
}

//集合Aの各点pとp*(p_star)に対する線形近似直線 y = ax+bの(a,b)を出力
VectorXd weightedLeastSquares(vector<Point> A, Point p_star, double h)
{
  size_t A_size = A.size();
  MatrixXd x(A_size, 2);
  VectorXd y(A_size);
  MatrixXd w = MatrixXd::Zero(A_size, A_size);
  int i = 0;
  for (Point p : A) {
    x(i,0) = p.x;
    x(i,1) = 1;
    y(i) = p.y;
    w(i,i) = exp(-1 * pow((calcPointDistance(p, p_star)), 4) / pow(h, 2));
    ++i;
  }
  return weightedLeastSquares(x, y, w);
}

//集合Aの各点pとp*(p_star)に対する線形近似曲線 y = ax*2+bx+cの(a,b,c)を出力
VectorXd weightedLeastSquares2(vector<Point> A, Point p_star, double h)
{
  size_t A_size = A.size();
  MatrixXd x(A_size, 3);
  VectorXd y(A_size);
  MatrixXd w = MatrixXd::Zero(A_size, A_size);
  int i = 0;
  for (Point p : A) {
    x(i,0) = p.x * p.x;
    x(i,1) = p.x;
    x(i,2) = 1;
    y(i) = p.y;
    double dist = calcPointDistance(p, p_star);
    w(i,i) = 2 * pow(dist / h, 3) - 3 * pow(dist / h, 2) + 1;
    ++i;
  }
  return weightedLeastSquares(x, y, w);
}


//A内の点をp*中心theta回転させた点集合
// | cos  sin | |x|
// | -sin cos | |y|
vector<Point> rotate(vector<Point> A, Point p_star, double theta)
{
  vector<Point> A_hat = {};
  for(Point p : A) {
    double x_diff = p.x - p_star.x;
    double y_diff = p.y - p_star.y;
    double x = p_star.x + x_diff * cos(theta) + y_diff * sin(theta);
    double y = p_star.y + x_diff * -sin(theta) + y_diff * cos(theta);
    Point new_p(x, y);
    A_hat.push_back(new_p);
  }
  return A_hat;
}

vector<Point> moveTo(vector<Point> A, Point p_star)
{
  vector<Point> A_hat = {};
  for(Point p : A) {
    double x = p.x - p_star.x;
    double y = p.y - p_star.y;
    Point new_p(x, y);
    A_hat.push_back(new_p);
  }
  return A_hat;
}

//標準偏差を計算
double calcRo(vector<Point> A)
{
  double A_size = static_cast<double>(A.size());
  if (A_size == 1) return 1;
  // if (A_size == 1) return 0;
  double mean_x = 0;
  double mean_y = 0;
  for(Point p : A) {
    mean_x += p.x;
    mean_y += p.y;
  }
  mean_x /= A_size;
  mean_y /= A_size;
  double covXY = 0;
  double sdX = 0;
  double sdY = 0;
  for(Point p : A) {
    covXY += (p.x - mean_x) * (p.y - mean_y);
    sdX += (p.x - mean_x) * (p.x - mean_x);
    sdY += (p.y - mean_y) * (p.y - mean_y);
  }
  covXY /= A_size;
  sdX = sqrt(sdX / A_size);
  sdY = sqrt(sdY / A_size);
  return covXY / (sdX * sdY);
}

// tuple<Point, Point, double> Collect2(Point p, Point p_star, vector<Point> A, double h0, double ro0, double eh, Tree t)
tuple<Point, Point, double> Collect2(Point p, Point p_star, vector<Point> A, double h0, double ro0, double eh, Tree t, Point p_tan)
{
  double h = h0;
  VectorXd l; // Vector2d
  double theta;
  // double ro = 0;
  double ro = 1;
  // int count = 0;
  Point L;
  Point new_p_star;
  new_p_star.x = p_star.x;
  new_p_star.y = p_star.y;


  while(abs(ro) > ro0) {
  // while(abs(ro) < ro0) {
    A.clear();
    Collect1(p, p_star, &A, h, t);
    // Collect1(p, new_p_star, &A, h, t);
    // cout << A.size() << endl;
    l = weightedLeastSquares(A, p_star, h);//(a,b) <- y = ax + b　という近似曲線
    // l = weightedLeastSquares(A, new_p_star, h);//(a,b) <- y = ax + b　という近似曲線
    theta = atan(l(0));//arctan(a)

    L.x = cos(theta);
    L.y = sin(theta);
    Point normal_vec;
    normal_vec.x = l(0) * p.x - p.y + l(1) > 0 ? -L.y : L.y;
    normal_vec.y = l(0) * p.x - p.y + l(1) > 0 ? L.x : -L.x;
    double dist = abs(l(0) * p.x - p.y + l(1)) / sqrt(l(0) * l(0) + 1);
    new_p_star.x = p_star.x + normal_vec.x * dist;
    new_p_star.y = p_star.y + normal_vec.y * dist;

    vector<Point> A_hat = rotate(A, p_star, M_PI / 4 - theta);
    // vector<Point> A_hat = rotate(A, new_p_star, M_PI / 4 - theta);
    if(A.size() == t.vertex.size()) break;
    h += eh;
    ro = calcRo(A_hat);
    // std::cout << ++count << " " << ro << std::endl;
  }

  A = moveTo(A, p_star);
  Point o(0,0);
  A = rotate(A, o, -theta);
  VectorXd q = weightedLeastSquares2(A, p_star, h);
  new_p_star.x =  q(2) * sin(theta) + p_star.x;
  new_p_star.y =  q(2) * cos(theta) + p_star.y;


  // p*から近似直線におろした垂線を求める
  // Point L(cos(theta), sin(theta));
  // Point normal_vec;
  // normal_vec.x = l(0) * p.x - p.y + l(1) > 0 ? -L.y : L.y;
  // normal_vec.y = l(0) * p.x - p.y + l(1) > 0 ? L.x : -L.x;
  // double dist = abs(l(0) * p.x - p.y + l(1)) / sqrt(l(0) * l(0) + 1);
  //
  // Point new_p_star;
  // new_p_star.x = p_star.x + normal_vec.x * dist;
  // new_p_star.y = p_star.y + normal_vec.y * dist;

  // Lを返すのではなく素の傾きを返す説
  // return make_tuple(new_p_star, L, h);
  return make_tuple(new_p_star, p_tan, h);
}

/*
vector<tuple<Point, Point, double>> MLS(Tree t)
{
  Tree MST = prim(t);
  vector<tuple<Point, Point, double>> Sp = {};
  for(Point p : MST.vertex) {
    // std::cout << p.x << std::endl;
    vector<Point> A = {};
    // Sp.push_back(Collect2(p, p, A, 10, 0.7, 1, t));
    // Sp.push_back(Collect2(p, p, A, 80, 0.7, 4, MST)); // <- Good!
    Sp.push_back(Collect2(p, p, A, 10, 0.7, 4, MST)); //
    // Sp.push_back(Collect2(p, p, A, 10, 0.7, 1, MST));
  }
  return Sp;
}
*/

vector<tuple<Point, Point, double>> MLS(vector<Polyline> ps)
{
  Tree t(ps);
  // std::cout << "make tree" << std::endl;
  Tree MST = prim(t);
  std::cout << "prim finish" << std::endl;
  vector<tuple<Point, Point, double>> Sp = {};
  for(Polyline po : ps) {
    for(size_t i = 0; i < po.getPoints().size(); ++i) {
      Point p = po.getPoints().at(i);
      Point t = po.getTangents().at(i);
      // std::cout << p.x << std::endl;
      vector<Point> A = {};
      // Sp.push_back(Collect2(p, p, A, 10, 0.7, 1, t));
      // Sp.push_back(Collect2(p, p, A, 80, 0.7, 4, MST)); // <- Good!
      Sp.push_back(Collect2(p, p, A, 10, 0.7, 4, MST, t)); //
      // Sp.push_back(Collect2(p, p, A, 10, 0.7, 1, MST));
    }
  }
  return Sp;
}
