#ifndef BRARCHIVE_HPP
#define BRARCHIVE_HPP

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stream.h>

#include "BinaryStream.hpp"

namespace brarchive
{
	const uint64_t HEADER = 0x7d2725b1a0527026;
	struct FileEntry
	{
		std::string name;
		uint32_t length;
		uint32_t offset;
		std::string data;
	};

	std::vector<FileEntry> read(const std::filesystem::path& path);
	void write(const std::filesystem::path& path, const std::filesystem::path& out);

	inline bool verify_header(uint64_t value) { return value == brarchive::HEADER; };
};

#endif // !BRARCHIVE_HPP
