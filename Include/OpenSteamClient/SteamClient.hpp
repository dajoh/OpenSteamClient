#ifndef OPENSTEAMCLIENT_STEAMCLIENT_HPP
#define OPENSTEAMCLIENT_STEAMCLIENT_HPP

#include "Enums.hpp"
#include "Event.hpp"
#include "Platform.hpp"

namespace Sc
{
	struct ConnectEvent
	{
		EResult result;
	};

	struct DisconnectEvent
	{
	};

	class OPENSTEAMCLIENT_EXPORT SteamClient
	{
	public:
		SteamClient();
		~SteamClient();

		void Connect();
		void Disconnect();
		bool IsConnected() const;

		void Run(int timeout);

		Event<ConnectEvent> *OnConnect;
		Event<DisconnectEvent> *OnDisconnect;
	public:
		class Impl;
		Impl *impl;
	private:
		SteamClient(const SteamClient &);
		SteamClient &operator=(const SteamClient &);
	};
}

#endif