solution "OpenSteamClient"
	language "C++"
	location "Projects"
	targetdir "Binaries"
	includedirs { "Include" }
	configurations { "Debug", "Release", "DebugShared", "ReleaseShared" }

	configuration "Debug or DebugShared"
		flags { "Symbols" }
	configuration "Release or ReleaseShared"
		flags { "Optimize" }

	project "OpenSteamClient"
		files
		{
			"Source/**.h", "Source/**.cc",
			"Source/**.hpp", "Source/**.cpp",
			"Include/OpenSteamClient/**.hpp"
		}
		vpaths
		{
			["Source Files"] = "Source/**.cpp",
			["Source Files/Messages"] = "Source/Messages/**.cpp",
			["Source Files/Protobufs"] = "Source/Protobufs/**.cc",
			["Header Files"] = "Source/**.hpp",
			["Header Files/Messages"] = "Source/Messages/**.hpp",
			["Header Files/Protobufs"] = "Source/Protobufs/**.h",
			["Include Files"] = "Include/OpenSteamClient/**"
		}

		configuration "windows"
			defines { "_SCL_SECURE_NO_WARNINGS", "OPENSTEAMCLIENT_INTERNAL" }
			libdirs { "ThirdParty/Libraries" }
			includedirs { "ThirdParty/Include" }

		configuration "linux"
			buildoptions { "-std=c++11" }

		configuration "Debug"
			targetsuffix "_sd"
		configuration "Release"
			targetsuffix "_s"
		configuration "DebugShared"
			targetsuffix "_d"

		configuration "Debug or Release"
			kind "StaticLib"
		configuration "DebugShared or ReleaseShared"
			kind "SharedLib"
			defines { "OPENSTEAMCLIENT_SHARED" }

		configuration { "linux", "DebugShared or ReleaseShared"}
			links { "crypto++", "protobuf-lite" }
		configuration { "windows", "DebugShared" }
			links { "ws2_32", "cryptopp_d", "libprotobuf-lite_d" }
		configuration { "windows", "ReleaseShared" }
			links { "ws2_32", "cryptopp", "libprotobuf-lite" }

		configuration "linux"
			excludes { "Source/TcpConnectionWin32.cpp" }
		configuration "windows"
			excludes { "Source/TcpConnectionPosix.cpp" }
