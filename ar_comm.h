/**********************************************************************/
/*                            AmigaRobots                             */
/*                             ar_comm.h                              */
/*             Globale Deklarationen um die Kommunikation             */
/*           zwischen Roboter und Simulator zu erm�glichen            */
/* 29.7.93 Version 1.1  HB Protoswitch f�r SAS                        */
/**********************************************************************/
#include <exec/types.h>
#include <exec/exec.h>
#ifdef __SASC
#include <proto/all.h>
#else
#include <clib/exec_protos.h>
#endif

#define PORTNAME "Robotsim.entry"

struct RoboMsg
{
	struct Message msg;
	long	cmd,	/* Kommando */
			ret,		/* Return Val */
			arg1,		/* Argumente */
			arg2,
			arg3,
			arg4,
			arg5,
			arg6,
			arg7;
};

#define AR_CMD_SWAPTASK 0
#define AR_CMD_MOVEMENT 1
#define AR_CMD_SCAN		2
#define AR_CMD_SHOOT		3
#define AR_CMD_GETXY		16
#define AR_CMD_TRANSMIT	17
#define AR_CMD_RECEIVE	18
#define AR_CMD_DAMAGE	19
#define AR_CMD_SPEED		20
#define AR_CMD_BATTERY	21
#define AR_CMD_TICKS		22
#define AR_CMD_SIN 		23
#define AR_CMD_COS		24
#define AR_CMD_TAN		25
#define AR_CMD_ATAN		26
#define AR_CMD_SQRT		27
#define AR_CMD_SETPAT	28
#define AR_CMD_DEBUG		29
#define AR_CMD_BARMOUR	30
#define AR_CMD_BSHELLS	31
#define AR_CMD_SHELLS	32
#define AR_CMD_MAP		33
#define AR_CMD_SINV		34
#define AR_CMD_SHELLSTAT	35
#define AR_CMD_GINV		36
#define AR_CMD_ATAN2		37
#define AR_CMD_FLYING_SHELLS 38
#define AR_CMD_BIG_MAP 39
#define AR_CMD_HIT_FROM 40
#define AR_CMD_VERSION  41
#define AR_CMD_ASIN     42
#define AR_CMD_ACOS     43
#define AR_CMD_HIGHESTPLAYERID 44
#define AR_CMD_MYID     45
#define AR_CMD_CONFIGURE	128
#define AR_CMD_KILLTASK	129
