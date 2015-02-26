#include "opencv2\opencv.hpp"
#include <iostream>
#include <math.h>

using namespace cv;

#define ROUNDNUM(x) ((int)(x + 0.5f))

class Bounds {
public:
	int xMin, xMax, yMin, yMax;
	bool isObject();
};

const float AVERAGE = 1.0f;

void averageFrame(Mat& frame);

Vec3b getColorAt(Mat frame, int x, int y);

double getAvgColorAt(Mat frame, int x, int y);

Bounds findObject(Mat frame);

void drawObject(Mat& frame, Bounds draw);

void recursiveFlood(Mat frame, int x, int y, int cols, int rows, int& xMin, int& xMax, int& yMin, int& yMax);

void log(char* toPrint) {
	std::cout << toPrint << std::endl;
}

int main() {
	log("Starting..");

	VideoCapture camera(0);

	if (!camera.isOpened()) {
		log("Failed to load default camera!");
		return -1;
	}

	Mat edges;
	namedWindow("wat");
	resizeWindow("wat", 1024, 720);

	while (true) {
		Mat currentFrame;

		camera >> currentFrame;

		Mat detectFrame;

		currentFrame.copyTo(detectFrame);

		averageFrame(detectFrame);
		Bounds b = findObject(detectFrame);
		drawObject(currentFrame, b);

		imshow("wat", currentFrame);
		
		if (waitKey(30) >= 0) break;
	}

	return 0;
}

void averageFrame(Mat& frame) {
	unsigned char *input = (unsigned char*)(frame.data);

	for (int y = 0; y < frame.rows; y++) {
		Vec3b* row = frame.ptr<Vec3b>(y);
		for (int x = 0; x < frame.cols; x++) {
			Vec3b& color = row[x];

			color[0] = ROUNDNUM(color[0] / (255.0f / AVERAGE)) * (255.0f / AVERAGE);
			color[1] = ROUNDNUM(color[1] / (255.0f / AVERAGE)) * (255.0f / AVERAGE);
			color[2] = ROUNDNUM(color[2] / (255.0f / AVERAGE)) * (255.0f / AVERAGE);
		}
	}
}

Bounds findObject(Mat frame) {
	Bounds b;
	b.xMax = b.xMin = b.yMax = b.yMin = 0;
	for (int y = 0; y < frame.rows; y++) {
		Vec3b* row = frame.ptr<Vec3b>(y);
		for (int x = 0; x < frame.cols; x++) {
			Vec3b& color = row[x];
			Vec3b& tcolor = getColorAt(frame, x, y);
			double avg = (color[0] + color[1] + color[2]) / 3.0;
			if (abs(255 - avg) < 3) {
				int xMin = x, xMax = x, yMin = y, yMax = y;

				recursiveFlood(frame, x, y, frame.cols, frame.rows, xMin, xMax, yMin, yMax);

				/*while (xMin >= 0 && abs(255 - getAvgColorAt(frame, xMin, y)) < 3) {
					xMin--;
				}

				int xMax = x;
				while (xMax < frame.cols && abs(255 - getAvgColorAt(frame, xMax, y)) < 3) {
					xMax++;
				}

				int yMin = y;
				while (yMin >= 0 && abs(255 - getAvgColorAt(frame, x, yMin)) < 3) {
					yMin--;
				}

				int yMax = y;
				while (yMax < frame.rows && abs(255 - getAvgColorAt(frame, x, yMax)) < 3) {
					yMax++;
				}*/

				b.xMin = xMin;
				b.xMax = xMax;
				b.yMin = yMin;
				b.yMax = yMax;

				if (b.isObject())
					return b;
				
				//displayColor[0] = 0;
				//displayColor[1] = 255;
				//displayColor[2] = 0;
			}
		}
	}
	return b;
}

void recursiveFlood(Mat frame, int x, int y, int cols, int rows, int& xMin, int& xMax, int& yMin, int& yMax) {
	if (xMin > 0 && abs(255 - getAvgColorAt(frame, xMin, y)) < 3) {
		xMin--;
		recursiveFlood(frame, xMin, y, cols, rows, xMin, xMax, yMin, yMax);
	}
	if (xMax < cols && abs(255 - getAvgColorAt(frame, xMax, y)) < 3) {
		xMax++;
		recursiveFlood(frame, xMax, y, cols, rows, xMin, xMax, yMin, yMax);
	}
	if (yMin > 0 && abs(255 - getAvgColorAt(frame, x, yMin)) < 3) {
		yMin--;
		recursiveFlood(frame, x, yMin, cols, rows, xMin, xMax, yMin, yMax);
	}
	if (yMax < rows && abs(255 - getAvgColorAt(frame, x, yMax)) < 3) {
		yMax++;
		recursiveFlood(frame, x, yMax, cols, rows, xMin, xMax, yMin, yMax);
	}
}

void drawObject(Mat& frame, Bounds b) {
	for (int y = 0; y < frame.rows; y++) {
		Vec3b* row = frame.ptr<Vec3b>(y);
		for (int x = 0; x < frame.cols; x++) {
			Vec3b& color = row[x];

			if (x >= b.xMin && x <= b.xMax && y >= b.yMin && y <= b.yMax) {
				color[1] *= (1.0 / 255.0);
			}
		}
	}
}

double getAvgColorAt(Mat frame, int x, int y) {
	if (x < 0 || x >= frame.cols || y < 0 || y >= frame.rows)
		return 0;
	
	Vec3b pixel = getColorAt(frame, x, y);

	return (pixel[0] + pixel[1] + pixel[2]) / 3.0;
}

Vec3b getColorAt(Mat frame, int x, int y) {
	return frame.ptr<Vec3b>(y)[x];
}



bool Bounds::isObject() {
	return xMax - xMin > 20 && yMax - yMin > 30;
}

