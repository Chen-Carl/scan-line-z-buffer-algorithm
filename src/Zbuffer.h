#ifndef __ZBUFFER_H__
#define __ZBUFFER_H__

#include "RasterizeAlg.h"
#include "Triangle.h"

class Zbuffer : public RasterizeAlg
{
private:
    cv::Mat1f m_zBuffer;

public:
    Zbuffer() = delete;
    Zbuffer(int height, int width);
    cv::Mat3f operator()(const std::vector<Triangle> &triangle) override;
};

#endif