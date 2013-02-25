#ifndef CRYPTOHELPER_HPP
#define CRYPTOHELPER_HPP

#include <string>
#include <cstdint>
#include <cryptopp/osrng.h>
#include <cryptopp/crc.h>
#include <cryptopp/rsa.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/pkcspad.h>

namespace Sc
{
	using std::string;

	class CryptoHelper
	{
	public:
		string RngGenerateBlock(size_t size);
		uint32_t CrcCalculateDigest(const string &data);

		void RsaSetKey(const string &key);
		string RsaEncrypt(const string &data);
		string RsaDecrypt(const string &data);

		void SteamSetKey(const string &key);
		string SteamEncrypt(const string &data);
		string SteamDecrypt(const string &data);
	private:
		string PkcsPad(const string &data);
		string PkcsUnpad(const string &data);

		string m_steamKey;
		CryptoPP::CRC32 m_crc32;
		CryptoPP::RSA::PublicKey m_rsaPublicKey;
		CryptoPP::AutoSeededRandomPool m_random;
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption m_cbcEncryptor;
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption m_cbcDecryptor;
		CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption m_ecbEncryptor;
		CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption m_ecbDecryptor;
	};
}

#endif