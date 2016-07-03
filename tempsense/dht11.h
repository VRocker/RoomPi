#pragma once

#define DHT11_PIN 25

struct DHT11_Data
{
	int humidityWhole;
	int humidityFraction;

	int tempWhole;
	int tempFraction;
};

class DHT11
{
public:
	static bool ReadData(DHT11_Data* data);

};
