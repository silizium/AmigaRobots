/* Blinker ist schwer zu orten und repariert sich */

#include <stdlib.h>
#include <time.h>
#include <pcrobots.h>

#ifndef __SASC
char *_ProgramName="blinker.rob";
#endif

char image[2][5] =
{
	{0x15,0x0e,0x1b,0x0e,0x15},
	{0x00,0x00,0x04,0x00,0x00}
};

/* max gibt das maximum zurueck */
int max(int x, int y)
{
   if(x > y)
      return x;
   else
      return y;
}

/* min gibt das minimum zurueck */
int min(int x, int y)
{
   if(x < y)
      return x;
   else
      return y;
}

void main(void)
{
	int i,reminv,lastshot;
	int dir,id,range;

	configure(0,0,4,4,0,1);

	srand((long)&i+time(NULL));

	dir=0;
	lastshot = -50;
	while(1)
	{
		reminv = rand()%3+1;

		invisibility(1);
		set_pattern(image[1]);
		for(i=0; i<reminv; i++)
		{
			swaptask();
		}

		invisibility(0);
		set_pattern(image[0]);
		for(i=0; i<reminv; i++)
		{
			id=scan(dir,4,&range);
			if(id < 0)
			{
				dir += 7;
				dir %= 360;
			}

			if(id >= 0 && ticks()-lastshot >= 50)
			{
				if(!shells_left()) buy_shells(1);
				shoot(dir,range);
				lastshot = ticks();
			}
		}
		i = max( 0, min( (battery()-0)/50, 200-damage() ) );
		buy_armour(i);
	}
}
