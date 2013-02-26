protoc -IThirdParty/Protobufs --cpp_out=Source/Protobufs ThirdParty/Protobufs/iclient_objects.proto
protoc -IThirdParty/Protobufs --cpp_out=Source/Protobufs ThirdParty/Protobufs/content_manifest.proto
protoc -IThirdParty/Protobufs --cpp_out=Source/Protobufs ThirdParty/Protobufs/encrypted_app_ticket.proto
protoc -IThirdParty/Protobufs --cpp_out=Source/Protobufs ThirdParty/Protobufs/steammessages_base.proto
protoc -IThirdParty/Protobufs --cpp_out=Source/Protobufs ThirdParty/Protobufs/steammessages_clientserver.proto