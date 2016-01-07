/**********************************************************************/
/*                            ARStatistic                             */
/*          Statistische Auswertung der AmigaRobots-logfiles          */
/**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VERSION "1.0�"
#define MAXROBOTS 100
#define FCHARS 30

char buffer[80];
char arena[FCHARS];

struct Roboter
{
	char	name[FCHARS];
	int 	kills,
			killed,
			survived,
			ticks,
			armour,
			startarmour,
			mun,
			startmun;
}liste[MAXROBOTS];
short listsize = 0;


/* Stringcompare upper */
int strccmp(unsigned char *s1, unsigned char *s2)
{
	int res;

	while(*s1 || *s2)
	{
		if((res = toupper(*s1) - toupper(*s2)))
			return res;
		s1++;
		s2++;
	}
	return 0;
}

int findrobotname(char *name)
{
	int i;

	for(i=0;i<listsize;i++)
	{
		if(0 == strccmp(name,liste[i].name))
			return i;
	}
	/* neuer Roboter */
	strcpy(liste[i].name,name);
	liste[i].kills = 0;
	liste[i].killed = 0;
	liste[i].survived = 0;
	liste[i].ticks = 0;
	liste[i].armour = 0;
	liste[i].startarmour = 0;
	liste[i].mun = 0;
	liste[i].startmun = 0;
	listsize++;
	return i;
}

void daten_lesen(void)
{
	int i;
	short vermerk[MAXROBOTS];
	short killedby[MAXROBOTS];
	int id,
		armour,startarmour,
		ticks,hitbyindex,
		mun,startmun,zeichen;
	char alive,selfdestruct,
		name[FCHARS];
	
	while(!feof(stdin))
	{
		for(i=0;i<MAXROBOTS;i++)
		{
			vermerk[i] = -1;
			killedby[i] = 0;
		}

		gets(buffer); /* AmigaRobots */
		if(feof(stdin)) break;
		gets(buffer); /* Arena */
		strcpy(arena,buffer+7);
		gets(buffer); /* Roboterzahl */
		sscanf(buffer,"Roboteranzahl: %ld",&i);
		gets(buffer); /* Runden ... */
		gets(buffer); /* Werte ... */
		gets(buffer); /* ID Panzerung ... */

		while(i-- > 0)
		{
			gets(buffer); /* Roboterzeile */
			sscanf(buffer,"%ld%c %ld/%ld %ld %ld%c %ld/%ld %n",
				&id,
				&alive,
				&armour,
				&startarmour,
				&ticks,
				&hitbyindex,
				&selfdestruct,
				&mun,
				&startmun,
				&zeichen
			);
			strcpy(name,buffer+zeichen);
			vermerk[id] = findrobotname(name);
			if(alive != '+')
			{
				liste[vermerk[id]].killed++;
				killedby[id] = hitbyindex;
			}
			else
				liste[vermerk[id]].survived++;

			liste[vermerk[id]].ticks += ticks;
			liste[vermerk[id]].armour += armour;
			liste[vermerk[id]].startarmour += startarmour;
			liste[vermerk[id]].mun += mun;
			liste[vermerk[id]].startmun += startmun;
		}

		while(id-- > 0)
		{
			if(killedby[id] > 0 && vermerk[killedby[id]] >= 0)
				liste[vermerk[killedby[id]]].kills++;
		}
	}
}

void datendump(void)
{
	int i;

	printf("Name            Spiele   Z�ge             Lebend    Kills         Panzerung             Munition\n");
	for(i=0;i<listsize;i++)
	{
		printf("%-14.14s %4d %9d %7.0f� %4d %5.1f%% %4d %5.2f� %6d/%6d %5.1f%% %6d/%6d %5.1f%%\n",
			liste[i].name,
			liste[i].killed+liste[i].survived,
			liste[i].ticks,
			1.0*liste[i].ticks/(liste[i].killed+liste[i].survived),
			liste[i].survived,
			100.0*liste[i].survived/(liste[i].killed+liste[i].survived),
			liste[i].kills,
			1.0*liste[i].kills/(liste[i].killed+liste[i].survived),
			liste[i].armour,
			liste[i].startarmour,
			100.0*liste[i].armour/liste[i].startarmour,
			liste[i].mun,
			liste[i].startmun,
			100.0*liste[i].mun/liste[i].startmun
		);
	}
}

void main(void)
{
	puts("AmigaRobots Statistik V" VERSION);
	daten_lesen();
	datendump();
}

