#include "opencv2\opencv.hpp"
#include <iostream>
#include <math.h>

using namespace cv;

#define ROUNDNUM(x) ((int)(x + 0.5f))

const float AVERAGE = 2.0f;

void averageFrame(Mat& frame);

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

		averageFrame(currentFrame);

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

