/* Ein Testroboter der nichts tut ist zu nichts gut */
#ifndef __SASC
char *_ProgramName="Manowar.rob";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pcrobots.h>

void main(void)
{
	int x,y,dx,dy,dir;

	configure(2,2,2,2,2,1);

	while (1)
	{
		/*	Proludum zum Rand bewegen bevor man gesehen wird
.........*/
		invisibility(1);
		getxy(&x,&y);
		/*geringster Abstand zu den wänden prüfen */
		dx=abs(x-500);

		dy=abs(y-500);									
		if (dx>dy)
 	 	{
 	 		if (x>500) 
			{
				dir=0;
			}
			else
			{
			   dir=180;
			}
 	 	}
		else	
		{
 	 		if (y>500) 
			{
				dir=90;
			}
			else
			{
			   dir=270;
			}
		}
		do
		{
		/*maximum speed*/
			movement( 200,dir); 
			if(battery()<200 && damage()>30)
			{
				buy_armour(-1);
			}
		}
		while(speed()!=0);
		
		while(1) swaptask();
	}
}
