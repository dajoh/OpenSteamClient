#ifndef STEAMGCIMPL_HPP
#define STEAMGCIMPL_HPP

#include <OpenSteamClient/SteamGc.hpp>
#include "SteamBaseImpl.hpp"

namespace Sc
{
	class SteamGc::Impl : public SteamBaseImpl
	{
	public:
		Impl(SteamGc *parent, SteamClient::Impl *client);

		void SendGcMsg(const GcMsg &msg);
	protected:
		virtual void HandleMessage(const MessageEvent &ev);

		void HandleGcMsg(InputStream &stream);

		SteamGc *m_parent;
	};
}

#endif