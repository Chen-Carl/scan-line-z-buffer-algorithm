#include <opencv2/opencv.hpp>
#include "Rasterizer.h"
#include "ScanLineZbuffer.h"
#include "Zbuffer.h"

Rasterizer::Rasterizer(int height, int width) : 
    m_height(height),
    m_width(width)
{
    
}

cv::Mat3f Rasterizer::draw(std::vector<Triangle> &triangles)
{
    cv::Mat1f mvp = m_projection * m_view * m_model;
    std::vector<Triangle> viewSpaceTriangles;
    for (const auto &triangle : triangles)
    {
        std::array<cv::Vec3f, 3> viewSpacePos;
        std::array<cv::Mat1f, 3> viewSpaceModel {
            m_view * m_model * triangle.getVertex(0),
            m_view * m_model * triangle.getVertex(1),
            m_view * m_model * triangle.getVertex(2)
        };
        std::transform(viewSpaceModel.begin(), viewSpaceModel.end(), viewSpacePos.begin(), [](const cv::Mat1f &vertex) { 
            return cv::Vec3f(vertex(0, 0), vertex(1, 0), vertex(2, 0)); 
        });
        std::array<cv::Vec4f, 3> vertexes;
        for (int i = 0; i < 3; i++)
        {
            cv::Mat1f v = m_projection * viewSpaceModel[i];
            vertexes[i] = cv::Vec4f(
                v(0, 0) / v(3, 0),
                v(1, 0) / v(3, 0),
                v(2, 0) / v(3, 0),
                1
            );
        }
        cv::Mat1f invTrans = (m_view * m_model).inv().t();
        std::array<cv::Vec4f, 3> normals;
        for (int i = 0; i < 3; i++)
        {
            cv::Mat1f normal = invTrans * triangle.getNormal(i);
            normals[i] = cv::Vec4f(normal(0, 0), normal(1, 0), normal(2, 0), normal(3, 0));
        };
        float f1 = (50 - 0.1) / 2.0;
        float f2 = (50 + 0.1) / 2.0;
        for (auto &vertex : vertexes)
        {
            vertex[0] = 0.5 * m_width * (vertex[0] + 1);
            vertex[1] = 0.5 * m_height * (vertex[1] + 1);
            vertex[2] = vertex[2] * f1 + f2;
        }

        Triangle newTriangle = triangle;
        for (int i = 0; i < 3; i++)
        {
            newTriangle.setVertex(i, vertexes[i]);
            newTriangle.setNormal(i, cv::Vec3f(normals[i](0), normals[i](1), normals[i](2)));
            newTriangle.setColor(i, cv::Vec3f(148.0, 121.0, 92.0));
        }
        viewSpaceTriangles.push_back(newTriangle);
    }
    // setRasterizeAlg(ScanLineZbuffer(m_height, m_width, viewSpaceTriangles));
    setRasterizeAlg(Zbuffer(m_height, m_width));
    cv::Mat3f frameBuffer = m_rasterizeTriangles(viewSpaceTriangles);
    return frameBuffer;
}

cv::Mat1f Rasterizer::getModelMatrix(float angle)
{
    // angle = angle * M_PI / 180.0f;
    // cv::Mat1f rotation({4, 4}, {
    //     cos(angle), 0.0f, sin(angle), 0.0f,
    //     0.0f, 1.0f, 0.0f, 0.0f,
    //     -sin(angle), 0.0f, cos(angle), 0.0f,
    //     0.0f, 0.0f, 0.0f, 1.0f
    // });
    // cv::Mat1f scale({4, 4}, {
    //     2.5, 0, 0, 0,
    //     0, 2.5, 0, 0,
    //     0, 0, 2.5, 0,
    //     0, 0, 0, 1
    // });
    // cv::Mat1f translation({4, 4}, {
    //     1, 0, 0, 0,
    //     0, 1, 0, 0,
    //     0, 0, 1, 0,
    //     0, 0, 0, 1
    // });
    // return translation * scale * rotation;
	angle = angle / 180.0 * M_PI;
	cv::Mat1f model({4, 4},{
		std::cos(angle), -std::sin(angle), 0, 0,
		std::sin(angle), std::cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	});
    return model;
}

cv::Mat1f Rasterizer::getProjectionMatrix(float fov, float aspect, float near, float far)
{
    float t = std::tan(fov / 2.0 / 180.0 * M_PI) * std::abs(near);
    float b = -t;
    float r = t * aspect;
    float l = -r;
    near = -near;
    far = -far;
    cv::Mat1f persp({4, 4}, {
        near, 0, 0, 0,
        0, near, 0, 0,
        0, 0, near + far, -near * far,
        0, 0, 1, 0
    });
    cv::Mat1f ortho({4, 4}, {
        2 / (r - l), 0, 0, -(r + l) / (r - l),
        0, 2 / (t - b), 0, -(t + b) / (t - b),
        0, 0, 2 / (near - far), -(near + far) / (near - far),
        0, 0, 0, 1
    });
    return ortho * persp;
}

cv::Mat1f Rasterizer::getViewMatrix(const cv::Vec3f &eye)
{
    cv::Mat1f view({4, 4}, {
        1, 0, 0, -eye[0],
        0, 1, 0, -eye[1],
        0, 0, 1, -eye[2],
        0, 0, 0, 1
    });
    return view;
}