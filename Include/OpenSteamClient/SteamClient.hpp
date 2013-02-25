#ifndef OPENSTEAMCLIENT_STEAMCLIENT_HPP
#define OPENSTEAMCLIENT_STEAMCLIENT_HPP

#include "Enums.hpp"
#include "Event.hpp"
#include "NonCopyable.hpp"

namespace Sc
{
	struct ConnectEvent
	{
		EResult result;
	};

	struct DisconnectEvent
	{
	};

	class SteamClient : NonCopyable
	{
	public:
		SteamClient();
		~SteamClient();

		void Connect();
		void Disconnect();
		bool IsConnected() const;

		void Run(int timeout);

		Event<ConnectEvent> OnConnect;
		Event<DisconnectEvent> OnDisconnect;
	public:
		class Impl;
		Impl *impl;
	};
}

#endif