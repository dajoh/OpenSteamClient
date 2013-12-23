@echo off
ThirdParty\Binaries\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\iclient_objects.proto
ThirdParty\Binaries\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\content_manifest.proto
ThirdParty\Binaries\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\encrypted_app_ticket.proto
ThirdParty\Binaries\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\steammessages_base.proto
ThirdParty\Binaries\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\steammessages_clientserver.proto
pause