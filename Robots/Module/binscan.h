struct Target
{
	int id;
	int dir,range,res;
	int xpos,ypos,time;
	int roundscan,predir;
	int nextfriend,rangefriend;
	int lastrequest;
};


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
	struct
	{
		int res,next[2];
	}
 	const vec_res[10]=
	{
		{45,{0,0}}, 		/* Vorscannen */
		{45,{-23,90}},		/* iteratives Punktscannen */
		{23,{-12,23+12}},
		{12,{-7,12+7}},
		{6,{-3,6+3}},
		{3,{-2,3+2}},
		{2,{-1,2+1}},
		{1,{-1,1}},
		{0,{0,1}},
		{0,{0,1}}
	};

	int range,id,x,y;

	if(target->res < 0 || 9 < target->res || target->id < 0) target->res = 0;

	/* einmal ein Rundumscannen um den Nächsten zu finden */
	if(target->res == 0)
	{
		if(target->roundscan >= 4 || target->roundscan < 0)
		{
			target->roundscan = 0;
			target->range = 2000;
			target->id = -1;
			target->nextfriend = -1;
			target->rangefriend = 2000;
		}
		id = scan(
			towinkel(90*target->roundscan+target->dir),
			45,
			&range);

		if(id >= 0 && range < target->range && !isfriendly(id,table))
		{
			target->range = range;
			target->id = id;
			target->predir = towinkel(90*target->roundscan+target->dir);
		}
		if(id >= 0 && range < target->rangefriend && isfriendly(id,table))
		{
			target->nextfriend = id;
			target->rangefriend = range;
		}
		
		target->roundscan++;

		if(target->roundscan >= 4)
		{
			if(target->id >= 0) /* Feind gefunden */
			{
				target->res++; /* weitermachen */
				target->dir = target->predir;
			}
			else	/* kein Feind */
			{
				target->dir = towinkel(target->dir + 10);
				if(target->nextfriend >= 0)
				{
					if(requesttarget(target))
					{
						debug_flag(DB_NOTARGET,1);
						return 1;
					}
				}
			}
		}
		return 0;	/* Rundumscan */
	}/* ende Rundumscannen um den Nächsten zu finden */

	id = scan(
			target->dir,
			vec_res[target->res].res,
			&range);

	target->dir = 
		towinkel(target->dir + 
			vec_res[target->res].next[(id==target->id && target->id >= 0)?0:1]);

	if(target->id >= 0)
		target->res++;

	if(id == target->id && target->id >= 0)
	{
		target->range = range;
	}

	if(target->res > 9)	/* Auf ein Grad genau */
	{
		target->time = ticks();
		getxy(&x,&y);
		target->xpos = x + l_cos(target->dir)*target->range/100000;
		target->ypos = y + l_sin(target->dir)*target->range/100000;
		debug_flag(DB_NOTARGET,0);
		return 1;
	}
	else
	{
		return 0;
	}
}/* scantarget */

