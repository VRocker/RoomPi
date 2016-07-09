#include "TempSensorPacket.h"
#include <msgpack.h>
#include "../shared/clientsock/ClientSock.h"

void TempSensorPacket::PreparePacket( void* packer, unsigned char packetID)
{
	msgpack_pack_uint8((msgpack_packer*)packer, (uint8_t)PrimaryPacketIDs::SensorInfo);
	msgpack_pack_uint8((msgpack_packer*)packer, packetID);
}

void TempSensorPacket::TempAndHumidity(int temp, int humidity)
{
	msgpack_sbuffer* buffer = msgpack_sbuffer_new();
	if (buffer)
	{
		msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);

		if (pk)
		{
			PreparePacket(pk, (uint8_t)SensorPacketIDs::TempAndHumid);

			msgpack_pack_int(pk, temp);
			msgpack_pack_int(pk, humidity);

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
