#include "util.hpp"
std::string util::read_file(const std::filesystem::path& path)
{
	std::ifstream input(path, std::ios::binary);
	if (!input)
	{
		std::cerr << "Failed to open file for reading: " << path << std::endl;
		return "";
	};

	std::stringstream string;
	string << input.rdbuf();
	
	return string.str();
};