#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <opencv2/opencv.hpp>

class Texture
{
private:
    cv::Mat m_image;
    int m_width;
    int m_height;

public:
    Texture();
    Texture(const std::string &name);

    cv::Vec3f getColor(float u, float v) const;
};

#endif