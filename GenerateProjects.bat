@echo off
ThirdParty\Binaries\Premake4.exe --file=OpenSteamClient.lua clean
ThirdParty\Binaries\Premake4.exe --file=OpenSteamClient.lua vs2010
pause