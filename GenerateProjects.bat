@echo off
ThirdParty\Premake4.exe --file=OpenSteamClient.lua clean
ThirdParty\Premake4.exe --file=OpenSteamClient.lua vs2010
pause