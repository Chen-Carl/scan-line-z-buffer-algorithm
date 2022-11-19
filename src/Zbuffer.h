#ifndef __ZBUFFER_H__
#define __ZBUFFER_H__

#include "Triangle.h"

class Zbuffer
{
private:
    int m_height;
    int m_width;
    cv::Mat1f m_zBuffer;
    cv::Mat3f m_frameBuffer;

public:
    Zbuffer(int height = 0, int width = 0);
    cv::Mat3f operator()(const std::vector<Triangle> &triangle);

    static bool insideTriangle(int x, int y, const std::array<cv::Vec3f, 3> &v);
};

#endif