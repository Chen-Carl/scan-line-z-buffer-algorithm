#ifndef __RASTERIZEALG_H__
#define __RASTERIZEALG_H__

#include <opencv2/opencv.hpp>
#include "Triangle.h"

class RasterizeAlg
{
protected:
    int m_height;
    int m_width;
    cv::Mat3f m_frameBuffer;

public:
    RasterizeAlg() = delete;
    RasterizeAlg(int width, int height);
    virtual ~RasterizeAlg() { }
    
    virtual cv::Mat3f operator()(const std::vector<Triangle> &triangles) = 0;

    static bool insideTriangle(int x, int y, const std::array<cv::Vec3f, 3> &v);
};

#endif