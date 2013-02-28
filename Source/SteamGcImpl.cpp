#include "SteamGcImpl.hpp"
#include "Messages/MsgProto.hpp"
#include "Protobufs/steammessages_clientserver.pb.h"

namespace Sc
{
	// ------------------------------------------------------------------------
	// SteamGc::Impl Implementation
	// ------------------------------------------------------------------------

	SteamGc::Impl::Impl(SteamGc *parent, SteamClient::Impl *client) : SteamBaseImpl(client)
	{
		m_parent = parent;
	}

	void SteamGc::Impl::SendGcMsg(const GcMsg &msg)
	{
		OutputStream stream;
		MsgProto<CMsgGCClient> netMsg;

		netMsg.header.msg = EMsg_ClientToGC;
		netMsg.header.proto.set_steamid(m_steamId);
		netMsg.header.proto.set_client_sessionid(m_sessionId);

		netMsg.proto.set_appid(msg.appId);
		netMsg.proto.set_payload(msg.msgData);

		if(msg.isProto)
		{
			netMsg.proto.set_msgtype(msg.msgId | 0x80000000);
		}
		else
		{
			netMsg.proto.set_msgtype(msg.msgId);
		}

		netMsg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamGc::Impl::HandleMessage(const MessageEvent &ev)
	{
		InputStream stream(ev.data);
		SteamBaseImpl::HandleMessage(ev);

		switch(ev.msg)
		{
		case EMsg_ClientFromGC:
			HandleGcMsg(stream);
			break;
		}
	}

	void SteamGc::Impl::HandleGcMsg(InputStream &stream)
	{
		GcMsgEvent ev;
		MsgProto<CMsgGCClient> msg(stream);

		ev.msg.appId = msg.proto.appid();
		ev.msg.msgId = msg.proto.msgtype();
		ev.msg.msgData = msg.proto.payload();
		ev.msg.isProto = (msg.proto.msgtype() & 0x80000000) != 0;

		if(ev.msg.isProto)
		{
			ev.msg.msgId &= ~0x80000000;
		}

		m_client->Delay([=]()
		{
			m_parent->OnGcMsg->Call(ev);
		});
	}

	// ------------------------------------------------------------------------
	// SteamGc Implementation
	// ------------------------------------------------------------------------

	SteamGc::SteamGc(SteamClient &client)
	{
		impl = new Impl(this, client.impl);
		OnGcMsg = new Event<GcMsgEvent>();
	}

	SteamGc::~SteamGc()
	{
		delete OnGcMsg;
		delete impl;
	}

	void SteamGc::SendGcMsg(const GcMsg &msg)
	{
		impl->SendGcMsg(msg);
	}
}