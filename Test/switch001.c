/****************************************************************************
 File:			switch001.c

 Version:		0.05

 Description:	Random number and switch test

 Created on:	2015-02-22
 Created by:	April

 Board:			RichMCU RZ-51V2.0

 Jumpers:		Remove JPSMG - Disable 7-Segment LEDs

				将 JPSMG 数码管控制跳线拔掉使数码管停止工作。
****************************************************************************/
#include <STC89.H>

#define listPin		P14
#define starPin		P15

#define	HIGH		1
#define LOW			0

#define	EOL			0xff

#define	DELAY_STD	2000

void delay(unsigned int d)
{
	unsigned int t;

	for (t=0; t<d; t++);

} /* end delay */

unsigned char ledOn(unsigned char led) 
{
	return ~led;

} /* end ledOn */

unsigned char getRandomList(void)
{
	static unsigned int	rnd=0;
		
	unsigned char random;
	unsigned char rndList[] = { 6, 4, 0, 7, 6, 4, 2, 7, 3, 0, 5, 1, 5, 6, 5, 4, 5, 7, 4, 1,
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
//								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
//								7, 0, 4, 1, 5, 0, 3, 4, 2, 5, 7, 5, 1, 7, 6, 5, 4, 6, 7, 4,
//								6, 7, 0, 1, 6, 4, 5, 6, 5, 7, 0, 4, 5, 3, 2, 5, 2, 4, 5, 7,
					  		    EOL};

	random = rndList[rnd];
	rnd++;
	if (rndList[rnd] == EOL) rnd = 0;
		
	return random;

} /* end getRandomList */

void stateList(void)
{
	/* Init player selection to invalid */
	unsigned char playerList = 0xff;

	static unsigned int a=0;
	unsigned int slowdown[]= { 3500, 3600, 3800, 4200, 5000, 6600, 
							   9800, 16200, 29000, 54600, 0};

	/* Turn off all LEDs */
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;

	/* Set Pin ready for Input */
	listPin = HIGH;
	starPin = HIGH;
	P16 = HIGH;
	P17 = HIGH;

	while (listPin == HIGH);		/* Key Off */

	while (listPin == LOW)		   	/* Key On */
	{
		P0 = ledOn(1 << getRandomList());
		
		delay(DELAY_STD);	 //delay(2000)
	}

    while (slowdown[a]!= 0)
	{
		playerList = getRandomList();
		P0 = ledOn(1 << playerList);

		delay(slowdown[a]);
		a++;
	}

	while (starPin != LOW);
	
} /* end stateList */

void main(void)
{
//	for(;;)
//	{
		stateList();
//	}
} /* end main */