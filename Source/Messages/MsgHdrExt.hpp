#ifndef MESSAGES_MSGHDREXT_HPP
#define MESSAGES_MSGHDREXT_HPP

#include <OpenSteamClient/Enums.hpp>
#include <OpenSteamClient/SteamId.hpp>
#include "../InputStream.hpp"
#include "../OutputStream.hpp"

namespace Sc
{
	class MsgHdrExt
	{
	public:
		MsgHdrExt();
		MsgHdrExt(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		EMsg msg;
		uint8_t headerSize;
		uint16_t headerVersion;
		uint64_t targetJobId;
		uint64_t sourceJobId;
		uint8_t headerCanary;
		SteamId steamId;
		uint32_t sessionId;
	};

	// ------------------------------------------------------------------------
	// MsgHdrExt Implementation
	// ------------------------------------------------------------------------

	inline MsgHdrExt::MsgHdrExt()
	{
		msg = EMsg_Invalid;
		headerSize = 36;
		headerVersion = 2;
		targetJobId = UINT64_MAX;
		sourceJobId = UINT64_MAX;
		headerCanary = 239;
		sessionId = 0;
	}

	inline MsgHdrExt::MsgHdrExt(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgHdrExt::Serialize(OutputStream &stream)
	{
		stream.Write<uint32_t>(msg);
		stream.Write<uint8_t>(headerSize);
		stream.Write<uint16_t>(headerVersion);
		stream.Write<uint64_t>(targetJobId);
		stream.Write<uint64_t>(sourceJobId);
		stream.Write<uint8_t>(headerCanary);
		stream.Write<uint64_t>(steamId);
		stream.Write<uint32_t>(sessionId);
	}

	inline void MsgHdrExt::Deserialize(InputStream &stream)
	{
		uint32_t msgTemp;
		stream.Read<uint32_t>(msgTemp);
		stream.Read<uint8_t>(headerSize);
		stream.Read<uint16_t>(headerVersion);
		stream.Read<uint64_t>(targetJobId);
		stream.Read<uint64_t>(sourceJobId);
		stream.Read<uint8_t>(headerCanary);
		stream.Read<uint64_t>(steamId);
		stream.Read<uint32_t>(sessionId);
		msg = (EMsg)msgTemp;
	}
}

#endif