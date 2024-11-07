#ifndef BRARCHIVE_HPP
#define BRARCHIVE_HPP

#include <filesystem>
#include <iostream>
#include <fstream>

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

	class BinaryStream
	{
	public:
		std::fstream& mStream;

	public:
		BinaryStream(std::fstream& stream)
			: mStream(stream) {};

		// Readers
		uint8_t readByte();

		template<typename T>
		T read(std::size_t length);

		template<typename T>
		T readBigEndian(std::size_t length);

		uint32_t readBigEndianUnsignedInt();
		int32_t readBigEndianInt();
		uint32_t readUnsignedInt();
		int32_t readInt();

		uint64_t readBigEndianUnsignedLong();
		int64_t readBigEndianLong();
		uint64_t readUnsignedLong();
		int64_t readLong();

		std::string readString(std::size_t length);
		std::string readString();


		// Writers
		void writeByte(uint8_t value);

		template<typename T>
		void write(T value, int count);

		template<typename T>
		void writeBigEndian(T value, int count);

		void writeUnsignedInt(uint32_t value);
		void writeInt(int32_t value);

		void writeBigEndianUnsignedInt(uint32_t value);
		void writeBigEndianInt(int32_t value);

		void writeUnsignedLong(uint64_t value);
		void writeLong(int64_t value);

		void writeBigEndianUnsignedLong(uint64_t value);
		void writeBigEndianLong(int64_t value);

		void writeString(const std::string& str, uint8_t max_length);
	};

	std::vector<FileEntry> read(const std::filesystem::path& path);
	void write(const std::filesystem::path& path, const std::filesystem::path& out);
};

#endif // !BRARCHIVE_HPP
