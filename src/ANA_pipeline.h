#pragma once

#include <string>
#include <vector>
namespace ANA
{
class ANAPipeline
{
public:
    ANAPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);

private:
    static std::vector<char> readFile(const std::string& filename);
};
} // namespace ANA