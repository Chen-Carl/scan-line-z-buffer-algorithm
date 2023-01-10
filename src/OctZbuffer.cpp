#include "OctZbuffer.h"

OctZbuffer::OctZbuffer(int height, int width, const std::vector<Triangle> &triangles) :
    HierarchicalZbuffer(height, width)
{
    m_octTree = OctTree(triangles);
}

void OctZbuffer::traversal(std::shared_ptr<TreeNode> node)
{
    if (node != nullptr)
    {
        drawTreeNode(node);
        for (int i = 0; i < 8; i++)
        {
            traversal(node->children[i]);
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

void OctZbuffer::drawTreeNode(std::shared_ptr<TreeNode> node)
{
    auto [x, y, level] = getTreeNodeLevel(node);
    float zMax = node->aabb.getMaxBorder(2);
    if (zMax < m_zBuffers[level](x, y))
    {
        for (const Triangle &t : node->triangles)
        {
            drawTriangle(t);
        }
    }
}

std::tuple<int, int, int> OctZbuffer::getTreeNodeLevel(std::shared_ptr<TreeNode> node)
{
    // get node border
    int xMax = static_cast<int>(node->aabb.getMaxBorder(0));
    int xMin = static_cast<int>(node->aabb.getMinBorder(0));
    int yMax = static_cast<int>(node->aabb.getMaxBorder(1));
    int yMin = static_cast<int>(node->aabb.getMinBorder(1));
    xMax -= (xMax == m_width ? 1 : 0);
    xMin -= (xMin == m_width ? 1 : 0);
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
    traversal(m_octTree.getRoot());
    return m_frameBuffer;
}