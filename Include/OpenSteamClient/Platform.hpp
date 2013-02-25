#ifndef OPENSTEAMCLIENT_PLATFORM_HPP
#define OPENSTEAMCLIENT_PLATFORM_HPP

#if defined(_WIN32) && defined(OPENSTEAMCLIENT_SHARED)
#ifdef OPENSTEAMCLIENT_INTERNAL
#define OPENSTEAMCLIENT_EXPORT __declspec(dllexport)
#else
#define OPENSTEAMCLIENT_EXPORT __declspec(dllimport)
#endif
#else
#define OPENSTEAMCLIENT_EXPORT
#endif

#endif