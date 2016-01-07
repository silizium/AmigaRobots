/**********************************************************************/
/*                          Die Dicke Berta                           */
/*                                                                    */
/*  Frei nachempfunden nach deutschen Geschütz im ersten Weltkrieg.   */
/*====================================================================*/
/* Der Roboter hat ein Targettracking, eine intelligente Scanroutine  */
/*             und er schießt nur auf feindliche Roboter.             */
/*                 von Hanno Behrens, 11. Januar 1993                 */
/**********************************************************************/
#ifndef __SASC
char *_ProgramName="DickeBerta.rob";
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
#define REQUESTTARGET -1

#define DB_NOTARGET 0
#define DB_SEND 1

#define SPEED 0
#define MAN	2
#define RANGE 4
#define ARMOUR 4
#define ACC 0
#define INV 1

char image[8][5] = {
{0x00,0x0c,0x0f,0x0c,0x00},	/* rechts */
{0x04,0x0e,0x1c,0x0a,0x01},	/* im Uhrzeigersinn */
{0x00,0x0e,0x0e,0x04,0x04},
{0x04,0x0e,0x07,0x0a,0x10},
{0x00,0x06,0x1e,0x06,0x00},
{0x10,0x0a,0x07,0x0e,0x04},
{0x04,0x04,0x0e,0x0e,0x00},
{0x01,0x0a,0x1c,0x0e,0x04}
};
/**********************************************************************/
/*                             Strukturen                             */
/**********************************************************************/

struct Target
{
	int id;						/* zu suchendes Ziel */
	int dir,range;				/* Richtung und Entfernung */
	int lastvalidcall;		/* Letzte valide Ortung bei Aufruf */
	int lastvalidtime;		/* Letzter tatsächlich gesannter Zeitpunkt */
	int xpos,ypos,time;		/* Geschätzte x,y-Koordinate */
	int callnumber;			/* Zustand des Automaten */
	int neardir;				/* Richtung des nächsten Feindes bei Prescan */
	int nextfriend,rangefriend;	/* Nächster Freund von Prescan */
	int lastresult;			/* Ortung bei letztem Aufruf ? */
	int lastrequest;			/* wann wurde der letzte Request gesendet? */
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
	requesttarget
	weil der Roboter selbst abgeschottet ist, verlangt er von
	seinem nächsten Nachbarn Zielkoordinaten
*/
int requesttarget(struct Target *target)
{
	int  now;

	now=ticks();

	if(now - target->lastrequest >= 50)
	{
		if(transmit(target->nextfriend,REQUESTTARGET))
		{
			target->lastrequest = now;
			return 1;
		}
	}
	return 0;
}

/* 
	handlemsg
	beantwortet Messages
*/
void handlemsg(struct RobotTable *table, struct Target *target)
{
	int id,data;
	int now;

	debug_flag(DB_SEND,0);
	while(receive(&id,&data))
	{
		/* Nur freundlich gesinnten Robotern antworten */
		if(isfriendly(id,table)) 
		{
			switch(data)
			{
			case REQUESTTARGET:
				/* nur wenn er selbst ein valides Target hat */
				/* und  dieses nicht vom Requester kommt */
				if(target->id >= 0 && target->id != id)
				{
					data = target->xpos+target->ypos*1000+
						(target->time%1000)*1000000;
					transmit(id,data);
					debug_flag(DB_SEND,1);
				}
				break;
			default:
				/* wenn eigenes Target invalid, Request übernehmen */
				if(target->id < 0)
				{
					now = ticks();
					target->xpos = data%1000;
					target->ypos = (data/1000)%1000;
					target->time = (data/1000000)+(now/1000)*1000;
					if(now < target->time)	/* Überlauf? */
						target->time -= 1000;
					target->id = id;
					/* die einzige Möglichkeit, daß ein Target friendly
						wird, ist also nach einem Request */
				}
				break;
			}/* switch data */
		}
	}
}


/*
	Scantarget sucht "Rekursiv" nach einem Ziel
	wenn eins gefunden wurde, ist das Ergebnis gleich 1,
	sonst 0. Die Daten werden in die Targetstruktur
	eingetragen.
	Jeder Aufruf kostet nur einen Tick.
	Nach 9 Scans ist das Target gefunden.
*/
int scantarget(struct Target *target, struct RobotTable *table)
{
	int range,id,x,y;

	/* Automat */
	switch(target->callnumber)
	{
	default:	/* Reset bei erstem Durchlauf */
		target->id = -1;
		target->range = 2000;
		target->xpos = -1;
		target->ypos = -1;
		target->time = 0;
		target->neardir = target->dir;
		target->nextfriend = -1;
		target->rangefriend = 2000;
		target->lastvalidcall = 0;
		target->callnumber = 1;

	case 1:	/* Vorscannen 360° (4*90°) */
	case 2:
	case 3:
	case 4:
		id = scan(
			target->dir,
			45,
			&range);

		/* Nächster Feind */
		if(id >= 0 && range < target->range && !isfriendly(id,table))
		{
			target->range = range;
			target->id = id;
			target->neardir = target->dir;
			target->lastvalidcall = target->callnumber;
			target->lastvalidtime = ticks();
		}
		/* Nächster Freund */
		if(id >= 0 && range < target->rangefriend && isfriendly(id,table))
		{
			target->nextfriend = id;
			target->rangefriend = range;
		}

		target->dir = towinkel(target->dir + 90);

		break;

	case 5:	/* 30°-Bereich (1) */
		/* Vorscan auswerten */
		/* Feind gefunden */
		if(target->id >= 0)
		{
			target->dir = target->neardir;
		}
		else	/* kein Feind */
		{
			target->dir = towinkel(target->neardir + 10);
			if(target->nextfriend >= 0)
			{
				if(requesttarget(target))
				{
					debug_flag(DB_NOTARGET,1);
				}
				target->callnumber = 0;
				return 1;	/* Terminiert */
			}
		}

		id = scan(
			target->dir-15,
			30,
			&range);
			
		if(id == target->id)
		{
			target->lastresult = 1;
			target->range = range;
			target->lastvalidtime = ticks();
		}
		else
		{
			target->lastresult = 0;
		}
		break;

	case 6:	/* 30°-Bereich (2) */
		id = scan(
			target->dir+15,
			30,
			&range);
			
		/* vier Fälle unterscheiden */
		if(id == target->id)
		{
			target->range = range;
			target->lastvalidcall = target->callnumber;
			target->lastvalidtime = ticks();

			if(target->lastresult)	/* Beide */
			{
				/* Richtung bleibt */
			}
			else	/* nur dieser */
			{
				target->dir = towinkel(target->dir + 30);
			}
		}
		else
		{
			if(target->lastresult) /* nur letzter */	
			{
				target->dir = towinkel(target->dir - 30);
				target->lastvalidcall = target->callnumber;
			}
			else	/* Target verloren */
			{
				target->callnumber = 19;
			}
		}
		break;
	
	case 7:	/* 10°-Bereich (1) */
		id = scan(
			target->dir-5,
			10,
			&range);
			
		if(id == target->id)
		{
			target->lastresult = 1;
			target->range = range;
			target->lastvalidtime = ticks();
		}
		else
		{
			target->lastresult = 0;
		}
		break;

	case 8:	/* 10°-Bereich (2) */
		id = scan(
			target->dir+5,
			10,
			&range);
			
		/* vier Fälle unterscheiden */
		if(id == target->id)
		{
			target->range = range;
			target->lastvalidcall = target->callnumber;
			target->lastvalidtime = ticks();

			if(target->lastresult)	/* Beide */
			{
				/* Richtung bleibt */
			}
			else	/* nur dieser */
			{
				target->dir = towinkel(target->dir + 10);
			}
		}
		else
		{
			if(target->lastresult)	/* nur letzter */
			{
				target->dir = towinkel(target->dir - 10);
				target->lastvalidcall = target->callnumber;
			}
			else	/* Target verloren */
			{
				target->callnumber = 19;
			}
		}
		break;
	
	case 9:	/* 4°-Bereich (1) */
		id = scan(
			target->dir-3,
			4,
			&range);
			
		if(id == target->id)
		{
			target->lastresult = 1;
			target->range = range;
			target->lastvalidtime = ticks();
		}
		else
		{
			target->lastresult = 0;
		}
		break;

	case 10:	/* 4°-Bereich (2) */
		id = scan(
			target->dir+3,
			4,
			&range);
			
		/* vier Fälle unterscheiden */
		if(id == target->id)
		{
			target->range = range;
			target->lastvalidcall = target->callnumber;
			target->lastvalidtime = ticks();

			if(target->lastresult)	/* Beide */
			{
				/* Richtung bleibt */
			}
			else	/* nur dieser */
			{
				target->dir = towinkel(target->dir + 4);
			}
		}
		else
		{
			if(target->lastresult)	/* nur letzter */
			{
				target->dir = towinkel(target->dir - 4);
				target->lastvalidcall = target->callnumber;
			}
			else	/* Target verloren */
			{
				target->callnumber = 19;
			}
		}
		break;
	
	case 11:	/* 2°-Bereich (1) */
		id = scan(
			target->dir-1,
			1,
			&range);
			
		if(id == target->id)
		{
			target->lastresult = 1;
			target->range = range;
			target->lastvalidtime = ticks();
		}
		else
		{
			target->lastresult = 0;
		}
		break;

	case 12:	/* 2°-Bereich (2) */
		id = scan(
			target->dir+1,
			1,
			&range);
			
		/* vier Fälle unterscheiden */
		if(id == target->id)
		{
			target->range = range;
			target->lastvalidcall = target->callnumber;
			target->lastvalidtime = ticks();

			if(target->lastresult)	/* Beide */
			{
				/* Richtung bleibt */
			}
			else	/* nur dieser */
			{
				target->dir = towinkel(target->dir + 1);
			}
		}
		else
		{
			if(target->lastresult)	/* nur letzter */
			{
				target->dir = towinkel(target->dir - 1);
				target->lastvalidcall = target->callnumber;
			}
			else	/* Target verloren */
			{
				target->callnumber = 19;
			}
		}
		break;
	
	case 13:	/* 1°-Bereich (1) */
		id = scan(
			target->dir-1,
			1,
			&range);
			
		if(id == target->id)
		{
			target->lastresult = 1;
			target->range = range;
			target->lastvalidtime = ticks();
		}
		else
		{
			target->lastresult = 0;
		}
		break;

	case 14:	/* 1°-Bereich (2) */
		id = scan(
			target->dir+1,
			1,
			&range);
			
		/* vier Fälle unterscheiden */
		if(id == target->id)
		{
			target->range = range;
			target->lastvalidcall = target->callnumber;
			target->lastvalidtime = ticks();

			if(target->lastresult)	/* Beide */
			{
				/* Richtung bleibt */
			}
			else	/* nur dieser */
			{
				target->dir = towinkel(target->dir + 1);
			}
		}
		else
		{
			if(target->lastresult)	/* nur letzter */
			{
				target->dir = towinkel(target->dir - 1);
				target->lastvalidcall = target->callnumber;
			}
			else	/* Target verloren */
			{
				target->callnumber = 20;
			}
		}

	case 20:	/* Koordinaten berechnen */
		target->time = ticks();
		getxy(&x,&y);

		target->xpos = x + l_cos(target->dir)*target->range/100000;
		target->ypos = y + l_sin(target->dir)*target->range/100000;

		debug_flag(DB_NOTARGET,0);
	
		target->callnumber++;
		return 1; /* Terminiert */
	}
	target->callnumber++;
	return 0;	/* Automat terminiert noch nicht */
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
	static int lastdir; /* für Patternsetzen */

	getxy(&mx,&my);

	dx = mx - x;
	dy = my - y;

	range = distanz(dx,dy);
	angle = towinkel(180 + l_atan2(dy,dx));

	if(towinkel(angle+23)/45 != lastdir)
	{
		lastdir = towinkel(angle+23)/45;
		set_pattern(image[lastdir]);
	}
	if(!shells_left()) buy_shells(1);
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
			mx,my,now,	/* eigene Position */
			oldx, oldy, i;

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
			sind, ist fraglich, ebensobezweifle ich, daß das oben-
			genannte Verfahren überhaupt zu einer Lösung führt.
			Hier begnüge ich mich mit einer Näherung.
		*/

		getxy(&mx,&my);
		now = ticks();

		/*	  jetzt      vorl. Flugzeit                    Zeitintervall */
		nx = ct->xpos + dx*((ct->range/2)+now-ct->time) / dt,
		ny = ct->ypos + dy*((ct->range/2)+now-ct->time) / dt;

		i=0;
		do
		{
			/* Genäherte Flugzeit */
			ft = distanz(mx-nx,my-ny)/2;
			oldx = nx;
			oldy = ny;

			/* bessere Iteration des ersten Wertes */
			nx = ct->xpos + dx*(ft+now-ct->time) / dt;
			ny = ct->ypos + dy*(ft+now-ct->time) / dt;
			
			i++;
		}
		while((nx != oldx && ny != oldy) && i<10);

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
	data = (1000*y+x)+CODE*1000000;

	for(i=0;transmit(i,data);i++);

	return i;
}

void getfriends(struct RobotTable *robots)
{
	int id,data,x,y,mycode;

	getxy(&x,&y);
	mycode = 1000*y+x+CODE*1000000;

   while(receive(&id,&data))
   {
   	if(data/1000000 == CODE)
   	{
			if(data != mycode)
			{
	         robots->table[robots->friendly].id = id;
	         robots->table[robots->friendly].x  = data%1000;
	         robots->table[robots->friendly].y = (data%1000000)/1000;
	         robots->friendly++;
         }
         else /* eigene Message */
         {
				robots->myid = id;
         }
   	}/* wenn CODE richtig */
	}/* Bis alle Messages gelesen wurden */
}

void preparescantarget(struct Target *ct)
{
	int now;

	now = ticks();
	/* wenn zu lange nicht mehr tatsächlich gesehen, reset */
	if(ct->id < 0)	
	{
		ct->callnumber = -1;
		ct->id = -1;
	}
	else
	if(now > ct->lastvalidtime + 112)
	{
		ct->callnumber = -1;
	}
	else
	if(now > ct->lastvalidtime + 55)
	{
		ct->callnumber = ct->lastvalidcall-3;
	}
	else
	{
		ct->callnumber = min(ct->lastvalidcall-1,9);
	}
}

void main(void)
{
	struct Target ctarget,ltarget;
	int i,now,otherrobots,lastdamage;
	struct RobotTable *table;

	configure(SPEED,MAN,RANGE,ARMOUR,ACC,INV);
	set_pattern(image[0]);

	/* Logon */
	otherrobots = transmitid();
	table = malloc(
				sizeof(struct RobotTable) +
				sizeof(struct FriendlyRobot) * otherrobots);

	table->friendly = 0;
	table->otherrobots = otherrobots;
	swaptask();	/* Eine Runde aussetzen */
	getfriends(table);

	now = -170;
	ctarget.dir = 45;
	ctarget.id	= -1;
	ctarget.lastrequest = now;
	memcpy(&ltarget,&ctarget,sizeof(struct Target));

	while(1)
	{
		lastdamage = damage();

		preparescantarget(&ctarget);
		while(!scantarget(&ctarget,table));
		handlemsg(table,&ctarget);

		for(i=ticks()-now;i<49 && (ctarget.id >=0 || ltarget.id >= 0);i++) 
		{
			invisibility(1);
			swaptask(); /* Warten */
		}
		invisibility(0);

		shoottarget(&ctarget,&ltarget);

		if(ctarget.lastvalidcall >= ltarget.lastvalidcall || 
			ctarget.id != ltarget.id)
		{
			memcpy(&ltarget,&ctarget,sizeof(struct Target));
		}

		i = max( 0, min( (battery()-0)/50, 200-damage() ) );
		buy_armour(i);

		now = ticks();
	}
}
