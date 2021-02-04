#include <iostream>
#include <math.h>
#include <ctime>
#include <windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

static uint logCount = 0u;
static std::clock_t step1 = std::clock();

// Tracks face and eyes on a frame
// Returns 0 if successfull
// Returns -1 if no face 
// Returns -2 if no eyes/eyes are closed
int detectEyes(cv::Mat &frame, cv::CascadeClassifier &faceCascade, cv::CascadeClassifier &eyeCascade)
{
	cv::Mat grayscale;
	cv::cvtColor(frame, grayscale, CV_BGR2GRAY);
    cv::equalizeHist(grayscale, grayscale);

    //grayscale.convertTo(grayscale, -1, 1.5, 0); // Makes the detection to run with fewer precision
	
    std::vector<cv::Rect> faces;
	faceCascade.detectMultiScale(grayscale, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(50, 50));
	if (faces.size() == 0)
	{
		return -1;
	}
	cv::Mat face = grayscale(faces[0]); // crop the face
    rectangle(frame, faces[0].tl(), faces[0].br(), cv::Scalar(255, 128, 0), 2);
	
	// Face is detected, let's go to eyes
	std::vector<cv::Rect> eyes;
	eyeCascade.detectMultiScale(face, eyes, 1.15, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(5, 5));
	if (eyes.size() != 2)
	{
		return -2;
	}
	
	for (cv::Rect &eye : eyes)
	{
		rectangle(frame, faces[0].tl() + eye.tl(), faces[0].tl() + eye.br(), cv::Scalar(0, 255, 0), 2);
	}
	cv::Mat eye = face(eyes[0]); // crop the leftmost eye
	cv::equalizeHist(eye, eye);
	std::vector<cv::Vec3f> circles;

	cv::imshow("Eye", eye);
	return 0;
}

int main(int argc, char **argv)
{
	cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);
	cv::CascadeClassifier faceCascade;
	cv::CascadeClassifier eyeCascade;

	if (!faceCascade.load("haarcascade_frontalface_alt.xml"))
	{
		std::cerr << "Could not load face detector." << std::endl;
		return -1;
	}
	else if (!eyeCascade.load("haarcascade_eye_tree_eyeglasses.xml"/*"haarcascade_eye.xml"*/))
	{
		std::cerr << "Could not load eye detector." << std::endl;
		return -1;
	}

	cv::VideoCapture cap(0);
	if (!cap.isOpened())
	{
		std::cerr << "Webcam not detected." << std::endl;
		return -1;
	}
	cv::Mat frame;
	char scannedChar = NULL;

	while ((scannedChar = cv::waitKey(10)) != '\n' && scannedChar != '\r' && scannedChar != ' ')
	{
		cap >> frame;
		if (frame.empty()) 
			break;

		int returnVal = detectEyes(frame, faceCascade, eyeCascade);
		cv::imshow("Webcam", frame);

        if (returnVal == -1)
        {
            //cv::imwrite("PrecisionTest\\" + cv::String(std::to_string(logCount++) + ". No face.jpg"), frame);
        }
        else
		if (returnVal == -2) // If eyes weren't detected (when face was)
		{
            //cv::imwrite("PrecisionTest\\" + cv::String(std::to_string(logCount++) + ". No eyes.jpg"), frame);
			
            if ((std::clock() - step1) / (double)CLOCKS_PER_SEC >= 3.0) // If the time interval is >= 3 seconds
			{
				Beep(900, 500); //std::cout << '\a';
				std::cout << "[LOG]: Asleep!\n";
			}
			else
			{
				// No eyes but < 3 seconds so no alarm and move to next frame
			}
		}
		else // Eyes and face are found
		{
            //cv::imwrite("PrecisionTest\\" + cv::String(std::to_string(logCount++) + ". All found.jpg"), frame);
			step1 = std::clock(); // Update the clock
		}
	}
	return 0;
}
