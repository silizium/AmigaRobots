
/* WOLF ist ein pcroboter der erst versucht mit den anderen Wölfen
	Kontakt aufzunehmen, damit sie dann im Rudel über ein Opfer
	herfallen können.

	Programmiert und entworfen von D. Hackmann, Stolpmünderstr. 25b, 2 HH 73

*/


#include <stdlib.h>


/* Include the PCROBOTS header file for the special function definitions*/
#include "pcrobots.h"


/* Function prototypes */
void read_local_map(int x,int y);
void check_battery(void);
void check_armour(void);
void init_local_map(void);


/* Type definitions */
typedef unsigned char byte;

/* Define the robot's icon on the screen - this is a small smiley shape	*/
const char pattern[5]={4,10,17,10,4};


#define UNKNOWN 255

#define CSPEED 0
#define CMANOV 2
#define CRANGE 4
#define CARMOR 4
#define CACCEL 0
#define CINVIS 1

int maxspeed[5]={50,75,10,150,200};
int maxrange[5]={300,500,700,1000,1500};

int new_armour, last_armour, new_battery, last_battery,
		go_dir, myspeed,
		id, scan_angle, scan_res, range,
		shoot_angle, shoot_range;

byte map[100][100];

int now;


void	main(void)
{

	configure(CSPEED,CMANOV,CRANGE,CARMOR,CACCEL,CINVIS);
	now = ticks();
	scan_res = 5;
	scan_angle = 0;
	init_local_map();

	//------------------
	// Hier geht's los
	//------------------
	while(1)
	{
		now = ticks();
		check_armour();

		if( ( id = scan(scan_angle, scan_res, &range) ) >= 0 )
		{
			shoot_angle = scan_angle;
			shoot_range = range;
			shoot(shoot_angle, shoot_range);
		}
		else
		{
			scan_angle += 5;
			scan_angle %= 360;
		}


		if(now == ticks())
		{
			swaptask();
		}
	}
}

void init_local_map(void)
{
	int x,y;

	for( y = 0; y < 100; y++)
	{
		for( x = 0; x < 100; x++)
		{
			map[x][y] = UNKNOWN;
		}
	}
}


/* Copy the local map into the main map
 * x,y are the current 'map' coordinates */
void	read_local_map(int x,int y)
{
 int 	i,
	j,
	val;
 byte	*ptr;

/* Buffer to hold the local map before copying it to the main map	*/
 byte	local_map[81];


/* Obtain the local map */
 get_local_map(local_map);

/* Copy the local map into the main map */
 ptr=local_map;

/* Get coordinates of top left of local map area */
 x-=4;
 y-=4;

 for (i=0;i<9;i++)
 {
  for (j=0;j<9;j++)
  {
/* only copy if within the actual arena */
	if ((x<0)||(x>=100)||(y<0)||(y>=100))
	ptr++;
	else
	{
	val=*(ptr++);
	map[y][x]=val;	/* store the map contents */

	}
	x++;			/* next column */
  }
  y++;			/* next row */
  x-=9;
 }
}

void check_battery(void)
{
	new_battery=battery();
	if ((new_battery<(600-(myspeed*8))))
		myspeed=0;
	else
		myspeed=maxspeed[CSPEED];
}

void check_armour(void)
{
	new_armour=damage();
	if (new_armour<last_armour)
	{
	}
	last_armour=new_armour;

	if ((last_armour<50)&&(new_battery>400))
	{
		buy_armour(2);
	}
}