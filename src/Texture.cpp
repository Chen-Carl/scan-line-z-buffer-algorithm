#include "Texture.h"

Texture::Texture()
{
    m_image = cv::Mat();
    m_width = 0;
    m_height = 0;
}

Texture::Texture(const std::string &name)
{
    m_image = cv::imread(name);
    m_width = m_image.cols;
    m_height = m_image.rows;
}

cv::Vec3f Texture::getColor(float u, float v) const
{
    float x = u * m_width;
    float y = (1 - v) * m_height;
    cv::Vec3b color = m_image.at<cv::Vec3b>(y, x);
    return cv::Vec3f(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f);
}