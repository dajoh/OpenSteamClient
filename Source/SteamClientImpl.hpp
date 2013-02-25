#ifndef STEAMCLIENTIMPL_HPP
#define STEAMCLIENTIMPL_HPP

#include <ctime>
#include <OpenSteamClient/SteamId.hpp>
#include <OpenSteamClient/SteamClient.hpp>
#include "InputStream.hpp"
#include "TcpConnection.hpp"

namespace Sc
{
	struct MessageEvent
	{
		EMsg msg;
		const string &data;
	};

	class SteamClient::Impl
	{
	public:
		Impl(SteamClient *parent);
		~Impl();

		void Connect();
		void Disconnect();
		bool IsConnected() const;

		void Run(int timeout);
		void Send(const string &data);
		void Delay(const function<void()> &fn);

		Event<MessageEvent> OnMessage;
		Event<DisconnectEvent> OnDisconnect;
	private:
		void Beat();

		void HandleDisconnect();
		void HandleConnect(bool success);
		void HandleData(const string &data);
		void HandleMessage(const string &data);
		
		void HandleLogin(InputStream &stream);
		void HandleLogout(InputStream &stream);
		void HandleMultiMessage(InputStream &stream);
		void HandleEncryptResult(InputStream &stream);
		void HandleEncryptRequest(InputStream &stream);

		int m_beatPeriod;
		bool m_loggedIn;
		time_t m_lastBeat;
		SteamId m_steamId;
		uint32_t m_sessionId;
		
		bool m_encrypted;
		string m_sessionKey;
		EUniverse m_universe;
		SteamClient *m_parent;
		TcpConnection m_connection;
		list<function<void()>> m_delayedCalls;

		Impl(const Impl &);
		Impl &operator=(const Impl &);
	};
}

#endif