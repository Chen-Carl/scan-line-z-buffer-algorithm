#ifndef __OCTZBUFFER_H__
#define __OCTZBUFFER_H__

#include <opencv2/opencv.hpp>
#include "HierarchicalZbuffer.h"
#include "Triangle.h"
#include "utils/OctTree.h"

class OctZbuffer : public HierarchicalZbuffer
{
    using TreeNode = OctTree::OctTreeNode;
private:
    OctTree m_octTree;

public:
    OctZbuffer() = delete;
    OctZbuffer(int height, int width, const std::vector<Triangle> &triangles);
    virtual ~OctZbuffer() { }

    void traversal(std::shared_ptr<TreeNode> node);
    void drawTreeNode(std::shared_ptr<TreeNode> node);
    std::tuple<int, int, int> getTreeNodeLevel(std::shared_ptr<TreeNode> node);
    std::tuple<int, int, int> getTriangleLevelAndCoor(int xMin, int xMax, int yMin, int yMax);

    virtual cv::Mat3f operator()(const std::vector<Triangle> &triangles) override;
};

#endif