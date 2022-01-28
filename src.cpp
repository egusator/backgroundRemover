#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>

#define INPUT_VIDEO_FILE_NAME "live.avi"
#define OUTPUT_VIDEO_FILE_NAME "recorded.avi"
#define BACKGROUND_IMAGE_FILE_NAME "fon.jpg"
#define MASK_IMAGE_FILE_NAME "mask.jpg"

using namespace std;
using namespace cv;

int main(int, char *argv[]) {

	Mat sourceImage, backgroundImage = imread(BACKGROUND_IMAGE_FILE_NAME),
			resizedBackgroundImage;

	VideoCapture inputVideo(INPUT_VIDEO_FILE_NAME);

	int frame_width = inputVideo.get(cv::CAP_PROP_FRAME_WIDTH),
		frame_height = inputVideo.get(cv::CAP_PROP_FRAME_HEIGHT),
		fps = inputVideo.get(CAP_PROP_FPS);


	VideoWriter outputVideo(OUTPUT_VIDEO_FILE_NAME,
			VideoWriter::fourcc('M', 'J', 'P', 'G'),
			fps, Size(frame_width, frame_height));

	while (true) {

		inputVideo >> sourceImage;

		if (sourceImage.empty())
		    {
		        cout << "Video reading finished \n";
		        break;
		    }

		// if u wanna equalize the image before doing mask
		/*
		 vector<Mat> bgr_planes;
		 split(sourceImage, bgr_planes);

		 equalizeHist(bgr_planes[0], bgr_planes[0]);
		 equalizeHist(bgr_planes[1], bgr_planes[1]);
		 equalizeHist(bgr_planes[2], bgr_planes[2]);

		 merge(bgr_planes, sourceImage);
		 */

		Vec3b randomBackgroundPixel = sourceImage.at<Vec3b>(10, 10);

		Mat backgroundMask;

		inRange(sourceImage, Scalar(0, 0, 0),
				Scalar(randomBackgroundPixel[0] + 15,
						randomBackgroundPixel[1] + 15,
						randomBackgroundPixel[2] + 15), backgroundMask);

		// Ok, i see that the folowing operation (writing and reading (wtf is this real code haha))
		// is NOT SO GOOD. it will slow the process alot
		// I did it because mask image and the source image have to
		// be same format and i have some problems with the inRange function result
		// P.S. i will fix that thing in folowing commits

		imwrite(MASK_IMAGE_FILE_NAME, backgroundMask);
		backgroundMask = imread(MASK_IMAGE_FILE_NAME);

		resize(backgroundImage, resizedBackgroundImage,
				cv::Size(sourceImage.rows, sourceImage.cols), 0, 0);

		for (int i = 0; i < sourceImage.rows - 1; i++)
			for (int j = 0; j < sourceImage.cols - 1; j++) {
				Vec3b pixel = backgroundMask.at<Vec3b>(i, j);
				if ((pixel[0] >= 250) && (pixel[1] >= 250) && (pixel[2] >= 250))
					sourceImage.at<Vec3b>(i, j) = resizedBackgroundImage.at<
							Vec3b>(i, j);
			}

		outputVideo << sourceImage;

	}

	inputVideo.release();

	return 0;
}

