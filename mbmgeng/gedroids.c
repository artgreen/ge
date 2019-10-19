
/***************************************************************************
 *                                                                         *
 *   GEDROIDS.C                                                            *
 *                                                                         *
 *   Copyright (C) 1988, 89, 90, 91, 92 Michael B. Murdock                 *
 *                                                                         *
 *   This is the source for the Galactic Empire game module                *
 *                                                                         *
 *                                         M. Murdock     03/17/92         *
 *                                                                         *
 ***************************************************************************/

 /**************************************************************************
  * This program is free software; you can redistribute it and/or modify   *
  * under the terms of the GNU General Public License as published by the  *
  * Free Software Foundation; either version 2 of the License, or (at your *
  * option) any later version.                                             *
  *                                                                        *
  * This program is distributed in the hope that it will be useful,        *
  * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
  * General Public License for more details.                               *
  *                                                                        *
  * You should have received a copy of the GNU General Public License      *
  * along with this program; if not, write to the Free Software Foundation,*
  * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA            *
  *************************************************************************/

 /**************************************************************************
 * Authors Note: The source code to this program is being made available   *
 *               to the general public in the hopes that it will continue  *
 *               to live on and evolve. While I cannot provide the time    *
 *               to personally participate in its development any longer,  *
 *               I am eager to assist in the design discussions, testing   *
 *               and project discussions.                                  *
 *                                                                         *
 *               To foster a community for Galactic Empire developers I    *
 *               have created a project home page at...                    *
 *               http://mbbs.sitesage.net                                  *
 *                                                                         *
 *               I may be contacted via email at mmurdock@starphire.com    *
 ***************************************************************************/
 
/* NOTE: I like my tabs at 3 so if the code looks "messed up" try changing */
/*       your tabs to 3....    MBM                                         */

#ifdef PHARLAP

#include "gcomm.h"
#include "string.h"

#else

#include "stdio.h"
#include "ctype.h"
#include "dos.h"
#include "usracc.h"
#include "btvstf.h"
#include "stdlib.h"
#include "math.h"
#include "portable.h"
#include "dosface.h"
#endif

#include "majorbbs.h"
#include "message.h"

#include "gemain.h"


#define  GEDROIDS   1

#include "geglobal.h"

/* LOCAL GLOBAL DEFS ****************************************************/

char        droidname[UIDSIZ];


/**************************************************************************






**************************************************************************/





/**************************************************************************
** Droid Init FUnction                                                   **
**************************************************************************/

void  FUNC droid_init(WARSHP *ptr, int usrn, int class)
{

if (usrn < 0 || usrn >= nships)
	{
	logthis(spr("DROID_INIT:bad usrn [%d]",usrn));
	return;
	}

strncpy(droidname,"@Droid-",UIDSIZ);/* Bj Added name here */
sprintf(&droidname[7],"%d",usrn);

waruptr = warusroff(usrn);
warsptr = warshpoff(usrn);

logthis(spr("GE:INF:Adding %s user",droidname)); 

initusr(droidname);

memcpy(waruptr,&tmpusr,sizeof(WARUSR));  /* make it the current user */

/* make me a Ship */
logthis(spr("GE:INF:Adding %s ship - %d",ptr->userid,class));

initshp(droidname,class);

memcpy(ptr,&tmpshp,sizeof(WARSHP));  /* make is the current ship */
sprintf(ptr->shipname,"%s%u\0",shipclass[class].shipname,usrn*usrn+gernd()%100);

if (univmax < 20)
	{
	ptr->coord.xcoord    = rndm((double)univmax*2.0)-(double)univmax;
	ptr->coord.ycoord    = rndm((double)univmax*2.0)-(double)univmax;
	}
else
	{
	ptr->coord.xcoord    = rndm(39.9)-19.8;
	ptr->coord.ycoord    = rndm(39.9)-19.8;
	}

ptr->phasrtype = shipclass[class].max_phasr;
ptr->shieldtype = shipclass[class].max_shlds;

/* if murdonian transport - put tons on */
if(sameas(shipclass[ptr->shpclass].typename,"Murdonian Transport"))
	{	
	ptr->items[I_FLUXPOD] = gernd()%50;
	ptr->items[I_DECOYS] = gernd()%250;
	ptr->items[I_TORPEDO] = gernd()%250;
	ptr->items[I_MINE] = gernd()%100;
	ptr->items[I_JAMMERS] = gernd()%100;
	ptr->items[I_MISSILE] = gernd()%100;
	ptr->items[I_IONCANNON] = gernd()%25;
	ptr->items[I_GOLD] = gernd()%250;
	}
else
	{	
	ptr->items[I_FLUXPOD] = gernd()%50;
	ptr->items[I_DECOYS] = gernd()%25;
	ptr->items[I_MINE] = gernd()%10;
	ptr->items[I_JAMMERS] = gernd()%10;
	}


ptr->speed2b = rndm((double)(ptr->topspeed * 1000.0));
ptr->holdcourse = 0;

ptr->status = GESTAT_AUTO;
ptr->tick = CYBTICKTIME + gernd()%CYBTICKTIME;

/*gepdb(GEUPDATE,ptr->userid,ptr->shipno,ptr); DONT NEED TO MAKE PERM*/
prfmsg(DROIDNEW,gernd()%359);
outwar(FILTER,usrn,0);
}

/**************************************************************************
** Droid Lives FUnction                                                  **
**************************************************************************/

void  FUNC droid_lives(WARSHP *ptr, int usrn)
{

if (usrn < 0 || usrn >= nships)
	{
	logthis(spr("DROID_LIVES:bad usrn [%d]",usrn));
	return;
	}

sprintf(&droidname[7],"%d",usrn+1);

/* DEBUG
logthis(spr("GE:%s Lives",droidname)); */

/* reset the ticker to 255 to cause it to recalc */
ptr->tick = 255;

warusroff(usrn)->cash += CYB_ALLOW;

if(sameas(shipclass[ptr->shpclass].typename,"Lydorian Garbage Scow"))
	droid_act_class_10(ptr,usrn);    /* Lydorian Garbage Scow */
else
if(sameas(shipclass[ptr->shpclass].typename,"Murdonian Transport"))
	droid_act_class_11(ptr,usrn);    /* Murdonian Transport */
else
if(sameas(shipclass[ptr->shpclass].typename,"Vakory Survey Drone"))
	droid_act_class_12(ptr,usrn);    /* Vakory Survey Droid */



ptr->energy = 50000L;

if (ptr->tick == 255)
	{
	/* if just cruising around don't get back to me for some time */
	if (ptr->cantexit==0)
		{
		ptr->tick = (CYBTICKTIME + gernd()%CYBTICKTIME)*3;
		}
	else
		{
		ptr->tick = CYBTICKTIME + gernd()%CYBTICKTIME;
		}
	}
}


/* ptr to sender , usrn = reciever */
void  FUNC droid_annoy(WARSHP *ptr, int usrn, int rnd, int first, int last)
{

if ((gernd()%rnd) == 1)
	{
	prfmsg(first+gernd()%(last-first+1),ptr->shipname);
	outprfge(FILTER,usrn);
	}
}


/**************************************************************************
** Class 10 Action                                                       **
** Lydorian Garbage Scow 																 **
**************************************************************************/

void  FUNC droid_act_class_10(WARSHP *ptr, int usrn)
{

WARSHP   *wptr;
int      zothusn;

double   ddist;

/* am I being jammed ? */
if (ptr->jammer == 0)
	{
	/* look at all the other ships */
	for (zothusn=0 ; zothusn < nterms ; zothusn++)
		{
		wptr=warshpoff(zothusn);
		/* if not me, and playing, and not cyborg, go getem */
		if (ingegame(zothusn) && wptr->status == GESTAT_USER)
			{
			ddist = cdistance(&ptr->coord,&wptr->coord);
			ddist *= 10000;
			if (ddist < (double)shipclass[ptr->shpclass].scanrange)
				{
				ptr->tick = CYBTICKTIME + gernd()%CYBTICKTIME;
				droid_annoy(ptr,zothusn,4,DRDMSG6,DRDMSG6);
				}
			}
		}
	}
else
	{
	ptr->speed2b = 999.9; /* has no warp capability */
	ptr->holdcourse = gernd()%50 + 10;
	}
/* Runs with shields up all the time */
if (ptr->speed < 1000.0)
	shieldup(ptr,usrn);
else
	shielddn(ptr,usrn);
}


/**************************************************************************
** Class 11 Action                                                       **
** Murdonian Transport
**************************************************************************/

void  FUNC droid_act_class_11(WARSHP *ptr, int usrn)
{

WARSHP   *wptr;
int      zothusn;


double   ddist;
ddist = 999999.9;
/* am I being jammed ? */
if (ptr->jammer == 0)
	{
	/* look at all the other ships */
	for (zothusn=0 ; zothusn < nterms ; zothusn++)
		{
		wptr=warshpoff(zothusn);
		/* if not me, and playing, and not cyborg, go getem */
		if (ingegame(zothusn) && wptr->status == GESTAT_USER)
			{
			ddist = cdistance(&ptr->coord,&wptr->coord);
			ddist *= 10000;
			if (ddist < (double)shipclass[ptr->shpclass].scanrange)
				{
				if (ptr->holdcourse == 0)
					ptr->speed2b = rndm(999.9);
				if (ptr->speed < 1000.0)
					shieldup(ptr,usrn);
				else
					shielddn(ptr,usrn);
	
				ptr->tick = CYBTICKTIME + gernd()%CYBTICKTIME;
				droid_annoy(ptr,zothusn,4,DRDMSG11,DRDMSG15);
				}
			}
		}
	if (ptr->cantexit > 0 && ptr->lastfired >= 0)
		{
		logthis(spr("Droid (murtran) cantexit - lastfired = %d",ptr->lastfired));
		wptr = warshpoff(ptr->lastfired);
		zothusn = ptr->lastfired;

		droid_annoy(ptr,zothusn,4,DRDHLP11,DRDHLP15);

		/* fire phasers at the fool */
		if (ptr->where == 1 && wptr->where == 1 )
			{
			if (ddist < 30000)
				{
				ptr->degrees = (int)(cbearing(&ptr->coord,&wptr->coord,ptr->heading)+.5);
				firehp(ptr,usrn);
				}
			}
		else
		if ((ptr->where == 0 && wptr->where == 0)
			|| (ptr->where == 0 && wptr->where >= 2))
			{
			ptr->degrees = 0;
			ptr->percent = 2;
			if (ptr->phasr >= PMINFIRE && wptr->cloak != 10)
				{
				/* logthis(spr("GE:phaser fired %d",ptr->degrees));*/
				firep(ptr,usrn);
				}

			/* just to confuse them sometimes alter attack vector */

			if (ptr->holdcourse == 0 && gernd()%10 == 0)
				{
				ptr->speed2b = rndm(10000.0);
				ptr->head2b = rndm(359.9);
				ptr->holdcourse = gernd()%10 + 3;
				}
			}

		/* if we are in hyperspace and fighting and missiles detected
			get out of hyperspace */
			
		if (ptr->where == 1)
			{
			if (missl_attached(ptr,usrn))
				{
				ptr->speed2b = rndm(999.0);
				ptr->holdcourse = gernd()%15 + 5;
				}
			}
		else
			{
			shieldup(ptr,usrn);
			}
		}
	}
else
	{
	ptr->speed2b = (double)(ptr->topspeed * 1000);
	ptr->holdcourse = gernd()%50 + 10;
	}
}



/**************************************************************************
** Class 12 Action                                                       **
** Vakory Survey Droid 
**************************************************************************/

void  FUNC droid_act_class_12(WARSHP *ptr, int usrn)
{
int		i,j;

WARSHP   *wptr;
int      zothusn;

double   ddist;
ddist = 999999.9;
/* am I being jammed ? */
if (ptr->jammer == 0)
	{
	/* look at all the other ships */
	for (zothusn=0 ; zothusn < nterms ; zothusn++)
		{
		wptr=warshpoff(zothusn);
		/* if not me, and playing, and not cyborg, go getem */
		if (ingegame(zothusn) && wptr->status == GESTAT_USER)
			{
			ddist = cdistance(&ptr->coord,&wptr->coord);
			ddist *= 10000;
			if (ddist < (double)shipclass[ptr->shpclass].scanrange)
				{
				if (ptr->speed < 1000.0)
					shieldup(ptr,usrn);
				else
					shielddn(ptr,usrn);
	
				droid_annoy(ptr,zothusn,4,DRDMSG1,DRDMSG5);
				ptr->tick = CYBTICKTIME + gernd()%CYBTICKTIME;

				}
			}
		}
	if (ptr->cantexit > 0 && ptr->lastfired > 0)
		{
		logthis(spr("Droid (vakory) cantexit - lastfired = %d",ptr->lastfired));
		wptr = warshpoff(ptr->lastfired);
		zothusn = ptr->lastfired;

		droid_annoy(ptr,zothusn,4,DRDHLP1,DRDHLP5);

		/* fire phasers at the fool */
		if (ptr->where == 1 && wptr->where == 1 )
			{
			if (ddist < 30000)
				{
				ptr->degrees = (int)(cbearing(&ptr->coord,&wptr->coord,ptr->heading)+.5);
				firehp(ptr,usrn);
				}
			}
		else
		if ((ptr->where == 0 && wptr->where == 0)
			|| (ptr->where == 0 && wptr->where >= 2))
			{
			ptr->degrees = 0;
			ptr->percent = 2;
			if (ptr->phasr >= PMINFIRE && wptr->cloak != 10)
				{
				/* logthis(spr("GE:phaser fired %d",ptr->degrees));*/
				firep(ptr,usrn);
				}

			/* fire torpedoes at the fool */
			j = gernd()%2;
			for (i=0;i<j;++i)
				{
				ptr->items[I_TORPEDO] = (gernd()%5)+1;
				if (i>0) lockwarn = FALSE;
				torp(ptr,usrn,zothusn);
				}

			/* just to confuse them sometimes alter attack vector */

			if (ptr->holdcourse == 0 && gernd()%20 == 1)
				{
				ptr->speed2b = rndm(5000.0);
				ptr->head2b = rndm(359.9);
				ptr->holdcourse = gernd()%10 + 3;
				}
			}

		/* if we are in hyperspace and fighting and missiles detected
			speed up and loose them */

		if (missl_attached(ptr,usrn))
			{
			ptr->speed2b = rndm(5900.0)+5000.0;
			ptr->holdcourse = gernd()%5 + 5;
			}

		if (ptr->speed < 1000.0)
			shieldup(ptr,usrn);
		else
			shielddn(ptr,usrn);

		if (ptr->damage > 75)
			{
			if (ptr->items[I_MINE] > 0)
				laymine(ptr,usrn,10);
		
			if (ptr->items[I_JAMMERS] >0)
				jam(ptr,usrn);
	
			ptr->speed2b = (double)(ptr->topspeed * 1000);
			ptr->head2b = rndm(359.9);
			ptr->holdcourse = gernd()%30 + 20;
		
			}
		}
	}
else
	{
	ptr->speed2b = (double)(ptr->topspeed * 1000);
	ptr->holdcourse = gernd()%50 + 10;
	}

}



void   FUNC droid_won(WARSHP *ptr)
{
ptr->speed2b = rndm(5000.0);
}

void   FUNC droid_died(WARSHP *ptr)
{
ptr->status = GESTAT_AVAIL;
logthis(spr("GE:INF:%s Died!",ptr->userid));
}

int   FUNC missl_attached(WARSHP *ptr, int usrn)
{
int		i;
MISSILE  *mptr;

usrn=usrn;

for (i=0,mptr=ptr->lmissl;i<MAXMISSL;++i,++mptr)
	{
	if (mptr->distance > 0)
		{
		return(TRUE);
		}
	}
return(FALSE);
}

