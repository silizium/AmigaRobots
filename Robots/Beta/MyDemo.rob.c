/* 	MYDEMO.C - a demonstration PCRobots Robot in Turbo C		*/
/*		Written by Dirk Hackmann 1993	*/

#include <stdlib.h>
#include <time.h>

/* Include the PCROBOTS header file for the special function definitions*/
#include "pcrobots.h"


/* Function prototypes */
void	read_local_map(int x,int y);

/* Type definitions */
typedef unsigned char byte;

/* Define the robot's icon on the screen - this is a small smiley shape	*/
const char pattern[5]={27,4,4,17,14};

/*
 * Define possible angles of motion
 * x,y are x,y offsets on the map
 * angle is the angle of motion to specify
 */
struct 	{
	 int	x,
		y,
		angle;
	}
	angle[8]={{ 1, 0,0},
		  { 1, 1,45},
		  { 0, 1,90},
		  {-1, 1,135},
		  {-1, 0,180},
		  {-1,-1,225},
		  { 0,-1,270},
		  { 1,-1,315}};

/* Array to hold the map */
byte	map[100][100];

/* Current X and Y coordinates of the robot */
int	x_pos,
	y_pos;

/* Structure to hold information about the refueling points that
 * have been located.
 * x,y are coordinates (in map units)
 * last_visit is the time of the last visit (in game ticks)
 * 	to avoid revisiting refuelers too often
 */
struct  {
	 int 	x,
		y;
	 long	last_visit;
	} refueler[10];
int	num_refuelers=0;	/* number of refuelers that have
				   been found */

int	go_dir;			/* current direction (index to angle[]) */
int	last_battery;		/* last battery reading	*/
int	last_armour;		/* last armour status */

int	scan_angle=0;		/* Current scanning angle */
int	range;			/* values returned */
int	id;			/* from the scan routine */

void	main(void)
{
 int	i,
	j,
	try;
 int	x_square,
	y_square,
	next_x,
	next_y;
 int	new_armour,
	new_battery;
 int	invis_count=0;
 int	speed=0;
 long	now,
	last_shoot=-50;
 int	shot_counter=0;		/* this is used to detect shells hitting walls */

/* Configure robot - all normal, except with invisibility facility
 * This *MUST* be the first special function called by the robot   */
 configure(2,2,2,2,2,1);

 srand((unsigned) 3167);

 go_dir=rand() % 8;		/* Select initial direction */

/* Set the robot's icon */
 set_pattern(pattern);

/* Clear the map to '255' so we know where we haven't been yet */
 for (i=0;i<100;i++)
 {
  for (j=0;j<100;j++)
	map[i][j]=255;
 }

 last_armour=damage();  	/* get initial armour and battery levels */
 last_battery=battery();

/* Note - this is an *INFINITE* loop - the PCROBOTS executive program
	kills the robot off */
 while(1)
 {
/* Get the current position */
  getxy(&x_pos,&y_pos);

  now=ticks();

/* Get the current map square */
  x_square = x_pos/10;
  y_square = y_pos/10;
/* Get the next square to be moved into */
  next_x   = x_square+angle[go_dir].x;
  next_y   = y_square+angle[go_dir].y;

/* If the current map square, or the next map_square to be moved into,
 * are unknown, read the local map */
  if ((map[y_square][x_square] == 255) ||
	(map[next_y][next_x]   == 255))
  {
	read_local_map (x_square,y_square);
  }

/* Ok, now we know where we are, look to see if the target square contains
 * a wall or trap - if so, choose a new direction which is clear */

/* Quick and easy way of dealing with the robot being near an outside wall */
  if ((next_x<2) || (next_x>=98) || (next_y<2) || (next_y>=98))
  {
	go_dir=(go_dir + 4) % 8;
  }

  next_x   = x_square+angle[go_dir].x;
  next_y   = y_square+angle[go_dir].y;

  try=0;
  while ((map[next_y][next_x] == ARENA_WALL) ||
	 (map[next_y][next_x] == ARENA_DAMAGE) ||
	 ((next_x<0)||(next_x>=100)||(next_y<0)||(next_y>=100)))
  {
	debug_flag(0,1);
	srand((unsigned)now);
	go_dir=rand() % 8;

	next_x   = x_square+angle[go_dir].x;
	next_y   = y_square+angle[go_dir].y;

/* Failsafe check to avoid the program getting trapped here */
	if (try++ > 50)
	{
		debug_flag(1,1);
		break;
	}
  }
  debug_flag(0,0);


  movement(speed,angle[go_dir].angle);

/* Come out of invisibility if necessary */
  if (invis_count>0)
   invis_count--;
  else
   invisibility(0);

/* See if we've been hit! */
  new_armour=damage();
  if (new_armour<last_armour)
  {
   invisibility(1);
	go_dir=rand() % 8;
   invis_count=30;
  }
  last_armour=new_armour;

  new_battery=battery();

/* See if badly damaged- may be able to buy more armour */
  if ((last_armour<50)&&(new_battery>400))
  {
   buy_armour(2);
  }

/* If battery is depleted rest for a while */
  if ((new_battery<(600-(speed*8)))&&(invis_count==0))
   speed=0;
  else
   speed=50;


/* there's no point wasting time if my cannon hasn't reloaded yet */
  if ((now-last_shoot)>=50)
  {
   id=scan(scan_angle,10,&range);

// Check if no target, or target out of range - go to next angle if so */
   if ((id<0)||(range>700))
   {
    scan_angle=(scan_angle+10)%360;
    shot_counter=0;
   }
   else
   {
    shot_counter++;
    if (shot_counter==4)
    {
     if (get_shell_status()==SHELL_HIT_WALL)
     {
      scan_angle=(scan_angle+10)%360;
      id=-1;
     }
     shot_counter=0;
    }
    if (id>=0)
    {
     shoot(scan_angle,range);
     last_shoot=now;
    }
   }
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
