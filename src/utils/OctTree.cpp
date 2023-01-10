#include <stack>
#include "OctTree.h"
#include "log.h"

OctTree::OctTree(const std::vector<Triangle> &triangles)
{
    m_root = std::make_shared<OctTreeNode>();
    init(triangles);
}

void OctTree::init(const std::vector<Triangle> &triangles)
{
    for (const Triangle triangle : triangles)
    {
        cv::Vec3f n = triangle.getNormal();
        // quick rejection
        if (n.dot(cv::Vec3f(0, 0, 1)) > 0)
        {
            std::shared_ptr<OctTreeNode> node = m_root;
            int depth = m_depth;
            while (depth-- > 0)
            {
                if (depth != 0)
                {
                    AABB &aabb = node->aabb;
                    cv::Vec3f center = aabb.getCenter();
                    float r = aabb.getRadius();
                    AABB subAABB;
#define XX(idx, x1, y1, z1, x2, y2, z2) \
    subAABB = AABB(center + cv::Vec3f(x1, y1, z1), center + cv::Vec3f(x2, y2, z2)); \
    if (subAABB.containTriangle(triangle)) \
    { \
        if (node->children[idx] == nullptr) \
        { \
            node->children[idx] = std::make_shared<OctTreeNode>(); \
            node->children[idx]->aabb = subAABB; \
        } \
        node = node->children[idx]; \
        continue; \
    }
                    XX(0, 0, 0, 0, r, r, r)
                    XX(1, -r, 0, 0, 0, r, r)
                    XX(2, -r, -r, 0, 0, 0, r)
                    XX(3, 0, r, 0, r, 0, r)
                    XX(4, 0, 0, -r, r, r, 0)
                    XX(5, -r, 0, -r, 0, r, 0)
                    XX(6, -r, -r, -r, 0, 0, 0)
                    XX(7, 0, -r, -r, r, 0, 0)
#undef XX
                }
                node->triangles.push_back(triangle);
                break;
            }
        }
    }
}