#ifndef OUTPUTSTREAM_HPP
#define OUTPUTSTREAM_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <google/protobuf/message_lite.h>

namespace Sc
{
	using std::vector;
	using std::string;
	using google::protobuf::MessageLite;

	class OutputStream
	{
	public:
		void WriteRaw(const void *buffer, size_t amount);
		string GetBuffer() const;

		template<class T>
		void Write(const T &value);
	private:
		vector<uint8_t> m_buffer;
	};

	// ------------------------------------------------------------------------
	// OutputStream::Write Implementation
	// ------------------------------------------------------------------------

	template<class T>
	inline void OutputStream::Write(const T &value)
	{
		WriteRaw(&value, sizeof(T));
	}

	template<>
	inline void OutputStream::Write<string>(const string &value)
	{
		WriteRaw(value.c_str(), value.size());
	}

	template<>
	inline void OutputStream::Write<MessageLite>(const MessageLite &value)
	{
		string data = value.SerializeAsString();
		Write<string>(data);
	}
}

#endif