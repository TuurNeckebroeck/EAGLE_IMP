#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include "main.h"


//#define DEBUG


int main() {
    cout << "START" << endl;

    //Mat original = imread("raster.png");
    //int resizeFactor = 2;
    //resize(original,original,Size(original.cols/resizeFactor, original.rows/resizeFactor));

    VideoCapture cap("easy.mp4");
    if(!cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return 111;
    }

    for(;;) {
        Mat original;
        cap >> original;

        if(original.empty()) break;

        Mat img, hsv, red_mask;
        img = original.clone();
        cvtColor(img, hsv, cv::COLOR_RGB2HSV);
        inRange(hsv, Scalar(100, 20, 0), Scalar(135, 255, 255), red_mask);


        vector<Vec2f> allLines;
        vector<Vec2f> nonSimilarLines;
        HoughLines(red_mask, allLines, 1, CV_PI / 180, 250, 0, 0);

        for (size_t i = 0; i < allLines.size(); i++) {
            if(!containsSimilarLine(nonSimilarLines, allLines[i])){
                nonSimilarLines.push_back(allLines[i]);
            }

            // ILLUSTRATIEF VOOR OP EXTRA SCHERM: TOONT ALLE GEDETECTEERDE LIJNEN
            float rho = allLines[i][0], theta = allLines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            pt1.x = cvRound(x0 + 1000 * (-b));
            pt1.y = cvRound(y0 + 1000 * (a));
            pt2.x = cvRound(x0 - 1000 * (-b));
            pt2.y = cvRound(y0 - 1000 * (a));
            line(img, pt1, pt2, Scalar(0, 0, 200), 1, LINE_AA);
        }

#ifdef DEBUG
        cout << "\tNumber of lines detected: " << allLines.size() << "\t\t-->\t\t" << nonSimilarLines.size() << endl;
#endif

        for (size_t i = 0; i < nonSimilarLines.size(); i++) {
            float rho = nonSimilarLines[i][0], theta = nonSimilarLines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            pt1.x = cvRound(x0 + 1000 * (-b));
            pt1.y = cvRound(y0 + 1000 * (a));
            pt2.x = cvRound(x0 - 1000 * (-b));
            pt2.y = cvRound(y0 - 1000 * (a));
            line(img, pt1, pt2, Scalar(0, 255, 0), 1, LINE_AA);
        }


        imshow("Origineel", original);
        imshow("Detectie rood", red_mask);
        imshow("Detectie rode lijnen", img);

        if(nonSimilarLines.size()<4){
            cout << "LESS THAN 4 LINES DETECTED" << endl;
            // Ligt aan de video!! Er wordt van te dichtbij gefilmd: er zijn effectief maar 3 lijnen zichtbaar (1 horizontale)

#ifdef DEBUG
            for(size_t i = 0; i<allLines.size();i++){
                cout << "ALL    \t\t" << allLines[i][0] << "  \t\t" << allLines[i][1] << endl;
            }

            for(size_t i = 0; i<nonSimilarLines.size();i++){
                cout << "NONSIM \t\t" << nonSimilarLines[i][0] << "  \t\t" << nonSimilarLines[i][1] << endl;
            }
#endif

        }

        if (waitKey(20) >= 0) break;
    }


    return 0;
}

bool containsSimilarLine(vector<Vec2f>& lines, Vec2f& line){
    float angleTreshold = 0.25, distanceTreshold = 30;


    //cout << "Rho :" << line[0] << "  \t\ttheta: " << line[1] << endl;
    for(size_t i=0; i<lines.size(); i++){
        Vec2f oldLine = lines[i];

        // index 0: rho (distance)  index 1: theta (angle)
        float rho1 = oldLine[0], rho2 = line[0], theta1 = oldLine[1], theta2 = line[1];


        if(abs(theta1 - CV_PI) < angleTreshold){
            theta1 -= CV_PI;
            rho1 = -rho1;
        }

        if(abs(theta2 - CV_PI) < angleTreshold){
            theta2 -= CV_PI;
            rho2 = -rho2;
        }


        if(abs(theta1 - theta2) < angleTreshold && abs(rho1 - rho2) < distanceTreshold) {
            return true;
        }

    }
    return false;
}