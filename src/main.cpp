#include "Rasterizer.h"
#include "Triangle.h"
#include "OBJ_Loader.h"

std::optional<std::vector<Triangle>> loadModel(const std::string &filepath);

int main()
{
    const int width = 700;
    const int height = 700;
    const float angle = 140.0f;
    const cv::Vec3f eye { 0.0f, 0.0f, 10.0f };
    
    Rasterizer rasterizer(height, width);
    rasterizer.setTexture(Texture("models/spot/spot_texture.png"));
    
    auto triangles = loadModel("models/spot/spot_triangulated_good.obj");
    if (!triangles.has_value())
    {
        std::cerr << "Failed to load model" << std::endl;
        return -1;
    }

    rasterizer.setModel(Rasterizer::getModelMatrix(angle));
    rasterizer.setView(Rasterizer::getViewMatrix(eye));
    rasterizer.setProjection(Rasterizer::getProjectionMatrix(45.0f, 1, 0.1f, 50.0f));
    cv::Mat3f frameBuffer = rasterizer.draw(triangles.value());
    cv::imshow("frameBuffer", frameBuffer);
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