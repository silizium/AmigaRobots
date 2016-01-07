/* Hinzi&Berger_1, erster Test */

#include "pcrobots.h"
#ifndef __SASC
char *_ProgramName = "Hinzi&Berger_1";
#endif

/* ziel_id = aktuelles Ziel */
int scanwinkel;
int ziel_id,ziel_entfernung,aufloes,gesch,ladung;
long runde,geschossen=-50;
int neu; /* flag */

const char pattern[5] = {24,12,6,12,24};


main(void)
{


    configure(2,2,2,3,1,1);
    set_pattern(pattern);

    neu = 1;
    ziel_id = -1;
    scanwinkel = 0;
    aufloes = 44;


    while(1)
    {
        ziel_id = scan(scanwinkel,aufloes,&ziel_entfernung);
        if (ziel_id == -1)
        {
            aufloes = 44;
            neu = 1;
            do
            {
                if (ziel_id == -1) scanwinkel+=90;
                ziel_id = scan(scanwinkel,aufloes,&ziel_entfernung);
            } while (ziel_id == -1);
        }
        while ( (aufloes>1) && (neu))
        {
            aufloes = aufloes / 2;
            scanwinkel = scanwinkel - (aufloes / 2);
            if (scanwinkel < 0) scanwinkel += 360;
            ziel_id = scan(scanwinkel,aufloes,&ziel_entfernung);
            if (ziel_id == -1) scanwinkel += (aufloes*2);
        }
        neu = 0;

        runde = ticks();
       if ( runde > (geschossen+49) )
       {
           shoot(scanwinkel,ziel_entfernung);
           geschossen = ticks();
       }
       else
       {
           if (ziel_entfernung > 100)
           {
               ladung = battery();
               ladung = ladung / 100;
               switch (ladung)
               {

                    case 1 :
                    case 2 :
                    case 3 : movement(0,scanwinkel);
                             break;
                    case 4 :
                    case 5 : movement(30,scanwinkel);
                             break;

                    default: movement(50,scanwinkel);
                }
            }
            else movement(0,scanwinkel);
        }



    }  /* of while(1) */
}  /* main() */
