#include <iostream>
#include "brarchive.hpp"

int main(int argc, char* argv[])
{
	if (!argv[1] || !std::filesystem::exists(argv[1]))
	{
		std::cerr << "Failed to find input directory." << std::endl;
		return -1;
	};

	std::filesystem::path input(argv[1]);
	for (const auto& entry : std::filesystem::recursive_directory_iterator(input))
	{
		if (!entry.is_directory())
			continue;

		std::filesystem::path path = entry.path();
		std::filesystem::path relativePath = std::filesystem::relative(path, input);
		if (relativePath.string().find("__brarchive") != std::string::npos)
			continue;


		std::filesystem::path out = input / std::filesystem::path("__brarchive") / relativePath;
		brarchive::write(path, out);
	};
};