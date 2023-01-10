#include <fstream>
#include <iostream>
#include <json/json.h>

void readJsonFile()
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream ifs("models/settings.json", std::ifstream::binary);
    if (reader.parse(ifs, root))
    {
        std::cout << root["bunny"]["path"].asString() << std::endl;
    }
}

int main()
{
    readJsonFile();
    return 0;
}