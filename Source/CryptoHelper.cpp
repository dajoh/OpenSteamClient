#include <memory>
#include "CryptoHelper.hpp"

namespace Sc
{
	using std::shared_ptr;

	string CryptoHelper::RngGenerateBlock(size_t size)
	{
		shared_ptr<uint8_t> block(new uint8_t [size]);
		m_random.GenerateBlock(block.get(), size);
		return string((char *)block.get(), size);
	}

	uint32_t CryptoHelper::CrcCalculateDigest(const string &data)
	{
		uint32_t result;
		m_crc32.CalculateDigest((uint8_t *)&result, (uint8_t *)&data[0], data.size());
		return result;
	}

	void CryptoHelper::RsaSetKey(const string &key)
	{
		m_rsaPublicKey.Load(CryptoPP::StringSource(key, true));
	}

	string CryptoHelper::RsaEncrypt(const string &data)
	{
		CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(m_rsaPublicKey);

		auto inSize = data.size();
		auto outSize = encryptor.CiphertextLength(inSize);
		auto input = (const uint8_t *)data.c_str();
		auto output = shared_ptr<uint8_t>(new uint8_t [outSize]);

		encryptor.Encrypt(m_random, input, inSize, output.get());
		return string((char *)output.get(), outSize);
	}

	string CryptoHelper::RsaDecrypt(const string &data)
	{
		CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(m_rsaPublicKey);

		auto inSize = data.size();
		auto outSize = decryptor.CiphertextLength(inSize);
		auto input = (const uint8_t *)data.c_str();
		auto output = shared_ptr<uint8_t>(new uint8_t [outSize]);

		decryptor.Decrypt(m_random, input, inSize, output.get());
		return string((char *)output.get(), outSize);
	}

	void CryptoHelper::SteamSetKey(const string &key)
	{
		m_steamKey = key;
	}

	string CryptoHelper::SteamEncrypt(const string &data)
	{
		auto plain = PkcsPad(data);
		auto keySize = m_steamKey.size();
		auto plainSize = plain.size();
		auto keyData = (const uint8_t *)&m_steamKey[0];
		auto plainData = (const uint8_t *)&plain[0];
		auto cipherData = shared_ptr<uint8_t>(new uint8_t [plainSize + 16]);
		auto cipherIv = cipherData.get() + 0;
		auto cipherMsg = cipherData.get() + 16;

		m_random.GenerateBlock(cipherIv, 16);
		memcpy(cipherMsg, plainData, plainSize);

		CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption ecbEncryptor(keyData, keySize);
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption cbcEncryptor(keyData, keySize, cipherIv);
		ecbEncryptor.ProcessString(cipherIv, 16);
		cbcEncryptor.ProcessString(cipherMsg, plainSize);

		return string((char *)cipherData.get(), plainSize + 16);
	}

	string CryptoHelper::SteamDecrypt(const string &data)
	{
		auto buffer = data;
		auto keySize = m_steamKey.size();
		auto bufferSize = buffer.size();
		auto bufferData = (uint8_t *)&buffer[0];
		auto cipherIv = (uint8_t *)bufferData + 0;
		auto cipherMsg = (uint8_t *)bufferData + 16;
		auto keyData = (const uint8_t *)&m_steamKey[0];

		CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption ecbDecryptor(keyData, keySize);
		ecbDecryptor.ProcessString(cipherIv, 16);

		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption cbcDecryptor(keyData, keySize, cipherIv);
		cbcDecryptor.ProcessString(cipherMsg, bufferSize - 16);

		return PkcsUnpad(string(&buffer[16], bufferSize - 16));
	}

	string CryptoHelper::PkcsPad(const string &data)
	{
		string padded = data;
		size_t padAmount = (16 - (data.size() % 16));

		for(size_t i = 0; i < padAmount; i++)
		{
			padded.push_back(padAmount);
		}

		return padded;
	}

	string CryptoHelper::PkcsUnpad(const string &data)
	{
		size_t padAmount = data[data.size() - 1];
		return string(&data[0], data.size() - padAmount);
	}
}