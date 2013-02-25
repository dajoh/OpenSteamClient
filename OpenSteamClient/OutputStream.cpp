#include "OutputStream.hpp"

namespace Sc
{
	void OutputStream::WriteRaw(const void *buffer, size_t amount)
	{
		auto first = (const uint8_t *)buffer;
		auto last = first + amount;
		m_buffer.insert(m_buffer.end(), first, last);
	}

	string OutputStream::GetBuffer() const
	{
		return string((const char *)&m_buffer[0], m_buffer.size());
	}
}