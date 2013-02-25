#ifndef MESSAGES_MSGPROTO_HPP
#define MESSAGES_MSGPROTO_HPP

#include "MsgHdrProto.hpp"

namespace Sc
{
	template<class T>
	class MsgProto
	{
	public:
		MsgProto();
		MsgProto(InputStream &stream);

		void Serialize(OutputStream &stream);
		void Deserialize(InputStream &stream);
	public:
		MsgHdrProto header;
		T proto;
	};

	// ------------------------------------------------------------------------
	// MsgProto Implementation
	// ------------------------------------------------------------------------

	template<class T>
	inline MsgProto<T>::MsgProto()
	{
	}

	template<class T>
	inline MsgProto<T>::MsgProto(InputStream &stream)
	{
		Deserialize(stream);
	}

	template<class T>
	inline void MsgProto<T>::Serialize(OutputStream &stream)
	{
		header.Serialize(stream);
		stream.Write<MessageLite>(proto);
	}

	template<class T>
	inline void MsgProto<T>::Deserialize(InputStream &stream)
	{
		header.Deserialize(stream);
		stream.Read<MessageLite>(proto);
	}
}

#endif