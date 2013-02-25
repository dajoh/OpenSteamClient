#ifndef OPENSTEAMCLIENT_NONCOPYABLE_HPP
#define OPENSTEAMCLIENT_NONCOPYABLE_HPP

namespace Sc
{
	class NonCopyable
	{
	public:
		NonCopyable() {}
	private:
		NonCopyable(const NonCopyable &);
		NonCopyable &operator=(const NonCopyable &);
	};
}

#endif