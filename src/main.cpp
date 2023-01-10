#include "Rasterizer.h"
#include "Triangle.h"
#include "OBJ_Loader.h"

std::optional<std::vector<Triangle>> loadModel(const std::string &filepath);

int main()
{
    const int width = 1800;
    const int height = 1800;
    const float angle = 20.0f;
    const cv::Vec3f eye { 0.0f, 0.0f, 10.0f };
    
    Rasterizer rasterizer(height, width);
    auto triangles = loadModel("models/spot/spot_triangulated_good.obj");
    if (!triangles.has_value())
    {
        std::cerr << "Failed to load model" << std::endl;
        return -1;
    }

    rasterizer.setModel(Rasterizer::getModelMatrix(angle, cv::Vec3f(0, 1, 0)));
    rasterizer.setView(Rasterizer::getViewMatrix(eye));
    rasterizer.setProjection(Rasterizer::getProjectionMatrix(15.0f, 1, 0.1f, 50.0f));
    cv::Mat3f frameBuffer = rasterizer.draw(triangles.value());
    cv::Mat image(width, height, CV_32FC3, frameBuffer.data);
    cv::rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
    cv::imwrite("image.png", image);
    cv::waitKey(0);
    return 0;
}

std::optional<std::vector<Triangle>> loadModel(const std::string &filepath)
{
    objl::Loader loader;
    if (!loader.LoadFile(filepath))
    {
        std::cout << "Failed to load model: " << filepath << std::endl;
        return std::nullopt;
    }
    std::vector<Triangle> triangles;
    for (const auto &mesh : loader.LoadedMeshes)
    {
        for (size_t i = 0; i < mesh.Vertices.size(); i += 3)
        {
            std::shared_ptr<Triangle> triangle = std::make_shared<Triangle>();
            for (int j = 0; j < 3; j++)
            {
                triangle->setVertex(j, cv::Vec4f(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z, 1.0f));
                triangle->setNormal(j, cv::Vec3f(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z));
                triangle->setTexCoord(j, cv::Vec2f(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y));
            }
            triangles.push_back(*triangle);
        }
    }
    std::cout << "Loaded " << triangles.size() << " triangles" << std::endl;
    return triangles;
}