#ifndef INPUTSTREAM_HPP
#define INPUTSTREAM_HPP

#include <string>
#include <memory>
#include <google/protobuf/message_lite.h>

namespace Sc
{
	using std::string;
	using std::unique_ptr;
	using google::protobuf::MessageLite;

	class InputStream
	{
	public:
		InputStream(const string &data);

		void Reset();
		size_t GetDataLeft() const;

		void ReadRaw(void *buffer, size_t amount);
		void ReadNullTerminated(string &value);

		template<class T>
		void Read(T &value);

		template<class T>
		void Read(T &value, size_t size);
	private:
		size_t m_pos;
		size_t m_size;
		const string &m_data;
	};

	// ------------------------------------------------------------------------
	// InputStream::Read Implementation
	// ------------------------------------------------------------------------

	template<class T>
	inline void InputStream::Read(T &value)
	{
		ReadRaw(&value, sizeof(T));
	}

	template<>
	inline void InputStream::Read<string>(string &value, size_t size)
	{
		unique_ptr<char[]> buffer(new char [size]);
		ReadRaw(buffer.get(), size);
		value = string(buffer.get(), size);
	}

	template<>
	inline void InputStream::Read<MessageLite>(MessageLite &value, size_t size)
	{
		string data;
		Read<string>(data, size);
		value.ParseFromString(data);
	}

	template<>
	inline void InputStream::Read<string>(string &value)
	{
		Read<string>(value, m_size - m_pos);
	}

	template<>
	inline void InputStream::Read<MessageLite>(MessageLite &value)
	{
		Read<MessageLite>(value, m_size - m_pos);
	}
}

#endif