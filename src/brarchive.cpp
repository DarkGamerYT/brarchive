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

	// Verify header
	if (header != brarchive::HEADER)
	{
		std::cerr << "Invalid archive header for file: " << path << std::endl;
		return {};
	};

	uint32_t file_count = stream.readUnsignedInt();
	uint32_t version = stream.readUnsignedInt();

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

	// Read rest of file content
	const std::string& content = stream.readString();

	for (FileEntry& file : files)
	{
		file.data = content.substr(file.offset, file.length);
	};

	return files;
};

void brarchive::write(const std::filesystem::path& path, const std::filesystem::path& out)
{
	std::filesystem::create_directories(out.parent_path());
	std::fstream output(out.string() + ".brarchive", std::ios::binary | std::ios::out);
	if (!output)
	{
		std::cerr << "Failed to create output file: " << out << std::endl;
		return;
	};

	BinaryStream stream{ output };
	stream.writeBigEndianUnsignedLong(brarchive::HEADER); // Writes the magic number

	std::vector<FileEntry> files = {};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
	{
		if (!entry.is_regular_file())
			continue;

		FileEntry file;
		file.name = entry.path().filename().string();

		if (entry.path().extension() == ".json")
			file.length = static_cast<uint32_t>(std::filesystem::file_size(entry.path()));
		else file.length = 0;

		files.push_back(file);
	};

	stream.writeUnsignedInt(static_cast<uint32_t>(files.size())); // Number of entries
	stream.writeUnsignedInt(1); // Version

	// Writes file entries (name, offset, and length)
	uint32_t offset = 0;
	for (FileEntry& file : files)
	{
		stream.writeString(file.name, 247);

		stream.writeUnsignedInt(offset);
		stream.writeUnsignedInt(file.length);
		offset += file.length;
	};

	// Writes file content at the end of the .brarchive file
	for (const auto& file : files) {
		std::filesystem::path file_path = path / file.name;
		std::ifstream input(file_path, std::ios::binary);
		if (!input)
		{
			std::cerr << "Failed to open file for reading: " << file_path << std::endl;
			continue;
		};

		std::vector<char> data(file.length);
		input.read(data.data(), file.length);

		stream.mStream.write(data.data(), file.length);
	};
};