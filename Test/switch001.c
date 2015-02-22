/****************************************************************************
 File:			switch001.c

 Version:		0.02

 Description:	Random number and switch test

 Created on:	2015-02-22
 Created by:	April

 Board:			RichMCU RZ-51V2.0

 Jumpers:		Remove JPSMG - Disable 7-Segment LEDs

				将 JPSMG 数码管控制跳线拔掉使数码管停止工作。
****************************************************************************/
#include <STC89.H>

#define	HIGH	1
#define LOW		0

#define	EOL		0xff

char playerList;
char playerStar;

int  slowdown[]= {5000, 7500, 10000, 15000, 20000, 30000};

void delay(int n)
{
	int t;

	for (t=0; t<n; t++);
}

void main(void)
{
	/* Init player selection to invalid */
	char playerList = 0xff;
	char playerStar = 0xff;

	int	rnd=0;
	
	unsigned char random, x;

	unsigned char rndList[] = {7, 5, 3,	6, 2, 4, 1, 0, 6, 3, 6, 4, 2, 6, 4, 0, 1, 2, 6, 
					  		   4, 6, 3, 2, 0, 3, 5, 7, 2, 6, 3, 7, 1, 5, 0, 3, 5, 2, 4,
					  		   EOL};


	/* Turn off all LEDs */
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;

	/* Set Pin ready for Input */
	P14 = HIGH;
	P15 = HIGH;
	P16 = HIGH;
	P17 = HIGH;

    for (;;)
	{
		while (P14 == HIGH);		/* Key Off */

		while (P14 == LOW)
		{
			random = rndList[rnd];
			rnd++;
			if (rndList[rnd] == EOL) rnd = 0;
	
			x = 1 << random;
	
			P0 = ~x;
			
			delay(3000);
		}
		playerList = random;
	}
}