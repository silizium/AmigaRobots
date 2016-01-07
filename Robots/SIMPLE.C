/* 	SIMPLE.C - a simple demonstration PCRobots Robot in Turbo C	*/
/*		Written by P.D.Smith - July 1992			*/
#ifndef __SASC
char *_ProgramName="simple.rob";
#endif

#include <stdlib.h>
#include <time.h>

/* Include the PCROBOTS header file for the special function definitions */
#include "pcrobots.h"

#define randomize() srand(time(NULL))
#define random(range) rand()%range
/*
 * Define possible angles of motion
 * x,y are x,y positions on the local map
 * angle is the angle of motion to specify
 */
struct 	{
	 int	x,
		y,
		angle;
	}angle[8]={{5,4,0},{5,5,45},{4,5,90},{3,5,135},{3,4,180},
		   {3,3,225},{4,3,270},{5,3,315}};

void	main(void)
{
 char	local_map[9][9]; 	/* Buffer to hold the local map */
 int	x_pos,			/* Current X and Y coordinates of robot */
	y_pos;
 int	go_dir;			/* current direction (index to angle[]) */
 int	scan_angle=0;		/* Current scanning angle */
 int	range;			/* range returned from the scan routine */
 int	try;

/* Configure robot - all normal settings in this case
 * This *MUST* be the first special function called by the robot   */
 configure(2,2,2,2,2,0);

 randomize();

 go_dir=random(8);		/* Select initial direction */

/* Note - this is an *INFINITE* loop - the PCROBOTS executive program
   kills the robot off when necessary*/
 while(1)
 {
/* Get the current position */
  getxy(&x_pos,&y_pos);

/* Get a map of the local area */
  get_local_map((char *)local_map);

/* Quick and easy way of dealing with the robot being near an outside wall */
  if (x_pos<20)
   go_dir=(random(3)-1)%8;
  if (x_pos>=980)
   go_dir=random(3)+3;
  if (y_pos<20)
   go_dir=random(3)+1;
  if (y_pos>=980)
   go_dir=random(3)+5;

  try=0;
/* Look for a square which hasn't got a wall in it (ignore traps in this
 * simple program) */
  while (local_map[angle[go_dir].y][angle[go_dir].x] == ARENA_WALL)
  {
   go_dir=random(8);

/* Failsafe check to avoid the program getting trapped here */
   if (try++ > 10)
    break;
  }

/* Move in the new direction */
  movement(50,angle[go_dir].angle);

/* Check if target found, shoot at it if so, otherwise go to next angle */
  if (scan(scan_angle,5,&range)>=0)
  {
	if(!shells_left()) buy_shells(1);
   shoot(scan_angle,range);
  }
  else
   scan_angle=(scan_angle+5)%360;

 }
}


