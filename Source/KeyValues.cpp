#include <OpenSteamClient/Exception.hpp>
#include "KeyValues.hpp"

namespace Sc
{
	KeyValues::KeyValues()
	{
		m_type = KeyValuesType_None;
	}

	KeyValues::KeyValues(const string &name)
	{
		m_name = name;
		m_type = KeyValuesType_None;
	}

	KeyValues::KeyValues(InputStream &stream)
	{
		Deserialize(stream);
	}

	void KeyValues::Serialize(OutputStream &stream) const
	{
		throw Exception("Not implemented.");
	}

	void KeyValues::Deserialize(InputStream &stream)
	{
		uint8_t type;
		stream.Read<uint8_t>(type);
		stream.ReadNullTerminated(m_name);
		m_type = (KeyValuesType)type;
		Parse(stream);
	}

	void KeyValues::SetName(const string &name)
	{
		m_name = name;
	}

	void KeyValues::SetString(const string &value)
	{
		m_type = KeyValuesType_String;
		m_stringValue = value;
	}

	void KeyValues::SetWideString(const wstring &value)
	{
		m_type = KeyValuesType_WideString;
		m_wstringValue = value;
	}

	void KeyValues::SetInt32(int32_t value)
	{
		m_type = KeyValuesType_Int32;
		m_int32Value = value;
	}

	void KeyValues::SetColor(uint32_t value)
	{
		m_type = KeyValuesType_Color;
		m_uint32Value = value;
	}

	void KeyValues::SetUint64(uint64_t value)
	{
		m_type = KeyValuesType_Uint64;
		m_uint64Value = value;
	}

	void KeyValues::SetPointer(uint32_t value)
	{
		m_type = KeyValuesType_Pointer;
		m_uint32Value = value;
	}

	void KeyValues::SetFloat32(float32_t value)
	{
		m_type = KeyValuesType_Float32;
		m_float32Value = value;
	}

	const string &KeyValues::GetName() const
	{
		return m_name;
	}

	const string &KeyValues::GetString() const
	{
		if(m_type != KeyValuesType_String)
		{
			throw Exception("Invalid KeyValues conversion.");
		}

		return m_stringValue;
	}

	const wstring &KeyValues::GetWideString() const
	{
		if(m_type != KeyValuesType_WideString)
		{
			throw Exception("Invalid KeyValues conversion.");
		}

		return m_wstringValue;
	}

	int32_t KeyValues::GetInt32() const
	{
		if(m_type != KeyValuesType_Int32)
		{
			throw Exception("Invalid KeyValues conversion.");
		}

		return m_int32Value;
	}

	uint32_t KeyValues::GetColor() const
	{
		if(m_type != KeyValuesType_Color)
		{
			throw Exception("Invalid KeyValues conversion.");
		}

		return m_uint32Value;
	}

	uint64_t KeyValues::GetUint64() const
	{
		if(m_type != KeyValuesType_Uint64)
		{
			throw Exception("Invalid KeyValues conversion.");
		}

		return m_uint64Value;
	}

	uint32_t KeyValues::GetPointer() const
	{
		if(m_type != KeyValuesType_Pointer)
		{
			throw Exception("Invalid KeyValues conversion.");
		}

		return m_uint32Value;
	}

	float32_t KeyValues::GetFloat32() const
	{
		if(m_type != KeyValuesType_Float32)
		{
			throw Exception("Invalid KeyValues conversion.");
		}

		return m_float32Value;
	}

	KeyValuesType KeyValues::GetType() const
	{
		return m_type;
	}

	KeyValues &KeyValues::operator[](const string &name)
	{
		return *(m_children[name]);
	}

	const KeyValues &KeyValues::operator[](const string &name) const
	{
		auto it = m_children.find(name);

		if(it != m_children.end())
		{
			return *(it->second);
		}

		throw Exception("Non-existing KeyValues accessed.");
	}

	void KeyValues::Parse(InputStream &stream)
	{
		string name;
		uint8_t type;

		while(true)
		{
			stream.Read<uint8_t>(type);

			if(type == KeyValuesType_End)
			{
				break;
			}

			stream.ReadNullTerminated(name);

			switch(type)
			{
			case KeyValuesType_None:
				m_children[name].reset(new KeyValues(name));
				m_children[name]->Parse(stream);
				break;
			case KeyValuesType_String:
				stream.ReadNullTerminated(m_stringValue);
				m_children[name].reset(new KeyValues(name));
				m_children[name]->SetString(m_stringValue);
				break;
			case KeyValuesType_Int32:
				stream.Read<int32_t>(m_int32Value);
				m_children[name].reset(new KeyValues(name));
				m_children[name]->SetInt32(m_int32Value);
				break;
			case KeyValuesType_Float32:
				stream.Read<float32_t>(m_float32Value);
				m_children[name].reset(new KeyValues(name));
				m_children[name]->SetFloat32(m_float32Value);
				break;
			case KeyValuesType_Pointer:
				stream.Read<uint32_t>(m_uint32Value);
				m_children[name].reset(new KeyValues(name));
				m_children[name]->SetPointer(m_uint32Value);
				break;
			case KeyValuesType_WideString:
				throw Exception("Not implemented.");
			case KeyValuesType_Color:
				stream.Read<uint32_t>(m_uint32Value);
				m_children[name].reset(new KeyValues(name));
				m_children[name]->SetColor(m_uint32Value);
				break;
			case KeyValuesType_Uint64:
				stream.Read<uint64_t>(m_uint64Value);
				m_children[name].reset(new KeyValues(name));
				m_children[name]->SetUint64(m_uint64Value);
				break;
			}
		}
	}
}
