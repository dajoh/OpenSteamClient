#ifndef MESSAGES_MSGCLIENTCHATMSG_HPP
#define MESSAGES_MSGCLIENTCHATMSG_HPP

#include "MsgHdrExt.hpp"

namespace Sc
{
	class MsgClientChatMsg
	{
	public:
		MsgClientChatMsg();
		MsgClientChatMsg(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		MsgHdrExt header;
		SteamId chatterId;
		SteamId chatId;
		EChatEntryType type;
		string msg;
	};

	// ------------------------------------------------------------------------
	// MsgClientChatMsg Implementation
	// ------------------------------------------------------------------------

	inline MsgClientChatMsg::MsgClientChatMsg()
	{
		type = EChatEntryType_ChatMsg;
	}

	inline MsgClientChatMsg::MsgClientChatMsg(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgClientChatMsg::Serialize(OutputStream &stream)
	{
		header.Serialize(stream);
		stream.Write<uint64_t>(chatterId);
		stream.Write<uint64_t>(chatId);
		stream.Write<uint32_t>(type);
		stream.Write<string>(msg);
	}

	inline void MsgClientChatMsg::Deserialize(InputStream &stream)
	{
		uint32_t typeTemp;
		header.Deserialize(stream);
		stream.Read<uint64_t>(chatterId);
		stream.Read<uint64_t>(chatId);
		stream.Read<uint32_t>(typeTemp);
		stream.ReadNullTerminated(msg);
		type = (EChatEntryType)typeTemp;
	}
}

#endif