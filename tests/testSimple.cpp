#include <opencv2/opencv.hpp>
#include "Rasterizer.h"
#include "Triangle.h"

int main()
{
    const float angle = 0.0f;
    const cv::Vec3f eye { 0.0f, 0.0f, 5.0f };
    Rasterizer rasterizer(700, 700);
    rasterizer.setModel(Rasterizer::getModelMatrix(angle));
    rasterizer.setView(Rasterizer::getViewMatrix(eye));
    rasterizer.setProjection(Rasterizer::getProjectionMatrix(45.0f, 1, 0.1f, 50.0f));
    std::vector<Triangle> triangles;
    Triangle triangle1, triangle2;
    triangle1.setVertex(0, cv::Vec4f(2, 0, -2, 1));
    triangle1.setVertex(1, cv::Vec4f(0, 2, -2, 1));
    triangle1.setVertex(2, cv::Vec4f(-2, 0, -2, 1));
    triangle1.setColor(0, cv::Vec3f(217.0, 238.0, 185.0));
    triangle1.setColor(1, cv::Vec3f(217.0, 238.0, 185.0));
    triangle1.setColor(2, cv::Vec3f(217.0, 238.0, 185.0));
    triangles.push_back(triangle1);
    triangle2.setVertex(0, cv::Vec4f(3.5, -1, -5, 1));
    triangle2.setVertex(1, cv::Vec4f(2.5, 1.5, -5, 1));
    triangle2.setVertex(2, cv::Vec4f(-1, 0.5, -5, 1));
    triangle2.setColor(0, cv::Vec3f(185.0, 217.0, 238.0));
    triangle2.setColor(1, cv::Vec3f(185.0, 217.0, 238.0));
    triangle2.setColor(2, cv::Vec3f(185.0, 217.0, 238.0));
    triangles.push_back(triangle2);
    cv::Mat3f frameBuffer = rasterizer.draw(triangles);
    cv::Mat image(700, 700, CV_32FC3, frameBuffer.data);
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
    cv::imwrite("testSimple.png", image);
    cv::waitKey(0);
    return 0;
}