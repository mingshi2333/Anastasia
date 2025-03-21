#include "pipeline.h"
#include <vector>
#include <iostream>
#include <fstream>

namespace ana::vk
{
ANAPipeline::ANAPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
{
    createGraphicsPipeline(vertFilepath, fragFilepath);
}

std::vector<char> ANAPipeline::readFile(const std::string& filename)
{
    std::ifstream file{
        filename, std::ios::ate | std::ios::binary}; // ate is read start at the end of the file to determine the size of the file
    if (!file.is_open()) { throw std::runtime_error("failed to open file!"); }
    size_t fileSize = (size_t)file.tellg(); // get the position of the current character in the input stream
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

void ANAPipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
{
    auto vertCode = readFile(vertFilepath);
    auto fragCode = readFile(fragFilepath);
    std::cout << "Vertex Shader size" << vertCode.size() << std::endl;
    std::cout << "Fragment Shader size" << fragCode.size() << std::endl;
}

} // namespace ana::vk