//
// Created by tuur on 07/04/19.
//

#ifndef IMP_TUUR_MAIN_H
#define IMP_TUUR_MAIN_H

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

bool containsSimilarLine(vector<Vec2f>&, Vec2f&);
void getCornerPoints(vector<Point>& cornerPoints, Vec2f& relativePos, vector<Vec2f>& lines);
#endif //IMP_TUUR_MAIN_H
