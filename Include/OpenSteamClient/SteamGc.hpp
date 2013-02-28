#ifndef OPENSTEAMCLIENT_STEAMGC_HPP
#define OPENSTEAMCLIENT_STEAMGC_HPP

#include <string>
#include <cstdint>
#include "SteamClient.hpp"

namespace Sc
{
	using std::string;

	struct GcMsg
	{
		uint32_t appId;
		uint32_t msgId;
		string msgData;
		bool isProto;
	};

	struct GcMsgEvent
	{
		GcMsg msg;
	};

	class OPENSTEAMCLIENT_EXPORT SteamGc
	{
	public:
		SteamGc(SteamClient &client);
		~SteamGc();

		void SendGcMsg(const GcMsg &msg);

		Event<GcMsgEvent> *OnGcMsg;
	private:
		class Impl;
		Impl *impl;

		SteamGc(const SteamGc &);
		SteamGc &operator=(const SteamGc &);
	};
}

#endif