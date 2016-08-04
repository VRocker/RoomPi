#include "gpiohandler.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(m_gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(m_gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(m_gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(m_gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(m_gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(m_gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(m_gpio+38) // Pull up/pull down clock

template<>
gpiohandler* ISingleton< gpiohandler >::m_singleton = nullptr;

gpiohandler::gpiohandler()
	: m_gpio(nullptr), m_gpioMap(nullptr), m_gpioBase(0x20000000 + 0x200000)
{
	SetupIO();
}

gpiohandler::~gpiohandler()
{
	UnmapIO();
}

void gpiohandler::SetDirection(int pin, GPIO_Direction dir)
{
	// Need to put input before output
	INP_GPIO(pin);
	if (dir == GPIO_Direction::Out)
		OUT_GPIO(pin);
}

void gpiohandler::WriteGPIO(int pin, bool onOff)
{
	if (onOff)
	{
		// Pin On
		GPIO_SET = 1 << pin;
	}
	else
	{
		// Pin Off
		GPIO_CLR = 1 << pin;
	}
}

bool gpiohandler::ReadGPIO(int pin)
{
	if (GET_GPIO(pin))
		return true;
	return false;
}

void gpiohandler::SetupIO()
{
	int memfd = -1;
	if ((memfd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
	{
		fprintf(stderr, "Unable to open /dev/mem\n");
		return;
	}

	m_gpioMap = mmap(NULL, (4 * 1024), PROT_READ | PROT_WRITE, MAP_SHARED, memfd, m_gpioBase);

	close(memfd);

	if (m_gpioMap == MAP_FAILED)
	{
		fprintf(stderr, "mmap error: %d\n", (int)m_gpioMap);
		return;
	}

	m_gpio = (volatile unsigned*)m_gpioMap;
}

void gpiohandler::UnmapIO()
{
	if (m_gpioMap > 0)
	{
		munmap(m_gpioMap, (4 * 1024));
		m_gpioMap = nullptr;
	}

	m_gpio = nullptr;
}

void gpiohandler::SetGPIOBase(unsigned int type)
{
	UnmapIO();

	switch (type)
	{
	case TYPE_PI0:
	case TYPE_PI1:
		m_gpioBase = 0x20000000 + 0x200000;
		break;

	case TYPE_PI2:
	case TYPE_PI3:
		m_gpioBase = 0x3F000000 + 0x200000;
		break;

	default:
		break;
	}


	SetupIO();
}
