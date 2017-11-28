//
//  Utils.cpp
//  TelaDeCelular
//
//  Utils para calcular.
//
//  Created by Felipe Oliveira on 23/11/17.
//  Copyright © 2017 Felipe Oliveira. All rights reserved.
//

#include "Utils.hpp"
#include <iostream>

using namespace std;

Utils::Utils() : thresh(20), N(10), threshOfMescleRetct(100)
{
}

Utils::Utils(int thresh) : N(10), threshOfMescleRetct(100)
{
    this->setThresh(thresh);
}

Utils::Utils(int thresh, int N) : threshOfMescleRetct(100)
{
    this->setThresh(thresh);
    this->setN(N);
}

Utils::Utils(int thresh, int N, int threshOfMescleRetct)
{
    this->setThresh(thresh);
    this->setN(N);
    this->setThreshOfMescleRetct(threshOfMescleRetct);
}

Utils::~Utils()
{
}

double Utils::angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

void Utils::clearContorns(Contours input, Contours &output, int maxSizeWidth, int maxSizeHeight, int mimSizeWidth, int mimSizeHeight)
{
    ContourSimple tmp;

    for (int i = 0; i < static_cast<int>(input.size()); i++)
        for (int j = 0; j < static_cast<int>(input[i].size()); j++)
            if (input[i][j].x >= mimSizeWidth && input[i][j].y >= mimSizeHeight && input[i][j].x <= maxSizeWidth && input[i][j].y <= maxSizeHeight)
                tmp.push_back(input[i][j]);

    output.push_back(tmp);
    tmp.clear();
}

void Utils::findSquaresWithGray(const cv::Mat &image, Contours &squares)
{
    Contours contours;

    // find contours and store them all as a list
    findContours(image, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    ContourSimple approx;

    // test each contour
    for (size_t i = 0; i < contours.size(); i++)
    {
        // approximate contour with accuracy proportional
        // to the contour perimeter
        approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true) * 0.02, true);

        // square contours should have 4 vertices after approximation
        // relatively large area (to filter out noisy contours)
        // and be convex.
        // Note: absolute value of an area is used because
        // area may be positive or negative - in accordance with the
        // contour orientation
        if (approx.size() == 4 &&
            fabs(contourArea(cv::Mat(approx))) > 1000 &&
            isContourConvex(cv::Mat(approx)))
        {
            double maxCosine = 0;

            for (int j = 2; j < 5; j++)
            {
                // find the maximum cosine of the angle between joint edges
                double cosine = fabs(Utils::angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                maxCosine = MAX(maxCosine, cosine);
            }

            // if cosines of all angles are small
            // (all angles are ~90 degree) then write quandrange
            // vertices to resultant sequence
            if (maxCosine < 0.3)
                squares.push_back(approx);
        }
    }
}

void Utils::findSquares(const cv::Mat &image, Contours &squares)
{
    squares.clear();

    cv::Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, cv::Size(image.cols / 2, image.rows / 2));
    pyrUp(pyr, timg, image.size());
    Contours contours;

    // find squares in every color plane of the image
    for (int c = 0; c < 3; c++)
    {
        int ch[] = {c, 0};
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for (int l = 0; l < N; l++)
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if (l == 0)
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l + 1) * 255 / N;
            }

            // find contours and store them all as a list
            findContours(gray, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

            ContourSimple approx;

            // test each contour
            for (size_t i = 0; i < contours.size(); i++)
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true) * 0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if (approx.size() == 4 &&
                    fabs(contourArea(cv::Mat(approx))) > 1000 &&
                    isContourConvex(cv::Mat(approx)))
                {
                    double maxCosine = 0;

                    for (int j = 2; j < 5; j++)
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if (maxCosine < 0.3)
                        squares.push_back(approx);
                }
            }
        }
    }
}

void Utils::drawSquares(cv::Mat &image, Contours &squares, cv::Scalar s)
{
    for (size_t i = 0; i < squares.size(); i++)
    {
        const cv::Point *p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, s, 1, cv::LINE_AA);
    }
}

void Utils::drawSquares(cv::Mat &image, ContourSimple &squares, cv::Scalar s)
{
    const cv::Point *p = &squares[0];
    int n = static_cast<int>(squares.size());
    polylines(image, &p, &n, 1, true, s, 1, cv::LINE_AA);
}

void Utils::drawSquares(cv::Mat &image, RectsSimple &squares, cv::Scalar s)
{
    for (int i = 0; i < static_cast<int>(squares.size()); i++)
    {
        rectangle(image, squares[i], s, CV_FILLED, cv::LINE_AA);
    }
}

ContourSimple Utils::contoursConvexHull(Contours contours)
{
    ContourSimple result;
    ContourSimple pts;
    for (size_t i = 0; i < contours.size(); i++)
        for (size_t j = 0; j < contours[i].size(); j++)
            pts.push_back(contours[i][j]);

    convexHull(pts, result);
    return result;
}

void Utils::contourJoin(Contours input, ContourSimple &contors)
{
    for (int i = 0; i < static_cast<int>(input.size()); i++)
    {
        for (int j = 0; j < static_cast<int>(input[i].size()); j++)
        {
            ContourSimple p;
            ContourSimple pe;
            convexHull(input[i], p);
            approxPolyDP(p, pe, 1000, true);
            for (int c = 0; c < static_cast<int>(pe.size()); c++)
            {
                contors.push_back(pe[c]);
            }
        }
    }
}

void Utils::contourJoin(Contours input, Contours &contors)
{
    for (int i = 0; i < static_cast<int>(input.size()); i++)
    {
        ContourSimple p;
        ContourSimple pe;
        approxPolyDP(input[i], p, 0.1, true);
        convexHull(p, pe);
        contors.push_back(pe);
    }
}

void Utils::convertPolyToRect(Contours input, RectsSimple &re)
{
    for (int i = 0; i < static_cast<int>(input.size()); i++)
    {
        re.push_back(boundingRect(input[i]));
    }
}

int Utils::countAreaSum(Contours points)
{
    int sum = 0;
    for (int i = 0; i < static_cast<int>(points.size()); i++)
        sum += contourArea(points[i]);

    return sum;
}

double Utils::calcAreaOfAllRetangle(RectsSimple retangles)
{
    double result = 0.0;
    for (int i = 0; i < static_cast<int>(retangles.size()); i++)
    {
        result += retangles[i].area();
    }

    return result;
}

void Utils::mescleRetangle(RectsSimple input, RectsSimple &out)
{
    for (int i = 0; i < static_cast<int>(input.size()); i++)
    {
        if ((i + 1) >= static_cast<int>(input.size()))
            continue;

        cv::Rect first = input[i];
        cv::Rect second = input[i + 1];
        if (second.x < (first.x + threshOfMescleRetct) && second.y < (first.y + threshOfMescleRetct))
        {
            out.push_back(first | second);
        }
        i++;
    }
}

void Utils::Morph(const cv::Mat &src, cv::Mat &dst, int operation, int kernel_type, int size)
{
    cv::Point anchor = cv::Point(size, size);
    cv::Mat element = getStructuringElement(kernel_type, cv::Size(2 * size + 1, 2 * size + 1), anchor);
    morphologyEx(src, dst, operation, element, anchor);
}

double Utils::calcPercent(int contornCalc, int totalMaximo)
{
    return (static_cast<double>(contornCalc * 100) / static_cast<double>(totalMaximo));
}

void Utils::getRetanglesByImage(cv::Mat image, bool delimitationActive, cv::Rect delimitation, RectsSimple &simpleRetangleOutput, cv::Rect& delimited)
{

    if (image.empty())
    {
        throw std::invalid_argument("File not found or image 0x0");
        return;
    }

    Contours squares;
    cv::Mat copyImage(image), edges, celMask, cracks;

    cv::cvtColor(image, image, CV_RGB2GRAY);

    cv::Canny(image, edges, 195, 200, 3);
    this->Morph(edges, edges, cv::MORPH_CLOSE, cv::MORPH_ELLIPSE, 0);

    cv::threshold(image, celMask, 0, 255, cv::THRESH_OTSU);
    this->Morph(celMask, celMask, cv::MORPH_ERODE, cv::MORPH_ELLIPSE, 0);
    cv::bitwise_not(celMask, celMask);

    cracks = (edges & celMask);

    squares.clear();
    this->findSquaresWithGray(celMask, squares);

    Contours pontos;

    if (!delimitationActive)
    {
        if (squares.empty())
        {
            // Not found contorn retangle black im image.
            cv::floodFill(edges, cv::Point(0, 0), cv::Scalar(255));
            cv::findContours(cracks, pontos, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
            delimited = cv::Rect(0, 0, copyImage.cols, copyImage.rows);
        }
        else
        {
            copyImage = copyImage(cv::Rect(squares[0][0], squares[0][2]));
            cracks = cracks(cv::Rect(squares[0][0], squares[0][2]));
            delimited = cv::Rect(squares[0][0], squares[0][2]);

            cv::floodFill(cracks, cv::Point(0, 0), cv::Scalar(255));
            cv::findContours(cracks, pontos, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        }
    }
    else
    {
        copyImage = copyImage(delimitation);
        cracks = cracks(delimitation);
        delimited = delimitation;

        cv::floodFill(cracks, cv::Point(0, 0), cv::Scalar(255));
        cv::findContours(cracks, pontos, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    }

    ContourSimple ps = this->contoursConvexHull(pontos);

    Contours pontos2;
    this->contourJoin(pontos, pontos2);

    RectsSimple retangulos;
    this->convertPolyToRect(pontos2, retangulos);

    this->mescleRetangle(retangulos, simpleRetangleOutput);


}

double Utils::getRetanglesPercent(cv::Mat image, bool delimitationActive, cv::Rect delimitation)
{
    if (image.empty())
    {
        throw std::invalid_argument("File not found or image 0x0");
        return -1.0;
    }

    Contours squares;
    cv::Mat copyImage(image), edges, celMask, cracks;

    cv::cvtColor(image, image, CV_RGB2GRAY);

    cv::Canny(image, edges, 195, 200, 3);
    this->Morph(edges, edges, cv::MORPH_CLOSE, cv::MORPH_ELLIPSE, 0);

    cv::threshold(image, celMask, 0, 255, cv::THRESH_OTSU);
    this->Morph(celMask, celMask, cv::MORPH_ERODE, cv::MORPH_ELLIPSE, 0);
    cv::bitwise_not(celMask, celMask);

    cracks = (edges & celMask);

    squares.clear();
    this->findSquaresWithGray(celMask, squares);

    Contours pontos;

    if (!delimitationActive)
    {
        if (squares.empty())
        {
            // Not found contorn retangle black im image.
            cv::floodFill(edges, cv::Point(0, 0), cv::Scalar(255));
            cv::findContours(cracks, pontos, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        }
        else
        {
            copyImage = copyImage(cv::Rect(squares[0][0], squares[0][2]));
            cracks = cracks(cv::Rect(squares[0][0], squares[0][2]));

            cv::floodFill(cracks, cv::Point(0, 0), cv::Scalar(255));
            cv::findContours(cracks, pontos, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        }
    }
    else
    {
        copyImage = copyImage(delimitation);
        cracks = cracks(delimitation);

        cv::floodFill(cracks, cv::Point(0, 0), cv::Scalar(255));
        cv::findContours(cracks, pontos, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    }

    ContourSimple ps = this->contoursConvexHull(pontos);

    Contours pontos2;
    this->contourJoin(pontos, pontos2);

    RectsSimple retangulos;
    this->convertPolyToRect(pontos2, retangulos);

    RectsSimple output;

    this->mescleRetangle(retangulos, output);

    double contornCalc = this->calcAreaOfAllRetangle(output);

    double totalMaximo = copyImage.rows * copyImage.cols;

    double calcPercent = this->calcPercent(contornCalc, totalMaximo);

    return (calcPercent > 100.0 ? 100.0 : calcPercent);
}