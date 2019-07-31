#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <array>
#include "cv.hpp"
#include "fitting.hpp"
#include "polyline.hpp"

#define MAX_INDEX 45

using namespace std;
Mode mode;
vector<Point> drawing_line;
vector<Polyline> lines;
vector<Polyline> confirmed_lines;
vector<Polyline> undo_lines;
vector<Point> fitting_line;
double fitting_param;

array<double, MAX_INDEX> fitting_params = {};
size_t fp_index = 9;
cv::Point param_line;

Rect::Rect() {}

Rect::Rect(int x1, int y1, int x2, int y2) {
  this->lu.x = x1;
  this->lu.y = y1;
  this->rd.x = x2;
  this->rd.y = y2;
}

bool Rect::isOn(int x, int y) {
  return x >= this->lu.x && x <= this->rd.x && y >= this->lu.y && y <= this->rd.y;
}


// void drawPolyline(cv::Mat* img, vector<Point> polyline = drawing_line, int r = 255, int g = 0, int b = 0) {
void drawPolyline(cv::Mat* img, vector<Point> polyline, int r, int g, int b) {
  size_t size = polyline.size();
  int counter = 0;
  cv::Point pt;
  cv::Point *pts = new cv::Point[size];
  const int npts = (int)size;
  // int ncontours = 1; //折れ線の数
  int thickness = 1;
  int lineType = 16;
  int shift = 1;

  for(Point p : polyline) {
    pt.x = (int)(p.x * (1 << shift));
    pt.y = (int)(p.y * (1 << shift));
    pts[counter] = pt;
    ++counter;
  }
  cv::polylines(*img, &pts, &npts, 1, false, cv::Scalar(r, g, b), thickness, lineType, shift);
  delete [] pts;
}


// void drawLines(cv::Mat* img, vector<Polyline> target_lines = confirmed_lines, int r = 0, int g = 0, int b = 0) {
void drawLines(cv::Mat* img, vector<Polyline> target_lines, int r, int g, int b) {
  for(Polyline pl : target_lines) {
    drawPolyline(img, pl.getPoints(), r, g, b);
  }
}

void drawParam(cv::Mat* img) {
  cv::putText(*img, std::to_string(fitting_param), param_line,
              cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(0));
}

void undo()
{
  switch(mode) {
    case DRAW:
      if(lines.empty()) {
        if(!confirmed_lines.empty()) {
          undo_lines.clear();
          Polyline l = confirmed_lines.back();
          undo_lines.push_back(l);
          confirmed_lines.pop_back();
          mode = CONFIRM;
        }
      } else {
        Polyline l = lines.back();
        undo_lines.push_back(l);
        lines.pop_back();
      }
      break;
    case CONFIRMING:
      mode = DRAW;
      break;
    case CONFIRM:
      if(!confirmed_lines.empty()) {
        Polyline l = confirmed_lines.back();
        undo_lines.push_back(l);
        confirmed_lines.pop_back();
      }
      break;
  }
}

void redo()
{
  if(undo_lines.empty()) return;
  Polyline l = undo_lines.back();
  switch(mode) {
    case DRAW:
      lines.push_back(l);
      undo_lines.pop_back();
      break;
    case CONFIRMING:
      break;
    case CONFIRM:
      confirmed_lines.push_back(l);
      undo_lines.pop_back();
      break;
  }
}

void up() {
  cout << "up" << endl;
  if(fp_index > 0) {
    --fp_index;
    fitting_param = fitting_params.at(fp_index);
  }
}

void down() {
  cout << "down" << endl;
  if(fp_index < MAX_INDEX - 1) {
    ++fp_index;
    fitting_param = fitting_params.at(fp_index);
  }
}

bool delay_counter = false;
void mouse_callback(int event, int x, int y, int flags, void *param)
{
  // cv::Mat *img = static_cast<cv::Mat*>(param);
  vector<Rect> *rect = static_cast<vector<Rect>*>(param);

  if(rect->at(0).isOn(x, y)) {
    // if (event == cv::EVENT_LBUTTONDOWN) {
    //   // cout << "qqq" << endl;
    // //   Point p(x, y);
    // //   drawing_line.push_back(p);
    // //   cout << "qqq2" << endl;
    // }
    // マウスの左ボタンがクリックされていて、マウスが動いたとき
    if (flags == cv::EVENT_FLAG_LBUTTON && event == cv::EVENT_MOUSEMOVE) {
      cout << "moving " << x << " " << y << endl;
      if(delay_counter == false) {
        undo_lines.clear();
      }
      delay_counter = true;
      mode = DRAW;
      // if(delay_counter % 2 == 0) {
      Point p(x, y);
      drawing_line.push_back(p);
        //debug
        // cv::circle(*img, cv::Point(x, y), 6, cv::Scalar(0, 0, 0));
      // }
    }
    if (event == cv::EVENT_LBUTTONUP) {
      if(delay_counter && drawing_line.size() > 1) {
        Point p(x, y);
        drawing_line.push_back(p);
        Polyline pl(drawing_line);
        lines.push_back(pl);
      }
      // drawPolyline(img);
      drawing_line.clear();
      delay_counter = false;
    }
  } else if (rect->at(1).isOn(x, y)) { //confirm
    if (event == cv::EVENT_LBUTTONDOWN) {
      cout << "confirm" << endl;
      fitting();
    }
  } else if (rect->at(2).isOn(x, y)) { //up
    if (event == cv::EVENT_LBUTTONDOWN) {
      up();
      if(mode == CONFIRMING) {
        fitting_line = tangentOptimization(Sp, fitting_param);
        std::cout << " tangentOptimization finished!" << std::endl;
      }
    }
  } else if (rect->at(3).isOn(x, y)) { //down
    if (event == cv::EVENT_LBUTTONDOWN) {
      down();
      if(mode == CONFIRMING) {
        fitting_line = tangentOptimization(Sp, fitting_param);
        std::cout << " tangentOptimization finished!" << std::endl;
      }
    }
  } else if (rect->at(4).isOn(x, y)) { //undo
    if (event == cv::EVENT_LBUTTONDOWN) {
      cout << "undo" << endl;
      undo();
    }
  } else if (rect->at(5).isOn(x, y)) { //redo
    if (event == cv::EVENT_LBUTTONDOWN) {
      redo();
    }
  } else if(delay_counter ) {
    // cout << "hoge1" << endl;
    if(drawing_line.size() > 1) {
      Polyline pl(drawing_line);
      lines.push_back(pl);
    }
    // cout << "hoge2" << endl;
    drawing_line.clear();
    delay_counter = false;
  }
}


void init() {
  // Mode mode = DRAW;
  // vector<Point> drawing_line = {};
  // vector<Polyline> lines = {};
  // vector<Polyline> confirmed_lines = {};
  // vector<Polyline> undo_lines = {};
  // vector<Point> fitting_line = {};
  // Mode mode = DRAW;
  // double fitting_param = 1e-3;

  drawing_line = {};
  lines = {};
  confirmed_lines = {};
  undo_lines = {};
  fitting_line = {};
  fitting_param = 1e-3;
  for(int i = 2; i < 7; ++i) {
    for(int j = 9; j > 0; --j) {
      size_t index = (size_t)((i - 2) * 9 + (9 - j));
      fitting_params.at(index) = j * pow(10, -i);
    }
  }
}

cv::Mat init_img(vector<Rect> *rect)
{
  init();

  int size_w = 1200;
  int size_h = 768;
  cv::Mat img(size_h, size_w, CV_8UC3, cv::Scalar(255,255,255));
  Rect draw_rect(10, 70, size_w - 200, size_h - 10);
  Rect confirm_rect(size_w - 180, 100, size_w - 20, 150);
  Rect up_rect(size_w - 180, 180, size_w - 110, 230);
  Rect down_rect(size_w - 90, 180, size_w - 20, 230);
  param_line.x = up_rect.lu.x;
  param_line.y = down_rect.rd.y + 50;
  Rect undo_rect(size_w - 180, size_h - 200, size_w - 110, size_h - 150);
  Rect redo_rect(size_w - 90, size_h - 200, size_w - 20, size_h - 150);
  vector<Rect> rs = {draw_rect, confirm_rect, up_rect, down_rect, undo_rect, redo_rect};

  int counter = 0;

  for(Rect r : rs) {
    if (counter == 0) {
      cv::rectangle(img, r.lu, r.rd, cv::Scalar(0,0,0), 5);
    } else {
      cv::rectangle(img, r.lu, r.rd, cv::Scalar(0,0,0), 1);
    }
    ++counter;
  }
  cv::putText(img, "Fitting", cv::Point(confirm_rect.lu.x, confirm_rect.rd.y),
              cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(0));
  cv::putText(img, "Up", cv::Point(up_rect.lu.x, up_rect.rd.y),
              cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(0));
  cv::putText(img, "Down", cv::Point(down_rect.lu.x, down_rect.rd.y),
              cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(0));
  cv::putText(img, "Undo", cv::Point(undo_rect.lu.x, undo_rect.rd.y),
              cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(0));
  cv::putText(img, "Redo", cv::Point(redo_rect.lu.x, redo_rect.rd.y),
              cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(0));
  cv::putText(img, "\"Enter\":fitting, \"z\":undo, \"x\":redo, \"UP\":up, \"DOWN\":down", cv::Point(10, 60),
              cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar::all(0));


  rect->swap(rs);
  return img;
}
