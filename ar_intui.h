
struct TextAttr TOPAZ80 = {
	(STRPTR)"topaz.font",
	TOPAZ_EIGHTY,0,0
};
struct NewScreen NewScreenStructure = {
	0,0,	/* screen XY origin relative to View */
	640,512,	/* screen width and height */
	4,	/* screen depth (number of bitplanes) */
	0,1,	/* detail and block pens */
	LACE+HIRES,	/* display modes for this screen */
	CUSTOMSCREEN,	/* screen type */
	&TOPAZ80,	/* pointer to default screen font */
	NULL,	/* screen title */
	NULL,	/* first in list of custom screen gadgets */
	NULL	/* pointer to custom BitMap structure */
};

#define NEWSCREENSTRUCTURE NewScreenStructure

USHORT Palette[] = {
	0x0AAA,	/* color #0 */
	0x0000,	/* color #1 */
	0x0EEE,	/* color #2 */
	0x068B,	/* color #3 */
	0x0FFF,	/* color #4 */
	0x0FF0,	/* color #5 */
	0x0F00,	/* color #6 */
	0x00D0,	/* color #7 */
	0x0C00,	/* color #8 */
	0x000C,	/* color #9 */
	0x0090,	/* color #10 */
	0x0807,	/* color #11 */
	0x0069,	/* color #12 */
	0x0050,	/* color #13 */
	0x0952,	/* color #14 */
	0x0B72	/* color #15 */
#define PaletteColorCount 16
};

#define PALETTE Palette

UBYTE UNDOBUFFER[12];

struct IntuiText ArenaIText1 = {
	3,1,COMPLEMENT,	/* front and back text pens, drawmode and fill byte */
	0,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"dummy",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem1 = {
	NULL,	/* next MenuItem structure */
	0,0,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	40,8,	/* hit box width and height */
	ITEMTEXT+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText1,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	NULL,	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct Menu ArenaMenu5 = {
	NULL,	/* next Menu structure */
	312,0,	/* XY origin of Menu hit box relative to screen TopLeft */
	79,0,	/* Menu hit box width and height */
	NULL,	/* Menu flags */
	"Entfernen",	/* text of Menu name */
	&ArenaMenuItem1	/* MenuItem linked list pointer */
};

struct IntuiText ArenaIText2 = {
	3,1,COMPLEMENT,	/* front and back text pens, drawmode and fill byte */
	0,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"dummy",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem2 = {
	NULL,	/* next MenuItem structure */
	0,0,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	40,8,	/* hit box width and height */
	ITEMTEXT+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText2,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	NULL,	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct Menu ArenaMenu4 = {
	&ArenaMenu5,	/* next Menu structure */
	250,0,	/* XY origin of Menu hit box relative to screen TopLeft */
	55,0,	/* Menu hit box width and height */
	NULL,	/* Menu flags */
	"Status",	/* text of Menu name */
	&ArenaMenuItem2	/* MenuItem linked list pointer */
};

struct IntuiText ArenaIText3 = {
	3,1,COMPLEMENT,	/* front and back text pens, drawmode and fill byte */
	0,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"dummy",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem3 = {
	NULL,	/* next MenuItem structure */
	0,0,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	40,8,	/* hit box width and height */
	ITEMTEXT+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText3,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	NULL,	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct Menu ArenaMenu3 = {
	&ArenaMenu4,	/* next Menu structure */
	172,0,	/* XY origin of Menu hit box relative to screen TopLeft */
	71,0,	/* Menu hit box width and height */
	NULL,	/* Menu flags */
	"Position",	/* text of Menu name */
	&ArenaMenuItem3	/* MenuItem linked list pointer */
};

struct IntuiText ArenaIText4 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	19,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"EXplosion schnell",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem8 = {
	NULL,	/* next MenuItem structure */
	0,32,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	195,8,	/* hit box width and height */
	CHECKIT+ITEMTEXT+COMMSEQ+MENUTOGGLE+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText4,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'X',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct IntuiText ArenaIText5 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	19,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"feste Positionen",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem7 = {
	&ArenaMenuItem8,	/* next MenuItem structure */
	0,24,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	195,8,	/* hit box width and height */
	CHECKIT+ITEMTEXT+COMMSEQ+MENUTOGGLE+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText5,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'P',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct IntuiText ArenaIText6 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	19,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Halt bei einem",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem6 = {
	&ArenaMenuItem7,	/* next MenuItem structure */
	0,16,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	195,8,	/* hit box width and height */
	CHECKIT+ITEMTEXT+COMMSEQ+MENUTOGGLE+ITEMENABLED+HIGHCOMP+CHECKED,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText6,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'H',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct IntuiText ArenaIText7 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	19,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Einzelschritt",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem5 = {
	&ArenaMenuItem6,	/* next MenuItem structure */
	0,8,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	195,8,	/* hit box width and height */
	CHECKIT+ITEMTEXT+COMMSEQ+MENUTOGGLE+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText7,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'E',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct IntuiText ArenaIText8 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	19,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Start Simulation",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem4 = {
	&ArenaMenuItem5,	/* next MenuItem structure */
	0,0,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	195,8,	/* hit box width and height */
	CHECKIT+ITEMTEXT+COMMSEQ+MENUTOGGLE+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText8,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'S',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct Menu ArenaMenu2 = {
	&ArenaMenu3,	/* next Menu structure */
	78,0,	/* XY origin of Menu hit box relative to screen TopLeft */
	87,0,	/* Menu hit box width and height */
	MENUENABLED,	/* Menu flags */
	"Simulation",	/* text of Menu name */
	&ArenaMenuItem4	/* MenuItem linked list pointer */
};

struct IntuiText ArenaIText9 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	0,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Programmende",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem14 = {
	NULL,	/* next MenuItem structure */
	0,40,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	144,8,	/* hit box width and height */
	ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText9,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'Q',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct IntuiText ArenaIText10 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	0,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Farbwahl",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem13 = {
	&ArenaMenuItem14,	/* next MenuItem structure */
	0,32,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	144,8,	/* hit box width and height */
	ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText10,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'F',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct IntuiText ArenaIText11 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	0,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Konfiguration",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem12 = {
	&ArenaMenuItem13,	/* next MenuItem structure */
	0,24,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	144,8,	/* hit box width and height */
	ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText11,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'K',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct IntuiText ArenaIText12 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	0,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Arena laden",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem11 = {
	&ArenaMenuItem12,	/* next MenuItem structure */
	0,16,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	144,8,	/* hit box width and height */
	ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText12,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'A',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct IntuiText ArenaIText13 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	0,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Roboter laden",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem10 = {
	&ArenaMenuItem11,	/* next MenuItem structure */
	0,8,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	144,8,	/* hit box width and height */
	ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText13,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'L',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct IntuiText ArenaIText14 = {
	0,1,JAM1,	/* front and back text pens, drawmode and fill byte */
	0,0,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Information",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct MenuItem ArenaMenuItem9 = {
	&ArenaMenuItem10,	/* next MenuItem structure */
	0,0,	/* XY of Item hitbox relative to TopLeft of parent hitbox */
	144,8,	/* hit box width and height */
	ITEMTEXT+COMMSEQ+ITEMENABLED+HIGHCOMP,	/* Item flags */
	0,	/* each bit mutually-excludes a same-level Item */
	(APTR)&ArenaIText14,	/* Item render  (IntuiText or Image or NULL) */
	NULL,	/* Select render */
	'I',	/* alternate command-key */
	NULL,	/* SubItem list */
	MENUNULL	/* filled in by Intuition for drag selections */
};

struct Menu ArenaMenu1 = {
	&ArenaMenu2,	/* next Menu structure */
	0,0,	/* XY origin of Menu hit box relative to screen TopLeft */
	71,0,	/* Menu hit box width and height */
	MENUENABLED,	/* Menu flags */
	"Programm",	/* text of Menu name */
	&ArenaMenuItem9	/* MenuItem linked list pointer */
};

#define ArenaMenuList1 ArenaMenu1

struct NewWindow ArenaNewWindowStructure1 = {
	0,12,	/* window XY origin relative to TopLeft of screen */
	500,500,	/* window width and height */
	0,1,	/* detail and block pens */
	MENUPICK+VANILLAKEY,	/* IDCMP flags */
	BACKDROP+BORDERLESS+ACTIVATE+NOCAREREFRESH,	/* other window flags */
	NULL,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	NULL,	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	500,500,	/* minimum width and height */
	500,500,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};

struct NewWindow RobotNewWindowStructure2 = {
	500,12,	/* window XY origin relative to TopLeft of screen */
	140,500,	/* window width and height */
	0,1,	/* detail and block pens */
	ACTIVEWINDOW,	/* IDCMP flags */
	GIMMEZEROZERO+NOCAREREFRESH,	/* other window flags */
	NULL,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	NULL,	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	140,500,	/* minimum width and height */
	140,500,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};

UBYTE ConfigConfigGad_timefactorSIBuff[12];
struct StringInfo ConfigConfigGad_timefactorSInfo = {
	ConfigConfigGad_timefactorSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

SHORT ConfigBorderVectors1[] = {
	0,0,
	63,0,
	63,9,
	0,9,
	0,0
};
struct Border ConfigBorder1 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	ConfigBorderVectors1,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText ConfigIText15 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	-89,1,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Zeitfaktor",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget ConfigGad_timefactor = {
	NULL,	/* next gadget */
	100,93,	/* origin XY of hit box relative to window TopLeft */
	62,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY+STRINGRIGHT+LONGINT,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	(APTR)&ConfigBorder1,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&ConfigIText15,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&ConfigConfigGad_timefactorSInfo,	/* SpecialInfo structure */
	5,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

UBYTE ConfigConfigGad_explosionSIBuff[12];
struct StringInfo ConfigConfigGad_explosionSInfo = {
	ConfigConfigGad_explosionSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

SHORT ConfigBorderVectors2[] = {
	0,0,
	63,0,
	63,9,
	0,9,
	0,0
};
struct Border ConfigBorder2 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	ConfigBorderVectors2,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText ConfigIText16 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	-89,1,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Explosion",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget ConfigGad_explosion = {
	&ConfigGad_timefactor,	/* next gadget */
	100,78,	/* origin XY of hit box relative to window TopLeft */
	62,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY+STRINGRIGHT+LONGINT,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	(APTR)&ConfigBorder2,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&ConfigIText16,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&ConfigConfigGad_explosionSInfo,	/* SpecialInfo structure */
	4,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

UBYTE ConfigConfigGad_delaySIBuff[12];
struct StringInfo ConfigConfigGad_delaySInfo = {
	ConfigConfigGad_delaySIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

SHORT ConfigBorderVectors3[] = {
	0,0,
	63,0,
	63,9,
	0,9,
	0,0
};
struct Border ConfigBorder3 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	ConfigBorderVectors3,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText ConfigIText17 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	-89,1,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Verzögern",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget ConfigGad_delay = {
	&ConfigGad_explosion,	/* next gadget */
	100,63,	/* origin XY of hit box relative to window TopLeft */
	62,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY+STRINGRIGHT+LONGINT,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	(APTR)&ConfigBorder3,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&ConfigIText17,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&ConfigConfigGad_delaySInfo,	/* SpecialInfo structure */
	4,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

UBYTE ConfigConfigGad_shellsSIBuff[12];
struct StringInfo ConfigConfigGad_shellsSInfo = {
	ConfigConfigGad_shellsSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

SHORT ConfigBorderVectors4[] = {
	0,0,
	63,0,
	63,9,
	0,9,
	0,0
};
struct Border ConfigBorder4 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	ConfigBorderVectors4,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText ConfigIText18 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	-89,1,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Munition",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget ConfigGad_shells = {
	&ConfigGad_delay,	/* next gadget */
	100,48,	/* origin XY of hit box relative to window TopLeft */
	62,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY+STRINGRIGHT+LONGINT,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	(APTR)&ConfigBorder4,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&ConfigIText18,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&ConfigConfigGad_shellsSInfo,	/* SpecialInfo structure */
	4,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

UBYTE ConfigConfigGad_reloadSIBuff[12];
struct StringInfo ConfigConfigGad_reloadSInfo = {
	ConfigConfigGad_reloadSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

SHORT ConfigBorderVectors5[] = {
	0,0,
	63,0,
	63,9,
	0,9,
	0,0
};
struct Border ConfigBorder5 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	ConfigBorderVectors5,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText ConfigIText19 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	-89,1,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Aufladen",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget ConfigGad_reload = {
	&ConfigGad_shells,	/* next gadget */
	100,33,	/* origin XY of hit box relative to window TopLeft */
	62,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY+STRINGRIGHT+LONGINT,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	(APTR)&ConfigBorder5,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&ConfigIText19,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&ConfigConfigGad_reloadSInfo,	/* SpecialInfo structure */
	3,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

UBYTE ConfigConfigGad_ticksSIBuff[12];
struct StringInfo ConfigConfigGad_ticksSInfo = {
	ConfigConfigGad_ticksSIBuff,	/* buffer where text will be edited */
	UNDOBUFFER,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

SHORT ConfigBorderVectors6[] = {
	0,0,
	63,0,
	63,9,
	0,9,
	0,0
};
struct Border ConfigBorder6 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	ConfigBorderVectors6,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText ConfigIText20 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	-89,1,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Spielzüge",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget ConfigGad_ticks = {
	&ConfigGad_reload,	/* next gadget */
	100,18,	/* origin XY of hit box relative to window TopLeft */
	62,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY+STRINGRIGHT+LONGINT,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	(APTR)&ConfigBorder6,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&ConfigIText20,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&ConfigConfigGad_ticksSInfo,	/* SpecialInfo structure */
	2,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT ConfigBorderVectors7[] = {
	0,0,
	93,0,
	93,17,
	0,17,
	0,0
};
struct Border ConfigBorder7 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	ConfigBorderVectors7,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText ConfigIText21 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	4,4,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Übernehmen",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget ConfigGadget2 = {
	&ConfigGad_ticks,	/* next gadget */
	20,-36,	/* origin XY of hit box relative to window TopLeft */
	92,16,	/* hit box width and height */
	GRELBOTTOM,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&ConfigBorder7,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&ConfigIText21,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	1,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

SHORT ConfigBorderVectors8[] = {
	0,0,
	79,0,
	79,17,
	0,17,
	0,0
};
struct Border ConfigBorder8 = {
	-1,-1,	/* XY origin relative to container TopLeft */
	3,0,JAM1,	/* front pen, back pen and drawmode */
	5,	/* number of XY vectors */
	ConfigBorderVectors8,	/* pointer to XY vectors */
	NULL	/* next border in list */
};

struct IntuiText ConfigIText22 = {
	1,0,JAM2,	/* front and back text pens, drawmode and fill byte */
	2,4,	/* XY origin relative to container TopLeft */
	&TOPAZ80,	/* font pointer or NULL for default */
	"Verwerfen",	/* pointer to text */
	NULL	/* next IntuiText structure */
};

struct Gadget ConfigGadget1 = {
	&ConfigGadget2,	/* next gadget */
	-98,-36,	/* origin XY of hit box relative to window TopLeft */
	78,16,	/* hit box width and height */
	GRELBOTTOM+GRELRIGHT,	/* gadget flags */
	RELVERIFY,	/* activation flags */
	BOOLGADGET,	/* gadget type flags */
	(APTR)&ConfigBorder8,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	&ConfigIText22,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	NULL,	/* SpecialInfo structure */
	0,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};

#define ConfigGadgetList3 ConfigGadget1

struct NewWindow ConfigNewWindowStructure3 = {
	175,154,	/* window XY origin relative to TopLeft of screen */
	234,152,	/* window width and height */
	0,1,	/* detail and block pens */
	GADGETDOWN+GADGETUP+VANILLAKEY,	/* IDCMP flags */
	WINDOWDRAG+ACTIVATE+NOCAREREFRESH,	/* other window flags */
	&ConfigGadget1,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	"Konfiguration",	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	5,5,	/* minimum width and height */
	640,512,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};

struct NewWindow StatusNewWindowStructure4 = {
	0,275,	/* window XY origin relative to TopLeft of screen */
	480,110,	/* window width and height */
	0,1,	/* detail and block pens */
	CLOSEWINDOW+ACTIVEWINDOW+VANILLAKEY,	/* IDCMP flags */
	WINDOWSIZING+WINDOWDRAG+WINDOWCLOSE+GIMMEZEROZERO+NOCAREREFRESH,	/* other window flags */
	NULL,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	"Status",	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	5,5,	/* minimum width and height */
	640,512,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};


void HandleEvent(object)
APTR object;
{
  if (object == (APTR)&ConfigGadget1) { gf_confcancel(object); return; }
  if (object == (APTR)&ConfigGadget2) { gf_confok(object); return; }
  if (object == (APTR)&ConfigGad_ticks) { gf_nextgadget(object); return; }
  if (object == (APTR)&ConfigGad_reload) { gf_nextgadget(object); return; }
  if (object == (APTR)&ConfigGad_shells) { gf_nextgadget(object); return; }
  if (object == (APTR)&ConfigGad_delay) { gf_nextgadget(object); return; }
  if (object == (APTR)&ConfigGad_explosion) { gf_nextgadget(object); return; }
  if (object == (APTR)&ConfigGad_timefactor) { gf_nextgadget(object); return; }
  if (object == (APTR)&ArenaMenuItem9) { mf_info(object); return; }
  if (object == (APTR)&ArenaMenuItem10) { mf_runrobots(object); return; }
  if (object == (APTR)&ArenaMenuItem11) { mf_loadarena(object); return; }
  if (object == (APTR)&ArenaMenuItem12) { mf_config(object); return; }
  if (object == (APTR)&ArenaMenuItem13) { mf_color(object); return; }
  if (object == (APTR)&ArenaMenuItem14) { mf_quit(object); return; }
  if (object == (APTR)&ArenaMenuItem4) { mf_runsim(object); return; }
  if (object == (APTR)&ArenaMenuItem5) { mf_singlestep(object); return; }
  if (object == (APTR)&ArenaMenuItem6) { mf_stoponone(object); return; }
  if (object == (APTR)&ArenaMenuItem7) { mf_prepos(object); return; }
  if (object == (APTR)&ArenaMenuItem8) { mf_fastexplosion(object); return; }
}
#define HANDLEEVENT HandleEvent

/* end of PowerWindows source generation */
