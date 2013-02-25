#include <cstdint>
#include <cryptopp/zinflate.h>
#include "ZipHelper.hpp"
#include "InputStream.hpp"

namespace Sc
{
	#pragma pack(push, 1)
	struct ZipHeader
	{
		uint32_t magic;
		uint16_t version;
		uint16_t bitFlags;
		uint16_t compression;
		uint16_t mTime;
		uint16_t cTime;
		uint32_t crc;
		uint32_t compressedSize;
		uint32_t decompressedSize;
		uint16_t nameLength;
		uint16_t fieldLength;
	};
	#pragma pack(pop)

	std::string DecompressZip(const std::string &zip)
	{
		string name;
		ZipHeader header;
		InputStream stream(zip);

		stream.Read<ZipHeader>(header);
		stream.Read<string>(name, header.nameLength);

		string input = zip.substr(sizeof(ZipHeader) + header.nameLength);
		string output;

		CryptoPP::StringSource(input, true,
			new CryptoPP::Inflator(
				new CryptoPP::StringSink(output)
			)
		);

		return output.substr(0, header.decompressedSize);
	}
}