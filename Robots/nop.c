/* Ein Testroboter der nichts tut ist zu nichts gut */
#ifndef __SASC
char *_ProgramName="nop.rob";
#endif

#include <pcrobots.h>

void main(void)
{
	configure(2,2,2,2,2,0);

	while(1)
	{
		swaptask();
	}
}
