#ifndef MESSAGES_MSGCLIENTCHATENTER_HPP
#define MESSAGES_MSGCLIENTCHATENTER_HPP

#include <OpenSteamClient/Exception.hpp>
#include "MsgHdrExt.hpp"
#include "../KeyValues.hpp"

namespace Sc
{
	using std::unordered_map;

	class MsgClientChatEnter
	{
	public:
		MsgClientChatEnter();
		MsgClientChatEnter(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		MsgHdrExt header;
		SteamId chatId;
		SteamId friendId;
		EChatRoomType chatType;
		SteamId ownerId;
		SteamId clanId;
		uint8_t chatFlags;
		EChatRoomEnterResponse response;

		uint32_t memberCount;
		string chatName;
		list<KeyValues> memberInfo;
	};

	// ------------------------------------------------------------------------
	// MsgClientChatEnter Implementation
	// ------------------------------------------------------------------------

	inline MsgClientChatEnter::MsgClientChatEnter()
	{
		chatType = EChatRoomType_Friend;
		chatFlags = 0;
		response = EChatRoomEnterResponse_Success;
		memberCount = 0;
	}

	inline MsgClientChatEnter::MsgClientChatEnter(InputStream &stream)
	{
		Deserialize(stream);
	}

	inline void MsgClientChatEnter::Serialize(OutputStream &stream)
	{
		throw Exception("Not implemented.");
	}

	inline void MsgClientChatEnter::Deserialize(InputStream &stream)
	{
		uint32_t chatTypeTemp;
		uint32_t responseTemp;

		header.Deserialize(stream);
		stream.Read<uint64_t>(chatId);
		stream.Read<uint64_t>(friendId);
		stream.Read<uint32_t>(chatTypeTemp);
		stream.Read<uint64_t>(ownerId);
		stream.Read<uint64_t>(clanId);
		stream.Read<uint8_t>(chatFlags);
		stream.Read<uint32_t>(responseTemp);

		if(responseTemp == EChatRoomEnterResponse_Success)
		{
			stream.Read<uint32_t>(memberCount);
			stream.ReadNullTerminated(chatName);

			for(uint32_t i = 0; i < memberCount; i++)
			{
				memberInfo.push_back(KeyValues(stream));
			}
		}

		chatType = (EChatRoomType)chatTypeTemp;
		response = (EChatRoomEnterResponse)responseTemp;
	}
}

#endif