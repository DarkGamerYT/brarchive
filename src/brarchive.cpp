#include "brarchive.hpp"
std::vector<brarchive::FileEntry> brarchive::read(const std::filesystem::path& path)
{
	std::fstream archive(path, std::ios::binary | std::ios::in);
	if (!archive)
	{
		std::cerr << "Failed to open archive file: " << path.string() << std::endl;
		return {};
	};


	BinaryStream stream{ archive };
	uint64_t header = stream.readBigEndianUnsignedLong(); // Magic number

	if (!brarchive::verify_header(header))
	{
		std::cerr << "Invalid archive header for file: " << path << std::endl;
		return {};
	};


	uint32_t file_count = stream.readUnsignedInt();
	uint32_t version = stream.readUnsignedInt();


	std::vector<brarchive::FileEntry> files = brarchive::_read_files(file_count, stream);

	// Read rest of file content
	const std::string& content = stream.readString();
	archive.close();

	for (FileEntry& file : files)
	{
		file.data = brarchive::_read_data(content, file);
	};

	return files;
};

void brarchive::write(const std::filesystem::path& path, const std::filesystem::path& out)
{
	const std::string& outfile = out.string() + ".brarchive";

	std::filesystem::create_directories(out.parent_path());
	std::fstream output(outfile, std::ios::binary | std::ios::out);
	if (!output)
	{
		std::cerr << "Failed to create output file: " << out << std::endl;
		return;
	};

	std::vector<brarchive::FileEntry> files = brarchive::_get_directory_files(path);
	if (files.size() == 0)
	{
		output.close();
		std::filesystem::remove(outfile);
		return;
	};


	BinaryStream stream{ output };
	stream.writeBigEndianUnsignedLong(brarchive::HEADER); // Writes the magic number
	stream.writeUnsignedInt(static_cast<uint32_t>(files.size())); // Number of entries
	stream.writeUnsignedInt(1); // Version

	for (FileEntry& file : files)
	{
		stream.writeString(file.name, 247);
	};

	uint32_t offset = 0;
	for (auto& file : files)
	{
		std::filesystem::path file_path = path / file.name;
		if (!std::filesystem::exists(file_path))
			continue;

		const std::string& data = util::read_file(file_path);
		if (data.empty())
			continue;


		rapidjson::Document json;
		json.Parse<rapidjson::kParseCommentsFlag>(data.c_str());

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		json.Accept(writer);

		stream.writeUnsignedInt(offset); // Write the offset


		if (std::filesystem::path(file.name).extension() == ".json")
		{
			const std::string& str = buffer.GetString();
			file.length = static_cast<uint32_t>(str.size());

			stream.writeUnsignedInt(file.length);
			offset += file.length;

			stream.mStream.write(str.c_str(), file.length);
		}
		else {
			file.length = 0;
			stream.mStream.write("", file.length);
		};
	};

	output.close();
};

std::vector<brarchive::FileEntry> brarchive::_read_files(uint32_t file_count, BinaryStream& stream)
{
	std::vector<brarchive::FileEntry> files = {};
	for (uint32_t i = 0; i < file_count; i++)
	{
		uint8_t name_length = stream.readByte();
		const std::string& name = stream.readString(247).substr(0, name_length); // File name

		uint32_t offset, length;
		offset = stream.readUnsignedInt();
		length = stream.readUnsignedInt();

		files.push_back({ name, length, offset });
	};

	return files;
};

const char* brarchive::_read_data(const std::string& content, FileEntry& file)
{
	rapidjson::Document json;
	json.Parse<rapidjson::kParseCommentsFlag>(content.substr(file.offset, file.length).c_str());

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	json.Accept(writer);

	return buffer.GetString();
};


std::vector<brarchive::FileEntry> brarchive::_get_directory_files(const std::filesystem::path& path)
{
	std::vector<brarchive::FileEntry> files = {};
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (!entry.is_regular_file())
			continue;

		files.push_back({ entry.path().filename().string() });
	};

	return files;
};