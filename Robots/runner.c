#ifndef __SASC
char *_ProgramName="runner.rob";
#endif
#include <pcrobots.h>

void main(void)
{
	int lastdamage,lastbatt = 1000;
	int rightspeed;

	configure(4,2,0,0,4,0);
	
	while(1)
	{

		lastdamage = damage();
		while(lastdamage == damage())
		{
			rightspeed = (battery()<100)?(battery()-lastbatt)*10+speed():200;
			lastbatt=battery();
			movement(rightspeed,0);
		}


		lastdamage = damage();
		while(lastdamage == damage())
		{
			rightspeed = (battery()<100)?(battery()-lastbatt)*10+speed():200;
			lastbatt=battery();
			movement(rightspeed,180);
		}
	}
}