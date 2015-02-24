/****************************************************************************
 File:			switch001.c

 Version:		0.13

 Description:	Random number and switch test

 Created on:	2015-02-24
 Created by:	April

 Board:			RichMCU RZ-51V2.0

 Jumpers:		Remove JPSMG - Disable 7-Segment LEDs

				将 JPSMG 数码管控制跳线拔掉使数码管停止工作。
****************************************************************************/
#include <STC89.H>

#define listPin		P14
#define starPin		P15

//Temporary
#define matchPin	P16
#define resetPin	P17

//#define yesPin	P16	
//#define noPin		P17

#define	HIGH		1
#define LOW			0

#define	EOL			0xff

#define	DELAY_STD	2500

unsigned char playerList, playerStar;
static unsigned int a=0;
unsigned int slowdown[]= { 3500, 3600, 3800, 4200, 5000, 6600, 
							   9800, 16200, 29000, 54600, 0};


void delay(unsigned int d){
	unsigned int t;

	for (t=0; t<d; t++);

} /* end delay */


unsigned char ledOn(unsigned char led){
	return ~led;

} /* end ledOn */


void ledBlink(unsigned int times, space){
	unsigned int t;

	for (t=0; t<times; t++){
		P0 = 0xff;
		delay(space);

		P0 = ledOn(0xff);
		delay(space);
	}
}


void initialize(void){
	/* Turn off all LEDs */
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;

	/* Set Pin ready for Input */
	listPin	 = HIGH;
	starPin	 = HIGH;
	matchPin = HIGH;
	resetPin = HIGH;
}
	

/******RANDOM****///////////////////////////
unsigned char getRandom(unsigned char choice){
	static unsigned int	rnd=0;
	unsigned char random;

	if(choice == 1){
		unsigned char rndList[] = { 
								6, 4, 0, 7, 6, 4, 2, 7, 3, 0, 5, 1, 5, 6, 5, 4, 5, 7, 4, 1,
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
//								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
//								7, 0, 4, 1, 5, 0, 3, 4, 2, 5, 7, 5, 1, 7, 6, 5, 4, 6, 7, 4,
//								6, 7, 0, 1, 6, 4, 5, 6, 5, 7, 0, 4, 5, 3, 2, 5, 2, 4, 5, 7,
					  		    EOL};
		random = rndList[rnd];
		rnd++;
		if (rndList[rnd] == EOL) rnd = 0;		
	}

	if(choice == 2){
		unsigned char rndStar[] = {
//		 						6, 4, 0, 7, 6, 4, 2, 7, 3, 0, 5, 1, 5, 6, 5, 4, 5, 7, 4, 1,
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
//								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
								7, 0, 4, 1, 5, 0, 3, 4, 2, 5, 7, 5, 1, 7, 6, 5, 4, 6, 7, 4,
//								6, 7, 0, 1, 6, 4, 5, 6, 5, 7, 0, 4, 5, 3, 2, 5, 2, 4, 5, 7,
					  		    EOL};

		random = rndStar[rnd];
		rnd++;
		if (rndStar[rnd] == EOL) rnd = 0;
	}

	else{
		unsigned char rndLose[] = {
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
								EOL};

		random = rndLose[rnd];
		rnd++;
		if (rndLose[rnd] == EOL) P0 = 0xff;
	}
		
	return random;
 	
} /* end getRandom */


///****LIST***///////////////////////////
void stateList(void){
	/* Init player selection to invalid */
	playerList = 0xff;

	while (listPin == HIGH);			/* Key Off */

	while (listPin == LOW){		   		/* Key On */
		P0 = ledOn(1 << getRandom(1));  //get random no. from List array
		
		delay(DELAY_STD);	 			//delay(2000)
	}

    while (slowdown[a]!= 0){
		playerList = getRandom(1);		//get random no. from List array
		P0 = ledOn(1 << playerList);

		delay(slowdown[a]);
		a++;
	}

	while (starPin != LOW);
	
} /* end stateList */


////****STAR***//////////////////////////////
void stateStar(void){
	/* Init player selection to invalid */
	playerStar = 0xff;

	while (starPin == HIGH);			/* Key Off */

	while (starPin == LOW){		   		/* Key On */
		P0 = ledOn(1 << getRandom(2));  //get random no. from the Star array
		
		delay(DELAY_STD);	 			//delay(2000)
	}

    while (slowdown[a]!= 0){		    
		playerStar = getRandom(2);		//get random no. from the Star array
		P0 = ledOn(1 << playerStar);

		delay(slowdown[a]);
		a++;
	}
  	
	while (matchPin != LOW);
//	while ((yesPin != LOW) || (noPin != LOW));
	
} /* end stateStar */


/////***MATCH***///////////////////////
void win(void){
	char n;

	for(n=0; n<5; n++){			  //blink LED
			P0 = 0xff;
			delay(500);
			P0 = 0;
			delay(500);
	}
} /* end win */


void lose(void){
	 P0 = ledOn(1 << getRandom(3));
} /* end lose */


void stateMatch(){

//	if(((playerList == star) && (yesPin == LOW)) || ((playerList != playerStar) && (noPin == LOW)))
//		 win();
//	else lose();

	/**TEMPORARY - not enough buttons*****/
	if((playerList != playerStar) && (matchPin == LOW)) win();
	else									lose();

	while (resetPin != LOW);
} /* end stateMatch */


void main(void){
	for(;;){
		initialize();
		stateList();
		stateStar();
//		stateMatch();
	}
} /* end main */
