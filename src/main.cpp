#include <iostream>
#include "brarchive.hpp"

int main(int argc, char* argv[])
{
	if (!argv[1])
	{
		std::cerr << "No args." << std::endl;
		return -1;
	};

	std::string command(argv[1]);
	if (command == "generate")
	{
		if (!argv[2] || !std::filesystem::exists(argv[2]))
		{
			std::cerr << "Failed to find input directory." << std::endl;
			return -1;
		};

		std::filesystem::path input(argv[2]);
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
	}
	else if (command == "extract")
	{
		if (!argv[2] || !std::filesystem::exists(argv[2]))
		{
			std::cerr << "Failed to find input directory." << std::endl;
			return -1;
		};

		std::filesystem::path input(argv[2]);
		if (std::filesystem::is_regular_file(input))
		{
			const std::filesystem::path& directory = input.parent_path();

			const auto& files = brarchive::read(input);
			for (auto& file : files)
			{
				std::fstream output(directory / file.name, std::ios::out);
				if (!output)
				{
					std::cerr << "Failed to create output file: " << std::filesystem::path(directory) / file.name << std::endl;
					continue;
				};

				output << file.data;
				output.close();
			};

			return 0;
		};

		for (const auto& entry : std::filesystem::recursive_directory_iterator(input))
		{
			if (!entry.is_regular_file())
				continue;

			std::filesystem::path path = entry.path();
			std::filesystem::path relativePath = std::filesystem::relative(path, input);
			if (relativePath.string().find("__brarchive") == std::string::npos)
				continue;

			const auto& files = brarchive::read(input / relativePath);
			const std::string& filename = std::filesystem::relative(relativePath, "__brarchive").string();
			const std::string& directory = filename.substr(0, filename.find_last_of("."));

			std::filesystem::create_directories(input / directory);
			for (auto& file : files)
			{
				std::fstream output(input / directory / file.name, std::ios::out);
				if (!output)
				{
					std::cerr << "Failed to create output file: " << std::filesystem::path(directory) / file.name << std::endl;
					continue;
				};

				output << file.data;
				output.close();
			};
		};
	}
	else {
		std::cout << "Unknown command: " << command << std::endl;
	};
};