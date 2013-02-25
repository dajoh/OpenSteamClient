#include <cstdint>
#include <OpenSteamClient/Exception.hpp>
#include "InputStream.hpp"

namespace Sc
{
	InputStream::InputStream(const string &data) : m_data(data)
	{
		m_pos = 0;
		m_size = m_data.size();
	}

	void InputStream::Reset()
	{
		m_pos = 0;
	}

	size_t InputStream::GetDataLeft() const
	{
		return m_size - m_pos;
	}

	void InputStream::ReadRaw(void *buffer, size_t amount)
	{
		auto src = (const uint8_t *)m_data.c_str();
		auto dst = (uint8_t *)buffer;

		if(m_pos + amount > m_size)
		{
			throw Exception("Not enough data left in stream.");
		}

		memcpy(dst, &src[m_pos], amount);
		m_pos += amount;
	}

	void InputStream::ReadNullTerminated(string &value)
	{
		value = m_data.c_str() + m_pos;
		m_pos += value.size() + 1;
	}
}