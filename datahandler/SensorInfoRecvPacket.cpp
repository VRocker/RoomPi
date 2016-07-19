#include "SensorInfoRecvPacket.h"
#include <msgpack.h>
#include "PacketDefs.h"
#include <stdio.h>

void SensorInfoRecvPacket::ParsePacket(void * _unpacker)
{
	//msgpack_unpacked msg = *(msgpack_unpacked*)_msg;

	msgpack_unpacked msg;
	msgpack_unpacked_init(&msg);
	msgpack_unpack_return ret = msgpack_unpacker_next((msgpack_unpacker*)_unpacker, &msg);

	if (ret)
	{
		// Parse the sensorInfo PacketID
		if (msg.data.type == MSGPACK_OBJECT_POSITIVE_INTEGER)
		{
			printf( "Second Packet ID: %i\n", (unsigned char)msg.data.via.u64);
			switch ((SensorPacketIDs)msg.data.via.u64)
			{
			case SensorPacketIDs::TempAndHumid:
			{
				ParseTempAndHumidity(_unpacker);
			}
			break;

			case SensorPacketIDs::DoorStatus:
			{
				ParseDoorStatus(_unpacker);
			}
			break;
			}
		}
	}

	msgpack_unpacked_destroy(&msg);
}

void SensorInfoRecvPacket::ParseTempAndHumidity(void * _unpacker)
{
	printf( "Temp and Humid...\n" );
	msgpack_unpacked msg;
	msgpack_unpacked_init(&msg);

	msgpack_unpack_return ret = msgpack_unpacker_next((msgpack_unpacker*)_unpacker, &msg);

	if ( ret )
	{
		if ((msg.data.type == MSGPACK_OBJECT_POSITIVE_INTEGER) || (msg.data.type == MSGPACK_OBJECT_NEGATIVE_INTEGER))
		{
			int temp = msg.data.via.i64;
			printf( "Temp: %i\n", temp );

			// TODO: Send this somewhere
		}

		// Grab the humidity
		ret = msgpack_unpacker_next((msgpack_unpacker*)_unpacker, &msg);

		if (ret)
		{
			if ((msg.data.type == MSGPACK_OBJECT_POSITIVE_INTEGER) || (msg.data.type == MSGPACK_OBJECT_NEGATIVE_INTEGER))
			{
				int humidity = msg.data.via.i64;
				printf( "Humidity: %i\n", humidity );

				// TODO: Send this somewhere
			}
		}
	}

	msgpack_unpacked_destroy(&msg);
}

void SensorInfoRecvPacket::ParseDoorStatus(void * _unpacker)
{
	msgpack_unpacked msg;
	msgpack_unpacked_init(&msg);

	msgpack_unpack_return ret = msgpack_unpacker_next((msgpack_unpacker*)_unpacker, &msg);

	if (ret)
	{
		if (msg.data.type == MSGPACK_OBJECT_BOOLEAN)
		{
			bool doorStatus = msg.data.via.boolean;

			// TODO: Send this somewhere
		}
	}

	msgpack_unpacked_destroy(&msg);
}
