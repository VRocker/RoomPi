#include "dht11.h"
#include "gpiohandler.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

void delay (unsigned int howLong)
{
  struct timespec sleeper;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, 0) ;
}

void delayMicrosecondsHard (unsigned int howLong)
{
  struct timeval tNow, tLong, tEnd ;

  gettimeofday (&tNow, NULL) ;
  tLong.tv_sec  = howLong / 1000000 ;
  tLong.tv_usec = howLong % 1000000 ;
  timeradd (&tNow, &tLong, &tEnd) ;

  while (timercmp (&tNow, &tEnd, <))
    gettimeofday (&tNow, NULL) ;
}

void delayMicroseconds (unsigned int howLong)
{
  struct timespec sleeper ;
  unsigned int uSecs = howLong % 1000000 ;
  unsigned int wSecs = howLong / 1000000 ;

  /**/ if (howLong ==   0)
    return ;
  else if (howLong  < 100)
    delayMicrosecondsHard (howLong) ;
  else
  {
    sleeper.tv_sec  = wSecs ;
    sleeper.tv_nsec = (long)(uSecs * 1000L) ;
    nanosleep (&sleeper, NULL) ;
  }
}

bool DHT11::ReadData(DHT11_Data* data)
{
	unsigned char dht11_dat[5] = { 0 };
	unsigned int timings = 0, counter = 0;
	bool lastState = true;

	gpiohandler::getSingleton()->SetDirection(DHT11_PIN, GPIO_Direction::Out);

	gpiohandler::getSingleton()->WriteGPIO(DHT11_PIN, false);
	delay (18); // 18 milliseconds
	gpiohandler::getSingleton()->WriteGPIO(DHT11_PIN, true);
	//delayMicroseconds( 20 ); // 40 Microseconds
	gpiohandler::getSingleton()->SetDirection(DHT11_PIN, GPIO_Direction::In);

	for ( unsigned int i = 0; i < 85; ++i )
	{
		counter = 0;
		while ( gpiohandler::getSingleton()->ReadGPIO(DHT11_PIN) == lastState )
		{
			delayMicroseconds(1);
			if (++counter == 255 )
				break;
			
		}
		lastState = gpiohandler::getSingleton()->ReadGPIO(DHT11_PIN);

		if (counter == 255 )
			break;

		if ( ( i >= 4 ) && (!(i % 2)) )
		{
			dht11_dat[timings / 8] <<= 1;
			if ( counter > 16 )
				dht11_dat[timings / 8] |= 1;
			++timings;
		}
	}

	if ( ( timings >= 40 ) &&
		( dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF ) ) )
	{
		data->humidityWhole = dht11_dat[0];
		data->humidityFraction = dht11_dat[1];
		data->tempWhole = dht11_dat[2];
		data->tempFraction = dht11_dat[3];

		return true;
	}

	return false;
}
