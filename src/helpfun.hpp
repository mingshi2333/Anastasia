#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h> // for readlink
#include <limits.h> // for PATH_MAX

namespace helpfunc
{
std::string getExecutablePath();

std::vector<char> readFile(const std::string& filename);
} // namespace helpfunc