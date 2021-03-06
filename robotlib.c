/**********************************************************************/
/*                              RoboLib                               */
/*            Linkmodul zur Kommunikation mit AmigaRobots             */
/*                        � 1992 von H.Behrens                        */
/*                                                                    */
/* 29.7.93 HB Version 1.1, including dosextens                        */
/**********************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <dos/dosextens.h>
#include "ar_comm.h"
#include "pcrobots.h"

/**********************************************************************/
/*                         Globale Variablen                          */
/**********************************************************************/
extern char *_ProgramName;

struct RoboMsg msg;
struct MsgPort *port=NULL;
struct MsgPort *rport=NULL;
struct Process *robotask;

/**********************************************************************/
/*                              Routinen                              */
/**********************************************************************/

void cleanup(char *text,int err)
{
#ifdef DEBUG
	puts(text);
#endif
	exit(err);
}

void docmd(void)
{
	
	msg.msg.mn_Node.ln_Type = NT_MESSAGE;
	msg.ret = 0;
	PutMsg(port, &msg.msg);
	WaitPort(rport);
	GetMsg(rport);
	if(msg.cmd == AR_CMD_KILLTASK)
		cleanup("Programmende",0);
}

/**********************************************************************/
/*             Login im Simulator, zuweisen des Gameports             */
/**********************************************************************/

int 	configure(int speed, int manouevre,int range,
		  int armour,int acceleration,
		  int invisibility)
{
	if(port) return 0; /* schon configuriert */

	port = FindPort(PORTNAME);

	if(!(port)) /* finde erst einmal den Rendezvous-Port */
		cleanup("Rendezvousport nicht gefunden",20);

	robotask = (struct Process *)FindTask(0L);

	/* RoboMsg init */
	rport = &robotask->pr_MsgPort;
	msg.msg.mn_ReplyPort = rport;
	msg.msg.mn_Length = sizeof(struct RoboMsg);
	
	msg.cmd = AR_CMD_CONFIGURE;
	msg.ret = NULL;
	msg.arg1 = speed; 
	msg.arg2 = manouevre;
	msg.arg3 = range;
	msg.arg4 = armour;
	msg.arg5 = acceleration;
	msg.arg6 = invisibility;
	msg.arg7 = (long)_ProgramName;
	
	docmd();
	
	if(!(port = (struct MsgPort *)msg.ret)) /* und ab nun Spieler */
	{	/* Fehler, aus irgendeinem Grunde nicht angenommen */
		cleanup("Nicht vom Simulator angenommen",20);
	}
	return 1;
}/* Ende Config */

/**********************************************************************/
/*                     Und die �brigen Befehle...                     */
/**********************************************************************/

void swaptask(void)
{
	msg.cmd = AR_CMD_SWAPTASK;
	docmd();
}

void movement(int speed, int angle)
{
	msg.cmd = AR_CMD_MOVEMENT;
	msg.arg1 = speed;
	msg.arg2 = angle;
	docmd();
}

int scan(int angle,int res,int *range)
{
	msg.cmd = AR_CMD_SCAN;
	msg.arg1 = angle;
	msg.arg2 = res;
	docmd();
	*range = msg.arg3;
	return msg.ret;
}

int shoot(int angle,int range)
{
	msg.cmd = AR_CMD_SHOOT; 
	msg.arg1 = angle;
	msg.arg2 = range;
	docmd();
	return msg.ret;
}

void getxy(int *x,int *y)
{
	msg.cmd = AR_CMD_GETXY;
	docmd();
	*x = msg.arg1;
	*y = msg.arg2;
}

int transmit(int target,int data)
{
	msg.cmd = AR_CMD_TRANSMIT;
	msg.arg1 = target;
	msg.arg2 = data;
	docmd();
	return msg.ret;
}

int receive(int *source,int *data)
{
	msg.cmd = AR_CMD_RECEIVE;
	docmd();
	*source = msg.arg1;
	*data = msg.arg2;
	return msg.ret;
}

int damage(void)
{
	msg.cmd = AR_CMD_DAMAGE;
	docmd();
	return msg.ret;
}

int speed(void)
{
	msg.cmd = AR_CMD_SPEED;
	docmd();
	return msg.ret;
}

int battery(void)
{
	msg.cmd = AR_CMD_BATTERY;
	docmd();
	return msg.ret;
}

long	ticks(void)
{
	msg.cmd = AR_CMD_TICKS;
	docmd();
	return msg.ret;
}

long	l_sin(int angle)
{
	msg.cmd = AR_CMD_SIN;
	msg.arg1 = angle;
	docmd();
	return msg.ret;
}

long	l_asin(int ratio)
{
	msg.cmd = AR_CMD_ASIN;
	msg.arg1 = ratio;
	docmd();
	return msg.ret;
}

long	l_cos(int angle)
{
	msg.cmd = AR_CMD_COS;
	msg.arg1 = angle;
	docmd();
	return msg.ret;
}

long	l_acos(int ratio)
{
	msg.cmd = AR_CMD_ACOS;
	msg.arg1 = ratio;
	docmd();
	return msg.ret;
}

long	l_tan(int angle)
{
	msg.cmd = AR_CMD_TAN;
	msg.arg1 = angle;
	docmd();
	return msg.ret;
}

int 	l_atan(long ratio)
{
	msg.cmd = AR_CMD_ATAN;
	msg.arg1 = ratio;
	docmd();
	return msg.ret;
}

long	l_sqrt(long square)
{
	msg.cmd = AR_CMD_SQRT;
	msg.arg1 = square;
	docmd();
	return msg.ret;
}

void set_pattern(const char *pattern)
{
	msg.cmd = AR_CMD_SETPAT;
	msg.arg1 = (long)pattern;
	docmd();
}

void	debug_flag(int flag_no,int flag_set)
{
	msg.cmd = AR_CMD_DEBUG;
	msg.arg1 = flag_no;
	msg.arg2 = flag_set;
	docmd();
}

void	buy_armour(int units)
{
	msg.cmd = AR_CMD_BARMOUR;
	msg.arg1 = units;
	docmd();
}

void	buy_shells(unsigned int units)
{
	msg.cmd = AR_CMD_BSHELLS;
	msg.arg1 = units;
	docmd();
}

int shells_left(void)
{
	msg.cmd = AR_CMD_SHELLS;
	docmd();
	return msg.ret;
}

void get_local_map(char *buffer)
{
	msg.cmd = AR_CMD_MAP;
	msg.arg1 = (long)buffer;
	docmd();
}

void	invisibility(int invis_flag)
{
	msg.cmd = AR_CMD_SINV;
	msg.arg1 = invis_flag;
	docmd();
}

int get_shell_status(void)
{
	msg.cmd = AR_CMD_SHELLSTAT;
	docmd();
	return msg.ret;
}

int is_invisible(void)
{
	msg.cmd = AR_CMD_GINV;
	docmd();
	return msg.ret;
}

int l_atan2(int y,int x)
{
	msg.cmd = AR_CMD_ATAN2;
	msg.arg1 = y;
	msg.arg2 = x;
	docmd();
	return msg.ret;
}

int flying_shells(void)
{
	msg.cmd = AR_CMD_FLYING_SHELLS;
	docmd();
	return msg.ret;
}

void get_big_map(char *buffer)
{
	msg.cmd = AR_CMD_BIG_MAP;
	msg.arg1 = (long)buffer;
	docmd();
}

int hit_from(void)
{
	msg.cmd = AR_CMD_HIT_FROM;
	docmd();
	return msg.ret;
}

int highest_player_id(void)
{
	msg.cmd = AR_CMD_HIGHESTPLAYERID;
	docmd();
	return msg.ret;
}

int myid(void)
{
	msg.cmd = AR_CMD_MYID;
	docmd();
	return msg.ret;
}

int version(void)
{
	msg.cmd = AR_CMD_VERSION;
	docmd();
	return msg.ret;
}
