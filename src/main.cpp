#include <fstream>
#include <json/json.h>
#include "Rasterizer.h"
#include "Triangle.h"
#include "OBJ_Loader.h"

std::optional<std::vector<Triangle>> loadModel(const std::string &filepath);
void rasterizeModel(std::string model);

int main()
{
    rasterizeModel("bunny");
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

void rasterizeModel(std::string model)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream ifs("models/settings.json", std::ifstream::binary);
    if (reader.parse(ifs, root))
    {
        const std::string filepath = root[model]["path"].asString();
        const int width = root[model]["width"].asInt();
        const int height = root[model]["height"].asInt();
        const float angle = root[model]["angle"].asFloat();
        const cv::Vec3f eye { 
            root[model]["eye"][0].asFloat(), 
            root[model]["eye"][1].asFloat(), 
            root[model]["eye"][2].asFloat() 
        };
        const cv::Vec3f axis {
            root[model]["axis"][0].asFloat(),
            root[model]["axis"][1].asFloat(),
            root[model]["axis"][2].asFloat(),
        };
        const float eyeFov = root[model]["project"][0].asFloat();
        const float aspect = root[model]["project"][1].asFloat();
        const float near = root[model]["project"][2].asFloat();
        const float far = root[model]["project"][3].asFloat();

        Rasterizer rasterizer(height, width);
        auto triangles = loadModel(filepath);
        if (!triangles.has_value())
        {
            std::cerr << "Failed to load model" << std::endl;
            return;
        }

        rasterizer.setModel(Rasterizer::getModelMatrix(angle, axis));
        rasterizer.setView(Rasterizer::getViewMatrix(eye));
        rasterizer.setProjection(Rasterizer::getProjectionMatrix(eyeFov, aspect, near, far));
        cv::Mat3f frameBuffer = rasterizer.draw(triangles.value());
        cv::Mat image(width, height, CV_32FC3, frameBuffer.data);
        cv::rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        cv::imwrite("results/" + model + ".png", image);
    }
}