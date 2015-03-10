/****************************************************************************
 File:			switch001.c

 Version:		0.30

 Description:	Random number and switch test

 Created on:	2015-03-09
 Created by:	April

 Board:			RichMCU RZ-51V2.0

 Jumpers:		Remove JPSMG - Disable 7-Segment LEDs

				将 JPSMG 数码管控制跳线拔掉使数码管停	止工作。
****************************************************************************/
#include <STC89.H>

#define	DEBUG
#define	RICHMCU

#define	TIMER_RUN		0					// 1: ON 0: OFF

#define	LOW				0
#define	HIGH			1

#define	EOL				0xff
#define	NO_PLAYER		0xff

#define	LED_ON			0
#define	LED_OFF			1
#define	LED_PORT_OFF	0xff

#define	KEY_ON			0
#define	KEY_OFF			1

#define	DELAY_STD		2500
#define	DELAY_KEYBOUNCE	10

#define	ledStatusYellow	P36
#define	ledStatusGreen	P37

#define	audioBusy		P17

#ifdef	RICHMCU
#define	portList		P0
#define	portStar		P0
#define	portSwitch		P1

#define listKey			P14
#define starKey			P15
#define yesKey			P16		
#define noKey			P17
#define	ansKey			P16
#define	demoKey			P17
#else
#define	portList		P2
#define	portStar		P0
#define	portSwitch		P1

#define listKey			P10
#define starKey			P11
#define yesKey			P12
#define noKey			P13
#define	ansKey			P14
#define	demoKey			P15
#endif


#define	RND_LIST		1
#define	RND_STAR		2
#define	RND_LOSE		3

#define	PLAYER_WIN		1
#define	PLAYER_LOSE		0

#define	SFX_BKG_MUSIC		0
#define	SFX_KEY_CLICK		1
#define	SFX_LIST_START		11
#define	SFX_LIST_SLOWDN		12
#define SFX_LIST_SPEEDUP	13
#define	SFX_STAR_START		21
#define	SFX_STAR_SLOWDN		22
#define	SFX_WIN				81
#define	SFX_LOSE			82


/******************************************
 Global Variables
 ******************************************/
unsigned int isrTimer0Cnt;

unsigned char serialRxData;				// Data received from Serial Port

unsigned char playerPickedList;
unsigned char playerPickedStar;

unsigned int delaySlowdown[]= { 3500,  3600,  3800,  4200, 5000, 6600, 
						  		9800, 16200, 29000, 54600, EOL};

unsigned int delaySpeedUp[]= {	54600, 29000, 16200, 9800, 6600, 5000,
								4200, 3800, 3600, 3500, EOL};


/******************************************
 Prototypes
 ******************************************/
unsigned char getRandom(unsigned char choice);
void serialTx(unsigned char txdata);
void audioPlay(unsigned char sfx);
void audioStop(void);


/*********************************PROGRAM**********************************/
void delay(unsigned int d) {
	unsigned int t;

	for (t=0; t<d; t++);
} /* end delay */


void delay1ms(void)   //误差 -0.651041666667us
{
    unsigned char a,b;

    for(b=102;b>0;b--)
        for(a=3;a>0;a--);

} /* delay1ms */


void delayms(unsigned int timeMS)
{
	unsigned int t;

	for (t=0; t<timeMS; t++)
		delay1ms();

} /* end delayms */


unsigned char ledOn(unsigned char led) {

	return ~led;
} /* end ledOn */


/******RANDOM****///////////////////////////
unsigned char getRandom(unsigned char choice) {
	static unsigned int	rnd=0;
	unsigned char random;

	/* Store data into ROM area */
	code const unsigned char rndList[] = { 
								6, 4, 0, 7, 6, 4, 2, 7, 3, 0, 5, 1, 5, 6, 5, 4, 5, 7, 4, 1,
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
								7, 0, 4, 1, 5, 0, 3, 4, 2, 5, 7, 5, 1, 7, 6, 5, 4, 6, 7, 4,
								6, 7, 0, 1, 6, 4, 5, 6, 5, 7, 0, 4, 5, 3, 2, 5, 2, 4, 5, 7,
					  		    EOL};

	code const unsigned char rndStar[]={
								6, 7, 0, 1, 6, 4, 5, 6, 5, 7, 0, 4, 5, 3, 2, 5, 2, 4, 5, 7,
								7, 0, 4, 1, 5, 0, 3, 4, 2, 5, 7, 5, 1, 7, 6, 5, 4, 6, 7, 4,
		 						6, 4, 0, 7, 6, 4, 2, 7, 3, 0, 5, 1, 5, 6, 5, 4, 5, 7, 4, 1,
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
					  		    EOL};

	code const unsigned char rndLose[]={
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
								EOL};	
	
	if(choice == 1) {
		random = rndList[rnd++];
		if (rndList[rnd] == EOL) rnd = 0;		
	}

	if(choice == 2) {
		random = rndStar[rnd++];
		if (rndStar[rnd] == EOL) rnd = 0;
	}
	else{
		random = rndLose[rnd++];
		if (rndLose[rnd] == EOL) rnd = 0;
	}
		
	return random;
} /* end getRandom */

unsigned char audioIsBusy(void) {
	return ~audioBusy;			 	// 1: Busy  0: Ready for command
} /* audioIsBusy */


/* MP3 module command format:
	0x7E: Start Code
	0xFF: Version
	0x06: Length of data excluding Start Code, Checksum and End Code
	CMD:  1 Byte of command
	FBK:  1: Feedback info required   0: Silent
	PAR1: High Byte of Data
	PAR2: Low  Byte of Data
	CHK:  Checksum
	0xEF: End of Data

*/
void audioSendCmd(unsigned char * audioCmd) {
	unsigned char checksum=0;
	int i;

	for (i=2; i<8; i++) {
		checksum += audioCmd[i];
	}
	audioCmd[8] = ~checksum;

	for (i=0; i<10; i++) {		// Send command to MP3 module
		serialTx(audioCmd[i]);
	}
} /* audioSendCmd */


void audioPause(void) {
	static unsigned char audioCmdPause[]={0x7e, 0xff, 0x06, 0x0e, 00, 00, 00, 0xfe, 0xfe, 0xef};
	audioSendCmd(audioCmdPause);
} /* audioPause */


void audioPlay(unsigned char audiotrack) {
	static unsigned char audioCmdPlay[]={0x7e, 0xff, 0x06, 0x03, 00, 01, 01, 0xfe, 0xfe, 0xef};

	audioCmdPlay[6] = audiotrack;
	audioSendCmd(audioCmdPlay);

#ifdef DEBUG
	if (audiotrack == SFX_WIN)  P1=0xF0;
	if (audiotrack == SFX_LOSE) P1=0xF3;
#endif
} /* audioPlay */


void audioPlayLoop(unsigned char audiotrack) {
	static unsigned char audioCmdPlayLoop[]={0x7e, 0xff, 0x06, 0x03, 00, 01, 01, 0xfe, 0xfe, 0xef};

	audioCmdPlayLoop[6] = audiotrack;
	audioSendCmd(audioCmdPlayLoop);
} /* audioPlayLoop */


void initAudio(void) {	
	/* Stop Audio */
	audioStop();
} /* initAudio */


unsigned char getKey(unsigned char key) {
	unsigned char keyStatus;

	do {
		keyStatus = key;

		delayms(DELAY_KEYBOUNCE);

	} while (key != keyStatus);
	
	return keyStatus;

} /* getKey */


void serialTx(unsigned char txdata) {
	SBUF = txdata;			// Send data to Serial Port
	while (TI != 1);		// Wait until data transmitted
	TI = 0;					// Reset Transmit Flag
} /* serialTx */


void initSerial(void) {
	/** Setup TMOD=0x20 */
	TMOD = 0x20;

	/** Setup SCON=0x50 **/
	SM0 = 0;				// Serial port in Mode 1
	SM1 = 1;
	SM2 = 0;
	REN = 1;				// Enable Serial Port Receive
	TB8 = 0;				// 8-bit data Tx
	RB8 = 0;				// 8-bit data Rx
	TI  = 0;				// Clear Transmit Interrupt Flag
	RI	= 0;				// Clear Receive  Interrupt Flag

	TH1 = 0xfd;				// X'tal=11.0592MHz 9600baud
	TL1 = 0xfd;

	/** Setup PCON=0x00 **/
	PCON = 0x00;

	ES = 1; 				// Enable Serial Interrupt

	TR1 = 1;				// Enable Timer 1 as baud rate generator
} /* initSerial */


void initAll(void) {
	/* Timer0 controlled blinking Status LEDs */
	ledStatusYellow = LED_ON;
	ledStatusGreen  = LED_OFF;

 	/* Turn off all LEDs */
	P0 = LED_PORT_OFF;
	P1 = LED_PORT_OFF;
	P2 = LED_PORT_OFF;

	/* Set Pin ready for Input */
	listKey	 = HIGH;
	starKey	 = HIGH;
	yesKey	 = HIGH;
	noKey	 = HIGH;
	ansKey	 = HIGH;
	demoKey  = HIGH;
		
	/* Init player selection to invalid */
	playerPickedList = NO_PLAYER;

//	/* Play Music */
//	audioPlayLoop(SFX_BKG_MUSIC);

} /* initAll */


///****LIST***///////////////////////////
void stateList(void) {
	while (getKey(listKey) == KEY_OFF);					// listKey Off, wait for the listKey pressed

	audioPlay(SFX_KEY_CLICK);							// listKey is ON now
	while (audioIsBusy());								// Sound Effect finished?
   	audioPlayLoop(SFX_LIST_START);

	while (getKey(listKey) == KEY_ON) {					// listKey is still On
		portList = ledOn(1 << getRandom(RND_LIST));     // Get random no. from List array
		
		delay(DELAY_STD);
	}				  									// listKey Off now
} /* stateList */


void stateListPost(void) {
	int a;

   	a = 0;
    while (delaySlowdown[a]!= EOL) {
		audioPlay(SFX_LIST_SLOWDN);
		playerPickedList = getRandom(RND_LIST);			// Get random no. from List array
		portList = ledOn(1 << playerPickedList);	    // Show picked Name of Constellation

		delay(delaySlowdown[a++]);
	}
} /* end stateListPost */


////****STAR***//////////////////////////////
void stateStar(void) {
	while (getKey(starKey) == KEY_OFF);					// starKey Off, wait for starKey pressed

	audioPlay(SFX_KEY_CLICK);							// starKey is ON now
	while (audioIsBusy());								// Sound Effect finished?
   	audioPlay(SFX_STAR_START);
											  	
	while (getKey(starKey) == KEY_ON) {		  		 	// starKey is still ON
		portStar = ledOn(1 << getRandom(RND_STAR));	    // Get random no. from the Star array
		
		delay(DELAY_STD);
	}													// starKey Off
} /* end stateStar */


void stateStarPost(void) {
	int a;

    a = 0;
	while (delaySlowdown[a]!= EOL) {
		audioPlay(SFX_STAR_SLOWDN);		    
		playerPickedStar = getRandom(RND_STAR);			// Get random no. from the Star array
		portStar = ledOn(1 << playerPickedStar); 		// Show picked Constellation

		delay(delaySlowdown[a++]);
	}
} /* end stateStar */


unsigned char stateMatch(void) {
	unsigned char match;
	unsigned char yKey, nKey;

	do {
		yKey = getKey(yesKey);
		nKey = getKey(noKey);	
	} while ( ((yKey == KEY_OFF) && (nKey == KEY_OFF)) ||
		      ((yKey == KEY_ON)  && (nKey == KEY_ON) ));			// Wait until either Yes/No key pressed		
    
	audioPlay(SFX_KEY_CLICK);
	while (audioIsBusy());											// Sound Effect finished?

	if ( ((playerPickedList == playerPickedStar) && (yKey == KEY_ON)) || 
		 ((playerPickedList != playerPickedStar) && (nKey == KEY_ON)))
		 	match = PLAYER_WIN;
	else	match = PLAYER_LOSE;

	return match;

} /* end stateMatch */


void flashAnswer(void) {
	portList = LED_PORT_OFF;
	portStar = LED_PORT_OFF;
																  
	delayms(200);

	portList = ledOn(1 << playerPickedList);		// Show Name of Constellation
	portStar = ledOn(1 << playerPickedList); 		// Show picked Constellation

	delayms(500);
} /* end flashAnswer */


/////***MATCH***///////////////////////
void playerWin(void) {
	audioPlay(SFX_WIN);
} /* end playerWin */


void playerLose(void) {
	int a;
	unsigned char ansList;

	audioPlay(SFX_LOSE);

    a = 0;
	while (delaySpeedUp[a]!= EOL) {
		audioPlay(SFX_LIST_SPEEDUP);		
		    
		ansList = getRandom(RND_LOSE);			// Get random no. from the Lose array
		portList = ledOn(1 << ansList); 		// Show randomed Constellation Name

		delay(delaySpeedUp[a++]);
	}
	portList = ledOn(0x00);
	
} /* end playerLose */

	   
void initTimer0(void) {
	isrTimer0Cnt = 0;  	//isrTimer0 interrupt calling counter

	EA = 0;				//Disable all inerrupts

	TMOD = T0_M1;		//Set Timer0 to Mode 1

	TR0 =  0;			//Disable Timer0

	TH0=(65536-50000) >> 8;  	//50,000 timer counts
	TL0=(65536-50000) & 0xff; 	//50,000 timer counts

	ET0 = 1; 					//Enable Timer0 Interrupt

	TR0 = TIMER_RUN;			//Start Timer0
} /* initTimer */

void interruptDisable(void) {
	EA = 0;						// Disable Interrupts
} /* interruptDisable */


void interruptEnable(void) {
	EA = 1;						// Enable Interrupts
} /* InetrruptEnable */


void main(void) {
	interruptDisable();				// Disable All Interrupts
		
	initTimer0(); 					// Star timer 0 for status LED
	initSerial();				    // Setup Serial Port
	initAudio();					// Initialize MP3 module
	interruptEnable();				// Enable All Interrupts

	for(;;) {		
		initAll(); 					// Dark all LEDs and get port ready for switch I/P
		stateList();				// Wait for the Player to pick the Name of Constellation
		stateListPost();		   	// Player picked the Name of Constellation
		stateStar();				// Wait for the Player to pick the Constellation
		stateStarPost();			// Player picked the Constellation

		if (stateMatch() == PLAYER_WIN) 	playerWin();
		else			  					playerLose();

#ifdef	DEBUG
		while (getKey(starKey) == KEY_OFF)	// Wait for reset the game...
#else
		while (getKey(demoKey) == KEY_OFF)	// Wait for reset the game... 
#endif
		{
			flashAnswer();
		}
	}
} /* end main */


void isrSerial(void) interrupt 4 using 2 {
	if (RI == 1) {				// Receive somthing from Serial Port
		RI = 0;					// Clear Serial Port Receive Flag
		serialRxData = SBUF;	// Get data from Serial Port
	}
	
	if (TI == 1) {				// Serial Port data transmitted
		TI = 0;					// Clear Serial Port Transmit Flag
	}
} /* isrSerial */


void isrTimer0(void) interrupt 1 using 3 {
	isrTimer0Cnt++;

	if (isrTimer0Cnt >= 10) {
		ledStatusYellow = ~ledStatusYellow;
		ledStatusGreen  = ~ledStatusGreen;
		isrTimer0Cnt=0;
	}

 	TR0 = 0;					//Disable Timer0
	
	TH0=(65536-50000) >> 8;		//50,000 timer counts
	TL0=(65536-50000) & 0xff;	//50,000 timer counts
	
	TR0 = TIMER_RUN;			//Enable Timer0 again		
} /* isrTimer0 */

										 