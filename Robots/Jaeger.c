
/**********************************************************************/
/*                               Jäger                                */
/*====================================================================*/
/*           Roboter für AmigaRobots mit Targettracking...            */
/*                 von Hanno Behrens, 8. Januar 1993                  */
/**********************************************************************/
#ifndef __SASC
char *_ProgramName="Jäger.rob";
#endif

#include "PCRobots.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef max
#undef min
#undef abs

/**********************************************************************/
/*                             Konstanten                             */
/**********************************************************************/

/* Konfiguration */
#define CODE 123

#define SPEED 0
#define MAN	2
#define RANGE 4
#define ARMOUR 4
#define ACC 0
#define INV 1

char image[5] = {0x04,0x15,0x1f,0x1f,0x11};

/**********************************************************************/
/*                             Strukturen                             */
/**********************************************************************/

struct Target
{
	int id;
	int direction,range,resolution;
	int xpos,ypos,time;
};

struct FriendlyRobot
{
	int id,x,y;
};

struct RobotTable
{
	int myid;
	int friendly;
	int otherrobots;
	struct FriendlyRobot table[0];	/* So viele wie Roboter da sind */
};

/**********************************************************************/
/*                              Routinen                              */
/**********************************************************************/

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

/* abs gibt absolutwert zurueck */
int abs(int x)
{
   if(x < 0)
      return -x;
   else
      return x;
}

/* towinkel projiziert eine Zahl in den Bereich 0-359 */
int towinkel(int winkel)
{
   if(winkel < 0)
      return 360+winkel%360;
   else
      return winkel%360;
}

/* distanz errechnet Abstand zweier Punkte */

int distanz(int dx,int dy)
{
   return(l_sqrt(dx*dx+dy*dy));
}

/* Rechnet xy-Differenz in Winkel um */
int winkel(int dx, int dy)
{
	return towinkel(180+l_atan2(dy,dx));
}

int isfriendly(int id, struct RobotTable *table)
{
	int i;

	for(i=0;i<table->friendly;i++)
	{
		if(id == table->table[i].id) return 1;
	}
	return 0;
}

/*
	Scantarget sucht "Rekursiv" nach einem Ziel
	wenn eins gefunden wurde, ist das Ergebnis gleich 1,
	sonst 0. Die Daten werden in die Targetstruktur
	eingetragen.
	Jeder Aufruf kostet nur einen Tick.
*/
int scantarget(struct Target *target, struct RobotTable *table)
{
	int range,id,res2,x,y;

	if(target->resolution > 1)
		res2 = target->resolution/2 + target->resolution%2;
	else
		res2 = 0;

	id = scan(target->direction,target->resolution,&range);

	/* Erster Durchlauf */
	if(target->resolution >= 45)
	{
		target->id = id;	/* Nur auf den Nächsten reagieren */
		if(id < 0 || isfriendly(id,table)) /* Hier ist keiner */
		{
			target->direction = towinkel(target->direction+10);
			target->id = -1;
			return 0;
		}
	}

	if(id == target->id)
	{
		target->direction = towinkel(target->direction - res2);
		target->resolution = res2;
		target->range = range;
	}
	else
	{
		target->direction = towinkel(
			target->direction + target->resolution + res2);
		target->resolution = res2;
	}

	if(target->resolution == 0 ||
		l_tan(target->resolution)*target->range < 100000
		)	/* Auf ein Pixel genau */
	{
		target->time = ticks();
		getxy(&x,&y);
		target->xpos = x + l_cos(target->direction)*target->range/100000;
		target->ypos = y + l_sin(target->direction)*target->range/100000;
		return 1;
	}
	else
	{
		return 0;
	}
}/* scantarget */

/*
	shootxy
	shießt auf den Punkt x,y
*/

void shootxy(int x, int y)
{
	int mx,my,
		dx,dy,
		angle,range;

	getxy(&mx,&my);

	dx = mx - x;
	dy = my - y;

	range = distanz(dx,dy);
	angle = towinkel(180 + l_atan2(dy,dx));

	shoot(angle,range);
}

/*
	shoottarget
	Schießt auf ein Target, aber auf die Stelle, an der sich das
	Target befinden wird, wenn die Kugel eintrifft.
*/

void shoottarget(struct Target *ct, struct Target *lt)
{
	int 	dx,dy,dt,	/* Deltawerte */
			nx,ny,ft,	/* geschätzte Werte */
			mx,my;		/* eigene Position */

	if(ct->id == lt->id && ct->id >= 0)
	{
		dx = ct->xpos - lt->xpos;
		dy = ct->ypos - lt->ypos;
		dt = ct->time - lt->time;

		/* Maximale Geschwindigkeit begrenzen */
		if(abs(dx*100/dt) > 100) dx = (dx>0)?dt:-dt;
		if(abs(dy*100/dt) > 100) dy = (dy>0)?dt:-dt; 

		/* die genaue Formel ist ein echter Akt, sie lautet
			x=dx*Ft/dt und y=dy*Ft/dt, mit Ft gleich Flugzeit und
			Fd = Flugdistanz = Ft*2 = 
				sqrt( (x0+x)^2 + (y0+y)^2 ) mit x0,y0 Abschußkoordinaten
			jetzt muß x und y eingesetzt werden und die quadratische
			Gleichung nach Ft aufgelöst werden. Viel Spaß!
			Ob die Zwishenwerte überhaupt noch mit longs darstellbar
			sind, ist fraglich.
			Hier begnüge ich mich mit einer Näherung.
		*/

		getxy(&mx,&my);


		/*	  jetzt      vorl. Flugzeit     Zeitintervall */
		nx = ct->xpos + dx*(ct->range/2) / dt;
		ny = ct->ypos + dy*(ct->range/2) / dt;

		/* Genäherte Flugzeit */
		ft = distanz(mx-nx,my-ny)/2;

		/* bessere Iteration des ersten Wertes */
		nx = ct->xpos + dx*ft / dt;
		ny = ct->ypos + dy*ft / dt;

		shootxy(nx,ny);
	}
	else	/* keine zwei gültigen Werte */
	if(ct->id >= 0)
	{
		shootxy(ct->xpos,ct->ypos);
	}
	else
	if(lt->id >= 0)
	{
		shootxy(lt->xpos,lt->ypos);
	}
}

int transmitid(void)
{
   int i,x,y,data;

	getxy(&x,&y);
	data = (1000*x+y)+CODE*1000000;

	for(i=0;transmit(i,data);i++);

	return i;
}

void getfriends(struct RobotTable *robots)
{
	int id,data,x,y,mycode;

	getxy(&x,&y);
	mycode = 1000*x+y+CODE*1000000;

   while(receive(&id,&data))
   {
   	if(data/1000000 == CODE)
   	{
			if(data != mycode)
			{
	         robots->table[robots->friendly].id = id;
	         robots->table[robots->friendly].y  = data%1000;
	         robots->table[robots->friendly].x = (data%1000000)/1000;
	         robots->friendly++;
         }
         else /* eigene Message */
         {
				robots->myid = id;
         }
   	}/* wenn CODE richtig */
	}/* Bis alle Messages gelesen wurden */
}

void main(void)
{
	struct Target ctarget,ltarget;
	int i,now,otherrobots;
	struct RobotTable *table;

	configure(SPEED,MAN,RANGE,ARMOUR,ACC,INV);
	set_pattern(image);

	/* Logon */
	otherrobots = transmitid();
	table = malloc(
				sizeof(struct RobotTable) +
				sizeof(struct FriendlyRobot) * otherrobots);

	table->friendly = 0;
	table->otherrobots = otherrobots;
	swaptask();	/* Eine Runde aussetzen */
	getfriends(table);

	ctarget.direction = 0;
	ctarget.id			= -1;
	ltarget.id = -2;

	now = -50;
	while(1)
	{
		memcpy(&ltarget,&ctarget,sizeof(struct Target));

		ctarget.resolution = 45;
		while(!scantarget(&ctarget,table));

		for(i=ticks()-now;i<49;i++) swaptask(); /* Warten */

		shoottarget(&ctarget,&ltarget);

		i = max( 0, min( (battery()-0)/50, 200-damage() ) );
		buy_armour(i);

		now = ticks();
	}
}
