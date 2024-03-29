//
// Created by yjxiong on 11/18/15.
//
#include "common.h"

void calcDenseFlow(string file_name, int bound, int type, int step,
                   vector<vector<uchar> >& output_x,
                   vector<vector<uchar> >& output_y,
                   vector<vector<uchar> >& output_img){

    VideoCapture video_stream(file_name);
    CHECK(video_stream.isOpened())<<"Cannot open video stream \""
                                  <<file_name
                                  <<"\" for optical flow extraction.";

    Mat capture_frame, capture_image, prev_image, capture_gray, prev_gray;
    Mat flow, flow_split[2];


    bool initialized = false;
    for(int iter = 0;; iter++){
        video_stream >> capture_frame;
        if (capture_frame.empty()) break; // read frames until end

        //build mats for the first frame
        if (!initialized){
            initializeMats(capture_frame, capture_image, capture_gray,
                           prev_image, prev_gray);
            capture_frame.copyTo(prev_image);
            cvtColor(prev_image, prev_gray, CV_BGR2GRAY);
            initialized = true;
//            LOG(INFO)<<"Initialized";
        }else if(iter % step == 0){
            capture_frame.copyTo(capture_image);
            cvtColor(capture_image, capture_gray, CV_BGR2GRAY);
            calcOpticalFlowFarneback(prev_gray, capture_gray, flow,
                                     0.702, 5, 10, 2, 7, 1.5,
                                     cv::OPTFLOW_FARNEBACK_GAUSSIAN );

            vector<uchar> str_x, str_y, str_img;
            split(flow, flow_split);
            encodeFlowMap(flow_split[0], flow_split[0], str_x, str_y, bound);
            imencode(".jpg", capture_image, str_img);

            output_x.push_back(str_x);
            output_y.push_back(str_y);
            output_img.push_back(str_img);
//            LOG(INFO)<<iter;

            std::swap(prev_gray, capture_gray);
            std::swap(prev_image, capture_image);
        }
    }

}