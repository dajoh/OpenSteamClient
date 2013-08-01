#include "SteamUserImpl.hpp"
#include "Messages/MsgProto.hpp"
#include "Protobufs/steammessages_clientserver.pb.h"

namespace Sc
{
	// ------------------------------------------------------------------------
	// SteamUser::Impl Implementation
	// ------------------------------------------------------------------------

	SteamUser::Impl::Impl(SteamUser *parent, SteamClient::Impl *client) : SteamBaseImpl(client)
	{
		m_parent = parent;
	}

	void SteamUser::Impl::LogIn(LoginDetails details)
	{
		OutputStream stream;
		MsgProto<CMsgClientLogon> msg;

		msg.header.msg = EMsg_ClientLogon;
		msg.header.proto.set_steamid(SteamId(0, 1, EAccountType_Individual, EUniverse_Public));
		msg.header.proto.set_client_sessionid(0);

		msg.proto.set_obfustucated_private_ip(0xFFFFFFFF);
		msg.proto.set_account_name(details.username);
		msg.proto.set_password(details.password);
		msg.proto.set_protocol_version(65575);
		msg.proto.set_client_os_type(EOSType_WinXP);
		msg.proto.set_client_language("english");
		msg.proto.set_steam2_ticket_request(false);
		msg.proto.set_client_package_version(1771);
		msg.proto.set_eresult_sentryfile(EResult_FileNotFound);

		if(details.authCode != "")
		{
			msg.proto.set_auth_code(details.authCode);
		}

		msg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	void SteamUser::Impl::LogOut()
	{
		OutputStream stream;
		MsgProto<CMsgClientLogOff> msg;

		msg.header.msg = EMsg_ClientLogOff;
		msg.header.proto.set_steamid(m_steamId);
		msg.header.proto.set_client_sessionid(m_sessionId);

		msg.Serialize(stream);
		m_client->Send(stream.GetBuffer());
	}

	bool SteamUser::Impl::IsLoggedIn() const
	{
		return m_loggedIn;
	}

	void SteamUser::Impl::HandleDisconnect(const DisconnectEvent &ev)
	{
		SteamBaseImpl::HandleDisconnect(ev);
	}

	void SteamUser::Impl::HandleLogout(InputStream &stream)
	{
		SteamBaseImpl::HandleLogout(stream);
		MsgProto<CMsgClientLoggedOff> msg(stream);

		if(m_loggedIn)
		{
			m_client->Delay([=]()
			{
				LoggedOutEvent ev;
				ev.result = (EResult)msg.proto.eresult();
				m_parent->OnLoggedOut->Call(ev);
			});
		}
	}

	void SteamUser::Impl::HandleLogin(InputStream &stream)
	{
		SteamBaseImpl::HandleLogin(stream);
		MsgProto<CMsgClientLogonResponse> msg(stream);

		m_client->Delay([=]()
		{
			LoggedInEvent ev;
			ev.result = (EResult)msg.proto.eresult();
			ev.steamId = msg.header.proto.steamid();
			m_parent->OnLoggedIn->Call(ev);
		});
	}

	// ------------------------------------------------------------------------
	// SteamUser Implementation
	// ------------------------------------------------------------------------

	SteamUser::SteamUser(SteamClient &client)
	{
		impl = new Impl(this, client.impl);
		OnLoggedIn = new Event<LoggedInEvent>();
		OnLoggedOut = new Event<LoggedOutEvent>();
	}

	SteamUser::~SteamUser()
	{
		delete OnLoggedOut;
		delete OnLoggedIn;
		delete impl;
	}

	void SteamUser::LogIn(LoginDetails details)
	{
		impl->LogIn(details);
	}

	void SteamUser::LogOut()
	{
		impl->LogOut();
	}

	bool SteamUser::IsLoggedIn() const
	{
		return impl->IsLoggedIn();
	}
}
