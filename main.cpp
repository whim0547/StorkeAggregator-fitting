#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "polyline.hpp"
#include "fitting.hpp"
#include "cv.hpp"

int main()
{
  // vector<Polyline> cluster = {c1,c2,c3,c4,c5,c6,c7,c8,c9,c0};
  // 矢印キー
  const int CV_WAITKEY_CURSORKEY_TOP    = 2490368;
  const int CV_WAITKEY_CURSORKEY_BOTTOM = 2621440;
  const int CV_WAITKEY_CURSORKEY_RIGHT  = 2555904;
  const int CV_WAITKEY_CURSORKEY_LEFT   = 2424832;
  // エンターキーとか
  const int CV_WAITKEY_ENTER            = 13;

  std::cout << "start" << std::endl;
  vector<Rect> rect = {};
  cv::Mat img = init_img(&rect);

  cv::Mat draw_img = img.clone();

  string window_name = "Fitting";
  cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE); //Windowのタイトルとサイズ設定

  cv::setMouseCallback(window_name, mouse_callback, (void *)&rect);
  // cv::setMouseCallback(window_name, mouse_callback);
  cv::imshow(window_name, draw_img);
  while(true) {
    drawPolyline(&draw_img);
    drawLines(&draw_img, lines, 128, 128, 128);
    drawLines(&draw_img);
    drawParam(&draw_img);
    if(mode == CONFIRMING) {
      drawPolyline(&draw_img, fitting_line, 0, 0, 255);
    }
    cv::imshow(window_name, draw_img);
    draw_img = img.clone();
    int key = cv::waitKey(1);
    if(key == 'q')
      break;
    switch (key) {
      case 'z':
        undo();
        break;
      case 'x':
        redo();
        break;
      case 'u':
      case CV_WAITKEY_CURSORKEY_TOP:
        up();
        break;
      case 'd':
      case CV_WAITKEY_CURSORKEY_BOTTOM:
        down();
        break;
      case CV_WAITKEY_ENTER:
        fitting();
        break;
    }
  }
  //cv::waitKey(0);
  cv::destroyAllWindows();
  std::cout << "Finish" << std::endl;


  return 0;
}
