#include "brarchive.hpp"
namespace brarchive
{
	// Readers
	uint8_t BinaryStream::readByte()
	{
		uint8_t value = 0;
		mStream.read(reinterpret_cast<char*>(&value), sizeof(value));

		return value;
	};

	template<typename T>
	T BinaryStream::read(std::size_t length)
	{
		std::vector<uint8_t> bytes(length);
		mStream.read(reinterpret_cast<char*>(bytes.data()), length);

		T value = 0;
		for (size_t i = 0; i < length; ++i)
			value |= (static_cast<T>(bytes[i]) << (8 * i));

		return value;
	};

	template<typename T>
	T BinaryStream::readBigEndian(std::size_t length)
	{
		std::vector<uint8_t> bytes(length);
		mStream.read(reinterpret_cast<char*>(bytes.data()), length);

		T value = 0;
		for (size_t i = 0; i < length; ++i)
			value = (value << 8) | bytes[i];

		return value;
	};

	uint32_t BinaryStream::readUnsignedInt()
	{
		return this->read<uint32_t>(4);
	};
	int32_t BinaryStream::readInt()
	{
		uint32_t raw = this->readUnsignedInt();
		return *(int32_t*)&raw;
	};

	uint32_t BinaryStream::readBigEndianUnsignedInt()
	{
		return this->readBigEndian<uint32_t>(4);
	};
	int32_t BinaryStream::readBigEndianInt()
	{
		uint32_t raw = this->readBigEndianUnsignedInt();
		return *(int32_t*)&raw;
	};

	uint64_t BinaryStream::readUnsignedLong()
	{
		return this->read<uint64_t>(8);
	};
	int64_t BinaryStream::readLong()
	{
		uint64_t raw = this->readUnsignedLong();
		return *(int64_t*)&raw;
	};

	uint64_t BinaryStream::readBigEndianUnsignedLong()
	{
		return this->readBigEndian<uint64_t>(8);
	};
	int64_t BinaryStream::readBigEndianLong()
	{
		uint64_t raw = this->readBigEndianUnsignedLong();
		return *(int64_t*)&raw;
	};

	std::string BinaryStream::readString(std::size_t length)
	{
		std::string result(length, '\0');
		mStream.read(result.data(), length);
		return result;
	};

	std::string BinaryStream::readString()
	{
		std::streampos currentPos = mStream.tellg();
		mStream.seekg(0, std::ios::end);
		std::streampos endPos = mStream.tellg();

		std::size_t length = static_cast<std::size_t>(endPos - currentPos);
		mStream.seekg(currentPos);

		return this->readString(length);
	};


	// Writers
	void BinaryStream::writeByte(uint8_t value)
	{
		mStream.write(reinterpret_cast<const char*>(&value), sizeof(value));
	};

	template<typename T>
	void BinaryStream::write(T value, int count)
	{
		for (int i = 0; i < count; i++)
		{
			uint8_t byte = static_cast<uint8_t>(value >> (8 * i));
			mStream.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
		};
	};

	template<typename T>
	void BinaryStream::writeBigEndian(T value, int count)
	{
		for (int i = count - 1; i >= 0; i--)
		{
			uint8_t byte = static_cast<uint8_t>(value >> (8 * i));
			mStream.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
		};
	};

	void BinaryStream::writeUnsignedInt(uint32_t value)
	{
		this->write<uint32_t>(value, 4);
	};
	void BinaryStream::writeInt(int32_t value)
	{
		this->writeUnsignedInt(value & 0xffffffff);
	};

	void BinaryStream::writeBigEndianUnsignedInt(uint32_t value)
	{
		this->writeBigEndian<uint32_t>(value, 4);
	};
	void BinaryStream::writeBigEndianInt(int32_t value)
	{
		this->writeBigEndianUnsignedInt(value & 0xffffffff);
	};

	void BinaryStream::writeUnsignedLong(uint64_t value)
	{
		this->write<uint64_t>(value, 8);
	};
	void BinaryStream::writeLong(int64_t value)
	{
		this->writeUnsignedLong(value & 0xffffffffffffffff);
	};

	void BinaryStream::writeBigEndianUnsignedLong(uint64_t value)
	{
		this->writeBigEndian<uint64_t>(value, 8);
	};
	void BinaryStream::writeBigEndianLong(int64_t value)
	{
		this->writeBigEndianUnsignedLong(value & 0xffffffffffffffff);
	};

	void BinaryStream::writeString(const std::string& str, uint8_t max_length)
	{
		uint8_t name_length = std::min(static_cast<uint8_t>(str.size()), max_length);
		this->writeByte(name_length);

		mStream.write(str.c_str(), name_length);
		uint8_t padding = max_length - name_length;
		if (padding > 0)
		{
			std::vector<char> null_padding(padding, '\0');
			mStream.write(null_padding.data(), padding);
		};
	};
};