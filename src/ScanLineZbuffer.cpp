#include "ScanLineZbuffer.h"

ScanLineZbuffer::ScanLineZbuffer(int height, int width, const std::vector<Triangle> &triangles) :
    m_height(height),
    m_width(width),
    m_triangles(triangles)
{
    m_edgeTable = std::vector<std::list<std::shared_ptr<Edge>>>(height);
    m_triangleTable = std::vector<std::list<int>>(height);
    m_triangle2Edge = std::vector<std::array<std::shared_ptr<Edge>, 3>>(m_triangles.size(), std::array<std::shared_ptr<Edge>, 3>());
    initTable();
    m_frameBuffer = cv::Mat3f(height, width, cv::Vec3f(0, 0, 0));
    m_zBuffer = cv::Mat1f(height, width, std::numeric_limits<float>::max());
}

cv::Mat3f ScanLineZbuffer::operator()(const std::vector<Triangle> &triangles)
{
    // active edge table (AET)
    std::list<EdgePair> activeEdgeTable;
    // active polygon table (APT)
    std::set<int> activeTriangleTable;

    for (int h = 0; h < m_height; h++)
    {
        for (const auto &edgePair : activeEdgeTable)
        {
            auto &edge1 = edgePair.first;
            auto &edge2 = edgePair.second;
            if (edge1->y > h || edge2->y > h)
            {
                activeEdgeTable.remove(edgePair);
                if (!insertEdgePair(edge1->index, h, activeEdgeTable))
                {
                    activeTriangleTable.erase(edge1->index);
                }
            }
        }
        for (int x : m_triangleTable[h])
        {
            activeTriangleTable.insert(x);
            insertEdgePair(x, h, activeEdgeTable);
        }

        for (const auto &edgePair : activeEdgeTable)
        {
            auto &edge1 = edgePair.first;
            auto &edge2 = edgePair.second;
            int x1 = static_cast<int>(edge1->x);
            int x2 = static_cast<int>(edge2->x);
            for (int x = x1; x < x2; x++)
            {
                float z = m_triangles[edge1->index].getInterpolateZ(x, h);
                if (z < m_zBuffer(h, x))
                {
                    m_zBuffer(h, x) = z;
                    m_frameBuffer(h, x) = m_triangles[edge1->index].getUniformColor();
                }
            }
        }
    }
    return m_frameBuffer;
}

void ScanLineZbuffer::initTable()
{
    for (size_t i = 0; i < m_triangles.size(); i++)
    {
        if (m_triangles[i].getMinX() < m_width
            && m_triangles[i].getMaxX() >= 0
            && m_triangles[i].getMinY() < m_height
            && m_triangles[i].getMaxY() >= 0)
        {
            m_triangleTable[m_triangles[i].getMaxY()].push_back(i);
            for (size_t j = 0; j < 3; j++)
            {
                Edge edge;
                cv::Vec4f begin = m_triangles[i].getVertex(j);
                cv::Vec4f end = m_triangles[i].getVertex((j + 1) % 3);
                // assert begin[1] > end[1]
                if (begin[1] < end[1])
                {
                    std::swap(begin, end);
                }
                edge.x = begin[0] / begin[3];
                edge.y = begin[1] / begin[3];
                edge.dx = (end[0] / end[3] - begin[0] / begin[3]) / (end[1] / end[3] - begin[1] / begin[3]);
                edge.dy = static_cast<int>(end[1] / end[3] - begin[1] / begin[3]);
                edge.index = i;
                m_edgeTable[static_cast<int>(begin[1] / begin[3])].push_back(std::make_shared<Edge>(edge));
                m_triangle2Edge[i][j] = std::make_shared<Edge>(edge);
            }
        }
    }
}

bool ScanLineZbuffer::cross(const std::shared_ptr<Edge> edge, int line) const
{
    return edge->y + edge->dy < line && edge->y >= line;
}

bool ScanLineZbuffer::insertEdgePair(int index, int line, std::list<EdgePair> &activeEdgeTable)
{
    EdgePair edgePair;
    bool flag = false;
    for (const auto &edge : m_triangle2Edge[index])
    {
        if (cross(edge, line))
        {
            if (edgePair.first == nullptr)
            {
                edgePair.first = edge;
            }
            else
            {
                edgePair.second = edge;
            }
        }
    }
    if (edgePair.first != nullptr && edgePair.second != nullptr)
    {
        activeEdgeTable.push_back(edgePair);
        flag = true;
        if (edgePair.first->x > edgePair.second->x)
        {
            std::swap(edgePair.first, edgePair.second);
        }
    }
    return flag;
}