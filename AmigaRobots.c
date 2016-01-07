/**********************************************************************/
/*                            AmigaRobots                             */
/*                        © von Hanno Behrens 1993                    */
/*                    email: H.Behrens@amtrash.comlink.de             */
/**********************************************************************/
/*#define DEBUG*/
/*#define OWNFONT*/ /* Versuch einen Customfont zuzulassen */
/* Powerwindows */
#include <exec/types.h>
#include <exec/exec.h>
#include <proto/all.h>

#include <exec/io.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
/* eigene Grafikroutinen */
#include <graphics/gfxmacros.h>
#include <libraries/reqbase.h>	/* Für den Requester */
#include <proto/req.h>
/* für WB-Startup */
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <workbench/icon.h>

#include "PCRobots.h"
#include "ar_comm.h"
#include "version.h"
char *ver = "$VER: AmigaRobots V" VERSION " " DATUM " von H. Behrens";

#define max(a,b) __builtin_max(a,b)
#define min(a,b) __builtin_min(a,b)
#define abs(a) __builtin_abs(a)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <dos.h>

void PlotShell(long x, long y);
void DisplayPattern(struct RobotPlayer *player);
void RemoveAllPlayers(void);
int  RemovePlayer(struct RobotPlayer *player);
void HandleEvent(APTR object);
void mf_quit(APTR object);
void mf_info(APTR object);
void mf_color(APTR object);
void mf_prepos(APTR object);
void mf_fastexplosion(APTR object);
void mf_singlestep(APTR object);
void mf_runsim(APTR object);
void mf_loadarena(APTR object);
void mf_runrobots(APTR object);
void mf_config(APTR object);
void mf_stoponone(APTR object);
void gf_confcancel(struct Gadget *gad);
void gf_confok(struct Gadget *gad);
void gf_nextgadget(struct Gadget *gad);
int filerequest(
	char *title, 
	char *dir,
	char *file,
	char *showpattern,
	char *hidepattern,
	int maxselect
);
void __stdargs SimpleRequest(char *title, char *str,...);
short __stdargs TwoGadRequest(char *title, char *str,...);
void DisplayArena(void);
struct RobotPlayer *GetNewRobots(void);

#include "ar_intui.h"

/**********************************************************************/
/*                 Ein paar Defines zum Überleben...                  */
/**********************************************************************/
#define MI_LOADROBOT ArenaMenuItem13
#define MI_LOADARENA ArenaMenuItem11
#define MI_RUNSIM		ArenaMenuItem4
#define MI_SINGLESTEP	ArenaMenuItem5
#define MI_STOPONONE ArenaMenuItem6
#define MI_PREPOS ArenaMenuItem7
#define MI_FASTEXP ArenaMenuItem8
#define MI_DUMMY ArenaMenuItem1

#define M_POSITION	ArenaMenu3
#define M_STATUS	ArenaMenu4
#define M_ENTFERNEN	ArenaMenu5

#define NextPlayer(player) (struct RobotPlayer *)player->Node.mln_Succ
#define LastPlayer(player) (struct RobotPlayer *)player->Node.mln_Pred
#define FirstPlayer() ((struct RobotPlayer *)gl.Players.mlh_Head)
#define ValidPlayer(player) (player->Node.mln_Succ)

/**********************************************************************/
/*                             Konstanten                             */
/**********************************************************************/
#define AREA_SIZE	200

#define MAXTICKS 0
#define NAMELEN 31
#define MAXSHELLS_IN_AIR 14
#define TICKS_TO_RELOAD 50
#define MAX_MESSAGES 20
#define DEFAULTFILE "PCROBOTS.RNA"
#define COLORFILE "AmigaRobots.colors"
#define BUFFERLEN 256
#define MAX_REFUELPOINTS 10

#define ESC 27
#define RETURN 13

#define SHELL_MISS	0
#define SHELL_WALL	1
#define SHELL_50		2
#define SHELL_25		3
#define SHELL_5		4

char def_pat[5]={
	0x0e,
	0x1f,
	0x1b,
	0x1f,
	0x0e
};

char dead_pat[5]={
	0x1b,
	0x11,
	0x1b,
	0x1b,
	0x1f
};

char *cmd_text[] =
{
	"%d void swaptask(void)",
	"%d void movement(Geschwindigkeit: %d, Richtung: %d°)",
	"%d = scan(Richtung: %d°,Auflösung: %d,Entfernung: %d)",
	"%d = shoot(Richtung: %d°,Distanz: %d)"
};

short vec_speed[5]		= {50,75,100,150,200};
short vec_manouevre[5] = {20,35,50,75,100}; /* % */
short vec_range[5] 		= {300,500,700,1000,1500};
short vec_armour[5] 	= {50,75,100,150,200};
short vec_acceleration[5] = {5,7,10,15,20};
short vec_shells[2] = {40,20}; /* AmigaRobots 2/1, PCRobots immer 9/10 */

/**********************************************************************/
/*                             Strukturen                             */
/**********************************************************************/
struct RobotPlayer
{
	struct MinNode	Node; /* erster Eintrag */
	struct MsgPort	MPort;
	char name[NAMELEN+1];

	struct Task *playertask; /* für RemTask() */
	short	id;		/* configure positiv wenn nicht begraben, sonst -id */
	short	speed,manouevre,range,armour,acceleration,invisibility;

	short lasthitby;	/* der letzte Robot, der getroffen hat */
	int	dietime;		/* Sterbezeitpunkt */
	short	debug[2];	/* Debugflags */
	short	batteries;	/* werden max auf 10000 aufgeladen */
	short	c_armour;	/* Damagepoints = Überlebenspunkte */

	/*char	pattern[5];	/* Aussehen */
	UWORD *pattern;	/* Pointer in Chipmem */

	/* Movement */
	long	x,y;	/* Position */
	short	xinc,yinc;
	short	c_speed;	/* Momentane Geschwindigkeit */
	short	d_speed;	/* erwünschte Geschwindigkeit */
	short c_angle,d_angle;

	/* Shoot */	
	short	shells;	/* Maximalanzahl */
	short	c_shells,			/* Aktuelle Anzahl Kugeln */
			lastfiredshell,	/* verstrichene Zeit seit Feuern */
			shellsinair,		/* fliegende Kugeln */
			hit_from;			/* Getroffen worden aus Richtung (siehe hit_from) */
	short	lastshellstatus;	/* siehe get_shell_status */
	struct	/* Repräsentation einer fliegenden Granate */
	{
		long xpos,ypos; /* x/y pos * 200 */
		short xinc,yinc;	/* maximal 400 */
		short times;		/* maximal 707 mal */
	}flying_shell[MAXSHELLS_IN_AIR];

	/* Transmit/Receive */
	short	mes_free,		/* Zeigt auf freien Eintrag */
			mes_numbers;	/* Unglesene Messages */
	struct
	{
		int id,value;
	} message_queue[MAX_MESSAGES];

	/* Tarnung */
	short	invisible,	/* 1 wenn unsichtbar */
			inv_count;	/*	wird erhöht, solange unsichtbar */

	/* für die zusätzlichen Menüs */
	struct IntuiText	MenText;		
	struct MenuItem	PosItem;
	struct MenuItem	StatusItem;
	struct MenuItem	EntfernenItem;

};

/**********************************************************************/
/*                         Globale Variablen                          */
/**********************************************************************/
extern struct IntuitionBase *IntuitionBase;
struct ReqLib *ReqBase = NULL;
#ifdef OWNFONT
struct Library *DiskfontBase = NULL;
#endif

long vec_sin[360];
long vec_cos[360];
long vec_tan[360];

struct
{
	struct MsgPort *LogonPort; 
	struct MinList Players;
	short	SigBit;	/* Signalbit für die Playerports */
	short	NumberPlayers;
	short	PlayersAlive;

	/* Filerequesting */
	struct Process *myproc;
	struct ReqFileRequester FileRequest;
	char	filename[FCHARS+1];
	char	dirname[DSIZE+1];
	char	pathname[FCHARS+DSIZE+2];

	char	arenafilename[FCHARS+1];
	char	arenadir[DSIZE+1];

	char	robotfilename[FCHARS+1];
	char	robotdir[DSIZE+1];

	/* Screens und Windows */
	short				lib_Version;
	ULONG				monitorID;		/* für 2.0 */
	ULONG 			Width, Height; /* Screengröße */
	struct Rectangle rect;			/* Sichtbarer Bereich */
	struct Screen *MainScreen;
	struct Window *ArenaWindow;
	struct Window *RobotWindow;
	struct Window *ConfigWindow;
	struct Window *StatusWindow;
	APTR	 oldwindow;	/* Altes Process-Window */
	
#ifdef OWNFONT
	/* für den Font */
	struct TextFont *textfont;
	struct TextAttr textattr;
#endif

	/* für die Area-Befehle */
	WORD					areabuffer[AREA_SIZE];
	struct AreaInfo	areainfo;
	struct TmpRas		tmpras;
	PLANEPTR 			planeptr;

	long maskarena,maskrobot,masklogon,maskplayer,maskconf,
			maskstatus; /* Maskenbits */

	struct /* Voreingestellte Positionen */
	{
		long x,y;
	}preposition[10];

	char arenaname[FCHARS+1];
	char arena[100][100];
	char itb1[NAMELEN+1],itb2[NAMELEN+1];	/* Buffer für IText */
	char buffer[BUFFERLEN];	/* Buffer für fgets */

	/* Steuervariablen */
	short timefactor;
	long	maxticks;		/* Danach wird Abgebrochen, wenn maxticks > 0 */
	short	running;
	short singlestep;
	short	stoponone;		/* Programm endet bei einem Überlebenden */
	short	reload;			/* um wieviel wird jede Runde die Batterie geladen */
	short delay;			/* WaitTOF's jede Runde */
	FILE	*outfile;		/* Aufgabefile statt stdio */
	short	prepos;			/* Vorgewählte Positionen benutzen */
	short	quickmode;		/* Nur einmal durchlaufen */
	short fastexplosion;
	short pcrobots;		/* PCRobots-kompatible Wände */
	short exp_darstellen;	/* Wie oft WaitTOF() */
	struct RobotPlayer *pos_id;			/* Roboter für neue Position */
	struct RobotPlayer *status_id;
	short playerentfernt;	/* ist normalerweise FALSE, wenn ein player entfernt wird, */
									/* wird in mf_entfernen TRUE gesetzt und die Runde abgebrochen */

	int st_cmd,st_arg[4];

	int version;	/* Versionsnummer von AmigaRobots (ab 1.3) */
	short	id;	/* wird hochgezählt */
	
	short refuelpoints;
	struct
	{
		short x,y,load,robotloading;
	}refuel[MAX_REFUELPOINTS];
	
	/* Swaptask */
	long ticks;
}gl;

/**********************************************************************/
/*                         Lowlevel Routinen                          */
/**********************************************************************/

void cleanup(char *text, int status)
{
	struct RoboMsg *msg;

	onexit(NULL);
	onbreak(NULL);

#ifdef DEBUG
	puts(text);
#endif
	if(gl.outfile) fclose(gl.outfile);

	if(gl.LogonPort) /* Wenn Port geöffnet wurde */
	{
		/* Noch anstehende Spieler abwimmeln */
		while(msg = (struct RoboMsg *)GetMsg(gl.LogonPort))
		{
			msg->cmd = AR_CMD_KILLTASK;
			ReplyMsg((struct Message *)msg);
		}
		DeletePort(gl.LogonPort);	/* Port löschen */
	}

	/* Noch vorhandene Spieler löschen */
	RemoveAllPlayers();

	if(gl.SigBit != -1) FreeSignal(gl.SigBit);

#ifdef OWNFONT
	if(DiskfontBase && gl.textfont) CloseFont(gl.textfont);
#endif
	if(gl.ConfigWindow) CloseWindow(gl.ConfigWindow);
	if(gl.StatusWindow) CloseWindow(gl.StatusWindow);

	if(gl.planeptr && gl.MainScreen)
		FreeRaster(gl.planeptr,gl.MainScreen->Width,gl.MainScreen->Height);

#ifdef ArenaMenuList1
	if(gl.ArenaWindow) ClearMenuStrip(gl.ArenaWindow);
#endif
	if(gl.ArenaWindow) CloseWindow(gl.ArenaWindow);
	if(gl.RobotWindow) CloseWindow(gl.RobotWindow);
#ifdef NEWSCREENSTRUCTURE
	if(gl.MainScreen) 
	{
		if(gl.lib_Version >= 37)	/* Public Screen entfernen */
		{
			struct List *PubScreenList;
			struct PubScreenNode *ScreenNode;

			PubScreenList = LockPubScreenList();
			for(ScreenNode = (struct PubScreenNode *)PubScreenList -> lh_Head ; 
				ScreenNode -> psn_Node.ln_Succ ; 
				ScreenNode = (struct PubScreenNode *)ScreenNode -> psn_Node.ln_Succ)
			{
				if(ScreenNode -> psn_Screen == gl.MainScreen)
					break;
			}

			if(ScreenNode)
			{
				while(ScreenNode -> psn_VisitorCount)
				{
					UnlockPubScreenList();
					SimpleRequest("Aufforderung",
						"Bitte alle fremden Fenster auf diesem Schirm schließen,\n"
						"da sonst der PublicScreen nicht geschlossen werden darf."
					);
					PubScreenList = LockPubScreenList();
				}
				Forbid();

				UnlockPubScreenList();

				PubScreenStatus(gl.MainScreen,PSNF_PRIVATE);

				Permit();
			}
			else
				UnlockPubScreenList();
		}
		CloseScreen(gl.MainScreen);
	}
#endif
	if (ReqBase) 
	{
		/* Reqlib sauber verlassen */
		if(gl.oldwindow)
		{
			gl.myproc->pr_WindowPtr = gl.oldwindow;
			PurgeFiles(&gl.FileRequest);
		}
		CloseLibrary(
		(struct Library *)ReqBase);
	}
#ifdef OWNFONT
	if (DiskfontBase) CloseLibrary(DiskfontBase);
#endif
	if (GfxBase) CloseLibrary(
		(struct Library *)GfxBase);
	if (IntuitionBase) CloseLibrary(
		(struct Library *)IntuitionBase);

	exit(status);
}

void exittrap(void)
{
	cleanup("Exit",0);
}

int breaktrap(void)
{
	return 0;	/* Break ignorieren und weiterlaufen */
}

/**********************************************************************/
/*                         Initialisierungen                          */
/**********************************************************************/

/*
	initmaster
	Wenn noch kein Masterport vorhanden, richtet diese 
	Routine einen ein. Wird nur einmalig aufgerufen.
*/

void init_master(void)
{
	short i;

	onbreak(breaktrap);
	onexit(exittrap);

	gl.SigBit = AllocSignal(-1); /* Signalbit für Player */
	if(gl.SigBit == -1)
		cleanup("Kein Signalbit frei.",20);

	if(FindPort(PORTNAME))
	{
		cleanup("Master bereits aktiv!", 20);
	}

	if(!(gl.LogonPort = CreatePort(PORTNAME, 0)))
	{
		cleanup("Kein LogonPort!", 20);
	}

	/* Spielerliste Initialisieren */
	NewList((struct List *)&gl.Players);

	gl.singlestep = FALSE;
	gl.maxticks = MAXTICKS;
	gl.stoponone = TRUE;
	gl.reload = 40;
	gl.ConfigWindow = NULL;
	gl.maskconf = 0;
	gl.pos_id = NULL;
	gl.StatusWindow = NULL;
	gl.maskstatus = 0;
	gl.outfile = NULL;
	gl.prepos = FALSE;
	gl.quickmode = FALSE;
	gl.fastexplosion = TRUE;
	gl.exp_darstellen = 2;
	gl.filename[0] = '\0';
	gl.pathname[0] = '\0';
	gl.arenafilename[0] = '\0';
	strcpy(gl.arenadir,"Arena");
	gl.robotfilename[0] = '\0';
	strcpy(gl.robotdir,"Robots");
	gl.pcrobots = FALSE;
	gl.timefactor = 1;
#ifdef OWNFONT
	gl.textfont = NULL;
	gl.textattr.ta_Name = "topaz\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	gl.textattr.ta_YSize = 8;
	gl.textattr.ta_Style = FS_NORMAL;
	gl.textattr.ta_Flags = FPF_ROMFONT|FPF_DISKFONT;
#endif
	gl.monitorID = PAL_MONITOR_ID|HIRESLACE_KEY;
	gl.rect.MinX = 0;	gl.rect.MinY = 0;
	gl.rect.MaxX = 640;	gl.rect.MaxY = 512;
	gl.Width = 640; gl.Height = 512;
	gl.version = 1000*atol(ver+19)+100*atol(ver + 21)+atol(ver + 23)%100;

	for(i=0;i<360;i++)
	{
		vec_sin[i] = sin(i*PI/180)*100000.0;
		vec_cos[i] = cos(i*PI/180)*100000.0;
		vec_tan[i] = tan(i*PI/180)*100000.0;
	}
}

void initreq(void)
{
	gl.oldwindow = NULL;

	ReqBase = (struct ReqLib *)
					OpenLibrary("req.library", 0);
	if (ReqBase == NULL)
	{
		cleanup("Keine req.library",20);
	}

	gl.myproc = (struct Process *)FindTask((char *)0);
	gl.oldwindow = gl.myproc->pr_WindowPtr;
	gl.myproc->pr_WindowPtr = (APTR)gl.ArenaWindow;
}

void initglobals(void)
{
	if(gl.StatusWindow != NULL)
	{
		CloseWindow(gl.StatusWindow);
		gl.maskstatus = 0;
		gl.StatusWindow = NULL;
		gl.status_id = NULL;
	}
	gl.NumberPlayers = 0;
	gl.PlayersAlive = 0;
	gl.id = 0;
	gl.ticks = 0;

	/* Maskenbits errechnen */
	gl.maskarena = 1<<gl.ArenaWindow->UserPort->mp_SigBit;
	gl.maskrobot = 1<<gl.RobotWindow->UserPort->mp_SigBit;
	gl.masklogon = 1<<gl.LogonPort->mp_SigBit;
	gl.maskplayer = 1<<gl.SigBit;
}

void initplayer(struct RobotPlayer *player)
{
	short i;

	player->speed = vec_speed[player->speed];
	player->manouevre = vec_manouevre[player->manouevre];
	player->range = vec_range[player->range];
	player->armour = vec_armour[player->armour];
	player->acceleration = vec_acceleration[player->acceleration];
	player->shells = vec_shells[player->invisibility];

	player->lasthitby = 0;

	/* default Pattern initialisieren */
	for(i=0;i<5;i++)
		player->pattern[i] = def_pat[i];

	
	player->debug[0] = 0;
	player->debug[1] = 0;
	player->batteries = 10000;
	player->c_armour = player->armour;

	if(gl.prepos && player->id <= 10)
	{
		player->x = gl.preposition[player->id-1].x;
		player->y = gl.preposition[player->id-1].y;
		if(player->x >= 0) DisplayPattern(player);
	}
	else
	{
		player->x = -1;
		player->y = -1;
	}

	player->xinc = 0;
	player->yinc = 0;
	player->c_speed = 0;
	player->d_speed = player->c_speed;
	player->c_angle = 0; 
	player->d_angle = player->c_angle;

	player->c_shells = player->shells;
	player->lastfiredshell = -TICKS_TO_RELOAD;
	player->shellsinair = 0;
	player->lastshellstatus = SHELL_NOT_AVAIL;
	for(i=0;i<MAXSHELLS_IN_AIR;i++)
	{
		player->flying_shell[i].xpos = -1;
		player->flying_shell[i].ypos = -1;
		player->flying_shell[i].xinc = 0;
		player->flying_shell[i].yinc = 0;
		player->flying_shell[i].times = 0;
	}

	player->mes_free = 0;
	player->mes_numbers = 0;
	for(i=0;i<MAX_MESSAGES;i++)
	{
		player->message_queue[i].id = 0;
		player->message_queue[i].value = 0;
	}

	player->invisible = 0;
	player->inv_count = 0;
	
	/* Menüstrukturen initialisieren */
	player->MenText.FrontPen = abs(player->id)%8+8;
	player->MenText.BackPen = 0;
	player->MenText.DrawMode = JAM2;
	player->MenText.LeftEdge = 0;
	player->MenText.TopEdge = 0;
	player->MenText.ITextFont = &TOPAZ80;
	player->MenText.IText = player->name;
	player->MenText.NextText = NULL;

	player->PosItem.NextItem = NULL; /* Eintrag in makemenu */
	player->PosItem.LeftEdge = 0;
	player->PosItem.TopEdge = 0;
	player->PosItem.Width = strlen(player->name)*10; /* 10 Zeichen */
	player->PosItem.Height = 8;
	player->PosItem.Flags = ITEMTEXT+ITEMENABLED+HIGHCOMP;
	player->PosItem.MutualExclude = 0;
	player->PosItem.ItemFill = (APTR)&player->MenText;
	player->PosItem.SelectFill = NULL;
	player->PosItem.Command = NULL;
	player->PosItem.SubItem = NULL;
	player->PosItem.NextSelect = NULL;

	player->StatusItem.NextItem = NULL; /* Eintrag in makemenu */
	player->StatusItem.LeftEdge = 0;
	player->StatusItem.TopEdge = 0;
	player->StatusItem.Width = strlen(player->name)*10; /* 10 Zeichen */
	player->StatusItem.Height = 8;
	player->StatusItem.Flags = ITEMTEXT+ITEMENABLED+HIGHCOMP;
	player->StatusItem.MutualExclude = 0;
	player->StatusItem.ItemFill = (APTR)&player->MenText;
	player->StatusItem.SelectFill = NULL;
	player->StatusItem.Command = NULL;
	player->StatusItem.SubItem = NULL;
	player->StatusItem.NextSelect = NULL;

	player->EntfernenItem.NextItem = NULL; /* Eintrag in makemenu */
	player->EntfernenItem.LeftEdge = 0;
	player->EntfernenItem.TopEdge = 0;
	player->EntfernenItem.Width = strlen(player->name)*10; /* 10 Zeichen */
	player->EntfernenItem.Height = 8;
	player->EntfernenItem.Flags = ITEMTEXT+ITEMENABLED+HIGHCOMP;
	player->EntfernenItem.MutualExclude = 0;
	player->EntfernenItem.ItemFill = (APTR)&player->MenText;
	player->EntfernenItem.SelectFill = NULL;
	player->EntfernenItem.Command = NULL;
	player->EntfernenItem.SubItem = NULL;
	player->EntfernenItem.NextSelect = NULL;

}

/*
	loadarena
	Lade die Arenadefinition aus einem File ein
*/
void loadarena(char *filename)
{
	FILE *fp;
	short x,y,lineend;

	for(x=0;x<10;x++)
	{
		gl.preposition[x].x = -1;
		gl.preposition[x].y = -1;
	}

	fp = fopen(filename,"r");
	if(fp == NULL)
	{
		gl.arenaname[0] = '\0';
		for(y=0;y<100;y++);
			for(x=0;x<100;x++);
				gl.arena[y][x]=ARENA_FREE;
	}
	else
	{
		for(x=strlen(filename);filename[x] != ':' &&
			filename[x] != '/' && x;x--);

		if(filename[x] == ':' || filename[x] == '/') x++;

		strncpy(gl.arenaname, filename+x, FCHARS);

		for(y=0; y<100; y++)
		{
			lineend = FALSE;
			if(!fgets(gl.buffer,BUFFERLEN,fp))
				lineend = TRUE;
			for(x=0;x<100;x++)
			{
				if(!lineend)
				{
					switch(toupper(gl.buffer[x]))
					{
					case 'D':
						gl.arena[y][x]=ARENA_DAMAGE;
						break;
					case 'R':
						gl.arena[y][x]=ARENA_REFUEL;
						break;
					case 'X':
						gl.arena[y][x]=ARENA_WALL;
						break;
					case '\0':	/* Vorzeitiges Zeilenende */
						lineend = TRUE;
					case '.':
					default :
						gl.arena[y][x]=ARENA_FREE;
						if(isdigit(gl.buffer[x]))
						{
							gl.preposition[gl.buffer[x]-'0'].x = x*2000+5*200;
							gl.preposition[gl.buffer[x]-'0'].y = y*2000+5*200;
						}
						break;
					}
				}
				else
				{
					gl.arena[y][x]=ARENA_FREE;
				}
			} /* for x */
		} /* for y */
		fclose(fp);
	}
}

void loadcolors(void)
{
	FILE *fp;
	short i;
	long color;
	
	fp = fopen(COLORFILE,"r");
	if(fp != NULL)
	{
		for(i=0; i<PaletteColorCount; i++)
		{
			if(fscanf(fp,"%lx,",&color))
				Palette[i]=color;
		}
		fclose(fp);
	}
}

void savecolors(void)
{
	FILE *fp;
	struct ColorMap *cm;
	short i;
	
	cm = gl.MainScreen->ViewPort.ColorMap;
	fp = fopen(COLORFILE,"w");
	if(fp != NULL)
	{
		for(i=0; i<PaletteColorCount; i++)
		{
			fprintf(fp,"%03X,",GetRGB4(cm,i));
		}
		fprintf(fp,"\n");
		fclose(fp);
	}
}

/**********************************************************************/
/*                             Menüaufbau                             */
/**********************************************************************/

void makemenu(void)
{
	struct RobotPlayer *player;

	ClearMenuStrip(gl.ArenaWindow);

	if(gl.running)
	{
		MI_LOADARENA.Flags &= ~ITEMENABLED;	/* Lade Arena */
		MI_LOADROBOT.Flags &= ~ITEMENABLED;
		MI_RUNSIM.Flags |= CHECKED;
	}
	else
	{
		MI_LOADARENA.Flags |= ITEMENABLED;
		MI_LOADROBOT.Flags |= ITEMENABLED;
		MI_RUNSIM.Flags &= ~CHECKED;

		if(gl.stoponone && gl.NumberPlayers < 2)
		{
			MI_RUNSIM.Flags &= ~ITEMENABLED;
		}
		else
		{
			MI_RUNSIM.Flags |= ITEMENABLED;
		}
	}

	if(gl.singlestep)
	{
		MI_SINGLESTEP.Flags |= CHECKED;
	}
	else
	{
		MI_SINGLESTEP.Flags &= ~CHECKED;
	}
	
	if(gl.stoponone)
	{
		MI_STOPONONE.Flags |= CHECKED;
	}
	else
	{
		MI_STOPONONE.Flags &= ~CHECKED;
	}
	
	if(gl.prepos)
	{
		MI_PREPOS.Flags |= CHECKED;
	}
	else
	{
		MI_PREPOS.Flags &= ~CHECKED;
	}
	
	if(gl.fastexplosion)
	{
		MI_FASTEXP.Flags |= CHECKED;
	}
	else
	{
		MI_FASTEXP.Flags &= ~CHECKED;
	}
	
	if(gl.NumberPlayers == 0)
	{
		M_POSITION.FirstItem = &MI_DUMMY; /* dummy */
		M_POSITION.Flags &= ~MENUENABLED;
		M_STATUS.FirstItem = &MI_DUMMY; /* dummy */
		M_STATUS.Flags &= ~MENUENABLED;
		M_ENTFERNEN.FirstItem = &MI_DUMMY; /* dummy */
		M_ENTFERNEN.Flags &= ~MENUENABLED;
	}
	else
	{
		M_POSITION.FirstItem = &FirstPlayer()->PosItem;
		M_POSITION.Flags |= MENUENABLED;
		M_STATUS.FirstItem = &FirstPlayer()->StatusItem;
		M_STATUS.Flags |= MENUENABLED;
		M_ENTFERNEN.FirstItem = &FirstPlayer()->EntfernenItem;
		M_ENTFERNEN.Flags |= MENUENABLED;
	}

	for(player=FirstPlayer();
		ValidPlayer(player);	
		player=NextPlayer(player))
	{
		player->PosItem.TopEdge = (abs(player->id)-1)*8;
		player->StatusItem.TopEdge = (abs(player->id)-1)*8;
		player->EntfernenItem.TopEdge = (abs(player->id)-1)*8;

		if(ValidPlayer((NextPlayer(player))))
		{
			player->PosItem.NextItem = &(NextPlayer(player))->PosItem;
			player->StatusItem.NextItem = &(NextPlayer(player))->StatusItem;
			player->EntfernenItem.NextItem = &(NextPlayer(player))->EntfernenItem;
		}
		else
		{
			player->PosItem.NextItem = NULL;
			player->StatusItem.NextItem = NULL;
			player->EntfernenItem.NextItem = NULL;
		}
	}
	
	if(gl.quickmode)	/* im Quickmode Menüs abschalten */
	{
		ArenaMenu1.Flags &= ~MENUENABLED;
		ArenaMenu2.Flags &= ~MENUENABLED;
		M_POSITION.Flags &= ~MENUENABLED;
		M_STATUS.Flags &= ~MENUENABLED;
		M_ENTFERNEN.Flags &= ~MENUENABLED;
	}

	SetMenuStrip(gl.ArenaWindow,&ArenaMenuList1);	/* attach any Menu */
}

/**********************************************************************/
/*                        Powerwindowsroutinen                        */
/**********************************************************************/

void openscreen(void)
{
	short i;
	WORD pens[]={~0};

#ifdef NEWSCREENSTRUCTURE
	struct Screen *sC;
	struct ViewPort *vP;
#endif
	struct Window *wG;	/* we fetch the RastPort pointer from here */
	struct RastPort *rpG;

	IntuitionBase = (struct IntuitionBase *)
		OpenLibrary("intuition.library", 0);
	if (IntuitionBase == NULL)
	{
		cleanup("Keine Intuition Library!",20);
	}
	gl.lib_Version = IntuitionBase->LibNode.lib_Version; /* 2.0 Abfrage */
	GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);

#ifdef OWNFONT
	DiskfontBase = (struct Library *)OpenLibrary("diskfont.library",0);
	gl.textfont = OpenDiskFont(&gl.textattr);
#endif

#ifdef NEWSCREENSTRUCTURE
	NewScreenStructure.DefaultTitle = 
		"AmigaRobots Version " VERSION " von Hanno Behrens © " DATUM;
	if(gl.lib_Version < 37)
		sC = OpenScreen(&NewScreenStructure);	/* open screen if present */
	else
	{
		NewScreenStructure.Width = gl.Width;
		NewScreenStructure.Height = gl.Height;
		sC = OpenScreenTags(&NewScreenStructure,
			SA_PubName,			"AmigaRobots",
			SA_AutoScroll,				TRUE,
			SA_DisplayID,		gl.monitorID,
			SA_Pens, 			(ULONG)pens,
			SA_DClip,			(ULONG)&gl.rect,
			TAG_DONE
		);
		if(sC) PubScreenStatus(sC,0);
	}
	gl.MainScreen = sC;
	if(!sC)
		cleanup("Kein Screen!",20);

#ifdef PALETTE
	vP = &sC->ViewPort;
	LoadRGB4(vP, Palette, PaletteColorCount);
#endif
#endif

	ArenaNewWindowStructure1.Screen = sC;
	wG = OpenWindow(&ArenaNewWindowStructure1);	/* open the window */
	gl.ArenaWindow = wG;
	if(!wG)
	{
		cleanup("Kein Window!",20);
	}

	rpG = wG->RPort;	/* get a rastport pointer for the window */

	/* Areastruktur initialisieren */
	for(i=0; i<AREA_SIZE; i++)
		gl.areabuffer[i]=0;
	InitArea(&gl.areainfo, gl.areabuffer, (2*AREA_SIZE)/5);
	rpG->AreaInfo = &gl.areainfo;

	gl.planeptr = AllocRaster(sC->Width, sC->Height);
	if(!gl.planeptr)
		cleanup("Kein Speicher für TmpRast",20);
	InitTmpRas(&gl.tmpras, gl.planeptr, RASSIZE(sC->Width, sC->Height));
	rpG->TmpRas = &gl.tmpras;

#ifdef ArenaMenuList1
	makemenu();
#endif

#ifdef ArenaIntuiTextList2
	PrintIText(rpG,&ArenaIntuiTextList2,0,0);	/* Print the text if there is
						any */
#endif

#ifdef ArenaBorderList1
	DrawBorder(rpG,&ArenaBorderList1,0,0);	/* Draw the borders if there are
						any */
#endif

#ifdef ArenaImageList1
	DrawImage(rpG,&ArenaImageList1,0,0);	/* Draw the images if there are any */
#endif

	RobotNewWindowStructure2.Screen = sC;
	wG = OpenWindow(&RobotNewWindowStructure2);	/* open the window */
	if(wG)
	{
		gl.RobotWindow = wG;
	}
	else
	{
		cleanup("Kein Window!",20);
	}

	rpG = wG->RPort;	/* get a rastport pointer for the window */

#ifdef RobotMenuList2
	SetMenuStrip(wG,&RobotMenuList2);	/* attach any Menu */
#endif

#ifdef RobotIntuiTextList3
	PrintIText(rpG,&RobotIntuiTextList3,0,0);	/* Print the text if there is
						any */
#endif

#ifdef RobotBorderList2
	DrawBorder(rpG,&RobotBorderList2,0,0);	/* Draw the borders if there are
						any */
#endif

#ifdef RobotImageList2
	DrawImage(rpG,&RobotImageList2,0,0);	/* Draw the images if there are any */
#endif

}
/**********************************************************************/
/*                           Menüfunktionen                           */
/**********************************************************************/
void gf_nextgadget(struct Gadget *gad)
{
	if(gad->NextGadget && gl.ConfigWindow)
		ActivateGadget(gad->NextGadget, gl.ConfigWindow, NULL);
}


void gf_confcancel(struct Gadget *gad)
{
	if(gl.ConfigWindow)
		CloseWindow(gl.ConfigWindow);
	gl.ConfigWindow = NULL;
	gl.maskconf = 0;
}

void gf_confok(struct Gadget *gad)
{
	gl.maxticks =	abs(((struct StringInfo *)
							ConfigGad_ticks.SpecialInfo)->LongInt);
	gl.reload =	abs(((struct StringInfo *)
							ConfigGad_reload.SpecialInfo)->LongInt);
	vec_shells[0] = abs(((struct StringInfo *)
							ConfigGad_shells.SpecialInfo)->LongInt);
	gl.delay = abs(((struct StringInfo *)
							ConfigGad_delay.SpecialInfo)->LongInt);
	gl.exp_darstellen = abs(((struct StringInfo *)
							ConfigGad_explosion.SpecialInfo)->LongInt);
	gl.timefactor = max(1,abs(((struct StringInfo *)
							ConfigGad_timefactor.SpecialInfo)->LongInt));


	if(gl.pcrobots == TRUE) 
		vec_shells[1] = 9*vec_shells[0]/10;
	else
		vec_shells[1] = vec_shells[0]/2;

	if(gl.ConfigWindow)
		CloseWindow(gl.ConfigWindow);
	gl.ConfigWindow = NULL;
	gl.maskconf = 0;
}

void mf_entfernen(unsigned int id)
{
	struct RobotPlayer *player;
	short i;

	for(player = FirstPlayer();
		ValidPlayer(player);
		player = NextPlayer(player))
	{
		if(abs(player->id) == id)
		{
			if(TwoGadRequest("Roboter entfernen",
					"Robotername: %s ID: %ld\n"
					"Roboter wirklich entfernen?",
					player->name,(long)abs(player->id)))
			{
				/* Roboter löschen */
				DisplayPattern(player);
				for(i=0;i<MAXSHELLS_IN_AIR;i++)
				{
					if(player->flying_shell[i].xpos >= 0)
					{
						PlotShell(player->flying_shell[i].xpos,
							player->flying_shell[i].ypos);
					}
				}

				/* Und Task entfernen */
				if(RemovePlayer(player))
				{
					gl.playerentfernt = TRUE;
				}
				else	/* Doch nicht entfernt, weil Timeout und keine Bestätigung*/
				{
					DisplayPattern(player);
					for(i=0;i<MAXSHELLS_IN_AIR;i++)
					{
						if(player->flying_shell[i].xpos >= 0)
						{
							PlotShell(player->flying_shell[i].xpos,
								player->flying_shell[i].ypos);
						}
					}
				}
			}
			break;
		}
	}
}

void mf_status(unsigned int id)
{
	struct RobotPlayer *player;

	if(gl.StatusWindow == NULL)
	{
		StatusNewWindowStructure4.Screen = gl.MainScreen;
		gl.StatusWindow = OpenWindow(&StatusNewWindowStructure4);
		gl.maskstatus = 1<<gl.StatusWindow->UserPort->mp_SigBit;
	}
	
	if(gl.StatusWindow)
	{
		for(player = FirstPlayer(), gl.pos_id = NULL;
			ValidPlayer(player);
			player = NextPlayer(player))
		{
			if(abs(player->id) == id)
			{
				gl.status_id = player;
				break;
			}
		}
	}
	else
	{
		gl.status_id = NULL;
		gl.maskstatus = 0;
	}
}

void mf_position(unsigned int id)
{
	struct RobotPlayer *player;

	if(gl.pos_id == NULL) /* Pos noch nicht gewählt? */
	{
		for(player = FirstPlayer();
			ValidPlayer(player);
			player = NextPlayer(player))
		{
			if(abs(player->id) == id)
			{
				gl.pos_id = player;
				ModifyIDCMP(gl.ArenaWindow,
					gl.ArenaWindow->IDCMPFlags | MOUSEBUTTONS);
				break;
			}
		}
	}
	else	/* Pos war schon angewählt, also cancel */
	{
		ModifyIDCMP(gl.ArenaWindow,
			gl.ArenaWindow->IDCMPFlags & ~MOUSEBUTTONS);
		gl.pos_id = NULL;
	}
}

void mf_config(APTR object)
{
	sprintf(((struct StringInfo *)ConfigGad_ticks.SpecialInfo)->Buffer,
		"%d",
		gl.maxticks);
	sprintf(((struct StringInfo *)ConfigGad_reload.SpecialInfo)->Buffer,
		"%d",
		gl.reload);
	sprintf(((struct StringInfo *)ConfigGad_shells.SpecialInfo)->Buffer,
		"%d",
		vec_shells[0]);
	sprintf(((struct StringInfo *)ConfigGad_delay.SpecialInfo)->Buffer,
		"%d",
		gl.delay);
	sprintf(((struct StringInfo *)ConfigGad_explosion.SpecialInfo)->Buffer,
		"%d",
		gl.exp_darstellen);
	sprintf(((struct StringInfo *)ConfigGad_timefactor.SpecialInfo)->Buffer,
		"%d",
		gl.timefactor);

	((struct StringInfo *)ConfigGad_ticks.SpecialInfo)->LongInt = gl.maxticks;
	((struct StringInfo *)ConfigGad_reload.SpecialInfo)->LongInt = gl.reload;
	((struct StringInfo *)ConfigGad_shells.SpecialInfo)->LongInt = vec_shells[0];
	((struct StringInfo *)ConfigGad_delay.SpecialInfo)->LongInt = gl.delay;
	((struct StringInfo *)ConfigGad_delay.SpecialInfo)->LongInt = gl.exp_darstellen;
	((struct StringInfo *)ConfigGad_delay.SpecialInfo)->LongInt = gl.timefactor;

	ConfigNewWindowStructure3.Screen = gl.MainScreen;

	if(gl.ConfigWindow == NULL)
	{
		gl.ConfigWindow = OpenWindow(&ConfigNewWindowStructure3);
		gl.maskconf = 1<<gl.ConfigWindow->UserPort->mp_SigBit;
		if(gl.ConfigWindow) ActivateGadget(&ConfigGad_ticks,gl.ConfigWindow,NULL);
   }
}

void mf_stoponone(APTR object)
{
	struct MenuItem *mi;
	mi = (struct MenuItem *)object;

	if(mi->Flags & CHECKED)
	{
		gl.stoponone = TRUE;
	}
	else
	{
		gl.stoponone = FALSE;
	}
	makemenu();
}

void mf_singlestep(APTR object)
{
	struct MenuItem *mi;
	mi = (struct MenuItem *)object;

	if(mi->Flags & CHECKED)
	{
		gl.singlestep = TRUE;
	}
	else
	{
		gl.singlestep = FALSE;
	}
}

void mf_prepos(APTR object)
{
	struct MenuItem *mi;
	mi = (struct MenuItem *)object;

	if(mi->Flags & CHECKED)
	{
		gl.prepos = TRUE;
	}
	else
	{
		gl.prepos = FALSE;
	}
}

void mf_fastexplosion(APTR object)
{
	struct MenuItem *mi;
	mi = (struct MenuItem *)object;

	if(mi->Flags & CHECKED)
	{
		gl.fastexplosion = TRUE;
	}
	else
	{
		gl.fastexplosion = FALSE;
	}
}

void mf_runrobots(APTR object)
{
	struct ESStructure *current;

	if(filerequest("Roboter laden",
			gl.robotdir,gl.robotfilename,"*.ROB","",30))
	{
		if(gl.FileRequest.ExtendedSelect != NULL)
		{
			for(current = gl.FileRequest.ExtendedSelect;
				current != 0;
				current = current->NextFile)
			{
				sprintf(gl.buffer,"run >nil: %s/%s",
					gl.dirname,current->thefilename
				);
				system(gl.buffer);
			}
		}
		else /* Einzelnes File */
		{
			sprintf(gl.buffer,"run >nil: %s",
				gl.pathname
			);
			system(gl.buffer);
		}
		strncpy(gl.robotfilename,gl.filename,FCHARS);
		strncpy(gl.robotdir,gl.dirname,DSIZE);
	}
}

void mf_loadarena(APTR object)
{
	if(gl.running == FALSE &&
		filerequest("Arena laden",
			gl.arenadir,gl.arenafilename,"*.RNA","*.BAK",0))
	{
		loadarena(gl.pathname);
		DisplayArena();
		strncpy(gl.arenafilename,gl.filename,FCHARS);
		strncpy(gl.arenadir,gl.dirname,DSIZE);
	}
}

void mf_runsim(APTR object)
{
	struct MenuItem *mi;
	mi = (struct MenuItem *)object;

	if(mi->Flags & CHECKED)
	{
		gl.running = TRUE;
	}
	else
	{
		gl.running = FALSE;
	}
	makemenu();
}

void mf_color(APTR object)
{
	ColorRequester(1L);
	savecolors();
}

void mf_quit(APTR object)
{
	if(gl.running)
	{
		if(TwoGadRequest("Programmende","Wollen Sie wirklich AmigaRobots verlassen?"))
			cleanup("Ende",0);
	}
	else
	{
		cleanup("Ende",0);
	}
}

void mf_info(APTR object)
{
	SimpleRequest("Information",
"                     AmigaRobots Version " VERSION "\n"
"                   © " DATUM " von Hanno Behrens\n"
"\n"
"Dieses  Programm  ist  Fairware.   Die Nutzung ist frei, aber auf\n"
"eigene  Verantwortung.   Bei  Gefallen  freue  ich mich über eine\n"
"kleine Aufmerksamkeit.  Liegt die  Aufmerksamkeit  so bei 15.- DM\n"
"Mark, wird der Benutzer von mir registriert.\n"
"\n"
"Bei   Zusendung   einer   3½\"  Diskette  und  eines  ausreichend\n"
"frankierten  und adressierten Rückumschlags, lege ich das neueste\n"
"Update  bei.  Wenn  auf  der  Diskette  neue  Roboter  (bitte mit\n"
"Quelltext!)   sind,   teste   ich  diese  im  Kampf  gegen  schon\n"
"vorhandene.  Unterlegene Roboter werden mit Quelltext, überlegene\n"
"ohne Quelltext beigelegt.\n"
"\n"
"Die  von mir verwaltete Rangliste wird beigelegt.  Für den ersten\n"
"Platz lasse ich mir etwas besonderes einfallen...\n"
"\n"
"Jede kommerzielle oder militärische Nutzung ist untersagt, ebenso\n"
"die Weitergabe für mehr als 5.- DM über dem Leerdiskettenpreis.\n"
"\n"
"Über  Kritik  und Ideen jeglicher Art würde ich mich sehr freuen,\n"
"dazu muß man nicht registriert sein. \n"
"\n"
"    Internet/Z-Netz: H.Behrens@amtrash.comlink.de\n"
"               Real: Hanno Behrens, Overland 9\n"
"                     22159 Hamburg, Germany\n"
"  BLZ : 200 505 50   Konto 1284/453 105\n"
"\n"
	);
}

void do_menu(void)
{
	UWORD code;
	ULONG class;
	APTR object;
	WORD mousex,mousey;
	struct MenuItem *mi; /* für MenuItems von Status, Position und Entfernen-Menü */

	struct IntuiMessage *message;	/* the message the IDCMP sends us */

	if(!gl.quickmode) /* keine Menüabfrage im quickmode */
	{
		while( (message = (struct IntuiMessage *)	/* Messages löschen */
			GetMsg(gl.RobotWindow->UserPort) ) != NULL)
		{
			ReplyMsg((struct Message *) message);
			ActivateWindow(gl.ArenaWindow); /* und ArenaWindow aktivieren */
		}

		while( gl.ConfigWindow && (message = (struct IntuiMessage *)
			GetMsg(gl.ConfigWindow->UserPort) ) != NULL)
		{
			code = message->Code;  /* MENUNUM */
			object = message->IAddress;  /* Gadget */
			class = message->Class;
			ReplyMsg((struct Message *)message);
			switch(class)
			{
			case GADGETUP: /* Gagdets */
			case GADGETDOWN:
				HandleEvent(object);
				break;
			case VANILLAKEY:
				switch(code)
				{
					case ESC:
						gf_confcancel(NULL);
						break;
					case RETURN:
						gf_confok(NULL);
						break;
					default:
						break;
				}
				break;
			default:
				break;
			}
		}
		
		while( gl.StatusWindow &&
			(message = (struct IntuiMessage *)	/* Messages löschen */
			GetMsg(gl.StatusWindow->UserPort) ) != NULL)
		{
			class = message->Class;
			ReplyMsg((struct Message *) message);
			switch(class)
			{
			case VANILLAKEY:
				if(code != ESC)
					break;
			case CLOSEWINDOW: /* Gagdets */
				CloseWindow(gl.StatusWindow);
				gl.StatusWindow = NULL;
				gl.maskstatus = 0;
				gl.status_id = NULL;
				break;
			default:
				ActivateWindow(gl.ArenaWindow); /* und ArenaWindow aktivieren */
				break;
			}
		}
	} /* EndIF !gl.quickmode */

	while( (message = (struct IntuiMessage *)
		GetMsg(gl.ArenaWindow->UserPort) ) != NULL)
	{
		code = message->Code;  /* MENUNUM */
		object = message->IAddress;  /* Gadget */
		class = message->Class;
		mousex = message->MouseX;
		mousey = message->MouseY;
		ReplyMsg((struct Message *)message);

		switch(class)
		{
		case VANILLAKEY:
			switch(code)
			{
			case ESC:
				mf_quit(NULL);
				break;
			default:
				break;
			}
			break;
		case GADGETUP:	/* Gagdets */
		case GADGETDOWN:
			HandleEvent(object);
			break;
		case MENUPICK:	/* MenuItems */
			if(MENUNUM(code) == 2 || 
				MENUNUM(code) == 3 ||
				MENUNUM(code) == 4 )	/* Pos/Stat/Entfernen */
			{
				mi = ItemAddress(&ArenaMenuList1,code); /* Die Adresse des MenuItems holen */
				/* Position und Statusbefehl */
				switch(MENUNUM(code))
				{
				case 2:	/* Position */
					mf_position(mi->TopEdge/8+1); /* jeder Roboter hat eine Position, entsprechend */
					break;
				case 3:	/* Status */
					mf_status(mi->TopEdge/8+1);	/* seiner ID */
					break;
				case 4:	/* Entfernen */
					mf_entfernen(mi->TopEdge/8+1);
					break;
				default:
					break;
				}
			}
			else
				/* irgend ein anderes Menü */
			{
				HandleEvent(ItemAddress(&ArenaMenuList1,code));
			}
			break;
		case MOUSEBUTTONS: /* Position */
			switch(code)
			{
			case SELECTDOWN:	/* ignorieren */
				break;
			case SELECTUP:
				if(gl.pos_id)	/* muß valid sein */
				{
					if(gl.pos_id->x >= 0) DisplayPattern(gl.pos_id);
					gl.pos_id->x = mousex*400; /* 500*2*200 */
					gl.pos_id->y = mousey*400;
					DisplayPattern(gl.pos_id);
				}	/* und canceln */
			case MENUUP:
			case MENUDOWN:
			default:
				ModifyIDCMP(gl.ArenaWindow,
					gl.ArenaWindow->IDCMPFlags & ~MOUSEBUTTONS);
				gl.pos_id = NULL;
			}
			break;
		default:
			break;
		}
	}
}

/**********************************************************************/
/*                           Matheroutinen                            */
/**********************************************************************/

short makewinkel(register short winkel)
{
	winkel %= 360;
	if(winkel < 0)
		winkel += 360;
	return winkel;
}

/**********************************************************************/
/*                            Req-Routinen                            */
/**********************************************************************/

short __stdargs TwoGadRequest(char *title, char *str,...)
{
	va_list ap;
	struct TRStructure trs;
	short res;

	va_start(ap,str);

	trs.Text = str;
	trs.Controls = ap;
	trs.Window = 0;
	trs.MiddleText = 0;
	trs.PositiveText = "Ja";
	trs.NegativeText = "Nein";
	trs.Title = title;
	trs.KeyMask = -1;
	trs.textcolor = 1;
	trs.detailcolor = 3;
	trs.blockcolor = 2;
	trs.versionnumber = REQVERSION;
	trs.Timeout = 0;	/* you could put a timeout here if you wanted */
	trs.AbortMask = 0;	/* If you wanted to abort this requester from another */
						/* process, pass the mask for Signal() in this element. */
						/* Then just Signal() the process that brought up the requester */
	trs.rfu1 = 0;

	res = TextRequest(&trs);
	va_end(ap);
	return(res);
}

/*
	SimpleRequest
	einfacher Anzeiger
*/
void __stdargs SimpleRequest(char *title,char *str,...)
{
	va_list ap;
	struct TRStructure trs;

	va_start(ap,str);

	trs.Text = str;
	trs.Controls = ap;
	trs.Window = 0;
	trs.MiddleText = 0;
	trs.PositiveText = 0;
	trs.NegativeText = "Weiter";
	trs.Title = title;
	trs.KeyMask = -1;
	trs.textcolor = 1;
	trs.detailcolor = 3;
	trs.blockcolor = 2;
	trs.versionnumber = REQVERSION;
	trs.Timeout = 0;	/* you could put a timeout here if you wanted */
	trs.AbortMask = 0;	/* If you wanted to abort this requester from another */
						/* process, pass the mask for Signal() in this element. */
						/* Then just Signal() the process that brought up the requester */
	trs.rfu1 = 0;
	TextRequest(&trs);
	va_end(ap);
}

int filerequest(
	char *title,
	char *dir,
	char *file,
	char *showpattern,
	char *hidepattern,
	int maxselect
)
{
	gl.FileRequest.VersionNumber = REQVERSION;
	gl.FileRequest.Title = title;

	strncpy(gl.dirname,dir,DSIZE);
	gl.FileRequest.Dir = gl.dirname;

	strncpy(gl.filename,file,FCHARS);
	gl.FileRequest.File = gl.filename;

	gl.pathname[0] = 0;
	gl.FileRequest.PathName = gl.pathname;

	gl.FileRequest.Window = NULL;
	gl.FileRequest.MaxExtendedSelect = maxselect; 
	gl.FileRequest.numlines = 0;
	gl.FileRequest.numcolumns = 0;
	gl.FileRequest.devcolumns = 0;
	gl.FileRequest.Flags = 	FRQCACHINGM|FRQCACHEPURGEM|
									FRQLOADINGM; 
	if(maxselect > 0)
		gl.FileRequest.Flags |= FRQEXTSELECTM;

	gl.FileRequest.dirnamescolor  = 0;
	gl.FileRequest.filenamescolor = 0;
	gl.FileRequest.devicenamescolor = 0;
	gl.FileRequest.fontnamescolor = 0;
	gl.FileRequest.fontsizescolor = 0;
	gl.FileRequest.detailcolor = 0;
	gl.FileRequest.blockcolor = 0;
	gl.FileRequest.gadgettextcolor = 0;
	gl.FileRequest.textmessagecolor = 0;
	gl.FileRequest.stringnamecolor = 0;
	gl.FileRequest.stringgadgetcolor = 0;
	gl.FileRequest.boxbordercolor = 0;
	gl.FileRequest.gadgetboxcolor = 0;

	strncpy(gl.FileRequest.Hide,hidepattern,WILDLENGTH);
	strncpy(gl.FileRequest.Show,showpattern,WILDLENGTH);

	return FileRequester(&gl.FileRequest);
}


/**********************************************************************/
/*                        High-Level Routinen                         */
/**********************************************************************/

void DisplayPattern(struct RobotPlayer *player)
{
	register struct RastPort *rp;

	rp = gl.ArenaWindow->RPort;
	SetWrMsk(rp, 8+abs(player->id)%8);	/* Roboterfarbe setzen */
	SetDrMd(rp, COMPLEMENT);
	BltTemplate((PLANEPTR)player->pattern,
		11,	/* left pattern offset */
		2,		/* Modulo */
		rp,
		player->x/400-2,player->y/400-2,
		5,5	/* Breite, Höhe */
	);
}

void RemoveAllPlayers(void)
{
	while(gl.Players.mlh_Head->mln_Succ) 
	{
		while(!RemovePlayer((struct RobotPlayer *)gl.Players.mlh_Head));
	}
}

int RemovePlayer(struct RobotPlayer *player)
{
	struct RoboMsg *msg;
	short timeout;	

	ClearMenuStrip(gl.ArenaWindow);
	for(timeout=100;timeout>0 && !(msg = (struct RoboMsg *)GetMsg(&player->MPort));timeout--)			
		Delay(1L);
	if(msg)	/* Roboter hat reagiert */
	{
		msg->cmd = AR_CMD_KILLTASK;
		ReplyMsg((struct Message *)msg);	
	}
	else	/* Roboter reagiert nicht auf Anruf */
	{
		if(!TwoGadRequest("Roboter entfernen",
				"Robotername: %s, ID: %ld\n"
				"Der   Roboter   reagiert   nicht,   wahrscheinlich   enthält  die\n"
				"Hauptschleife  keinen  AmigaRobots-Befehl.   Es  könnte aber auch\n"
				"sein,  daß der Roboter in einer Berechnung hängt, dann sollte man\n"
				"diese   Berechnung   abwarten   und  das  Entfernen  noch  einmal\n"
				"versuchen.   Der  Roboter  kann  nur  noch mit RemTask() entfernt\n"
				"werden.\n"
				"Soll ich das wirklich tun?\n",
				player->name,(long)abs(player->id)
				))
			return FALSE; /* Roboter doch nicht entfernen */
		RemTask(player->playertask); /* Task beseitigen */
	}

	SetAPen(gl.RobotWindow->RPort,1);
	SetDrMd(gl.RobotWindow->RPort,JAM1);
	Move(gl.RobotWindow->RPort,0,abs(player->id)*20);
	Draw(gl.RobotWindow->RPort,104,abs(player->id)*20+18);

	Remove((struct Node *)player);	/* Aus Spielerliste entfernen */
	gl.NumberPlayers--;	/* ein Spieler weniger */

	FreeMem(player->pattern,10);	/* Chipmem freigeben */
	free(player);
	makemenu();
	return TRUE;
}

struct RobotPlayer *AllocPlayer(struct RoboMsg *msg)
{
	struct RobotPlayer *player;
	short i;
	char *pc;
			
	if(player = (struct RobotPlayer *)malloc(
		sizeof(struct RobotPlayer) ))
	{
		/* Chipmem für pattern */
		if(!(player->pattern = AllocMem(10,MEMF_CHIP)))
		{
			free(player);
			return NULL;
		}

		player->playertask = msg->msg.mn_ReplyPort->mp_SigTask;

		pc = (char *)msg->arg7;

		for(i=strlen(pc);pc[i] != ':' && pc[i] != '/' && i;i--);
		if(pc[i] == ':' || pc[i] == '/') i++;

		strncpy(player->name, pc+i, NAMELEN);
		player->id = ++gl.id;

		/* Datenbereiche forcieren */
		player->speed = msg->arg1 % 5;
		player->manouevre = msg->arg2 % 5;
		player->range = msg->arg3 % 5;
		player->armour = msg->arg4 % 5;
		player->acceleration = msg->arg5 % 5;
		player->invisibility = msg->arg6 % 2;

		/* Zehnpunktecheck machen */
		i = 10;
		i -= player->speed;
		i -= player->manouevre;
		if(i >= player->range)
			i -= player->range;
		else
		{
			player->range = i;
			i = 0;
		}

		if(i >= player->armour)
			i -= player->armour;
		else
		{
			player->armour = i;
			i = 0;
		}

		if(i >= player->acceleration)
			i -= player->acceleration;
		else
		{
			player->acceleration = i;
			i = 0;
		}

		/* Messageport initialisieren */
		player->MPort.mp_Node.ln_Name = NULL;
		player->MPort.mp_Node.ln_Pri = 0;
		player->MPort.mp_Node.ln_Type = NT_MSGPORT;
		player->MPort.mp_Flags = PA_SIGNAL;
		player->MPort.mp_SigBit = 0;	/*gl.SigBit;*/ /* Signalbit fürs wecken... */
		player->MPort.mp_SigTask = FindTask(0);
		
		/* Messageliste initialisieren */
		NewList(&(player->MPort.mp_MsgList));

		/* Trage neuen Spieler in die Liste ein */
		AddTail((struct List *)&gl.Players ,(struct Node *)player);

		gl.NumberPlayers++;	/* ein Spieler mehr */
		gl.PlayersAlive = gl.NumberPlayers;

		initplayer(player);
		makemenu();
	}
	return player;
}

struct RobotPlayer *GetNewRobots(void)
{
	struct RoboMsg *msg;
	struct RobotPlayer *player = NULL;

	 /* Wenn ein neuer Logon */
	while( (msg = (struct RoboMsg *)GetMsg(gl.LogonPort)) )
	{
		if(msg->cmd == AR_CMD_CONFIGURE)
		{
			if(player = AllocPlayer(msg))
			{
				msg->ret = (long)&player->MPort;
			}
		}
		ReplyMsg((struct Message *)msg);		
	}
	return player;
}

/**********************************************************************/
/*                          Grafikroutinen                            */
/**********************************************************************/

void DisplayArena(void)
{
	short x,y;
	struct RobotPlayer *player;

	SetDrMd(gl.ArenaWindow->RPort,JAM1);
	SetWrMsk(gl.ArenaWindow->RPort,0xff);	/* Maske freigeben */

	SetAPen(gl.ArenaWindow->RPort, 0);
	SetRast(gl.ArenaWindow->RPort, 0);
	SetOPen(gl.ArenaWindow->RPort, 2);
	SetBPen(gl.ArenaWindow->RPort, 0);

	gl.refuelpoints = 0;
	for(y=0; y<100; y++)
		for(x=0; x<100; x++)
		{
			switch(gl.arena[y][x])
			{
			case ARENA_WALL:
				SetAPen(gl.ArenaWindow->RPort, 2);
				RectFill(gl.ArenaWindow->RPort,x*5,y*5,x*5+4,y*5+4);
				break;
			case ARENA_DAMAGE:
				SetAPen(gl.ArenaWindow->RPort, 6);
				RectFill(gl.ArenaWindow->RPort,x*5,y*5,x*5+4,y*5+4);
				break;
			case ARENA_REFUEL:
				if(gl.refuelpoints < MAX_REFUELPOINTS)
				{
					gl.refuel[gl.refuelpoints].x = x;	/* Points initialisieren */
					gl.refuel[gl.refuelpoints].y = y;
					gl.refuel[gl.refuelpoints].load = 20000;
					gl.refuelpoints++;
					SetAPen(gl.ArenaWindow->RPort, 7);
					RectFill(gl.ArenaWindow->RPort,x*5,y*5,x*5+4,y*5+4);
					break;
				}
				else
				{
					gl.arena[y][x] = ARENA_FREE;
				}
			case ARENA_FREE:
			default:
				break;
			}
			
		}
	/* Keine weitere Umrahmung mehr */
	gl.ArenaWindow->RPort->Flags &= ~AREAOUTLINE;

	for(	player = FirstPlayer(); 
			ValidPlayer(player); 
			player = NextPlayer(player)) 
	{
		if(player->x >= 0) DisplayPattern(player);	/* Wenn Position schon gewählt wurde */
	}
}

void ShowRobots(void)
{
	struct RobotPlayer *player;
	short i;	/* Zeilensprungvariable */
	
	struct IntuiText it2 =
	{
		1,0,	/* Farbe */
		JAM2,
		0,9,	/* Pos */
		&TOPAZ80,
		gl.itb2,	/* String */
		NULL
	};
	struct IntuiText it1 =
	{
		1,0,	/* Farbe */
		JAM2,
		0,0,	/* Pos */
		&TOPAZ80,
		gl.itb1,	/* String */
		NULL	/* nachher &it2 */
	};
	
	sprintf(gl.itb2,"Runden: %06d",gl.ticks);
	PrintIText(gl.RobotWindow->RPort,&it2,0,0);

	it1.NextText = &it2;

	for(	player = FirstPlayer(); 
			ValidPlayer(player); 
			player = NextPlayer(player)) 
	{
		if(player->id > 0)
		{
			sprintf(gl.itb1,"%-10.10s  %c %c%c",
				player->name,
				((player->invisible)?'I':' '),
				((player->debug[0])?'1':' '),
				((player->debug[1])?'2':' ')
			);
			sprintf(gl.itb2,"B:%4d P: %3d",
				player->batteries/10,
				player->c_armour
			);

			if(player->c_armour <= 0)	/* Darstellung für Begraben */
			{
				it1.FrontPen = 1;
				it2.FrontPen =it1.FrontPen;
				it1.DrawMode = JAM2;
				it2.DrawMode = JAM2;
				PrintIText(gl.RobotWindow->RPort,&it1,0,abs(player->id)*20);
				PrintIText(gl.RobotWindow->RPort,&it1,1,abs(player->id)*20+1);
				it1.DrawMode = JAM1;
				it2.DrawMode = JAM1;
			}
			else
			{
				it1.DrawMode = JAM2;
				it2.DrawMode = JAM2;
			}

			it1.FrontPen = abs(player->id)%8+8;
			it2.FrontPen =it1.FrontPen;
			PrintIText(gl.RobotWindow->RPort,&it1,0,abs(player->id)*20);
		}
	}
	
	/* StatusWindow */
	if(gl.StatusWindow && gl.status_id)
	{
		SetRast(gl.StatusWindow->RPort,0);
		it1.NextText = NULL;
		it1.FrontPen = abs(gl.status_id->id)%8+8;

		i=0;
		sprintf(gl.itb1,"%s",gl.status_id->name);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,"ID:%02d %s",
			abs(gl.status_id->id),
			(gl.status_id->id > 0)?"lebt":"tot"
		);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,"Konfig: G%d/M%d/R%d/P%d/B%d/U%d",
			gl.status_id->speed,
			gl.status_id->manouevre,
			gl.status_id->range,
			gl.status_id->armour,
			gl.status_id->acceleration,
			gl.status_id->invisibility
		);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,"Pos.  : x %3d y %3d",
			gl.status_id->x / 200,
			gl.status_id->y / 200
		);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,"Gesch.: %3d/%3d",
			gl.status_id->c_speed,
			gl.status_id->d_speed
		);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,"Richt.: %3d/%3d",
			gl.status_id->c_angle,
			gl.status_id->d_angle
		);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,"Batt. : %4d",
			gl.status_id->batteries/10
		);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,"Munit.: %4d",
			gl.status_id->c_shells
		);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,"IStat : %s",
			(gl.status_id->invisible==1)?"unsichtbar":"sichtbar  "
		);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,"Letzter Befehl:");
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;

		sprintf(gl.itb1,cmd_text[gl.st_cmd],
			gl.st_arg[0],gl.st_arg[1],gl.st_arg[2],gl.st_arg[3]
		);
		PrintIText(gl.StatusWindow->RPort,&it1,0,i);
		i+=9;
	}
}

/*
	SetRandomPosForAll
	sucht zufällig für alle Roboter mit einer illegalen Position
	eine neue. Illegal sind negative Werte oder Positionen auf
	bereits belegten Arenaplätzen
*/

void SetRandomPosForAll(void)
{
	struct RobotPlayer *player;

	srand(time(NULL));	/* seed initialisieren */
	for(	player = FirstPlayer(); 
			ValidPlayer(player); 
			player = NextPlayer(player)) 
	{
		if(player->x >= 0) DisplayPattern(player);	/* Wenn Position schon gewählt wurde */
		while(	/* bis gültige Position gefunden */
			player->x<0 || player->x>199999 || 
			player->y<0 || player->y>199999 ||
			gl.arena[player->y/2000][player->x/2000] != ARENA_FREE)
		{
			player->x = rand()%200000;	/* 1000 * 200 */
			player->y = rand()%200000;
			
		}
		DisplayPattern(player);	
	}
}

/* 
	PlotShell
	zeichnet einen schwarzen Punkt auf den Bildschirm
*/

void PlotShell(long x, long y)
{
	SetWrMsk(gl.ArenaWindow->RPort,1);	/* Schwarz setzen */
	SetDrMd(gl.ArenaWindow->RPort,COMPLEMENT);
	WritePixel(gl.ArenaWindow->RPort,x/400,y/400);
}

void DrawExplosion(register short x, register short y)
{
	struct RastPort *rp;
	short i;

	if(gl.quickmode) return; /* im Quickmode keine Explosionen */

	rp = gl.ArenaWindow->RPort;
	x /= 2;	/* 0..500 */
	y /= 2;
	
	SetDrMd(rp,COMPLEMENT);

	if(gl.fastexplosion)
	{
		SetWrMsk(rp,1);	/* Weiß = 4 */
		AreaCircle(rp,x,y,3);
		AreaEnd(rp);
		WaitBlit();

		SetWrMsk(rp,3);	/* Gelb = 5 */
		AreaCircle(rp,x,y,13);
		AreaEnd(rp);
		WaitBlit();

		SetWrMsk(rp,6);	/* Rot  = 6 */
		AreaCircle(rp,x,y,25);
		AreaEnd(rp);
		WaitBlit();
	
		for(i=gl.exp_darstellen; i > 0; i--)
		{
			WaitTOF();
		}

		SetWrMsk(rp,6);	/* Rot  = 6 */
		AreaCircle(rp,x,y,25);
		AreaEnd(rp);
		WaitBlit();

		SetWrMsk(rp,3);	/* Gelb = 5 */
		AreaCircle(rp,x,y,13);
		AreaEnd(rp);
		WaitBlit();

		SetWrMsk(rp,1);	/* Weiß = 4 */
		AreaCircle(rp,x,y,3);
		AreaEnd(rp);
		WaitBlit();
	}
	else	/* langsame Explosion */
	{
		for(i=0;i<=25;i++)
		{
			if(i<=3)
			{
				SetWrMsk(rp,4); /* Weiß */
			}
			else
			if(i<=13)
			{
				SetWrMsk(rp,5);	/* Gelb */
			}
			else
			{
				SetWrMsk(rp,6);	/* Rot */
			}
			DrawCircle(rp,x,y,i);
		}

		WaitTOF();
		WaitTOF();

		for(i=0;i<=25;i++)
		{
			if(i<=3)
			{
				SetWrMsk(rp,4); /* Weiß */
			}
			else
			if(i<=13)
			{
				SetWrMsk(rp,5);	/* Gelb */
			}
			else
			{
				SetWrMsk(rp,6);	/* Rot */
			}
			DrawCircle(rp,x,y,i);
		}
	}/* else langsame Explosion */
}

/* 
	DoExplosion
	läßt eine shell explodieren und beschädigt alle 
	betroffenen Robots
	gibt den höchsten Schadenscode zurück
*/

int DoExplosion(long x, long y, struct RobotPlayer *shootingplayer)
{
	short id;
	short	damagecode = SHELL_MISS,r;
	long	dx,dy;
	struct RobotPlayer *player;
	
	id = shootingplayer->id;
	
	DrawExplosion(x/200,y/200);
	/* alle Roboter durchgehen */
	for(player = FirstPlayer();
		ValidPlayer(player);
		player=NextPlayer(player)
	)
	{
		if(player->c_armour > 0)
		{
			dx = (player->x - x)/200;
			dy = (player->y - y)/200;
			r = (short) sqrt((double)dx*dx + dy*dy);
			if(r <= 50)
			{
				player->c_armour -= 2; /* innerhalb 50 2 Punkte */
				player->lasthitby = id;
				damagecode = max(damagecode,SHELL_50);
				player->hit_from = makewinkel(180 + (short) /* Berechne Richtung, in der Explosion war */
					(atan2((double)dy,(double)dx)*180.0/PI));
			}
			if(r <= 25)
			{
				player->c_armour -= 6; /* innerhalb 50 8 Punkte */
				player->lasthitby = id;
				damagecode = max(damagecode,SHELL_25);
			}
			if(r <= 5)
			{
				player->c_armour -= 17; /* innerhalb 50 25 Punkte */
				player->lasthitby = id;
				damagecode = max(damagecode,SHELL_5);
			}
			player->c_armour = max(player->c_armour,0);
			if(player->c_armour == 0)	/* Energie vom Getöteten übernehmen */
			{
				shootingplayer->batteries += player->batteries;
				player->batteries = 0;
				shootingplayer->batteries = min(10000,shootingplayer->batteries);
			}
		}/* if alive */
	}/* For all Robots */
	return damagecode;
}

/*
	GetCmds
	führt die Roboterbefehle aus
*/

void GetCmds(register struct RobotPlayer *player)
{
	register struct RoboMsg *msg;	
	register struct RobotPlayer *locp;
	long dx,dy;
	short maxw,minw,w,r,range,id; /* für Scan */
	short i;	/* für Shoot und diverses */
	char *pc;	/* für setpattern */
	int cmd;		/* zum Speichern des letzten Commands nach Reply */

	player->MPort.mp_SigBit = gl.SigBit;	/* für's Wecken */

	do
	{
		while(!(msg = (struct RoboMsg *)GetMsg(&player->MPort)) && !gl.playerentfernt)
		{
			Wait(gl.maskplayer|gl.maskarena);
			do_menu();
		}

		if(msg) 
		{
			switch(msg->cmd)
			{
				/**********************************/
				/* Rechenzeit abgebende Kommandos */
				/**********************************/

			case AR_CMD_SWAPTASK:	/* Rechenzeit abgeben */
				/* void */
				break;

			case AR_CMD_MOVEMENT:	/* neue Richtung, Geschwindigkeit */
				/* void */
				player->d_speed = msg->arg1;
				msg->arg2 = makewinkel(msg->arg2);
				player->d_angle = msg->arg2;
				player->d_speed = max(player->d_speed, 0);
				player->d_speed = min(player->d_speed, player->speed);
				break;

			case AR_CMD_SCAN:			/* suchen */
				/* ret=id, arg3=range, Rückgabewerte */
				/* gibt immer den Dichtesten zurück */
				if(!player->invisible)	/* Nur wenn sichtbar */
				{
					/* Übergabeparameter richten */
					msg->arg1 = makewinkel(msg->arg1);
					if(msg->arg2 < 0)
						msg->arg2 = 0;
					if(msg->arg2 > 45)
						msg->arg2 = 45;

					maxw = makewinkel(msg->arg1 + msg->arg2);
					minw = makewinkel(msg->arg1 - msg->arg2);

					for(locp = FirstPlayer(),range=2000,id=-1; 
							ValidPlayer(locp);
							locp = NextPlayer(locp)
					)
					{
						if(locp != player &&  /* Nicht mit sich selbst */
							locp->invisible == 0	&& /* und der andere sichtbar */
							locp->c_armour > 0)	/* und am Leben */
						{
							/* Richtung bestimmen */
							dx = player->x/200 - locp->x/200;
							dy = player->y/200 - locp->y/200;
							w=makewinkel(180 + (short)
								(atan2((double)dy,(double)dx)*180.0/PI));
							r=(short) sqrt((double)dx*dx+dy*dy);	/* Pythagoras */

							/* ist er im Intervall minw,maxw ? */
							if(minw <= maxw)	/* Normalfall */
							{
								if(minw <= w && w <= maxw && r < range)
								{
									range=r;
									id=locp->id-1;
								}
							}
							else	/* ein Überlauf */
							{
								if( (minw <= w || w <= maxw) && r < range)
								{
									range=r;
									id=locp->id-1;
								}
							}	/* if intervall */
						}	/* if valid player */
					}	/* for all players */
					msg->ret = id;
					msg->arg3 = range;
				}
				else	/* unsichtbar */
				{
					msg->ret = -1;
				}
				break;

			case AR_CMD_SHOOT:		/* schießen */
				/* ret= 1|0 */
				
				if(player->c_shells > 0 &&		/* shells vorhanden */
					gl.ticks - player->lastfiredshell >= TICKS_TO_RELOAD && 
					player->shellsinair < MAXSHELLS_IN_AIR)
					/* es kann noch gefeuert werden */
				{
					i=0;
					/* erste freie Shell suchen */
					while(player->flying_shell[i].xpos >= 0 && i < MAXSHELLS_IN_AIR-1)
						i++;

					msg->arg1 = makewinkel(msg->arg1);

					/* shell konfigurieren */
					player->flying_shell[i].xpos = player->x;
					player->flying_shell[i].ypos = player->y;
					player->flying_shell[i].xinc = (short)
						(vec_cos[msg->arg1]*gl.timefactor/500.0);	/* 100000/500*2 = 400 */
					player->flying_shell[i].yinc = (short)
						(vec_sin[msg->arg1]*gl.timefactor/500);	/* siehe DoSim */
					player->flying_shell[i].times = 
						min(max(msg->arg2,1),player->range)/gl.timefactor; /* mindestens 1 */
					
					player->shellsinair++;
					player->lastfiredshell = gl.ticks;
					player->c_shells--;

					PlotShell(player->x,player->y); /* Darstellen */
					msg->ret = 1;
				}
				else
					msg->ret = 0;
					
				break;

				/***********************************/
				/* Rechenzeit behaltende Kommandos */
				/***********************************/

			case AR_CMD_GETXY:		/* eingene Position */
				/* Return arg1=x arg2=y */
				msg->arg1 = player->x/200;
				msg->arg2 = player->y/200;
				break;

			case AR_CMD_TRANSMIT:	/* Nachricht versenden */
				for(locp=FirstPlayer();
					ValidPlayer(locp);
					locp=NextPlayer(locp))
				{
					if(msg->arg1+1 == locp->id &&	/* Robot vorhanden */
						player->batteries > 10 &&	/* und Sendeenergie */
						locp->c_armour > 0 &&			/* und am Leben */
						locp->mes_numbers < MAX_MESSAGES)	/* und Queue nicht voll */ 
					{
						locp->message_queue[locp->mes_free].id =
							player->id;
						locp->message_queue[locp->mes_free].value =
							msg->arg2;
						locp->mes_numbers++;
						locp->mes_free++;
						locp->mes_free %= MAX_MESSAGES;
						player->batteries -= 10;
						msg->ret = 1;
						break;
					} /* if message sent */
				}/* for all players */
				break;

			case AR_CMD_RECEIVE:		/* Nachricht empfangen */
				/* Return: ret=dataexists arg1=id arg2=data */
				if(player->mes_numbers > 0)
				{
					i=player->mes_free - player->mes_numbers;

					if(i<0) /* Überlauf */
					{
						i+=MAX_MESSAGES;
					}
					player->mes_numbers--;
					msg->arg1 = player->message_queue[i].id-1;
					msg->arg2 = player->message_queue[i].value;
					msg->ret = 1;
				}
				break;
				
			case AR_CMD_DAMAGE:		/* Damagestatus */
				msg->ret = player->c_armour;
				break;

			case AR_CMD_SPEED:		/* Geschwindigkeitsstatus */
				msg->ret = player->c_speed;
				break;

			case AR_CMD_BATTERY:		/* Batteriestatus */
				msg->ret = player->batteries/10;
				break;

			case AR_CMD_TICKS:		/* Spielrunden */
				msg->ret = gl.ticks;
				break;

			case AR_CMD_SIN:			/* Sinus */
				msg->ret = (long) vec_sin[makewinkel(msg->arg1)];
				break;

			case AR_CMD_COS:			/* Cosinus */
				msg->ret = (long) vec_cos[makewinkel(msg->arg1)];
				break;

			case AR_CMD_TAN:			/* Tangens */
				msg->ret = (long) vec_tan[makewinkel(msg->arg1)];
				break;

			case AR_CMD_ATAN:			/* inverser Tangens */
				msg->ret = (short)(atan(msg->arg1/100000.0)*180.0/PI);
				break;

			case AR_CMD_SQRT:			/* Quadratwurzel */
				msg->ret = (long) sqrt((double)msg->arg1);
				break;

			case AR_CMD_SETPAT:		/* Pattern setzen */
				/* void */
				DisplayPattern(player);	/* Alte Darstellung löschen */
				for(i=0,pc=(char *)msg->arg1; i<5; i++)
				{
					player->pattern[i]=pc[i];
				}
				DisplayPattern(player);	
				break;

			case AR_CMD_DEBUG:		/* Debugflags ändern */
				/* void */
				player->debug[msg->arg1 & 1]=msg->arg2 & 1;
				break;

			case AR_CMD_BARMOUR:		/* Panzerung ändern */
				/* void */
				if(msg->arg1 >= 0)	/* Panzerung kaufen */
				{
					i = min(msg->arg1, player->batteries / 500);
					player->batteries -= 500 * i;
					player->c_armour += i;
					player->c_armour = min(player->armour, player->c_armour);
				}
				else	/* verkaufen */
				{
					i = min(-msg->arg1,player->c_armour);
					player->c_armour -= i;
					player->batteries += i * 500;
					player->batteries = min(player->batteries,10000);
					player->lasthitby = 0;
				}
				break;
				
			case AR_CMD_BSHELLS:		/* Munition kaufen */
				/* void */
				if(msg->arg1 > 0)
				{
					i = min(msg->arg1, player->batteries / (100 * msg->arg1));
					player->batteries -= 100 * i;
					player->c_shells += i;
					player->c_shells = min(player->shells, player->c_shells);
				}
				break;

			case AR_CMD_SHELLS:		/* Anzahl Munition */
				msg->ret = player->c_shells;
				break;

			case AR_CMD_MAP:			/* Umgebung erkunden */
				/* void */
				pc = (char *) msg->arg1;
				for(dy=0;dy<9;dy++)	/* Zeilen */
					for(dx=0;dx<9;dx++)
					{
						if(player->y/200+dy-4 < 0 ||	/* Randabfrage */
							player->x/200+dx-4 < 0 ||
							player->y/200+dy-4 > 999 ||
							player->x/200+dx-4 > 999)
						{
							pc[dy*9+dx] = ARENA_WALL;
						}
						else	/* gültige Position */
						{
							pc[dy*9+dx] = 
								gl.arena[(player->y/200+dy-4)/10][(player->x/200+dx-4)/10];
						}
					}
				break;

			case AR_CMD_SINV:			/* Unsichtbar machen */
				/* void */
				if(msg->arg1 & 1 == 1) /* unsichtbar machen */
				{
					if(player->invisibility != 0 &&	/* Tarnvorrichtung */
						player->invisible == 0 &&		/* inaktiv */
						player->inv_count == 0)			/* und reaktivierbar */
					{
						player->invisible = 1;
					}
				}
				else	/* sichtbar machen */
				{
					player->invisible = 0;
				}
				break;

			case AR_CMD_SHELLSTAT:	/* letzten Schuß abfragen */
				msg->ret = player->lastshellstatus;
				player->lastshellstatus = SHELL_NOT_AVAIL;
				break;

			case AR_CMD_GINV:			/* Status ob unsichtbar */
				msg->ret = player->invisible;
				break;
				
			case AR_CMD_ATAN2:		/* invers Tangens y/x */
				msg->ret = (short)
					(atan2((double)msg->arg1,(double)msg->arg2)*180.0/PI);
				break;

			/* Erweiterungen ab 1.3, nicht PCRobots-kompatibel */

			case AR_CMD_FLYING_SHELLS:		/* In der Luft befindliche Kugeln */
				msg->ret = 
					player->shellsinair;
				break;

			case AR_CMD_BIG_MAP:			/* Umgebung erkunden, aber gleich in 10er-Feldern */
				/* void */
				pc = (char *) msg->arg1;
				for(dy=0;dy<9;dy++)	/* Zeilen */
					for(dx=0;dx<9;dx++)
					{
						if(player->y/200+dy*10-40 < 0 ||	/* Randabfrage */
							player->x/200+dx*10-40 < 0 ||
							player->y/200+dy*10-40 > 999 ||
							player->x/200+dx*10-40 > 999)
						{
							pc[dy*9+dx] = ARENA_WALL;
						}
						else	/* gültige Position */
						{
							pc[dy*9+dx] = 
								gl.arena[(player->y/200+dy*10-40)/10][(player->x/200+dx*10-40)/10];
						}
					}
				break;

			case AR_CMD_HIT_FROM:		/* Getroffen worden aus Richtung */
				msg->ret = 
					player->hit_from;
				player->hit_from = SHELL_NOT_AVAIL;
				break;

			case AR_CMD_ASIN:			/* Arc-Sinus */
				msg->ret = (short)(asin(msg->arg1/100000.0)*180.0/PI);
				break;

			case AR_CMD_ACOS:			/* Arc-Cosinus */
				msg->ret = (short)(acos(msg->arg1/100000.0)*180.0/PI);
				break;

			case AR_CMD_HIGHESTPLAYERID:	/* höchste Playerid */
				msg->ret = (short)gl.id;
				break;

			case AR_CMD_MYID:
				msg->ret = (short)player->id;
				break;

			case AR_CMD_VERSION:
				msg->ret = (short)gl.version;
				break;

			default:
				break;
			}
			if(player == gl.status_id && msg->cmd < 16)
			{
				gl.st_cmd = msg->cmd;
				gl.st_arg[0] = msg->ret;
				gl.st_arg[1] = msg->arg1;
				gl.st_arg[2] = msg->arg2;
				gl.st_arg[3] = msg->arg3;
			}
			cmd = msg->cmd;
			ReplyMsg((struct Message *)msg);
		}/* EndIF (msg) */
	}
	while(cmd >= 16 && player->c_armour > 0 && !gl.playerentfernt);

	player->MPort.mp_SigBit = 0;
}

void DoSim(void)
{
	register struct RobotPlayer *player;
	short i, neednewinc;
	long newx, newy, newx2, newy2;

	for(player = FirstPlayer();	
		ValidPlayer(player);
		player = NextPlayer(player)
	)
	{
		if(player->shellsinair > 0) /* solange shells in der Luft sind, */
				/* unabhängig davon, ob der Roboter noch lebt oder nicht */
		{
			/* Animation der shells */
			for(i=0; i<MAXSHELLS_IN_AIR; i++)
			{
				if(player->flying_shell[i].xpos >= 0)
				{
					newx = player->flying_shell[i].xpos;
					newy = player->flying_shell[i].ypos;
					PlotShell(newx,newy); /* löschen */

					/* 2 mal inkrementieren, weil speed=400 */
					newx += player->flying_shell[i].xinc;
					newy += player->flying_shell[i].yinc;
					newx2 = newx + player->flying_shell[i].xinc;
					newy2 = newy + player->flying_shell[i].yinc;

					/* Kollisionsabfrage erster inc für Shell */
					if(newx < 0 ||
						newy < 0 ||
						newx > 199999 ||
						newy > 199999 ||
						(gl.pcrobots)? /* bei PCRobots-komatibilität auch an Hindernissen enden */
						gl.arena[newy/2000][newx/2000] != ARENA_FREE:
						gl.arena[newy/2000][newx/2000] == ARENA_WALL /* sonst nur an Wänden */
						)
					{
						player->flying_shell[i].xpos = -1;
						player->shellsinair--;
						player->lastshellstatus = SHELL_WALL;
					}
					else	/* noch nicht am Ziel? */
					if(--player->flying_shell[i].times > 0) 
					{
						if(/* zweiten inc checken */
							newx2 < 0 ||
							newy2 < 0 ||
							newx2 > 199999 ||
							newy2 > 199999 ||
							(gl.pcrobots)? /* bei PCRobots-komatibilität auch an Hindernissen enden */
							gl.arena[newy2/2000][newx2/2000] != ARENA_FREE:
							gl.arena[newy2/2000][newx2/2000] == ARENA_WALL /* sonst nur an Wänden */
							)
						{
							player->flying_shell[i].xpos = -1;
							player->shellsinair--;
							player->lastshellstatus = SHELL_WALL;
						}
						else /* zweiter inc ok */
						if(--player->flying_shell[i].times > 0)
						{
							player->flying_shell[i].xpos = newx2;
							player->flying_shell[i].ypos = newy2;
							PlotShell(newx2,newy2); /* An neuer Pos zeichnen */
						}
						else /* beim zweiten inc am Ende */
						{
							player->flying_shell[i].xpos = -1;
							player->shellsinair--;
							player->lastshellstatus =
								DoExplosion(newx2,newy2,player);
						}
					}
					else	/* schon beim ersten inc am Ende */
					{
						player->flying_shell[i].xpos = -1;
						player->shellsinair--;
						player->lastshellstatus =
							DoExplosion(newx,newy,player);
					}
				}/* if valid shell */
			}/* for all Shells: anim */
		}/* if solange noch shells fliegen */
		
		/* Wenn Roboter am Leben ist */
		if(player->c_armour > 0 && player->id > 0)
		{
			if(player->batteries < gl.timefactor* (gl.pcrobots? /* Energieüberschuß für Beschl. */
						player->c_speed:									/* PCRobots */
						player->c_speed*player->c_speed/100))		/* AmigaRobots */
			{
				player->d_speed = gl.pcrobots?	/* Geschwindigkeit auf Machbar begrenzen */
					player->batteries:
					(short)sqrt(player->batteries*100.0);
			}
			/* Wird sich die Geschwindigkeit verändern? */
			if(player->c_speed != player->d_speed)
			{
				neednewinc = 1;

				/* Beschleunigung */
				if(player->c_speed < player->d_speed) /* Wenn höhere Geschwindigkeit gewünscht */
				{
					i = min(min(player->acceleration,player->batteries/5), 
							player->d_speed - player->c_speed)*gl.timefactor;
 
					player->c_speed += i;
					player->batteries -= i/5;
				}
				else /* Langsamer werden */
				{
					player->c_speed -= 
						min(player->acceleration, 
							player->c_speed - player->d_speed)*gl.timefactor;
				}
			}
			else
				neednewinc = 0;	/* sonst init neednewinc */

			/* Das Fahren kostet auf jeden Fall Batterien */
			if(gl.pcrobots)
				player->batteries -= player->c_speed*gl.timefactor; /* bei PCRobots linear */
			else
				player->batteries -= (player->c_speed*player->c_speed/100)* /* bei AR quadratisch */
					gl.timefactor;

			if(player->batteries < 0)	/* leer */
			{
				player->batteries = 0;
				player->d_speed = 0;	
				/* neednewinc = 1; */
			}
			
			/* Lenkung wenn lenkbar (Manövierfähigkeit in Prozent) */
			if(player->c_speed <= player->speed*player->manouevre/100 &&
				player->c_angle != player->d_angle)
			{
				player->c_angle = player->d_angle;
				neednewinc = 1;
			}
			
			/* Neue Inkrementwerte berechnen */
			if(neednewinc != 0)	/* Nur wenn nötig, da sin und cos */
			{
				player->xinc = (short)
					(vec_cos[player->c_angle]*player->c_speed*gl.timefactor/100000);
				player->yinc = (short)
					(vec_sin[player->c_angle]*player->c_speed*gl.timefactor/100000);
			}
			
			/* Kollisionscheck mit Wand */
			newx = player->x+player->xinc;	/* Neue Position berechnen */
			newy = player->y+player->yinc;
			if(newx < 0 ||
				newy < 0 ||
				newx > 199999 ||
				newy > 199999 ||
				gl.arena[newy/2000][newx/2000] == ARENA_WALL
			)	/* unglültige Position */
			{
				player->xinc = 0;
				player->yinc = 0;
				player->c_speed = 0;
				player->d_speed = 0;
				player->c_angle = 0;
				player->d_angle = 0;
				player->lasthitby = 0;	/* Selbsttreffer */
				player->c_armour = max(0,player->c_armour-1);
			}
			else	/* Gültige Position einnehmen */
			{ 	
				/* Wenn andere Position */
				if(player->x != newx || player->y != newy)
				{
					DisplayPattern(player); /* löschen */
					player->x = newx;
					player->y = newy;
					DisplayPattern(player); /* und neu zeichnen */
				}
			}
			
			/* unsichtbar */
			if(player->invisible > 0)
			{
				if(player->inv_count <= 100)
				{
					player->inv_count++;
				}
				else	/* Maximale Zeit vorüber */
				{
					player->invisible = 0;	/* wieder sichtbar */
				}
			}
			else	/* Sichtbar */
			{
				if(player->inv_count > 0)	/* langsam erholen lassen */
					player->inv_count--;

				/* Batterien laden sich wieder auf, wenn sichtbar */
				player->batteries += gl.reload*gl.timefactor;	/* Normalerweise um 4 */

				/* Refuelstation */
				newx = player->x/2000;
				newy = player->y/2000;
				if(gl.arena[newy][newx] == ARENA_REFUEL)
				{
					for(i=0;i<gl.refuelpoints;i++)
					{
						if(newx == gl.refuel[i].x &&
							newy == gl.refuel[i].y)
						{
							/* nicht Refuel aufladen, wenn Robot sich auflädt */
							gl.refuel[i].robotloading = 1;
							/* Batterien laden */
							player->batteries = min(10000,
								player->batteries+min(100,gl.refuel[i].load)*gl.timefactor);
							gl.refuel[i].load = max(0,gl.refuel[i].load-100*gl.timefactor);
							break; /* Schleife abbrechen */
						}
					}/* for alle Refuelpoints */
				}/* wenn auf einem Refuelpoint */
				player->batteries = min(10000,player->batteries);	/* nur bis 10000 */
			}/* if un/sichtbar */

			/* Damagetrap */
			if(gl.arena[player->y/2000][player->x/2000] == ARENA_DAMAGE)
			{
				player->c_armour = max(player->c_armour-gl.timefactor,0);
				player->lasthitby = 0;
			}
		} /* endif Spieler am Leben */
		else	/* Spieler tot */
		if(player->id > 0) /* aber noch nicht für tot erklärt */
		{							/* und begraben... */
			DisplayPattern(player); /* löschen */
			for(i=0; i<5; i++)
			{
				player->pattern[i] = dead_pat[i];
			}
			DisplayPattern(player); /* neu zeichnen */
			ShowRobots();
			player->id = -player->id; /* R.I.P */
			player->dietime = gl.ticks;
			gl.PlayersAlive--;
		}/* tot und begraben */
	}/* for alle Spieler */

	/* Refuelpoints aufladen */
	for(i=0; i<gl.refuelpoints; i++)
	{
		if(gl.refuel[i].robotloading == 0)
		{
			gl.refuel[i].load = min(20000,gl.refuel[i].load+50*gl.timefactor);
		}
		gl.refuel[i].robotloading = 0;
	}
	gl.ticks+=gl.timefactor;
}

/*
	ShowResult
	zeigt den bisherigen Stand an
	gibt 0 zurück, wenn die Simulation beendet werden soll, 1, wenn
	sie fortgeführt werden soll.
*/

int ShowResult(void)
{
	char *pc;
	short i,ret;
	struct RobotPlayer *player;

	ret = 0;
	/* Ergebnis ausgeben */
	if( (pc = malloc(80*(gl.NumberPlayers+1)+130+FCHARS)) )
	{
		sprintf(pc,
			"AmigaRobots V" VERSION " von Hanno Behrens © " DATUM "\n"
			"Arena: %s\n"
			"Roboteranzahl: %d\n"
			"Runden Stop Munition Aufladen Schnell Vorpositionen\n"
			"%6d  %3s   %4d      %4d    %3s    %3s\n"
			" ID  Panzerung  Züge Treffer  Munition Name\n",
			gl.arenaname,
			gl.NumberPlayers,
         gl.maxticks,
         gl.stoponone?" an":"aus",
         vec_shells[0],
         gl.reload,
         gl.quickmode?" an":"aus",
         gl.prepos?" an":"aus"
		);
		for(player=FirstPlayer();
			ValidPlayer(player);
			player=NextPlayer(player))
		{
			i=strlen(pc);
			sprintf(pc+i,"%3d%c %4d/%4d %5d    %3d%c %4d/%4d %s\n",
				abs(player->id),
				(player->id>0)?'+':'-',
				player->c_armour,
				player->armour,
				(player->id>0)?gl.ticks:player->dietime,
				abs(player->lasthitby),
				(player->lasthitby < 0)?'P':(player->lasthitby == 0)?'V':' ',
				player->c_shells,
				player->shells,
				player->name);
		}/* for alle Spieler */
		if(gl.outfile)
			fputs(pc,gl.outfile);
		else
			puts(pc);

		if(!gl.quickmode)
			ret = TwoGadRequest("Ergebnis","%s\n\nSoll ich die Simulation fortsetzen?",pc);

		free(pc);
	}
	return ret;
}

void procargs(int argc, union{char **args; struct WBStartup *msg;}argv)
{
	int i;
	struct WBArg *arg;
	BPTR olddir;
	struct DiskObject *lock;
	char *val,**toolarray;
	long x,y;	/* Longwerte für sscanf */

	if(argc>0)	
	{
		for(i=1;i<argc;i++)
		{
			switch(argv.args[i][0])
			{
				case '-':	/* Kommandozeilenoption */
					switch(tolower(argv.args[i][1]))
					{
						case 'd':	/* Kein Stop nach einem */
							gl.stoponone = FALSE;
							break;
						case 'l':	/* Spielrunden */
							gl.maxticks = atoi(&argv.args[i][2]);
							gl.maxticks = abs(gl.maxticks);
							break;
						case 'o':	/* Ausgabe nach File  */
							gl.outfile = fopen(&argv.args[i][2],"a");
							break;
						case 'a':	/* Arenafile */
							strcpy(gl.pathname,&argv.args[i][2]);
							break;
						case 's':	/* Einzelschritt */
							gl.singlestep = TRUE;
							break;
						case 'q':	/* Quickmode */
							gl.quickmode = TRUE;
							break;
						case 'p':	/* Prepos an */
							gl.prepos = TRUE;
							break;
						case 'x':	/* schnelle Explosion aus */
							gl.fastexplosion = FALSE;
							break;
						case 'v':	/* Verzögerung bei Explosionsdarstellung */
							gl.exp_darstellen = atoi(&argv.args[i][2]);
							gl.exp_darstellen = abs(gl.exp_darstellen);
							break;
						case 'r':	/* Aufladefaktor */
							gl.reload = atoi(&argv.args[i][2]);
							gl.reload = abs(gl.reload);
							break;
						case 'm':	/* Munition */
							vec_shells[0] = atoi(&argv.args[i][2]);
							vec_shells[0] = abs(vec_shells[0]);
							if(gl.pcrobots == TRUE)
								vec_shells[1] = vec_shells[0]*9/10;
							else
								vec_shells[1] = vec_shells[0]/2;
							break;
						case 'c':	/* PCRobots Schadens- und Tankfelder */
							gl.pcrobots = TRUE;
							break;
						case 't':	/* Zeitfaktor */
							gl.timefactor = atoi(&argv.args[i][2]);
							gl.timefactor = abs(gl.timefactor);
							gl.timefactor = max(1,gl.timefactor);
							break;
						case 'i':	/* MonitorID */
							sscanf(&argv.args[i][2],"%lx,%d,%d,%d,%d",&gl.monitorID,
								&gl.Width,&gl.Height,&x,&y);
							gl.Width = max(gl.Width,640);
							gl.Height = max(gl.Height,512);
							gl.rect.MaxX = min(x,gl.Width);
							gl.rect.MaxY = min(y,gl.Height);
							break;
#ifdef OWNFONT
						case 'f':	/* Font */
							sscanf(&argv.args[i][2],"%s,%d",gl.textattr.ta_Name,&gl.textattr.ta_YSize);
							break;
#endif
						default:		/* Hilfe */
							printf("\033[1mAmigaRobots V" VERSION "\033[m\n"
								"von Hanno Behrens, © " DATUM "\n"
								"Aufruf: AmigaRobots [-OPT] [ROBOTS..]\n"
								"Optionen\n"
								"  d       - Kein Stop bei einem Überlebenden\n"
								"  l<nr>   - Spielrunden\n"
								"  o<file> - Ausgabe auf File\n"
								"  a<file> - Arenafile\n"
								"  s       - Einzelschritt\n"
								"  p       - Präpositionen nutzen\n"
								"  x       - schöne, langsame Explosionen\n"
								"  v       - Verzögerung bei schneller Explosion\n"
								"  q       - Schnellmodus mit einem Durchlauf\n"
								"  r<nr>   - Ladegechwindigkeit der Batterien\n"
								"  m<nr>   - Munition\n"
								"  c       - PCRobots kompatible Schadens- und Tankfelder\n"
								"  t<fac>  - Zeitfactor\n"
								"  i<id(hex),Schirm_Breite, Höhe,Sichtbar_Breite, Höhe>\n"
								"          - ab OS 2.0 MonitorID\n"
#ifdef OWNFONT
								"  f<Font,Größe> - Font\n"
#endif
							);
							cleanup("Hilfe",0);
							break;
					}
					break;
				default:
					sprintf(gl.buffer,"run >nil: %s",argv.args[i]);
					system(gl.buffer);
					Delay(2);
					GetNewRobots();
					break;
			}
		}
	}/* if cli */
	else	/* Workbench */
	{
		IconBase = OpenLibrary(ICONNAME,0);
		if(IconBase == NULL) return;
		
		for(i=0,arg=argv.msg->sm_ArgList;
			i < argv.msg->sm_NumArgs;
			i++,arg++)
		{
			if(i == 0)	/* Workbenchparameter */
			{
				if(arg->wa_Lock)	/* wenn Parameter existieren */
				{
					olddir = CurrentDir(arg->wa_Lock);
					if( (lock = GetDiskObject(arg->wa_Name)) )
					{
						toolarray = lock->do_ToolTypes;

						val = FindToolType(toolarray,"DEBUG");
						if(MatchToolValue(val,"ON"))
							gl.stoponone = FALSE;
						else
							gl.stoponone = TRUE;

						val = FindToolType(toolarray,"TICKS");
						if(val)
						{
							gl.maxticks = atoi(val);
							gl.maxticks = abs(gl.maxticks);
						}

						val = FindToolType(toolarray,"FILE");
						if(val)
							gl.outfile = fopen(val,"a");

						val = FindToolType(toolarray,"ARENA");
						if(val)
							strcpy(gl.pathname,val);

						val = FindToolType(toolarray,"SINGLESTEP");
						if( MatchToolValue(val,"ON"))
							gl.singlestep = TRUE;
						else
							gl.singlestep = FALSE;

						val = FindToolType(toolarray,"QUICKMODE");
						if( MatchToolValue(val,"ON"))
							gl.quickmode = TRUE;
						else
							gl.quickmode = FALSE;

						val = FindToolType(toolarray,"PREPOS");
						if( MatchToolValue(val,"ON"))
							gl.prepos = TRUE;
						else
							gl.prepos = FALSE;

						val = FindToolType(toolarray,"FASTEXPLOSION");
						if( MatchToolValue(val,"ON"))
							gl.fastexplosion = TRUE;
						else
							gl.fastexplosion = FALSE;

						val = FindToolType(toolarray,"EXP_TICKS");
						if(val)
						{
							gl.exp_darstellen = atoi(val);
							gl.exp_darstellen = abs(gl.exp_darstellen);
						}
						
						val = FindToolType(toolarray,"RELOAD");
						if(val)
						{
							gl.reload = atoi(val);
							gl.reload = abs(gl.reload);
						}
						
						val = FindToolType(toolarray,"SHELLS");
						if(val)
						{
							vec_shells[0] = atoi(val);
							vec_shells[0] = abs(vec_shells[0]);
							if(gl.pcrobots=TRUE)
								vec_shells[1] = vec_shells[0]*9/10;
							else
								vec_shells[1] = vec_shells[0]/2;
						}
						
						val = FindToolType(toolarray,"PCROBOTWALLS");
						if( MatchToolValue(val,"ON"))
							gl.pcrobots = TRUE;
						else
							gl.pcrobots = FALSE;

						val = FindToolType(toolarray,"TIMEFACTOR");
						if(val)
						{
							gl.timefactor = atoi(val);
							gl.timefactor = abs(gl.timefactor);
							gl.timefactor = max(1,gl.timefactor);
						}
						
						val = FindToolType(toolarray,"MONITORID");
						if(val)
						{
							sscanf(val,"%lx,%d,%d,%d,%d",&gl.monitorID,&gl.Width,&gl.Height,&x,&y);
							gl.Width = max(gl.Width,640);
							gl.Height = max(gl.Height,512);
							gl.rect.MaxX = min(x,gl.Width);
							gl.rect.MaxY = min(y,gl.Height);
						}

#ifdef OWNFONT
						val = FindToolType(toolarray,"FONT");
						if(val)
						{
							sscanf(val,"%s,%d",gl.textattr.ta_Name,&gl.textattr.ta_YSize);
						}
#endif

						FreeDiskObject(lock);
					}
					CurrentDir(olddir);
				}
			}
			else	/* Weitere Icons */
			{
				if(arg->wa_Lock)	/* wenn Parameter existieren */
				{
					olddir = CurrentDir(arg->wa_Lock);
					if( (lock = GetDiskObject(arg->wa_Name)) )
					{
						toolarray = lock->do_ToolTypes;

						val = FindToolType(toolarray,"FILETYPE");
						if(MatchToolValue(val,"ARENA"))
						{
							sprintf(gl.pathname,"Arena/%s",arg->wa_Name);
						}
						else
						{
							sprintf(gl.buffer,"run >nil: %s",arg->wa_Name);
							system(gl.buffer);
							Delay(2);
							GetNewRobots();
						}

						FreeDiskObject(lock);
					}
					CurrentDir(olddir);
				}
			}
		}/* for all i < numargs */
		
		CloseLibrary(IconBase);
	}
}

int main(int argc,union{char **args; struct WBStartup *msg;}argv)
{
	struct RobotPlayer *player;

	IntuitionBase = (struct IntuitionBase *)
							OpenLibrary("intuition.library", 0);
	if (IntuitionBase == NULL)
	{
		cleanup("Keine intuition.library",20);
	}
	GfxBase = (struct GfxBase *)
					OpenLibrary("graphics.library", 0);
	if (GfxBase == NULL)
	{
		cleanup("Keine graphics.library",20);
	}

	init_master(); /* Port einrichten */
	loadcolors();

	strcpy(gl.pathname,DEFAULTFILE);
	procargs(argc,argv);

	openscreen(); /* Screen und Window öffnen */
	initreq();

	initglobals(); /* Globale Variablen initialisieren */
	loadarena(gl.pathname);


	FOREVER
	{
		/* Phase eins: Spieler annehmen */
		makemenu();
		DisplayArena();
		SetRast(gl.RobotWindow->RPort,0); /* löschen */

		if(!gl.quickmode)
		{
			do
			{
				Wait(gl.maskarena |
					gl.maskrobot	|
					gl.masklogon	|
					gl.maskconf		|
					gl.maskstatus);
						
				do_menu();
				GetNewRobots();
				ShowRobots();
			}
			while(gl.running == FALSE);
		}
		else
		{
			ShowRobots();
			gl.running = TRUE;
			makemenu();
		}

		/* Phase zwei: Spieler treten an */
		SetRandomPosForAll();

		/* bis Menü abbricht oder einer übrig ist */
		while(1)
		{
			if(!gl.running ||
				(gl.maxticks && gl.maxticks <= gl.ticks) ||
				(gl.stoponone && gl.PlayersAlive < 2))
			{
				if(ShowResult())	/* Weitermachen? */
				{
					/* Weiterlaufen lassen */
					gl.running = TRUE;
					/* Wenn Maxticks überschritten wurde, unendlich */
					if(gl.maxticks && gl.maxticks <= gl.ticks)
					{
						gl.maxticks = 0;
					}
					/* Wenn nur noch einer am Leben ist... */
					if(gl.stoponone && gl.PlayersAlive < 2)
					{
						/* stoponone ausschalten */
						gl.stoponone = FALSE;
					}
					makemenu();
				}
				else	/* Spiel beenden */
				{
					break;
				}
				
			}
			
			for(	/* Für alle gültigen Spieler */	
				player = FirstPlayer(),gl.playerentfernt = FALSE; 
				ValidPlayer(player) && !gl.playerentfernt; 
				player = NextPlayer(player)) 
			{	
				if(player->c_armour > 0) GetCmds(player);
			}

			do_menu();
			if(gl.singlestep == TRUE)
			{
				Wait(gl.maskarena|gl.maskstatus);
				do_menu();
			}
			
			DoSim();
			ShowRobots();
			if(gl.delay) Delay(gl.delay);	/* Verzögerung */
		}/* while running... */

		gl.running = FALSE;

		/* Phase drei: Spielende */

		RemoveAllPlayers();	/* Alle noch aktiven Spieler löschen */
		initglobals();

		if(gl.quickmode) cleanup("Quickmode",0);
	} /* FOREVER */
} /* MAIN */

