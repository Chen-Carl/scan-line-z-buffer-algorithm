#ifndef __SHADER_H__
#define __SHADER_H__

#include <opencv2/opencv.hpp>
#include "Texture.h"

class FragmentShaderPayload
{
private:
    cv::Vec3f m_viewPos;
    cv::Vec3f m_normal;
    cv::Vec3f m_color;
    cv::Vec2f m_texCoord;
};

#endif