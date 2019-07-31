#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "polyline.hpp"
#include "fitting.hpp"
#include "cv.hpp"

vector<tuple<Point, Point, double>> Sp;

void fitting() {
  vector<Polyline> oriented_cluster;
  switch(mode) {
    case DRAW:
      if(lines.size() > 1) {
        cout << "MODE:confirming" << endl;
        mode = CONFIRMING;
        oriented_cluster = strokeOrientation(lines);
        std::cout << "Oriented!!!" << std::endl;
        Sp = MLS(oriented_cluster);
        std::cout << "MLS finished!!!" << std::endl;
        Sp = polylineExtraction(Sp);
        pointNeighberMerge(&Sp);
        std::cout << " polylineExtraction finished!" << std::endl;
        fitting_line = tangentOptimization(Sp, fitting_param);
        std::cout << " tangentOptimization finished!" << std::endl;
      }
      break;
    case CONFIRMING:
      cout << "MODE:confirm" << endl;
      mode = CONFIRM;
      confirmed_lines.push_back(fitting_line);
      fitting_line.clear();
      // fitting_line = tangentOptimization(Sp, fitting_param);
      // std::cout << " tangentOptimization finished!" << std::endl;
      lines.clear();
    case CONFIRM:
      break;
  }
}
