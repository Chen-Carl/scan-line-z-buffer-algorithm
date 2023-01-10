#include "Zbuffer.h"
#include "log.h"

#define __OUTPUT_DEBUG_INFO__ false

Zbuffer::Zbuffer(int height, int width) :
    RasterizeAlg(height, width)
{
    m_zBuffer = cv::Mat1f(width, height, std::numeric_limits<float>::max());
}


cv::Mat3f Zbuffer::operator()(const std::vector<Triangle> &triangles)
{
#if __OUTPUT_DEBUG_INFO__
    CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "========== triangle normal color ==========";
    for (const auto &t : triangles)
    {
        std::cout << t.getNormalColor() << std::endl;
    }
#endif
    std::cout << "Zbuffer rasterizing ..." << std::endl;
    for (const auto &t : triangles)
    {
        std::array<cv::Vec4f, 3> v;
        v[0] = t.getVertex(0);
        v[1] = t.getVertex(1);
        v[2] = t.getVertex(2);
        int minx = std::min({v[0][0], v[1][0], v[2][0]});
        int miny = std::min({v[0][1], v[1][1], v[2][1]});
        int maxx = std::max({v[0][0], v[1][0], v[2][0]});
        int maxy = std::max({v[0][1], v[1][1], v[2][1]});

        for (int i = minx; i <= maxx; i++)
        {
            for (int j = miny; j <= maxy; j++)
            {
                if (insideTriangle(i, j, t.getVertices3f()))
                {
                    auto [alpha, beta, gamma] = Triangle::computeBarycentric2D(i + 0.5, j + 0.5, v);
                    float wReciprocal = 1.0 / (alpha / v[0][3] + beta / v[1][3] + gamma / v[2][3]);
                    float zInterpolated = alpha * v[0][2] / v[0][3] + beta * v[1][2] / v[1][3] + gamma * v[2][2] / v[2][3];
                    zInterpolated *= wReciprocal;
                    if (zInterpolated < m_zBuffer(i, j))
                    {
                        m_zBuffer(i, j) = zInterpolated;
                        m_frameBuffer(i, j) = t.getNormalColor();
                    }
                }
            }
        }
    }
    return m_frameBuffer;
}
