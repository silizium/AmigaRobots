#ifndef __SASC
char *_ProgramName="Explorer.rob";
#endif

#include <stdlib.h>
#include <time.h>

#include "pcrobots.h"

#define randomize() srand(time(NULL))
#define random(range) rand()%range

const char pattern[5]={17,10,4,10,17};
typedef unsigned char byte;

struct 	{
	 int	x,
		y,
		angle,
		next;
	}
	angle[4]={{1,0,0,1},{0,1,90,2},{-1,0,180,3},{0,-1,270,0}};

byte map[100][100];
int	x_pos,
	y_pos;

struct	POINT
	{
	 int x,y;
	};

struct POINT	target;
struct POINT	last_map;


char	local_map[81];

struct POINT  	refueler[10];
int	num_refuelers=0;

struct	{
	 struct POINT 	pos;
	 int	link;
	}
	transport[20];

int	num_transports=0;

int	old_batt;

int	go_dir=0,
	last_dir=0;
int	force_dir=0;

void	main(void)
{
	int i,j,x,y,found;
	int x_square,y_square;
	char *ptr;

 int target_speed=50;
 target.x=100;
 target.y=750;

 randomize();

 configure(2,4,0,2,2,0);

 {
  for (i=0;i<100;i++)
  {
   for (j=0;j<100;j++)
   {
    map[i][j]=255;
   }
  }
 }

// target.x=-1;
 getxy(&last_map.x,&last_map.y);

 while(1)
 {
  getxy(&x_pos,&y_pos);
  x_square=x_pos/10;
  y_square=y_pos/10;
  if ((((x_pos-last_map.x)*(x_pos-last_map.x))+
	   ((y_pos-last_map.y)*(y_pos-last_map.y))>400)
	 ||(map[y_square][x_square]==255))
  {
   get_local_map(local_map);
   last_map.x=x_pos;
   last_map.y=y_pos;
   ptr=local_map;
   x=x_square-4;
	y=y_square-4;
   for (i=0;i<9;i++)
   {
    for (j=0;j<9;j++)
    {
     if ((x<0)||(x>=100)||(y<0)||(y>=100))
      ptr++;
     else
     {
      int val=*(ptr++);
      map[y][x]=val;
      switch(val)
      {
       case ARENA_REFUEL:{
			  found=-1;
			  for (i=0;i<num_refuelers;i++)
			  {
			   if ((refueler[i].x==x)&&(refueler[i].y==y))
			   {
			    found=i;
			    break;
			   }
			  }
			  if (found==-1)
			  {
			   refueler[num_refuelers].x=x;
			   refueler[num_refuelers].y=y;
			   target.x=x*10+5;
			   target.y=y*10+5;
			   num_refuelers++;
			  }
			 }
			 break;
      }
     }
     x++;
    }
    y++;
    x-=9;
   }
  }
  if (target.x==-1)
  {
   target.x=random(1000);
   target.y=random(1000);
   target_speed=50;
  }


  go_dir=-1;
  if (x_pos<target.x)
  {
   go_dir=0;
  }
  else
  {
   if (x_pos>target.x)
    go_dir=2;
  }
  if (go_dir==-1)
  {
   if (y_pos<target.y)
   {
    go_dir=1;
   }
   else
   {
    if (y_pos>target.y)
     go_dir=3;
   }
  }
/*
  if (last_dir==((go_dir+2)%4))
   go_dir=last_dir;
  else
   last_dir=go_dir;
*/
  if (go_dir==-1)
  {
   go_dir=0;
   last_dir=0;
   if (target_speed!=0)
   {
    target_speed=0;
    old_batt=battery()-10;
   }
//   target.x=-1;
  }

  if (speed()+target_speed==0)
  {
   int newbatt=battery();
   debug_flag(0,1);

   if (newbatt-old_batt<10)
   {
    debug_flag(1,1);
    target.x=-1;
   }
   else
   {
    debug_flag(1,0);
    old_batt=newbatt;
   }
  }
  else
   debug_flag(0,0);

  while ((map[y_square+angle[go_dir].y][x_square+angle[go_dir].x]==1)||
	 (y_square+angle[go_dir].y<0)||(y_square+angle[go_dir].y>=100)||
	 (x_square+angle[go_dir].x<0)||(x_square+angle[go_dir].x>=100))
  {
   go_dir=(angle[go_dir].next);
  }

  movement(target_speed,angle[go_dir].angle);

 }

}

