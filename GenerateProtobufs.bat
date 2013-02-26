@echo off
ThirdParty\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\iclient_objects.proto
ThirdParty\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\content_manifest.proto
ThirdParty\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\encrypted_app_ticket.proto
ThirdParty\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\steammessages_base.proto
ThirdParty\protoc.exe -IThirdParty\Protobufs --cpp_out=Source\Protobufs ThirdParty\Protobufs\steammessages_clientserver.proto
pause