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

void averageFrame(int rows, int cols, Vec3b** frame);

Vec3b getColorAt(Mat frame, int x, int y);

double getAvgColorAt(Vec3b** frame, int x, int y, int rows, int cols);

Bounds findObject(int rows, int cols, Vec3b** frame);

void drawObject(Mat& frame, Bounds draw);

void recursiveFlood(Vec3b** frame, int x, int y, int cols, int rows, int& xMin, int& xMax, int& yMin, int& yMax, int lastAvg);

Vec3b** toArray(Mat frame);

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

	while (true) {
		Mat currentFrame;

		camera >> currentFrame;

		Mat detectFrame;

		currentFrame.copyTo(detectFrame);

		Vec3b** detectArray = toArray(detectFrame);

		averageFrame(detectFrame.rows, detectFrame.cols, detectArray);

		Bounds b = findObject(detectFrame.rows, detectFrame.cols, detectArray);
		drawObject(currentFrame, b);

		//imshow("wat", detectFrame);
		imshow("wat", currentFrame);

		delete detectArray;
		
		if (waitKey(30) >= 0) break;
	}

	return 0;
}

void averageFrame(int rows, int cols, Vec3b **frame) {

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			Vec3b& color = frame[y][x];

			color[0] = ROUNDNUM(color[0] / (255.0f / AVERAGE)) * (255.0f / AVERAGE);
			color[1] = ROUNDNUM(color[1] / (255.0f / AVERAGE)) * (255.0f / AVERAGE);
			color[2] = ROUNDNUM(color[2] / (255.0f / AVERAGE)) * (255.0f / AVERAGE);
		}
	}
}

Vec3b** toArray(Mat frame) {
	Vec3b** toReturn = new Vec3b*[frame.rows];
	for (int y = 0; y < frame.rows; y++) {
		toReturn[y] = frame.ptr<Vec3b>(y);
	}
	return toReturn;
}

Bounds findObject(int rows, int cols, Vec3b** frame) {
	Bounds b;
	b.xMax = b.xMin = b.yMax = b.yMin = 0;
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			Vec3b& color = frame[y][x];
			double avg = (color[0] + color[1] + color[2]) / 3.0;
			if (abs(255 - avg) < 50) {
				int xMin = x, xMax = x, yMin = y, yMax = y;

				recursiveFlood(frame, x, y, cols, rows, xMin, xMax, yMin, yMax, avg);

				b.xMin = xMin;
				b.xMax = xMax;
				b.yMin = yMin;
				b.yMax = yMax;

				if (b.isObject())
					return b;
				else {
					x = xMax;
					y = yMax;
				}
			}
		}
	}
	return b;
}

void recursiveFlood(Vec3b** frame, int x, int y, int cols, int rows, int& xMin, int& xMax, int& yMin, int& yMax, int lastAvg) {
	if (xMin > 0 && abs(lastAvg - getAvgColorAt(frame, xMin, y, rows, cols)) < 3) {
		xMin--;
		recursiveFlood(frame, xMin, y, cols, rows, xMin, xMax, yMin, yMax, lastAvg);
	}
	if (xMax < cols && abs(lastAvg - getAvgColorAt(frame, xMax, y, rows, cols)) < 3) {
		xMax++;
		recursiveFlood(frame, xMax, y, cols, rows, xMin, xMax, yMin, yMax, lastAvg);
	}
	if (yMin > 0 && abs(lastAvg - getAvgColorAt(frame, x, yMin, rows, cols)) < 3) {
		yMin--;
		recursiveFlood(frame, x, yMin, cols, rows, xMin, xMax, yMin, yMax, lastAvg);
	}
	if (yMax < rows && abs(lastAvg - getAvgColorAt(frame, x, yMax, rows, cols)) < 3) {
		yMax++;
		recursiveFlood(frame, x, yMax, cols, rows, xMin, xMax, yMin, yMax, lastAvg);
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

double getAvgColorAt(Vec3b** frame, int x, int y, int rows, int cols) {
	if (x < 0 || x >= cols || y < 0 || y >= rows)
		return 0;
	
	Vec3b pixel = frame[y][x];

	return (pixel[0] + pixel[1] + pixel[2]) / 3.0;
}

Vec3b getColorAt(Mat frame, int x, int y) {
	return frame.ptr<Vec3b>(y)[x];
}



bool Bounds::isObject() {
	return xMax - xMin > 50 && yMax - yMin > 80;
}

