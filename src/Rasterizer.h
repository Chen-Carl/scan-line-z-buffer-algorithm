#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include <optional>
#include <opencv2/opencv.hpp>
#include "Triangle.h"
#include "Texture.h"

class Rasterizer
{
private:
    int m_height;
    int m_width;
    std::optional<Texture> m_texture;

    cv::Mat1f m_model;
    cv::Mat1f m_view;
    cv::Mat1f m_projection;

    std::function<cv::Mat3f(const std::vector<Triangle>)> m_rasterizeTriangles;

public:
    Rasterizer(int height = 0, int width = 0);
    Rasterizer(int height, int width, const std::function<void(const std::vector<Triangle> &triangles)> &rasterizeTriangles);

    void setModel(const cv::Mat1f &model) { m_model = model; }
    void setView(const cv::Mat1f &view) { m_view = view; }
    void setProjection(const cv::Mat1f &projection) { m_projection = projection; }
    void setTexture(const Texture &texture) { m_texture = texture; }
    void setRasterizeAlg(std::function<cv::Mat3f(const std::vector<Triangle> &triangles)> rasterizeTriangles) { m_rasterizeTriangles = rasterizeTriangles; }

    cv::Mat3f draw(std::vector<Triangle> &triangles);

    static cv::Mat1f getModelMatrix(float angle, cv::Vec3f axis = cv::Vec3f(0, 0, 1));
    static cv::Mat1f getProjectionMatrix(float fov, float aspect, float near, float far);
    static cv::Mat1f getViewMatrix(const cv::Vec3f &eye);
};

#endif