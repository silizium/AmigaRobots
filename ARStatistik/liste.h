/*
	Headerdatei für die Implementation einer Liste als
	abstraktem Datentyp in C
	(C) 1995 Hanno Behrens
	h.behrens@amtrash.comlink.de

	Version 1.1  30.1.96 savelist, loadlist, setparameters, getparameters
					Versionsnummer in VER_LIST, liste.c
	
*/

#define LISTFILEHEAD "LIST"				/* Erkennung beim Speichern */

typedef struct n_node {
	struct n_node *n_succ;	/* nächster Knoten */
	struct n_node *n_pred;	/* vorheriger Knoten */
	int n_pri;					/* Knotenpriorität (optional) */
	char *n_name;				/* Knotenname (optional) */
   unsigned int n_size;		/* Größe des Datenfeldes */
	char n_data[0];			/* hier beginnt das Datenfeld */
} NODE;

typedef struct {
	NODE *l_head;				/* Zeiger auf ersten Knoten */
	NODE *l_tail;				/* Immer NULL */
	NODE *l_tailpred;		/* Zeiger auf letzten Knoten */
	NODE *l_curr;				/* Zeiger auf momentanen Knoten oder NULL bei leer */
	unsigned int l_length;	/* Länge der Liste (Anzahl Knoten) */
} LIST;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
typedef int BOOL;

#ifdef __STDC__
LIST *newlist(void);
NODE *newnode(void *data, unsigned int size, int nodepri, char *name);
BOOL empty(LIST *l);
unsigned int listlen(LIST *l);
unsigned int getsize(NODE *n);
void *getdata(NODE *n);
BOOL putdata(NODE *n, void *data, unsigned int size);
char *getname(NODE *n);
NODE *setname(NODE *N, char *);
int getpri(NODE *n);
NODE *setpri(NODE *n, int pri);
NODE *getcurr(LIST *l);
NODE *setcurr(LIST *l, NODE *n);
NODE *getfirst(LIST *l);
NODE *getlast(LIST *l);
NODE *getnext(LIST *l);
NODE *getprev(LIST *l);
BOOL delnode(NODE *n);
NODE *addhead(LIST *l, NODE *n);
NODE *addtail(LIST *l, NODE *n);
NODE *insert(LIST *l, NODE *new);
NODE *removecurr(LIST *l);
NODE *remhead(LIST *l);
NODE *remtail(LIST *l);
BOOL clearlist(LIST *l);
BOOL dellist(LIST *l);
NODE *enqueue(LIST *l, NODE *n);
NODE *findname(LIST *l, char *name);
LIST *sortlist(LIST *l, int (*cmp)(NODE **, NODE **));
NODE *enqueuecmp(LIST *l, NODE *n, int (*cmp)(NODE *, NODE *));
/* V 1.1 */
int savelist(LIST *l, char *filename);
LIST *loadlist(char *filename);
void setparameters(char *save, char *load, char *tempdir);
void getparameters(char **save, char **load, char **tempdir);
#else
LIST *newlist();
NODE *newnode();
BOOL empty();
unsigned int listlen();
unsigned int getsize();
char *getdata();
BOOL putdata();
char *getname();
NODE *setname();
int getpri();
NODE *setpri();
NODE *getcurr();
NODE *setcurr();
NODE *getfirst();
NODE *getlast();
NODE *getnext();
NODE *getprev();
BOOL delnode();
NODE *addhead();
NODE *addtail();
NODE *insert();
NODE *removecurr();
NODE *remhead();
NODE *remtail();
BOOL clearlist();
BOOL dellist();
NODE *enqueue();
NODE *findname();
LIST *sortlist();
NODE *enqueuecmp();
/* V1.1 */
int savelist();
LIST *loadlist();
void setparameters();
void getparameters();
#endif
