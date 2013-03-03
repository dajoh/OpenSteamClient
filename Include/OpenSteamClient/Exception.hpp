#ifndef OPENSTEAMCLIENT_EXCEPTION_HPP
#define OPENSTEAMCLIENT_EXCEPTION_HPP

#include <string>
#include <exception>

namespace Sc
{
	class Exception : public std::exception
	{
	public:
		Exception(const char *error) : m_error(error) {}
		~Exception() throw() {}

		const char *what() { return m_error.c_str(); }
	private:
		std::string m_error;
	};
}

#endif
