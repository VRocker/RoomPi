#pragma once

#include "ISingleton.h"

enum class GPIO_Direction
{
	In,
	Out
};

typedef enum
{
	TYPE_PI0 = 0,
	TYPE_PI1 = 1,
	TYPE_PI2 = 2,
	TYPE_PI3 = 3,
} PITypes;

class gpiohandler : public ISingleton< gpiohandler >
{
public:
	gpiohandler();
	~gpiohandler();

	void SetDirection(int pin, GPIO_Direction dir);
	void WriteGPIO(int pin, bool onOff);
	bool ReadGPIO(int pin);

	void SetGPIOBase(unsigned int type);

private:
	void SetupIO();
	void UnmapIO();

private:
	volatile unsigned* m_gpio;
	void* m_gpioMap;
	unsigned int m_gpioBase;
};