#ifndef __OCTTREE_H__
#define __OCTTREE_H__

#include "Triangle.h"
#include "AABB.h"

class OctTree
{
public:
    struct OctTreeNode
    {
        AABB aabb;
        std::vector<Triangle> triangles;
        std::array<std::shared_ptr<OctTreeNode>, 8> children;

        OctTreeNode()
        {
            triangles = std::vector<Triangle>();
            aabb = AABB(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        }

        OctTreeNode(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
        {
            aabb = AABB(xMin, xMax, yMin, yMax, zMin, zMax);
        }
    };

private:
    int m_depth = 10;
    std::shared_ptr<OctTreeNode> m_root;

public:
    OctTree() { }
    OctTree(const std::vector<Triangle> &triangles);

    ~OctTree() { }

    std::shared_ptr<OctTreeNode> getRoot() const { return m_root; }
    void init(const std::vector<Triangle> &triangles);
};

#endif