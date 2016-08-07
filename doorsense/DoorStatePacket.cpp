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
	if (buffer)
	{
		msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);
		if (pk)
		{
			PreparePacket(pk, (uint8_t)SensorPacketIDs::DoorStatus);

			if (open)
				msgpack_pack_true(pk);
			else
				msgpack_pack_false(pk);

			buffer->data[buffer->size] = 0;

			// Send the data off to the DataHandler process
			ClientSock::GetSingleton()->Send(buffer->data, buffer->size);

				// Stops ZeroMQ from stalling as it must handle a recv before continuing
			ClientSock::GetSingleton()->Recv();

				// Cleanup memory
	
			msgpack_sbuffer_free(buffer);
			buffer = nullptr;
	
			msgpack_packer_free(pk);
			pk = nullptr;
		}
	}
}

void DoorStatePacket::SensorType(unsigned char type, bool enabled)
{
	msgpack_sbuffer* buffer = msgpack_sbuffer_new();
	if (buffer)
	{
		msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);

		if (pk)
		{
			PreparePacket(pk, (uint8_t)SensorPacketIDs::SensorType);

			msgpack_pack_uint8(pk, type);

			if (enabled)
				msgpack_pack_true(pk);
			else 
				msgpack_pack_false(pk);

			buffer->data[buffer->size] = 0;

			ClientSock::GetSingleton()->Send(buffer->data, buffer->size);

			ClientSock::GetSingleton()->Recv();

			msgpack_sbuffer_free(buffer);
			buffer = nullptr;

			msgpack_packer_free(pk);
			pk = nullptr;
		}
	}
}
