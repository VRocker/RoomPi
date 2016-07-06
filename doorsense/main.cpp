#include <stdio.h>
#include <unistd.h>
#include "gpiohandler.h"

int main()
{
	printf( "Setting direction to In\n" );
	gpiohandler::getSingleton()->SetDirection(25, GPIO_Direction::In);

	bool oldState = false;
	while ( true )
	{
		bool state = gpiohandler::getSingleton()->ReadGPIO(25);
		if ( state != oldState )
			printf( "Door state is now: %s\n", state ? "Closed" : "Open" );

		oldState = state;

		sleep( 1 );
	}
	return 0;
}