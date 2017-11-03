﻿#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace cv;
using namespace std;

Mat img; // startscreen Image
Mat imgTmp; // Tempeary image for the line 
Mat imgLines; // The line from the detect object
Mat imgOriginal; //Camera image
Mat imgHSV; // First HSV Image
Mat imgHSV2; // Second HSV Image
Mat imgThresholded; // First Threshold image
Mat imgThresholded2; // Second Threshold image
Mat imgBothThresh; // A combined Threshold image
Moments oMoments; // moments ? 
POINT p; // point
Mat frame; // VIdeo

int iLowH; // First low H value
int iHighH; // First High H value
int iLowS; // First Low S Value
int iHighS; // First High S value
int iLowV; // First low V value
int iHighV; // First High V value

int iLowH2; // Second low H value
int iHighH2; // Second High H value
int iLowS2; // Second low S value
int iHighS2; // Second High S value
int iLowV2; // Second low V value
int iHighV2; // Second High V value

int xposition = 0; // X position for the cursor
int yposition = 0; // Y position for the cursor
int time = 0; // Time where the cursor is
int iLastX = -1;
int iLastY = -1;

void objectTracking();

 int main(int argc, char** argv)
{

	VideoCapture cap(0); //capture the video from webcam

	if (!cap.isOpened())  // if not success, exit program
	{
		return -1;
	}

	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280); // Size of the camera
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	cap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G')); // set the source, so it runs faster
	

	// picture/ gamewindow
	namedWindow("Game", CV_WINDOW_AUTOSIZE);
	img = imread("1.jpg");
	imshow("Game", img);

	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	//red
	iLowH = 107;
	iHighH = 179;

	iLowS = 175;
	iHighS = 255;

	iLowV = 43;
	iHighV = 190;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	namedWindow("Control2", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	//Green
	iLowH2 = 48;
	iHighH2 = 85;

	iLowS2 = 105;
	iHighS2 = 255;

	iLowV2 = 48;
	iHighV2 = 225;

	//Create trackbars in "Control" window
	createTrackbar("LowH2", "Control2", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH2", "Control2", &iHighH, 179);

	createTrackbar("LowS2", "Control2", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS2", "Control2", &iHighS, 255);

	createTrackbar("LowV2", "Control2", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV2", "Control2", &iHighV, 255);



	//Capture a temporary image from the camera
	cap.read(imgTmp);
	

	//Create a black image with the size as the camera output
	imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;


	while (true)
	{

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{

			break;
		}

		objectTracking();

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{

			break;
		}
				
			
	}
		return(0);

	} 

 void objectTracking()
 {

	 cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	 cvtColor(imgOriginal, imgHSV2, COLOR_BGR2HSV);
	 inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image	
	 inRange(imgHSV2, Scalar(iLowH2, iLowS2, iLowV2), Scalar(iHighH2, iHighS2, iHighV2), imgThresholded2); //Threshold the image
	 imgBothThresh = imgThresholded + imgThresholded2;
	 imshow("Both THresh", imgBothThresh);

	 //morphological opening (removes small objects from the foreground)
	 erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	 dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	 //morphological closing (removes small holes from the foreground)
	 dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	 erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	 //morphological opening (removes small objects from the foreground)
	 erode(imgThresholded2, imgThresholded2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	 dilate(imgThresholded2, imgThresholded2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	 //morphological closing (removes small holes from the foreground)
	 dilate(imgThresholded2, imgThresholded2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	 erode(imgThresholded2, imgThresholded2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	 //Calculate the moments of the thresholded image
	 oMoments = moments(imgBothThresh);

	 double dM01 = oMoments.m01;
	 double dM10 = oMoments.m10;
	 double dArea = oMoments.m00;

	 // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
	 if (dArea > 10000)
	 {
		 //calculate the position of the object
		 double posX = dM10 / dArea;
		 double posY = dM01 / dArea;

		 if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
		 {
			 //Draw a red line from the previous point to the current point
			 line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
		 }

		 iLastX = posX;
		 iLastY = posY;
	 }

	 SetCursorPos(iLastX, iLastY); // Position of the cursor

								   //imshow("Thresholded Image", imgThresholded); //show the first thresholded image
								   //imshow("Thresholded Image2", imgThresholded2);//show the second thresholded image

	 imgOriginal = imgOriginal + imgLines;
	// imshow("Original", imgOriginal); //show the original image

	

	 //while (true) {

	 if (GetCursorPos(&p) && xposition != p.x && yposition != p.y)
	 {
		 xposition = p.x;
		 yposition = p.y;
		 //cursor position now in p.x and p.y

	 }
	 //cout << p.x << ",," << p.y << ",," ;

	 //}

	 //symbol detection
	 //if(


	 //if statement to check the time inside area
	 if ((480 < xposition) && (xposition < 744) && (151 < yposition) && (yposition < 252)) {
		 time++;
		 cout << time;
	 }
	 else {
		 time = 0;

		// cout << time;

	 }
	 // If cursor is inside area and have been there for a few seconds
	 if ((480 < xposition) && (xposition < 744) && (151 < yposition) && (yposition < 252) && (time >= 20))
	 {
		 namedWindow("Game", CV_WINDOW_FULLSCREEN);

		 // video player
		 namedWindow("Sample Video", 1);

		 VideoCapture cap("319.avi");
		 // if (!cap.isOpened())  // check if we succeeded
			 // return -1;

		 while (1) {
			 cap >> frame;
			 imshow("Sample Video", frame);
			 char c = waitKey(33);
			 if (c == 0)
				 break;

		 }
	 }
 }


	//detect specific size of object
	//Make it detect symbols drawn
	// remove background image
	//http://opencv-srf.blogspot.dk/2011/11/mouse-events.html
	//https://en.sfml-dev.org/forums/index.php?topic=8599.0
	//https://www.funprogramming.org/95-Is-the-mouse-inside-a-square.html
	//https://stackoverflow.com/questions/14090906/make-an-object-clickable-click-if-the-mouse-is-in-the-area-of-object
	//http://opencv-srf.blogspot.dk/2011/09/capturing-images-videos.html
