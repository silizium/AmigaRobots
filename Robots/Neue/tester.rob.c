/* Ein Testroboter der nichts tut ist zu nichts gut */
#ifndef __SASC
char *_ProgramName="tester.rob";
#endif

#include <stdio.h>
#include <pcrobots.h>

void main(void)
{
	int dir=0;

	configure(2,3,2,2,1,0);
	printf("Version: %d\n", version());
	printf("HightestPlayerID: %d MyID %d\n", highestplayerid(), myid());

	while(1)
	{
		if(flying_shells() < 3)
		{
			shoot(dir,200);
			dir +=17;
		}
		else
			swaptask();
	}
}
