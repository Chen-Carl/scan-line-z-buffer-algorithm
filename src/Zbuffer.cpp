#include "Zbuffer.h"

Zbuffer::Zbuffer(int height, int width) :
    m_height(height),
    m_width(width)
{
    m_zBuffer = cv::Mat1f(height, width, std::numeric_limits<float>::max());
    m_frameBuffer = cv::Mat3f(height, width, cv::Vec3f(0.0, 0.0, 0.0));
}


cv::Mat3f Zbuffer::operator()(const std::vector<Triangle> &triangle)
{
    std::cout << "Zbuffer rasterizing..." << std::endl;
    for (const auto &t : triangle)
    {
        std::array<cv::Vec4f, 3> v;
        v[0] = t.getVertex(0);
        v[1] = t.getVertex(1);
        v[2] = t.getVertex(2);
        int minx = std::min(v[0][0], std::min(v[1][0], v[2][0]));
        int miny = std::min(v[0][1], std::min(v[1][1], v[2][1]));
        int maxx = std::max(v[0][0], std::max(v[1][0], v[2][0]));
        int maxy = std::max(v[0][1], std::max(v[1][1], v[2][1]));

        for (int i = minx; i <= maxx; i++)
        {
            for (int j = miny; j <= maxy; j++)
            {
                if (insideTriangle(i, j, t.getVertexes3f()))
                {
                    auto [alpha, beta, gamma] = Triangle::computeBarycentric2D(i + 0.5, j + 0.5, v);
                    float wReciprocal = 1.0 / (alpha / v[0][3] + beta / v[1][3] + gamma / v[2][3]);
                    float zInterpolated = alpha * v[0][2] / v[0][3] + beta * v[1][2] / v[1][3] + gamma * v[2][2] / v[2][3];
                    zInterpolated *= wReciprocal;
                    if (zInterpolated < m_zBuffer(m_height - j - 1, i))
                    {
                        m_zBuffer(m_height - j - 1, i) = zInterpolated;
                        m_frameBuffer(m_height - j - 1, i) = t.getUniformColor();
                    }
                }
            }
        }
    }
    return m_frameBuffer;
}

bool Zbuffer::insideTriangle(int x, int y, const std::array<cv::Vec3f, 3> &v)
{   
    cv::Vec3f point { static_cast<float>(x), static_cast<float>(y), 0};
    bool res1 = ((point - v[0]).cross(v[1] - v[0])[2] < 0);
    bool res2 = ((point - v[1]).cross(v[2] - v[1])[2] < 0);
    bool res3 = ((point - v[2]).cross(v[0] - v[2])[2] < 0);
    return res1 && res2 && res3;
}
