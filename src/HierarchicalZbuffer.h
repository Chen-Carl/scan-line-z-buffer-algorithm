#ifndef __HIERARCHICALZBUFFER_H__
#define __HIERARCHICALZBUFFER_H__

#include "RasterizeAlg.h"

class HierarchicalZbuffer : public RasterizeAlg
{
protected:
    int m_zLevels;
    std::vector<cv::Mat1f> m_zBuffers;

public:
    HierarchicalZbuffer() = delete;
    HierarchicalZbuffer(int width, int height);
    virtual ~HierarchicalZbuffer() { }

    virtual void drawTriangle(const Triangle &triangle);
    virtual std::tuple<int, int, int> getTriangleLevel(const Triangle &triangle);
    void updateHierarchicalZbuffer(int i, int j);
    virtual cv::Mat3f operator()(const std::vector<Triangle> &triangles) override;
};


#endif