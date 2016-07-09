#include "DoorStatePacket.h"
#include <msgpack.h>
#include "../shared/clientsock/ClientSock.h"

void DoorStatePacket::PreparePacket( void* packer, unsigned char packetID)
{
	msgpack_pack_uint8((msgpack_packer*)packer, (uint8_t)PrimaryPacketIDs::SensorInfo);
	msgpack_pack_uint8((msgpack_packer*)packer, packetID);
}

void DoorStatePacket::State(bool open)
{
	msgpack_sbuffer* buffer = msgpack_sbuffer_new();
	msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);

	PreparePacket(pk, (uint8_t)SensorPacketIDs::DoorStatus);

	msgpack_pack_bool(pk, open);

	buffer->data[buffer->size] = 0;

	// Send the data off to the DataHandler process
	ClientSock::GetSingleton()->Send(buffer->data, buffer->size);

	// Stops ZeroMQ from stalling as it must handle a recv before continuing
	ClientSock::GetSingleton()->Recv();

	// Cleanup memory
	if (buffer)
	{
		msgpack_sbuffer_free(buffer);
		buffer = nullptr;
	}

	if (pk)
	{
		msgpack_packer_free(pk);
		pk = nullptr;
	}
}
