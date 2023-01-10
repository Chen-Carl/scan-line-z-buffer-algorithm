#ifndef __AABB_H__
#define __AABB_H__

#include <opencv2/opencv.hpp>
#include "Triangle.h"

class AABB
{
private:
    float m_xMin;
    float m_xMax;
    float m_yMin;
    float m_yMax;
    float m_zMin;
    float m_zMax;

public:
    AABB(float xMin = 0, float xMax = 0, float yMin = 0, float yMax = 0, float zMin = 0, float zMax = 0);
    AABB(cv::Vec3f minVec, cv::Vec3f maxVec);

    cv::Vec3f getCenter() const;
    float getRadius() const;
    float getMinBorder(int i) const;
    float getMaxBorder(int i) const;

    bool containTriangle(const Triangle &triangle) const;
};

#endif