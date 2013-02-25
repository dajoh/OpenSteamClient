#ifndef OPENSTEAMCLIENT_STEAMID_HPP
#define OPENSTEAMCLIENT_STEAMID_HPP

#include <cstdint>
#include "Enums.hpp"

namespace Sc
{
	class SteamId
	{
	public:
		SteamId();
		SteamId(uint64_t steamId);
		SteamId(uint32_t id, uint32_t instance, EAccountType type, EUniverse universe);

		void SetId(uint32_t id);
		void SetInstance(uint32_t instance);
		void SetType(EAccountType type);
		void SetUniverse(EUniverse universe);

		uint32_t GetId() const;
		uint32_t GetInstance() const;
		EAccountType GetType() const;
		EUniverse GetUniverse() const;

		operator uint64_t() const;
		operator uint64_t &();
		uint64_t *operator &();

		bool operator==(uint64_t other) const;
		bool operator!=(uint64_t other) const;
		bool operator==(SteamId other) const;
		bool operator!=(SteamId other) const;
	private:
		struct Internal
		{
			uint32_t id       : 32;
			uint32_t instance : 20;
			uint32_t type     : 4;
			uint32_t universe : 8;
		};

		union
		{
			uint64_t m_steamId;
			Internal m_internal;
		};
	};

	// ------------------------------------------------------------------------
	// SteamId Implementation
	// ------------------------------------------------------------------------

	inline SteamId::SteamId()
	{
		m_steamId = 0;
	}

	inline SteamId::SteamId(uint64_t steamId)
	{
		m_steamId = steamId;
	}

	inline SteamId::SteamId(uint32_t id, uint32_t instance, EAccountType type, EUniverse universe)
	{
		m_internal.id = id;
		m_internal.instance = instance;
		m_internal.type = type;
		m_internal.universe = universe;
	}

	inline void SteamId::SetId(uint32_t id)
	{
		m_internal.id = id;
	}

	inline void SteamId::SetInstance(uint32_t instance)
	{
		m_internal.instance = instance;
	}

	inline void SteamId::SetType(EAccountType type)
	{
		m_internal.type = type;
	}

	inline void SteamId::SetUniverse(EUniverse universe)
	{
		m_internal.universe = universe;
	}

	inline uint32_t SteamId::GetId() const
	{
		return m_internal.id;
	}

	inline uint32_t SteamId::GetInstance() const
	{
		return m_internal.instance;
	}

	inline EAccountType SteamId::GetType() const
	{
		return (EAccountType)m_internal.type;
	}

	inline EUniverse SteamId::GetUniverse() const
	{
		return (EUniverse)m_internal.universe;
	}

	inline SteamId::operator uint64_t() const
	{
		return m_steamId;
	}

	inline SteamId::operator uint64_t &()
	{
		return m_steamId;
	}

	inline uint64_t *SteamId::operator &()
	{
		return &m_steamId;
	}

	inline bool SteamId::operator==(uint64_t other) const
	{
		return m_steamId == other;
	}

	inline bool SteamId::operator!=(uint64_t other) const
	{
		return m_steamId != other;
	}

	inline bool SteamId::operator==(SteamId other) const
	{
		return m_steamId == other.m_steamId;
	}

	inline bool SteamId::operator!=(SteamId other) const
	{
		return m_steamId != other.m_steamId;
	}
}

#endif