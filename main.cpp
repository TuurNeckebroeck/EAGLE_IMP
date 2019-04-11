#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include "main.h"


//#define DEBUG

const float ANGLE_TRESHOLD = 0.25, DISTANCE_TRESHOLD = 30;





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
            /*float rho = allLines[i][0], theta = allLines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            circle(img, Point(x0,y0), 10, Scalar(255,0,0), 3, LINE_AA);
            pt1.x = cvRound(x0 + 1000 * (-b));
            pt1.y = cvRound(y0 + 1000 * (a));
            pt2.x = cvRound(x0 - 1000 * (-b));
            pt2.y = cvRound(y0 - 1000 * (a));
            line(img, pt1, pt2, Scalar(0, 0, 200), 1, LINE_AA);*/
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

        //circle(img, Point(0,0), 10, Scalar(0,0,255), 3, LINE_AA);
        vector<Point> corners;
        Vec2f relativePos;
        getCornerPoints(corners, relativePos, nonSimilarLines);
        for(size_t i = 0; i<corners.size(); i++){
            circle(img, corners[i], 10, Scalar(40,255,255), 3, LINE_AA);
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


    //cout << "Rho :" << line[0] << "  \t\ttheta: " << line[1] << endl;
    for(size_t i=0; i<lines.size(); i++){
        Vec2f oldLine = lines[i];

        // index 0: rho (distance)  index 1: theta (angle)
        float rho1 = oldLine[0], rho2 = line[0], theta1 = oldLine[1], theta2 = line[1];


        if(abs(theta1 - CV_PI) < ANGLE_TRESHOLD){
            theta1 -= CV_PI;
            rho1 = -rho1;
        }

        if(abs(theta2 - CV_PI) < ANGLE_TRESHOLD){
            theta2 -= CV_PI;
            rho2 = -rho2;
        }


        if(abs(theta1 - theta2) < ANGLE_TRESHOLD && abs(rho1 - rho2) < DISTANCE_TRESHOLD) {
            return true;
        }

    }
    return false;
}

void getCornerPoints(vector<Point>& cornerPoints, Vec2f& relativePos, vector<Vec2f>& lines){
    // TODO: Lijnen sorteren op verschil tussen theta en 0/PI -> misschien dmv min(theta, CV_PI - theta)
    // Uit deze volgorde weet men dan welke 2 lijnen +- evenwijdig lopen: men kan nu de hoeken beginnen zoekenv
    // Dit sorteren gebeurt misschien beter tijdens het pushen van de lijnen op de array -> efficienter aangezien
    // Er maar 2 soorten lijnen uit elkaar gehouden moeten worden

    if(lines.size() != 4) {
        cout << "!! HOEKEN SORTEREN MET AANTAL LIJNEN ANDERS DAN 4 KAN FOUTEN GEVEN !!" << endl;
    }

    //vector<Point> cornerPoints;
    vector<Vec2f> groupALines, groupBLines;
    vector<Vec2f> * firstGroupLines = &groupALines, * secGroupLines=&groupBLines;

    float firstAngle = lines[0][1];
    groupALines.push_back(lines[0]);

    for(size_t i = 1; i<lines.size(); i++) {
        Vec2f l = lines[i];
        if(abs(l[1] - firstAngle) > 2*ANGLE_TRESHOLD){
            groupBLines.push_back(l);
        } else{
            groupALines.push_back(l);
        }
    }

    if(abs(firstAngle) < ANGLE_TRESHOLD || abs(firstAngle - CV_PI) < ANGLE_TRESHOLD){
        vector<Vec2f> * tmpGroupLines = firstGroupLines;
        firstGroupLines = secGroupLines;
        secGroupLines = tmpGroupLines;
    }

    // firstGroup =

    for(size_t f = 0; f < (*firstGroupLines).size(); f++){
        float r1 = (*firstGroupLines)[f][0], th1 = (*firstGroupLines)[f][1];
        float rico = - (cos(th1)/sin(th1)), interc = r1 / sin(th1);
        for(size_t s=0; s<(*secGroupLines).size(); s++){
            // TODO x en y van snijpunt bepalen: zie opencv pagina over hough transform
            float r2 = (*secGroupLines)[s][0], th2 = (*secGroupLines)[s][1];
            float x = (r2*sin(th1) - r1*sin(th2)) / sin(th1-th2),
            y = rico * x + interc;

            cornerPoints.push_back(Point(x,y));
        }
    }

    float yEval

}

float evaluateLineIn(Vec2f& line, float& x){
    float r1 = line[0], th1 = line[1];
    float rico = - (cos(th1)/sin(th1)), interc = r1 / sin(th1);
    return rico * x + interc;
}

//vector<float> getRelativePosition(vector<Vec2f>& lines)