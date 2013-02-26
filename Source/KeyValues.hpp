#ifndef KEYVALUES_HPP
#define KEYVALUES_HPP

#include <cstdint>
#include <unordered_map>
#include "InputStream.hpp"
#include "OutputStream.hpp"

namespace Sc
{
	using std::wstring;
	using std::unordered_map;

	typedef float float32_t;
	typedef double float64_t;

	enum KeyValuesType
	{
		KeyValuesType_None,
		KeyValuesType_String,
		KeyValuesType_Int32,
		KeyValuesType_Float32,
		KeyValuesType_Pointer,
		KeyValuesType_WideString,
		KeyValuesType_Color,
		KeyValuesType_Uint64,
		KeyValuesType_End
	};

	class KeyValues
	{
	public:
		KeyValues();
		KeyValues(const string &name);
		KeyValues(InputStream &stream);
		
		void Serialize(OutputStream &stream) const;
		void Deserialize(InputStream &stream);

		void SetName(const string &name);
		void SetString(const string &value);
		void SetWideString(const wstring &value);
		void SetInt32(int32_t value);
		void SetColor(uint32_t value);
		void SetUint64(uint64_t value);
		void SetPointer(uint32_t value);
		void SetFloat32(float32_t value);

		const string &GetName() const;
		const string &GetString() const;
		const wstring &GetWideString() const;
		int32_t GetInt32() const;
		uint32_t GetColor() const;
		uint64_t GetUint64() const;
		uint32_t GetPointer() const;
		float32_t GetFloat32() const;
		KeyValuesType GetType() const;

		KeyValues &operator[](const string &name);
		const KeyValues &operator[](const string &name) const;
	private:
		void Parse(InputStream &stream);

		string m_name;
		KeyValuesType m_type;
		unordered_map<string, shared_ptr<KeyValues>> m_children;

		string m_stringValue;
		wstring m_wstringValue;
		int32_t m_int32Value;
		float32_t m_float32Value;
		uint32_t m_uint32Value;
		uint64_t m_uint64Value;
	};
}

#endif
