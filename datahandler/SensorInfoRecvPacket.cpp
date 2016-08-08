#include "SensorInfoRecvPacket.h"
#include <msgpack.h>
#include "PacketDefs.h"
#include <stdio.h>
#include "webapiv1.h"
#include "SensorDefs.h"

unsigned char SensorInfoRecvPacket::sensors = 0;

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

			case SensorPacketIDs::SensorType:
			{
				ParseSensorType(_unpacker);
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
		int temp = 0, humidity = 0;

		if ((msg.data.type == MSGPACK_OBJECT_POSITIVE_INTEGER) || (msg.data.type == MSGPACK_OBJECT_NEGATIVE_INTEGER))
		{
			temp = msg.data.via.i64;
			printf( "Temp: %i\n", temp );

			// TODO: Send this somewhere
		}

		// Grab the humidity
		ret = msgpack_unpacker_next((msgpack_unpacker*)_unpacker, &msg);

		if (ret)
		{
			if ((msg.data.type == MSGPACK_OBJECT_POSITIVE_INTEGER) || (msg.data.type == MSGPACK_OBJECT_NEGATIVE_INTEGER))
			{
				humidity = msg.data.via.i64;
				printf( "Humidity: %i\n", humidity );

				// TODO: Send this somewhere
			}
		}

		webapiv1::GetSingleton()->UpdateTemperature(temp, humidity);
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
			webapiv1::GetSingleton()->UpdateDoorState(doorStatus);
		}
	}

	msgpack_unpacked_destroy(&msg);
}

void SensorInfoRecvPacket::ParseSensorType(void * _unpacker)
{
	msgpack_unpacked msg;
	msgpack_unpacked_init(&msg);

	msgpack_unpack_return ret = msgpack_unpacker_next((msgpack_unpacker*)_unpacker, &msg);

	if (ret)
	{
		if (msg.data.type == MSGPACK_OBJECT_POSITIVE_INTEGER)
		{
			SensorTypes type = (SensorTypes)msg.data.via.u64;

			ret = msgpack_unpacker_next((msgpack_unpacker*)_unpacker, &msg);

			if (ret)
			{
				if (msg.data.type == MSGPACK_OBJECT_BOOLEAN)
				{
					bool enabled = msg.data.via.boolean;

					if (enabled)
						sensors |= (1 << (unsigned char)type);
					else 
						sensors &= ~(1 << (unsigned char)type);

					webapiv1::GetSingleton()->SetSensors(sensors);
				}
			}
		}
	}
}
