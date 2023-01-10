#include "OctZbuffer.h"

OctZbuffer::OctZbuffer(int height, int width) :
    HierarchicalZbuffer(height, width)
{

}

void OctZbuffer::traversal(std::shared_ptr<TreeNode> node, int depth)
{
    if (node != nullptr)
    {
        int x = static_cast<int>((node->aabb.getMinBorder(0) + 1.0f) * m_width / 2);
        int y = static_cast<int>(m_height - (node->aabb.getMinBorder(1) + 1.0f) * m_height / 2);
        for (int i = 0; i < m_zLevels - depth; i++)
        {
            x /= 2;
            y /= 2;
        }

        if (node->aabb.getMaxBorder(2) < m_zBuffers[m_zLevels - depth](x, y))
        {
            return;
        }

        for (int i = 0; i < 8; i++)
        {
            traversal(node->children[i], depth + 1);
        }

        for (const Triangle &triangle : node->triangles)
        {
            drawTriangle(triangle);
        }
    }
}
/*
void OctZbuffer::drawTriangle(const Triangle &triangle)
{
    int xMax = 0;
    int xMin = m_width - 1;
    int yMax = 0;
    int yMin = m_height - 1;
    float zMax = -std::numeric_limits<float>::max();

    std::array<cv::Vec3f, 3> v3fs;
    for (int i = 0; i < 3; i++)
    {
        v3fs[i] = cv::Vec3f(
            (triangle.getVertex3f(i)[0] + 1.0f) * m_width / 2,
            m_height - (triangle.getVertex3f(i)[1] + 1.0f) * m_height / 2,
            triangle.getVertex3f(i)[2]
        );
    }

    for (int i = 0; i < 3; i++)
    {
        xMax = std::max(xMax, static_cast<int>(v3fs[i][0]));
        xMin = std::min(xMin, static_cast<int>(v3fs[i][0]));
        yMax = std::max(yMax, static_cast<int>(v3fs[i][1]));
        yMin = std::min(yMin, static_cast<int>(v3fs[i][1]));
        zMax = std::max(zMax, v3fs[i][2]);
    }

    xMax = std::min(xMax, m_width - 1);
    yMax = std::min(yMax, m_height - 1);

    auto [level, x, y] = getTriangleLevelAndCoor(xMin, xMax, yMin, yMax);
    if (zMax < m_zBuffers[level](x, y))
    {
        return;
    }

    std::sort(v3fs.begin(), v3fs.end(), [](const cv::Vec3f &v1, const cv::Vec3f &v2) {
        return v1[1] < v2[1];
    });

    for (int y = static_cast<int>(v3fs[1][1]); y >= static_cast<int>(v3fs[0][1]); y--)
    {
        float step1 = (y - v3fs[0][1]) / (v3fs[1][1] - v3fs[0][1] + (v3fs[1][1] == v3fs[0][1] ? 1 : 0));
        float step2 = (y - v3fs[0][1]) / (v3fs[2][1] - v3fs[0][1] + (v3fs[2][1] == v3fs[0][1] ? 1 : 0));
        cv::Vec3f v1 = v3fs[0] + (v3fs[1] - v3fs[0]) * step1;
        cv::Vec3f v2 = v3fs[0] + (v3fs[2] - v3fs[0]) * step2;
        if (v1[0] > v2[0])
        {
            std::swap(v1, v2);
        }
        float dz = (v2[2] - v1[2]) / (v2[0] - v1[0] + (v2[0] == v1[0] ? 1 : 0));
        int xMin = static_cast<int>(v1[0]);
        int xMax = static_cast<int>(v2[0]);
        float z = v1[2];
        for (int x = xMin; x <= xMax; x++)
        {
            if (z > m_zBuffers[0](x, y))
            {
                m_frameBuffer(y, x) = triangle.getNormalColor();
                m_zBuffers[0](x, y) = z;
            }
            z += dz;
        }
    }

    for (int y = static_cast<int>(v3fs[2][1]); y >= static_cast<int>(v3fs[1][1]); y--)
    {
        float step1 = (y - v3fs[1][1]) / (v3fs[2][1] - v3fs[1][1] + (v3fs[2][1] == v3fs[1][1] ? 1 : 0));
        float step2 = (y - v3fs[0][1]) / (v3fs[2][1] - v3fs[0][1] + (v3fs[2][1] == v3fs[0][1] ? 1 : 0));
        cv::Vec3f v1 = v3fs[1] + (v3fs[2] - v3fs[1]) * step1;
        cv::Vec3f v2 = v3fs[0] + (v3fs[2] - v3fs[0]) * step2;
        if (v1[0] > v2[0])
        {
            std::swap(v1, v2);
        }
        float dz = (v2[2] - v1[2]) / (v2[0] - v1[0] + (v2[0] == v1[0] ? 1 : 0));
        int xMin = static_cast<int>(v1[0]);
        int xMax = static_cast<int>(v2[0]);
        float z = v1[2];
        for (int x = xMin; x <= xMax; x++)
        {
            if (z > m_zBuffers[0](x, y))
            {
                m_frameBuffer(y, x) = triangle.getNormalColor();
                m_zBuffers[0](x, y) = z;
            }
            z += dz;
        }
    }
}
*/
std::tuple<int, int, int> OctZbuffer::getTriangleLevelAndCoor(int xMin, int xMax, int yMin, int yMax)
{
    int xLevel = 0;
    int yLevel = 0;
    int level = 0;
    while (xMax != xMin)
    {
        xMax /= 2;
        xMin /= 2;
        xLevel++;
    }
    while (yMax != yMin)
    {
        yMax /= 2;
        yMin /= 2;
        yLevel++;
    }
    
    int x = xMax;
    int y = yMax;
    if (xLevel > yLevel)
    {
        level = xLevel;
        for (int i = 0; i < level - yLevel; i++)
        {
            y /= 2;
        }
    }
    else
    {
        level = yLevel;
        for (int i = 0; i < level - xLevel; i++)
        {
            x /= 2;
        }
    }
    return { level, x, y };
}

cv::Mat3f OctZbuffer::operator()(const std::vector<Triangle> &triangles)
{
    std::cout << "OctTree Hierarchical Zbuffer rasterizing ..." << std::endl;
    m_octTree = OctTree(triangles);
    traversal(m_octTree.getRoot(), 1);
    return m_frameBuffer;
}