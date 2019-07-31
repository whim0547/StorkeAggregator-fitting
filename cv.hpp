#pragma once

#include <opencv2/opencv.hpp>
#include "polyline.hpp"

using namespace std;
extern vector<Point> drawing_line;
extern vector<Polyline> lines;
extern vector<Polyline> confirmed_lines;
extern vector<Point> fitting_line;
extern vector<Polyline> undo_lines;
extern double fitting_param;
extern vector<tuple<Point, Point, double>> Sp;

enum Mode {
  DRAW = 1, //1: draw, 2: confirming, 3: confirm
  CONFIRMING = 2,
  CONFIRM = 3,
};

extern Mode mode;

class Rect {
public:
  Rect();

  Rect(int x1, int y1, int x2, int y2);

  bool isOn(int x, int y);

  cv::Point lu;//左上点
  cv::Point rd;//右下点
};

void drawPolyline(cv::Mat* img, vector<Point> polyline = drawing_line, int r = 255, int g = 0, int b = 0);
void drawLines(cv::Mat* img, vector<Polyline> target_lines = confirmed_lines, int r = 0, int g = 0, int b = 0);
void drawParam(cv::Mat* img);
void undo();
void redo();
void up();
void down();
void mouse_callback(int event, int x, int y, int flags, void *param);
cv::Mat init_img(vector<Rect> *rect);
