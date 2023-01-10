#include "ScanLineZbuffer.h"
#include "log.h"

#define __OUTPUT_DEBUG_INFO__ false

ScanLineZbuffer::ScanLineZbuffer(int height, int width, const std::vector<Triangle> &triangles) :
    RasterizeAlg(height, width),
    m_triangles(triangles)
{
    m_edgeTable = std::vector<std::list<std::shared_ptr<Edge>>>(m_height, std::list<std::shared_ptr<Edge>>());
    m_triangleTable = std::vector<std::vector<int>>(m_height, std::vector<int>());
    m_triangle2Edge = std::vector<std::array<std::shared_ptr<Edge>, 3>>(m_triangles.size(), std::array<std::shared_ptr<Edge>, 3>());
    initTable();
    m_zBuffer = cv::Mat1f(width, height, std::numeric_limits<float>::max());

#if __OUTPUT_DEBUG_INFO__
    CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "triangle table";
    // print triangle table
    int __count = 0;
    for (int i = 0; i < m_triangleTable.size(); i++)
    {
        if (!m_triangleTable[i].empty())
        {
            std::cout << "y = " << i << ": (total " << m_triangleTable[i].size() << ") ";
            __count += m_triangleTable[i].size();
            for (auto &triangle : m_triangleTable[i])
            {
                std::cout << m_triangles[triangle].getVertex(0) << " " << m_triangles[triangle].getVertex(1) << " " << m_triangles[triangle].getVertex(2) << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "scanning total triangles: " << __count << std::endl;
    CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "edge table";
    // print edge table
    __count = 0;
    for (int i = 0; i < m_edgeTable.size(); i++)
    {
        if (!m_edgeTable[i].empty())
        {
            std::cout << "y = " << i << ": " << "(total " << m_edgeTable[i].size() << ") ";
            __count += m_edgeTable[i].size();
            for (auto &edge : m_edgeTable[i])
            {
                std::cout << edge->x << " " << edge->dx << " " << edge->y << " " << edge->dy << "; ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << "scanning total edges: " << __count << std::endl;
#endif
}

cv::Mat3f ScanLineZbuffer::operator()(const std::vector<Triangle> &triangles)
{
#if __OUTPUT_DEBUG_INFO__
    CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "========== triangle normal color ==========";
    for (const auto &t : triangles)
    {
        std::cout << t.getNormalColor() << std::endl;
    }
#endif
    std::cout << "ScanLineZbuffer rasterizing ..." << std::endl;
    // active edge table (AET)
    std::list<EdgePair> activeEdgeTable;
    // active polygon table (APT)
    std::set<int> activeTriangleTable;

    for (int h = m_height - 1; h >= 0; h--)
    {
#if __OUTPUT_DEBUG_INFO__
        CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << " ========== scan line: " << h << " ==========";
#endif
        for (auto it = activeEdgeTable.begin(); it != activeEdgeTable.end(); it++)
        {
            auto &edge1 = it->first;
            auto &edge2 = it->second;
            if (edge1->dy >= 0 || edge2->dy >= 0)
            {
                edge1->flag = (edge1->dy >= 0);
                edge2->flag = (edge2->dy >= 0);
                it = activeEdgeTable.erase(it);
                if (!insertEdgePair(edge1->index, h, activeEdgeTable))
                {
                    activeTriangleTable.erase(edge1->index);
                }
            }
        }

        for (auto x : m_triangleTable[h])
        {
            activeTriangleTable.insert(x);
            insertEdgePair(x, h, activeEdgeTable);
        }

        for (const auto &edgePair : activeEdgeTable)
        {
            auto &edge1 = edgePair.first;
            auto &edge2 = edgePair.second;
            // int x1 = static_cast<int>(edge1->x);
            // int x2 = static_cast<int>(edge2->x) + 1;
            int x1 = std::round(edge1->x);
            int x2 = std::round(edge2->x);
#if __OUTPUT_DEBUG_INFO__
            CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "interval: x1 = " << x1 << ", x2 = " << x2;
            CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "edge1: [" << edge1->x << ", " << edge1->y << ", " << edge1->dx << ", " << edge1->dy << "]";
            CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "edge2: [" << edge2->x << ", " << edge2->y << ", " << edge2->dx << ", " << edge2->dy << "]";
#endif
            if (x1 > x2)
            {
                std::swap(x1, x2);
            }
            for (int x = x1; x <= x2; x++)
            {
                float z = m_triangles[edge1->index].getInterpolateZ(x, h);
                if (z < m_zBuffer(x, h))
                {
                    m_zBuffer(x, h) = z;
                    m_frameBuffer(x, h) = m_triangles[edge1->index].getNormalColor();
                }
            }
            edge1->x -= edge1->dx;
            edge2->x -= edge2->dx;
            edge1->dy++;
            edge2->dy++;
            edge1->y--;
            edge2->y--;
        }
#if __OUTPUT_DEBUG_INFO__
        // print active triangle table
        if (!activeTriangleTable.empty())
        {
            CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "activeTriangleTable: ";
            for (auto &triangle : activeTriangleTable)
            {
                std::cout << m_triangles[triangle].getVertex(0) << " " << m_triangles[triangle].getVertex(1) << " " << m_triangles[triangle].getVertex(2) << std::endl;
            }
            std::cout << std::endl;
        }
        else
        {
            CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "activeTriangleTable: empty";
        }
        // print active edge table
        if (!activeEdgeTable.empty())
        {
            CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "activeEdgeTable: ";
            for (auto &edgePair : activeEdgeTable)
            {
                auto &edge1 = edgePair.first;
                auto &edge2 = edgePair.second;
                std::cout << edge1->x << " " << edge1->dx << " " << edge1->y << " " << edge1->dy << "; ";
                std::cout << edge2->x << " " << edge2->dx << " " << edge2->y << " " << edge2->dy << "; ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        else
        {
            CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "activeEdgeTable: empty";
        }
#endif
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
    if (edge->y + edge->dy < line && edge->y + 1 >= line)
    {
        return true;
    }
    return false;
}

bool ScanLineZbuffer::insertEdgePair(int index, int line, std::list<EdgePair> &activeEdgeTable)
{
    EdgePair edgePair;
    bool flag = false;
    for (const auto &edge : m_triangle2Edge[index])
    {
        if (cross(edge, line) && edge->dy < 0)
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
#if __OUTPUT_DEBUG_INFO__
        CCZOE_LOG_DEBUG(CCZOE_LOG_ROOT()) << "insertEdgePair: " << edgePair.first->x << " " << edgePair.first->dx << " " << edgePair.first->y << " " << edgePair.first->dy << "; "
                                          << edgePair.second->x << " " << edgePair.second->dx << " " << edgePair.second->y << " " << edgePair.second->dy << "; ";
#endif
        activeEdgeTable.push_back(edgePair);
        flag = true;
        if (edgePair.first->x > edgePair.second->x)
        {
            std::swap(edgePair.first, edgePair.second);
        }
    }
    return flag;
}