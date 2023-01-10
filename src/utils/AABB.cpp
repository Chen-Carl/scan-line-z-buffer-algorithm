#include "AABB.h"

AABB::AABB(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) : 
    m_xMin(xMin),
    m_xMax(xMax),
    m_yMin(yMin),
    m_yMax(yMax),
    m_zMin(zMin),
    m_zMax(zMax)
{

}

AABB::AABB(cv::Vec3f minVec, cv::Vec3f maxVec) :
    m_xMin(minVec[0]),
    m_xMax(maxVec[0]),
    m_yMin(minVec[1]),
    m_yMax(maxVec[1]),
    m_zMin(minVec[2]),
    m_zMax(maxVec[2])
{

}

cv::Vec3f AABB::getCenter() const
{
    return cv::Vec3f(
        (m_xMin + m_xMax) / 2,
        (m_yMin + m_yMax) / 2,
        (m_zMin + m_zMax) / 2
    );
}

float AABB::getRadius() const
{
    return (m_xMax - m_xMin) / 2;
}

float AABB::getMinBorder(int i) const
{
    switch (i)
    {
    case 0:
        return m_xMin;
    case 1:
        return m_yMin;
    case 2:
        return m_zMin;
    }
    return -1;
}

float AABB::getMaxBorder(int i) const
{
    switch (i)
    {
    case 0:
        return m_xMax;
    case 1:
        return m_yMax;
    case 2:
        return m_zMax;
    }
    return -1;
}

bool AABB::containTriangle(const Triangle &triangle) const
{
    for (int i = 0; i < 3; i++)
    {
        cv::Vec3f vertex = triangle.getVertex3f(i);
        if ((vertex[0] > m_xMax || vertex[0] < m_xMin)
            || (vertex[1] > m_yMax || vertex[1] < m_yMin)
            || (vertex[2] > m_zMax || vertex[2] < m_zMin))
        {
            return false;
        }
    }
    return true;
}
