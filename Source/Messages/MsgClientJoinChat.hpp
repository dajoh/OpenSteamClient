#ifndef MESSAGES_MSGCLIENTJOINCHAT_HPP
#define MESSAGES_MSGCLIENTJOINCHAT_HPP

#include "MsgHdrExt.hpp"

namespace Sc
{
	class MsgClientJoinChat
	{
	public:
		MsgClientJoinChat();
		MsgClientJoinChat(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		MsgHdrExt header;
		SteamId chatId;
		uint8_t isVoiceSpeaker;
	};

	// ------------------------------------------------------------------------
	// MsgClientJoinChat Implementation
	// ------------------------------------------------------------------------

	inline MsgClientJoinChat::MsgClientJoinChat()
	{
		isVoiceSpeaker = 0;
	}

	inline MsgClientJoinChat::MsgClientJoinChat(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgClientJoinChat::Serialize(OutputStream &stream)
	{
		header.Serialize(stream);
		stream.Write<uint64_t>(chatId);
		stream.Write<uint8_t>(isVoiceSpeaker);
	}

	inline void MsgClientJoinChat::Deserialize(InputStream &stream)
	{
		header.Deserialize(stream);
		stream.Read<uint64_t>(chatId);
		stream.Read<uint8_t>(isVoiceSpeaker);
	}
}

#endif