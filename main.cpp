#include <stdlib.h>
#include <iostream>
#include <csignal>

#include "RioSerial.hpp"
#include "VisionPacket.hpp"

extern "C"{
    #include "neopixel.h"
}

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

/* DEBUG */
#define VERBOSE
#define DISPLAY_VIDEO 0     /* undef: no video   0: raw input   1: threshold */
//#define DRAW_CONTOURS
#define DRAW_CONVEX_HULL
//#define DRAW_BOUNDING_BOXES

/* CAMERA */
#define FRAME_WIDTH   640  /* Low Res: 640   HD: 1080 */
#define FRAME_HEIGHT  480  /* Low Res: 480   HD: 720  */
#define MAX_FPS       255

/* LIGHT RING */
//#define ACTIVATE_LIGHT_RING 0X0000FF00 /* bytes are arranged: WWBBGGRR where W represents white */
#define TOTAL_LED     12

/* LOW SEGMENTATION BOUNDARY */
#define LOW_SEG_H     30
#define LOW_SEG_L     75
#define LOW_SEG_S     240


/* HIGH SEGMENTATION BOUNDARY */
#define HIGH_SEG_H    33
#define HIGH_SEG_L    200
#define HIGH_SEG_S    255


/* OBJECT SEGMENTATION PARAMETERS */
#define IMAGE_DOWNSAMPLING_FACTOR 8
#define BOUNDING_BOX_MIN_WIDTH  FRAME_WIDTH  * 0.12
#define BOUNDING_BOX_MIN_HEIGHT FRAME_HEIGHT * 0.12

/**
 * Called during the destruction a std::atexit(<function>) object
 *
 * Used to turn off light ring when the program is killed
 */
void cleanup(){
    setLightRingColor(0);
}

void signalHandler(int signum){
    switch(signum){
        case SIGINT:
            std::cout << "Interractive attention signal caught" << std::endl;
            break;
        default:
            std::cout << "Caught signal: " << signum << std::endl;
    }

    std::exit(signum);
}

int main(int argc, char** argv){

    std::atexit(cleanup);

    std::signal(SIGINT, signalHandler);

    system("sudo modprobe bcm2835-v4l2");

    /* to list all camera options:
     * v4l2-ctl --list-ctrls
     */
    system("v4l2-ctl --set-ctrl=auto_exposure=1"); /* min=0 max=3 default=0 value=1 */
    system("v4l2-ctl --set-ctrl=white_balance_auto_preset=1"); /* min=0 max=9 default=1 value=0 */
    system("v4l2-ctl --set-ctrl=auto_exposure_bias=24"); /* min=0 max=24 default=12 value=0 */
    system("v4l2-ctl --set-ctrl=exposure_time_absolute=1000"); /* min=1 max=10000 step=1 default=1000 value=100*/

    RioSerial::open();

#ifdef ACTIVATE_LIGHT_RING
    setLightRingColor(ACTIVATE_LIGHT_RING);
#endif /*ACTIVATE_LIGHT_RING */

    cv::VideoCapture camera(0);
    if(!camera.isOpened()){
        std::cout << "ERROR: cannot open ribbon camera.  Shutting down..." << std::endl;
        return -1;
    }

    /* the static opencv variables cv::CAP_PROP_FRAME_WIDTH) and cv::CAP_PROP_FRAME_HEIGHT)
     * are not supported on the raspberry pi 3, so hardcoded indices are used instead.
     */
    camera.set(3, FRAME_WIDTH);
    camera.set(4, FRAME_HEIGHT);
    camera.set(cv::CAP_PROP_FPS, MAX_FPS);

#ifdef DISPLAY_VIDEO
    const std::string RIBBON_CAMERA_WINDOW = "Ribbon Camera";
    cv::namedWindow(RIBBON_CAMERA_WINDOW, cv::WINDOW_AUTOSIZE);
#endif /* DISPLAY_VIDEO */

    cv::Mat frame;
    cv::Mat hls;
    cv::Mat threshold;
    cv::Scalar lowerSegmentationBoundary(LOW_SEG_H,   LOW_SEG_L,  LOW_SEG_S);
    cv::Scalar higherSegmentationBoundary(HIGH_SEG_H, HIGH_SEG_L, HIGH_SEG_S);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Point> convexHull;
    while(1){

#ifdef VERBOSE
        int64 startTime = cv::getTickCount();
#endif /* VERBOSE */

        if(!camera.read(frame)){
            std::cout << "ERROR: cannot retrieve camera frame.  Shutting down..." << std::endl;
            return -1;
        }

        cv::resize(frame, hls, cv::Size(frame.cols / IMAGE_DOWNSAMPLING_FACTOR, frame.rows / IMAGE_DOWNSAMPLING_FACTOR));

        cv::cvtColor(hls, hls, CV_BGR2HLS);

        cv::inRange(hls, lowerSegmentationBoundary, higherSegmentationBoundary, threshold);

        cv::resize(threshold, threshold, cv::Size(frame.cols, frame.rows), 3, 3, cv::INTER_CUBIC);

        cv::findContours(threshold, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        VisionPacket visionPacket;
        for(int x = 0; x < contours.size(); x++){

            cv::Rect boundingBox = cv::boundingRect(contours[x]);
            if(boundingBox.width >= BOUNDING_BOX_MIN_WIDTH &&
               boundingBox.height >= BOUNDING_BOX_MIN_HEIGHT){
#ifdef DRAW_BOUNDING_BOXES
                cv::rectangle(frame, boundingBox, cv::Scalar(0, 0, 255));
#endif /* DRAW_BOUNDING_BOXES */

#ifdef DRAW_CONTOURS
            cv::drawContours(frame, contours, x, cv::Scalar(255, 0, 0), 1, 8, hierarchy, 0, cv::Point());
#endif /* DRAW_CONTOURS */


            cv::convexHull(cv::Mat(contours[x]), convexHull, false);
            visionPacket.addObject(convexHull);
#ifdef DRAW_CONVEX_HULL
            for(int p = 0; p < convexHull.size() - 1; p++){
                cv::line(frame, convexHull[p], convexHull[p + 1], cv::Scalar(0, 0, 255), 4);
            }
#endif /* DRAW_CONVEX_HULL */
            }
        }
        RioSerial::write(visionPacket.serialize());
        //std::cout << visionPacket.serialize() << std::endl;

#ifdef VERBOSE
        std::cout << "FPS: " << (cv::getTickFrequency() / (cv::getTickCount() - startTime)) << std::endl;
#endif /* VERBOSE */

#ifdef DISPLAY_VIDEO
#if DISPLAY_VIDEO == 0
        cv::imshow(RIBBON_CAMERA_WINDOW, frame);
#else
        cv::imshow(RIBBON_CAMERA_WINDOW, threshold);
#endif /* DISPLAY_VIDEO == 0 */
        if(cv::waitKey(1) >= 0){
            break;
        }
#endif /* DISPLAY_VIDEO */
    }

    return 0;
}
