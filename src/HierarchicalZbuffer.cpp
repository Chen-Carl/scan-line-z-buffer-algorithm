#include "HierarchicalZbuffer.h"
#include "log.h"

#define __OUTPUT_DEBUG_INFO__ false

HierarchicalZbuffer::HierarchicalZbuffer(int width, int height) : 
    RasterizeAlg(width, height)
{
    while (width >= 1 && height >= 1)
    {
        m_zBuffers.push_back(cv::Mat1f(width, height, std::numeric_limits<float>::max()));
        width /= 2;
        height /= 2;
    }
    m_zLevels = m_zBuffers.size();
}

void HierarchicalZbuffer::drawTriangle(const Triangle &t)
{
#if __OUTPUT_DEBUG_INFO__
    CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "========== drawing triangle ========== ";
    CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << t;
#endif
    auto [x, y, level] = getTriangleLevel(t);
    std::array<cv::Vec4f, 3> v;
    v[0] = t.getVertex(0);
    v[1] = t.getVertex(1);
    v[2] = t.getVertex(2);
    float zMax = std::max({v[0][2], v[1][2], v[2][2]});
#if __OUTPUT_DEBUG_INFO__
    std::cout << "zMax: " << zMax << std::endl;
    std::cout << "zBuffer: " << m_zBuffers[level](x, y) << std::endl;
#endif
    if (zMax < m_zBuffers[level](x, y))
    {
        int minx = std::min(v[0][0], std::min(v[1][0], v[2][0]));
        int miny = std::min(v[0][1], std::min(v[1][1], v[2][1]));
        int maxx = std::max(v[0][0], std::max(v[1][0], v[2][0]));
        int maxy = std::max(v[0][1], std::max(v[1][1], v[2][1]));

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
                    if (-zInterpolated < m_zBuffers[0](i, j))
                    {
                        m_zBuffers[0](i, j) = -zInterpolated;
                        m_frameBuffer(i, j) = t.getNormalColor();
                    }
                    // update hierarchical zbuffer
                    updateHierarchicalZbuffer(i, j);
                }
            }
        }
    }
    else
    {
#if __OUTPUT_DEBUG_INFO__
    std::cout << "----------> rejected" << std::endl;
#endif
    }
}

void HierarchicalZbuffer::updateHierarchicalZbuffer(int i, int j)
{

    for (int k = 1; k < m_zLevels; k++)
    {
        i -= (i % 2);
        j -= (j % 2);
        m_zBuffers[k](i / 2, j / 2) = std::max({
            m_zBuffers[k - 1](i, j), 
            m_zBuffers[k - 1](i + 1, j), 
            m_zBuffers[k - 1](i, j + 1), 
            m_zBuffers[k - 1](i + 1, j + 1)
        });
        i /= 2;
        j /= 2;
    }
}

std::tuple<int, int, int> HierarchicalZbuffer::getTriangleLevel(const Triangle &triangle)
{
    // get triangle border
    std::array<cv::Vec3f, 3> vertices = triangle.getVertices3f();
    int xMax = std::min(m_width, static_cast<int>(triangle.getMaxX()));
    int xMin = std::max(0, static_cast<int>(triangle.getMinX()));
    int yMax = std::min(m_height, static_cast<int>(triangle.getMaxY()));
    int yMin = std::max(0, static_cast<int>(triangle.getMinY()));
    xMax -= (xMax == m_width ? 1 : 0);
    yMax -= (yMax == m_height ? 1 : 0);

    int xLevel = 0, yLevel = 0;
    while (xMax - xMin > 1)
    {
        xMax /= 2;
        xMin /= 2;
        xLevel++;
    }
    while (yMax - yMin > 1)
    {
        yMax /= 2;
        yMin /= 2;
        yLevel++;
    }
    int x = xMin;
    int y = yMin;
    int level = std::max(xLevel, yLevel);
    if (xLevel > yLevel)
    {
        for (int i = 0; i < xLevel - yLevel; i++)
        {
            y /= 2;
        }
    }
    else
    {
        for (int i = 0; i < yLevel - xLevel; i++)
        {
            x /= 2;
        }
    }
    return std::make_tuple(x, y, level);
}

cv::Mat3f HierarchicalZbuffer::operator()(const std::vector<Triangle> &triangles)
{
    std::cout << "Hierarchical Zbuffer rasterizing ..." << std::endl;
    for (const auto &t : triangles)
    {
        drawTriangle(t);
    }
    return m_frameBuffer;
}
