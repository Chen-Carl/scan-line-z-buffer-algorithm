#include "Triangle.h"

Triangle::Triangle()
{
    m_vertices = std::array<cv::Vec4f, 3>();
    m_colors = std::array<cv::Vec3f, 3>();
    m_normal = std::array<cv::Vec3f, 3>();
    m_texCoords = std::array<cv::Vec2f, 3>();
}

float Triangle::getInterpolateZ(int x, int y) const
{
    auto [alpha, beta, gamma] = computeBarycentric2D(x, y, m_vertices);
    float z = 1.0 / (alpha / m_vertices[0][3] + beta / m_vertices[1][3] + gamma / m_vertices[2][3]);
    float zp = alpha * m_vertices[0][2] / m_vertices[0][3] + beta * m_vertices[1][2] / m_vertices[1][3] + gamma * m_vertices[2][2] / m_vertices[2][3];
    return zp * z;
}

cv::Vec3f Triangle::getInterpolateColor(int x, int y) const
{
    auto [alpha, beta, gamma] = computeBarycentric2D(x, y, m_vertices);
    cv::Vec3f color = alpha * m_colors[0] + beta * m_colors[1] + gamma * m_colors[2];
    return color;
}

cv::Vec3f Triangle::getUniformColor() const
{
    return m_colors[0];
}

cv::Vec3f Triangle::getNormalColor() const
{
    cv::Vec3f color = (m_normal[0] + m_normal[1] + m_normal[2]) / 3 * 255;
    return color;
}

std::tuple<float, float, float> Triangle::computeBarycentric2D(float x, float y, const std::array<cv::Vec4f, 3> &v)
{
    float c1 = (x * (v[1][1] - v[2][1]) + (v[2][0] - v[1][0]) * y + v[1][0] * v[2][1] - v[2][0] * v[1][1]) / (v[0][0] * (v[1][1] - v[2][1]) + (v[2][0] - v[1][0]) * v[0][1] + v[1][0] * v[2][1] - v[2][0] * v[1][1]);
    float c2 = (x * (v[2][1] - v[0][1]) + (v[0][0] - v[2][0]) * y + v[2][0] * v[0][1] - v[0][0] * v[2][1]) / (v[1][0] * (v[2][1] - v[0][1]) + (v[0][0] - v[2][0]) * v[1][1] + v[2][0] * v[0][1] - v[0][0] * v[2][1]);
    float c3 = (x * (v[0][1] - v[1][1]) + (v[1][0] - v[0][0]) * y + v[0][0] * v[1][1] - v[1][0] * v[0][1]) / (v[2][0] * (v[0][1] - v[1][1]) + (v[1][0] - v[0][0]) * v[2][1] + v[0][0] * v[1][1] - v[1][0] * v[0][1]);
    return {c1, c2, c3};
}

cv::Vec3f Triangle::getNormal() const
{
    std::array<cv::Vec3f, 3> v3f = getVertices3f();
    return (v3f[1] - v3f[0]).cross(v3f[2] - v3f[1]);
}

std::ostream &operator<<(std::ostream &os, const Triangle &triangle)
{
    os << "Triangle {" << std::endl;
    for (int i = 0; i < 3; i++)
    {
        os << "Vertex " << i << ": " << triangle.m_vertices[i] << std::endl;
    }
    os << "}";
    return os;
}
