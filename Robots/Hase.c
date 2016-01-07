/* Der Hase läuft einfach davon, wenn er getroffen wird */
/* dieser Roboter ist mehr als Demo für hit_from() zu verstehen */
#ifndef __SASC
char *_ProgramName="Hase.rob";
#endif

#include <pcrobots.h>

void main(void)
{
	int ld;

	configure(2,3,2,2,1,0);

	while(1)
	{
		ld=damage();
		swaptask();
		if(damage() < ld)
			movement(63,hit_from()+180);
	}
}
