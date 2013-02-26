#include "SteamBaseImpl.hpp"
#include "Messages/MsgProto.hpp"
#include "Protobufs/steammessages_clientserver.pb.h"

namespace Sc
{
	SteamBaseImpl::SteamBaseImpl(SteamClient::Impl *client)
	{
		m_client = client;
		m_steamId = 0;
		m_loggedIn = false;
		m_sessionId = 0;

		m_client->OnMessage.Add([this](const MessageEvent &ev)
		{
			HandleMessage(ev);
		});

		m_client->OnDisconnect.Add([this](const DisconnectEvent &ev)
		{
			HandleDisconnect(ev);
		});
	}

	void SteamBaseImpl::HandleMessage(const MessageEvent &ev)
	{
		InputStream stream(ev.data);

		switch(ev.msg)
		{
		case EMsg_ClientLogOnResponse:
			HandleLogin(stream);
			break;
		case EMsg_ClientLoggedOff:
			HandleLogout(stream);
			break;
		}
	}

	void SteamBaseImpl::HandleDisconnect(const DisconnectEvent &ev)
	{
		m_steamId = 0;
		m_loggedIn = false;
		m_sessionId = 0;
	}

	void SteamBaseImpl::HandleLogin(InputStream &stream)
	{
		MsgProto<CMsgClientLogonResponse> msg(stream);

		if(msg.proto.eresult() == EResult_OK)
		{
			m_steamId = msg.header.proto.steamid();
			m_loggedIn = true;
			m_sessionId = msg.header.proto.client_sessionid();
		}

		stream.Reset();
	}

	void SteamBaseImpl::HandleLogout(InputStream &stream)
	{
		m_steamId = 0;
		m_loggedIn = false;
		m_sessionId = 0;
	}
}
