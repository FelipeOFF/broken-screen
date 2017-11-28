//
//  Utils.hpp
//  TelaDeCelular
//
//  Created by Felipe Oliveira on 23/11/17.
//  Copyright © 2017 Felipe Oliveira. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/objdetect.hpp>

using Contours = std::vector<std::vector<cv::Point>>;
using ContourSimple = std::vector<cv::Point>;
using Rects = std::vector<std::vector<cv::Rect>>;
using RectsSimple = std::vector<cv::Rect>;

namespace std{
    class Utils
    {
    private:
        int thresh;
        int N;
        int threshOfMescleRetct;

    public:
        Utils();
        Utils(int);
        Utils(int, int);
        Utils(int, int, int);
        ~Utils();

        void setThresh(int value) { thresh = value; }
        int getThresh() { return thresh; }
        void setN(int value) { N = value; }
        int getN() { return N; }
        void setThreshOfMescleRetct(int value) { threshOfMescleRetct = value; }
        int getThreshOfMescleRetct() { return threshOfMescleRetct; }

        static void Morph(const cv::Mat &, cv::Mat &, int = cv::MORPH_OPEN, int = cv::MORPH_RECT, int = 1);
        double angle(cv::Point, cv::Point, cv::Point);
        void clearContorns(Contours, Contours &, int, int, int, int);
        void findSquaresWithGray(const cv::Mat &, Contours &);
        void findSquares(const cv::Mat &, Contours &squares);
        void drawSquares(cv::Mat &, Contours &, cv::Scalar = cv::Scalar(0, 255, 0));
        void drawSquares(cv::Mat &, ContourSimple &, cv::Scalar = cv::Scalar(0, 255, 0));
        void drawSquares(cv::Mat &, RectsSimple &, cv::Scalar = cv::Scalar(0, 255, 0));
        ContourSimple contoursConvexHull(Contours);
        void contourJoin(Contours, ContourSimple &);
        void contourJoin(Contours, Contours &);
        void convertPolyToRect(Contours, RectsSimple &);
        int countAreaSum(Contours);
        double calcAreaOfAllRetangle(RectsSimple);
        void mescleRetangle(RectsSimple, RectsSimple &);
        double calcPercent(int, int);
        void getRetanglesByImage(const cv::Mat, bool, cv::Rect, RectsSimple &, cv::Rect &);
        double getRetanglesPercent(const cv::Mat, bool, cv::Rect);
    };
}

#endif /* Utils_hpp */
