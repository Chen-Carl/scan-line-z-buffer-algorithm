#include "RasterizeAlg.h"

RasterizeAlg::RasterizeAlg(int width, int height) :
    m_width(width),
    m_height(height)
{
    m_frameBuffer = cv::Mat3f(m_width, m_height, cv::Vec3f(0, 0, 0));
}

bool RasterizeAlg::insideTriangle(int x, int y, const std::array<cv::Vec3f, 3> &v)
{   
    cv::Vec3f point { static_cast<float>(x), static_cast<float>(y), 0};
    bool res1 = ((point - v[0]).cross(v[1] - v[0])[2] < 0);
    bool res2 = ((point - v[1]).cross(v[2] - v[1])[2] < 0);
    bool res3 = ((point - v[2]).cross(v[0] - v[2])[2] < 0);
    return res1 && res2 && res3;
}