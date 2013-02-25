#include <OpenSteamClient/Exception.hpp>
#include "ZipHelper.hpp"
#include "PublicKeys.hpp"
#include "CryptoHelper.hpp"
#include "SteamClientImpl.hpp"
#include "Messages/MsgProto.hpp"
#include "Messages/MsgChannelEncryptResult.hpp"
#include "Messages/MsgChannelEncryptRequest.hpp"
#include "Messages/MsgChannelEncryptResponse.hpp"
#include "Protobufs/steammessages_clientserver.pb.h"

namespace Sc
{
	// ------------------------------------------------------------------------
	// SteamClient::Impl Implementation
	// ------------------------------------------------------------------------

	SteamClient::Impl::Impl(SteamClient *parent)
	{
		m_connection.OnDisconnect.Add([this]()
		{
			HandleDisconnect();
		});

		m_connection.OnConnect.Add([this](bool success) 
		{
			HandleConnect(success);
		});

		m_connection.OnReceive.Add([this](const string &data)
		{
			HandleData(data);
		});

		m_parent = parent;
		m_loggedIn = false;
	}

	SteamClient::Impl::~Impl()
	{
		if(IsConnected())
		{
			Disconnect();
		}
	}

	void SteamClient::Impl::Connect()
	{
		m_connection.Connect("72.165.61.174", "27017");
	}

	void SteamClient::Impl::Disconnect()
	{
		m_connection.Disconnect();
	}

	bool SteamClient::Impl::IsConnected() const
	{
		return m_connection.IsConnected();
	}

	void SteamClient::Impl::Run(int timeout)
	{
		if(!m_connection.IsConnected())
		{
			return;
		}

		if(m_loggedIn)
		{
			if((time(nullptr) - m_lastBeat) >= (m_beatPeriod / 1000))
			{
				Beat();
			}

			if(timeout < 0)
			{
				m_connection.Run(m_beatPeriod);
			}
			else
			{
				m_connection.Run(m_beatPeriod > timeout ? timeout : m_beatPeriod);
			}
		}
		else
		{
			m_connection.Run(timeout);
		}

		for(auto it = m_delayedCalls.begin(); it != m_delayedCalls.end(); ++it)
		{
			(*it)();
		}

		m_delayedCalls.clear();
	}

	void SteamClient::Impl::Send(const string &data)
	{
		if(m_encrypted)
		{
			CryptoHelper crypto; // cryptofix
			crypto.SteamSetKey(m_sessionKey);
			m_connection.Send(crypto.SteamEncrypt(data));
		}
		else
		{
			m_connection.Send(data);
		}
	}

	void SteamClient::Impl::Delay(const function<void()> &fn)
	{
		m_delayedCalls.push_back(fn);
	}

	void SteamClient::Impl::Beat()
	{
		OutputStream stream;
		MsgProto<CMsgClientHeartBeat> msg;

		msg.header.msg = EMsg_ClientHeartBeat;
		msg.header.proto.set_steamid(m_steamId);
		msg.header.proto.set_client_sessionid(m_sessionId);

		msg.Serialize(stream);
		Send(stream.GetBuffer());
		m_lastBeat = time(nullptr);
	}

	void SteamClient::Impl::HandleDisconnect()
	{
		m_steamId = 0;
		m_loggedIn = false;
		m_sessionId = 0;

		DisconnectEvent ev;
		OnDisconnect.Call(ev);

		Delay([=]()
		{
			m_parent->OnDisconnect->Call(ev);
		});
	}

	void SteamClient::Impl::HandleConnect(bool success)
	{
		if(success)
		{
			m_encrypted = false;
		}
		else
		{
			Delay([=]()
			{
				ConnectEvent ev;
				ev.result = EResult_ConnectFailed;
				m_parent->OnConnect->Call(ev);
			});
		}
	}

	void SteamClient::Impl::HandleData(const string &data)
	{
		if(m_encrypted)
		{
			CryptoHelper crypto; // cryptofix
			crypto.SteamSetKey(m_sessionKey);
			HandleMessage(crypto.SteamDecrypt(data));
		}
		else
		{
			HandleMessage(data);
		}
	}

	void SteamClient::Impl::HandleMessage(const string &data)
	{
		uint32_t msg;
		InputStream stream(data);

		stream.Read<uint32_t>(msg);
		stream.Reset();

		if(msg & 0x80000000)
		{
			msg &= ~0x80000000;
		}

		switch(msg)
		{
		case EMsg_Multi:
			HandleMultiMessage(stream);
			return;
		case EMsg_ChannelEncryptResult:
			HandleEncryptResult(stream);
			return;
		case EMsg_ChannelEncryptRequest:
			HandleEncryptRequest(stream);
			return;
		}

		switch(msg)
		{
		case EMsg_ClientLogOnResponse:
			HandleLogin(stream);
			break;
		case EMsg_ClientLoggedOff:
			HandleLogout(stream);
			break;
		}

		MessageEvent ev = {(EMsg)msg, data};
		OnMessage.Call(ev);
	}

	void SteamClient::Impl::HandleLogin(InputStream &stream)
	{
		MsgProto<CMsgClientLogonResponse> msg(stream);

		if(msg.proto.eresult() == EResult_OK)
		{
			m_steamId = msg.header.proto.steamid();
			m_loggedIn = true;
			m_lastBeat = time(nullptr);
			m_sessionId = msg.header.proto.client_sessionid();
			m_beatPeriod = msg.proto.out_of_game_heartbeat_seconds() * 1000;
		}
	}

	void SteamClient::Impl::HandleLogout(InputStream &stream)
	{
		m_steamId = 0;
		m_loggedIn = false;
		m_sessionId = 0;
	}

	void SteamClient::Impl::HandleMultiMessage(InputStream &stream)
	{
		MsgProto<CMsgMulti> msg(stream);

		if(msg.proto.size_unzipped() > 0)
		{
			msg.proto.set_message_body(DecompressZip(msg.proto.message_body()));
		}

		string msgData;
		uint32_t msgSize;
		InputStream multiStream(msg.proto.message_body());

		while(multiStream.GetDataLeft() != 0)
		{
			multiStream.Read<uint32_t>(msgSize);
			multiStream.Read<string>(msgData, msgSize);
			HandleMessage(msgData);
		}
	}

	void SteamClient::Impl::HandleEncryptResult(InputStream &stream)
	{
		MsgChannelEncryptResult result(stream);

		if(result.result == EResult_OK)
		{
			m_encrypted = true;
		}

		Delay([=]()
		{
			ConnectEvent ev;
			ev.result = result.result;
			m_parent->OnConnect->Call(ev);
		});
	}

	void SteamClient::Impl::HandleEncryptRequest(InputStream &stream)
	{
		string publicKey;
		CryptoHelper crypto;
		OutputStream outStream;
		MsgChannelEncryptRequest request(stream);
		MsgChannelEncryptResponse response;

		if(request.protocolVersion != 1)
		{
			throw Exception("Unsupported protocol version.");
		}

		m_sessionKey = crypto.RngGenerateBlock(32);
		m_universe = request.universe;
		publicKey = GetPublicKey(m_universe);

		if(publicKey.empty())
		{
			throw Exception("No public key for universe.");
		}

		crypto.RsaSetKey(publicKey);
		response.key = crypto.RsaEncrypt(m_sessionKey);
		response.keyCrc = crypto.CrcCalculateDigest(response.key);
		response.header.msg = EMsg_ChannelEncryptResponse;

		response.Serialize(outStream);
		Send(outStream.GetBuffer());
	}

	// ------------------------------------------------------------------------
	// SteamClient Implementation
	// ------------------------------------------------------------------------

	SteamClient::SteamClient()
	{
		impl = new Impl(this);
		OnConnect = new Event<ConnectEvent>();
		OnDisconnect = new Event<DisconnectEvent>();
	}

	SteamClient::~SteamClient()
	{
		delete OnDisconnect;
		delete OnConnect;
		delete impl;
	}

	void SteamClient::Connect()
	{
		impl->Connect();
	}

	void SteamClient::Disconnect()
	{
		impl->Disconnect();
	}

	bool SteamClient::IsConnected() const
	{
		return impl->IsConnected();
	}

	void SteamClient::Run(int timeout)
	{
		impl->Run(timeout);
	}
}