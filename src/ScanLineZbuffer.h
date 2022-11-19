#ifndef __SCANLINEZBUFFER_H__
#define __SCANLINEZBUFFER_H__

#include <opencv2/opencv.hpp>
#include "Triangle.h"

class ScanLineZbuffer
{
private:
    struct Edge
    {
        float x;    // upper x coordinate
        float y;    // upper y coordinate
        float dx;   // delta x bewteen 2 consecutive scan line
        int dy;     // how many scan lines the edge span 
        int index;  // index of triangle to which the edge belongs
    };

    using EdgePair = std::pair<std::shared_ptr<Edge>, std::shared_ptr<Edge>>;

private:
    int m_height;
    int m_width;
    std::vector<Triangle> m_triangles;
    std::vector<std::list<std::shared_ptr<Edge>>> m_edgeTable;
    std::vector<std::list<int>> m_triangleTable;
    std::vector<std::array<std::shared_ptr<Edge>, 3>> m_triangle2Edge;
    // frame buffer
    cv::Mat3f m_frameBuffer;
    cv::Mat1f m_zBuffer;
    void initTable();
    bool cross(const std::shared_ptr<Edge> edge, int line) const;
    bool insertEdgePair(int index, int line, std::list<EdgePair> &activeEdgeTable);

public:
    ScanLineZbuffer() = delete;
    ScanLineZbuffer(int height, int width, const std::vector<Triangle> &triangles);
    cv::Mat3f operator()(const std::vector<Triangle> &triangles);
};

#endif