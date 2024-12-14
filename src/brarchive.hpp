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
#include "util.hpp"

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

	std::vector<FileEntry> read(const std::filesystem::path&);
	void write(const std::filesystem::path&, const std::filesystem::path&);

	inline bool verify_header(uint64_t value) { return value == brarchive::HEADER; };

	std::vector<FileEntry> _read_files(uint32_t, BinaryStream&);
	const char* _read_data(const std::string&, FileEntry&);

	std::vector<FileEntry> _get_directory_files(const std::filesystem::path&);
};

#endif // !BRARCHIVE_HPP
