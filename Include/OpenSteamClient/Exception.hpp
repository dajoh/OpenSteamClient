#ifndef OPENSTEAMCLIENT_EXCEPTION_HPP
#define OPENSTEAMCLIENT_EXCEPTION_HPP

#include <exception>

namespace Sc
{
	class Exception : public std::exception
	{
	public:
		Exception(const char *error) : std::exception(error) {}
	};
}

#endif