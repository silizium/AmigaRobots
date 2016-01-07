/* Kampfroboter v.2.0 von Th. Soeffing */


/* includes  */

#include <stdlib.h>
#include <time.h>
#include "pcrobots.h"


/* definitionen  */

#define randomize() srand(time(NULL))
#define random(weite) rand()%weite


/* konstanten  */

const char patterna[] = {0x04,0x0a,0x15,0x0a,0x04};
const char patternb[] = {0x00,0x00,0x04,0x00,0x00};


/* strukturen  */

struct
{
	int	
		x,
		y,
		winkel;
} winkel[8]={{5,4,0},{5,5,45},{4,5,90},{3,5,135},{3,4,180},
		   {3,3,225},{4,3,270},{5,3,315}};


/* Programmstart  */

void	main(void)
{
	char	local_map[9][9];
	int	x_pos,
	y_pos;
	int	richtung;
	int	scan_winkel=0;
	int	weite ;
	int	test;
	int	power;
	int	panzerung;
	int	lastshot = -50, now;
	int	found,found_weite = 1000;
	int	id, id_alt = -1;
	int   x;


/* Roboterkonfiguration  */

	configure(0,0,4,4,2,1);


/* Erscheinungsbild "sichtbar"  */

	set_pattern(patterna);
	
	randomize();

	richtung=random(8);


/* Hauptschleife  */

	while(1)
	{
		getxy(&x_pos,&y_pos);
		get_local_map((char *)local_map);

		if (x_pos<20)
			richtung=(random(3)-1)%8;
		if (x_pos>=980)
			richtung=random(3)+3;
		if (y_pos<20)
			richtung=random(3)+1;
		if (y_pos>=980)
			richtung=random(3)+5;


		test=0;


/* Fahrtrichtungsbestimmung  */	

		while (local_map[winkel[richtung].y][winkel[richtung].x] == ARENA_WALL)
		{
			richtung=random(8);
			if (test++ > 10)
				break;
		}


/* Nummer vom Feind feststellen und fixieren  */

		id = scan(scan_winkel,2,&weite);
		id_alt = id;
		for (x=0;x<180;x++)
		{


/* Zustand von Batterie und Panzerung feststellen  */

			panzerung = damage();
			power = battery();


/* Schutzschilde für Batterieeinheiten kaufen  */
			if(!shells_left()) buy_shells(1);

			if (panzerung < 50 && power > 50)
				buy_armour(1);
			if (panzerung < 150 && power > 500)
				buy_armour(5);


/* Erscheinungsbild "unsichtbar"  */

			invisibility(1);
			set_pattern(patternb);


/* Roboter in Abhängigkeit von Batterie bewegen  */

			if(power < 50)
				movement(0,0);
			else if(power < 501)
				movement(30,winkel[richtung].winkel);
			else
				movement(60,winkel[richtung].winkel);
			invisibility(0);
			set_pattern(patterna);


/* nochmal nachscannen um sich genau einzuschießen  */
/* und Schuss abgeben  */

		 	if ((id = scan(scan_winkel,2,&weite))>=0)
			{
				if (id = id_alt)
				{
					found_weite = weite;
					found = scan_winkel;
				}
				else
				{
				 	scan(scan_winkel,2,&weite);
				}
			}
			else
			{
				scan_winkel=(scan_winkel+2)%360;
			}
			now = ticks();
			if(now - lastshot > 49)
			{
				if (found_weite > 20)
					shoot(found,found_weite);
				lastshot = now;
			}
		}
	}
}
