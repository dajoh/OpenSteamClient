#ifndef MESSAGES_MSGHDR_HPP
#define MESSAGES_MSGHDR_HPP

#include <OpenSteamClient/Enums.hpp>
#include "../InputStream.hpp"
#include "../OutputStream.hpp"

namespace Sc
{
	class MsgHdr
	{
	public:
		MsgHdr();
		MsgHdr(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		EMsg msg;
		uint64_t targetJobId;
		uint64_t sourceJobId;
	};

	// ------------------------------------------------------------------------
	// MsgHdr Implementation
	// ------------------------------------------------------------------------

	inline MsgHdr::MsgHdr()
	{
		msg = EMsg_Invalid;
		targetJobId = UINT64_MAX;
		sourceJobId = UINT64_MAX;
	}

	inline MsgHdr::MsgHdr(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgHdr::Serialize(OutputStream &stream)
	{
		stream.Write<uint32_t>(msg);
		stream.Write<uint64_t>(targetJobId);
		stream.Write<uint64_t>(sourceJobId);
	}

	inline void MsgHdr::Deserialize(InputStream &stream)
	{
		uint32_t msgTemp;
		stream.Read<uint32_t>(msgTemp);
		stream.Read<uint64_t>(targetJobId);
		stream.Read<uint64_t>(sourceJobId);
		msg = (EMsg)msgTemp;
	}
}

#endif