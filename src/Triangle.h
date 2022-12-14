#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <opencv2/opencv.hpp>

class Triangle
{
private:
    // the original coordinates of the triangle, v0, v1, v2 in counter clockwise order
    std::array<cv::Vec4f, 3> m_vertices;
    // color at each vertex
    std::array<cv::Vec3f, 3> m_colors;
    // the normal of the triangle
    std::array<cv::Vec3f, 3> m_normal;
    // the texture coordinates of the triangle
    std::array<cv::Vec2f, 3> m_texCoords;

public:
    Triangle();

    // set i-th vertex coordinate
    void setVertex(int index, const cv::Vec4f &vertex) { m_vertices[index] = vertex; }
    // set i-th vertex normal vector
    void setNormal(int index, const cv::Vec3f &normal) { m_normal[index] = normal; }
    // set i-th vertex color
    void setColor(int index, const cv::Vec3f &color) { m_colors[index] = color; }
    // set i-th vertex texture coordinate
    void setTexCoord(int index, const cv::Vec2f &texCoord) { m_texCoords[index] = texCoord; }

    // get i-th vertex coordinate
    cv::Vec4f getVertex(int index) const { return m_vertices[index]; }
    // get i-th Vec3f vertex coordinate
    cv::Vec3f getVertex3f(int index) const { 
        return cv::Vec3f(
            m_vertices[index][0] / m_vertices[index][3], 
            m_vertices[index][1] / m_vertices[index][3], 
            m_vertices[index][2] / m_vertices[index][3]
        );
    }
    // get all Vec3f vertices coordinate
    std::array<cv::Vec3f, 3> getVertices3f() const {
        return {
            getVertex3f(0),
            getVertex3f(1),
            getVertex3f(2)
        };
    }
    // get i-th vertex normal vector
    cv::Vec4f getNormal(int index) const { return cv::Vec4f(m_normal[index][0], m_normal[index][1], m_normal[index][2], 0); }

    cv::Vec3f getNormal() const;

    // get min x coordinate
    float getMinX() const { return std::min(std::min(m_vertices[0][0], m_vertices[1][0]), m_vertices[2][0]); }
    // get max x coordinate
    float getMaxX() const { return std::max(std::max(m_vertices[0][0], m_vertices[1][0]), m_vertices[2][0]); }
    // get min y coordinate
    float getMinY() const { return std::min(std::min(m_vertices[0][1], m_vertices[1][1]), m_vertices[2][1]); }
    // get max y coordinate
    float getMaxY() const { return std::max(std::max(m_vertices[0][1], m_vertices[1][1]), m_vertices[2][1]); }

    float getInterpolateZ(int x, int y) const;
    cv::Vec3f getInterpolateColor(int x, int y) const;
    cv::Vec3f getUniformColor() const;
    cv::Vec3f getNormalColor() const;

    static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const std::array<cv::Vec4f, 3> &v);
    
    friend std::ostream &operator<<(std::ostream &os, const Triangle &triangle);
};

#endif