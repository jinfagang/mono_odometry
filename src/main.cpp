//
// Created by JinTian on 18/03/2018.
//

#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv/cv.hpp>
#include <dirent.h>
#include "visual_odometry.h"
#include "cao/os.h"
#include "vector"

using namespace std;

void getAllImages(const string &imgPath, vector<string> &allImages) {
    // simply range from 000000 ~ 004540, for continue of sequences
    if (cao::os::exists(imgPath)) {
        for (int i = 0; i < 4540; ++i) {
            char img[256];
            sprintf(img, "%06d.png", i);
            allImages.push_back(cao::os::join(imgPath, img));
        }
    } else {
        cout << "path: " << imgPath << " not exist.\n";
    }
}

int main(int argc, char *argv[]) {

    string sequenceDir = argv[1];

    vector<string> allImages;
    getAllImages(sequenceDir, allImages);

    for (int j=0; j<allImages.size();j++) {
        if (cao::os::suffix(allImages[j]) != "png") {
            allImages.erase(allImages.begin() + j);
        }
    }

    for (int k = 0; k < 30; ++k) {
        cout << allImages[k] << endl;
    }



    auto *camera = new MonoCamera(1241, 376,
                                       718.8560, 718.8560,
                                        607.1928, 185.2157);
    VisualOdometry vo = VisualOdometry(camera);

    // Setup for show
    char text[100];
    int font_face = cv::FONT_HERSHEY_PLAIN;
    double font_scale = 1;
    int thickness = 1;
    cv::Point text_org(10, 50);
    cv::namedWindow("Road facing camera", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Trajectory", cv::WINDOW_AUTOSIZE);
    cv::Mat trajectory = cv::Mat::zeros(600, 600, CV_8UC3);

    double x=0.0, y=0.0,z=0.0;
    // iterate all images and get frameID
    for(int i=0; i<allImages.size(); i++) {

        cv::Mat img(cv::imread(allImages[i].c_str(), 0));
        assert(!img.empty());

        vo.addImage(img, i);
        cv::Mat curT = vo.getCurrentT();
        if (curT.rows != 0) {
            x = curT.at<double>(0);
            y = curT.at<double>(1);
            z = curT.at<double>(2);
        }

        int drawX = int(x) + 300;
        int drawY = int(z) + 100;

        cv::circle(trajectory, cv::Point(drawX, drawY),
        1, CV_RGB(255, 0, 0), 2);
        cv::rectangle(trajectory, cv::Point(10, 30), cv::Point(580, 60), CV_RGB(0, 0, 0), CV_FILLED);
        sprintf(text, "Coordinates: x = %02fm y = %02fm z = %02fm", x, y, z);

        cv::putText(trajectory, text, text_org, font_face, font_scale, cv::Scalar::all(255), thickness, 8);
        cv::imshow("Road facing camera", img);
        cv::imshow("Trajectory", trajectory);
        cv::waitKey(1);

    }

    delete camera;
    return 0;
}