#ifndef UTIL_HPP
#define UTIL_HPP

#include <filesystem>
#include <iostream>
#include <fstream>

namespace util
{
	std::string read_file(const std::filesystem::path&);
};

#endif // !UTIL_HPP