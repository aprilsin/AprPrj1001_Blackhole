/****************************************************************************
 File:			switch001.c

 Version:		0.4

 Description:	Random number and switch test

 Last modified:	2015-03-12
 Created on:	2015-02-22
 Created by:	April

 Board:			RichMCU RZ-51V2.0

 Jumpers:		Remove JPSMG - Disable 7-Segment LEDs

				将 JPSMG 数码管控制跳线拔掉使数码管停	止工作。
****************************************************************************/
#include <STC89.H>

//#define	DEBUG
//#define	RICHMCU

#define	TIMER_RUN		1					// 1: ON 0: OFF

#define	LOW				0
#define	HIGH			1

#define	EOD				0xff
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
#define	RND_ANS			3

#define	PLAYER_WIN		1
#define	PLAYER_LOSE		0

#define	SFX_BKG_MUSIC		0
#define	SFX_KEY_CLICK		1
#define	SFX_KEY_CLICK2		2
#define	SFX_KEY_CLICK3		3
#define	SFX_LIST_START		5
#define	SFX_LIST_SLOWDN		8
#define SFX_LIST_SPEEDUP	8
#define	SFX_STAR_START		5
#define	SFX_STAR_SLOWDN		8
#define	SFX_SHOW_ANS		4
#define	SFX_LOSE			7
#define	SFX_WIN				6


/******************************************
 Global Variables
 ******************************************/
unsigned int isrTimer0Cnt;

unsigned char demoKeyStatus;

unsigned char playerPickedList;
unsigned char playerPickedStar;

unsigned int delaySlowdown[]= { 3500,  3600,  3800,  4200, 5000, 6600, 
						  		9800, 16200, 29000, 54600, EOD};

unsigned int delaySpeedUp[]= {	54600, 29000, 16200, 9800, 6600, 5000,
								4200, 3800, 3600, 3500, EOD};

code const unsigned char audioSfxButton[]     = {3, 7, 11, EOD};
code const unsigned char audioSfxButtonFail[] = {15, 17, 23, EOD};
code const unsigned char audioSfxSlowdown[]   = {31, 33, EOD};
code const unsigned char audioSfxFailLong[]   = {35, EOD};
code const unsigned char audioSfxLose[]		  = {51, 55, 27, EOD};
code const unsigned char audioSfxWin[]		  = {105, 103, 19, 25, 27, 35, 45, 57, EOD};

/******************************************
 Prototypes
 ******************************************/
unsigned char getRandom(unsigned char choice);
void audioPlayTrack(unsigned char sfx);
void audioPlay(void);
void audioPause(void);


/*********************************PROGRAM**********************************/
void delay(unsigned int d){
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


unsigned char ledOn(unsigned char led){
	return ~led;
} /* end ledOn */


/******RANDOM****///////////////////////////
unsigned char getRandom(unsigned char choice){
	static unsigned int	rnd=0;
	unsigned char random;

	/* Store data into ROM area */
	code const unsigned char rndList[] = { 
								6, 4, 0, 7, 6, 4, 2, 7, 3, 0, 5, 1, 5, 6, 5, 4, 5, 7, 4, 1,
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
								7, 0, 4, 1, 5, 0, 3, 4, 2, 5, 7, 5, 1, 7, 6, 5, 4, 6, 7, 4,
								6, 7, 0, 1, 6, 4, 5, 6, 5, 7, 0, 4, 5, 3, 2, 5, 2, 4, 5, 7,
					  		    EOD};

	code const unsigned char rndStar[]={
								6, 7, 0, 1, 6, 4, 5, 6, 5, 7, 0, 4, 5, 3, 2, 5, 2, 4, 5, 7,
								7, 0, 4, 1, 5, 0, 3, 4, 2, 5, 7, 5, 1, 7, 6, 5, 4, 6, 7, 4,
		 						6, 4, 0, 7, 6, 4, 2, 7, 3, 0, 5, 1, 5, 6, 5, 4, 5, 7, 4, 1,
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
					  		    EOD};

	code const unsigned char rndLose[]={
								3, 4, 1, 2, 4, 0, 5, 0, 5, 7, 3, 1, 6, 3, 0, 2, 3, 5, 0, 3,
								6, 0, 4, 3, 6, 2, 0, 1, 5, 6, 4, 0, 7, 3, 6, 2, 3, 6, 2, 0,
								EOD};	
	
	if(choice == 1){
		random = rndList[rnd++];
		if (rndList[rnd] == EOD) rnd = 0;		
	}

	if(choice == 2){
		random = rndStar[rnd++];
		if (rndStar[rnd] == EOD) rnd = 0;
	}
	else{
		random = rndLose[rnd++];
		if (rndLose[rnd] == EOD) rnd = 0;
	}
		
	return random;
} /* end getRandom */


void serialInit(void)
{
    TMOD = 0x21;
    SCON = 0x50;
    TH1 = 0xFD;
    TL1 = TH1;
    PCON = 0x00;
    TR1 = 1;				
} /* serialInit */

void serialTx(unsigned char c)
{
    SBUF = c;
    while(!TI);
    TI = 0;
} /* serialTx */


unsigned char audioIsBusy(void) {
	return ~P17;			 	// 1: Busy  0: Ready for command
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
	static unsigned char audioCmdPause[]={0x7e, 0xff, 0x06, 0x0e, 0x00, 00, 00, 0xfe, 0xfe, 0xef};

	while (audioIsBusy());
	audioSendCmd(audioCmdPause);
} /* audioPlay */

void audioPlay(void) {
	static unsigned char audioCmdPlay[]={0x7e, 0xff, 0x06, 0x0d, 0x00, 01, 01, 0xfe, 0xfe, 0xef};

	while (audioIsBusy());
	audioSendCmd(audioCmdPlay);
} /* audioPlay */


void audioPlayTrack(unsigned char track) {
	static unsigned char audioCmdPlayTrack[]={0x7e, 0xff, 0x06, 0x03, 0x00, 0x00, 01, 0xfe, 0xfe, 0xef};

	while (audioIsBusy());
	audioCmdPlayTrack[6] = track;			// Audio Track  Number
	audioSendCmd(audioCmdPlayTrack);
} /* audioPlayTrack */


void audioInit(void) {
	while (audioIsBusy());			// Wait until audio is NOT busy
	delayms(5000);					// Wait 5 sec for the module to scan the devices (TF card)
		
	/* Stop Audio */
	audioPause();
} /* udioInit */


unsigned char getKey(unsigned char key) {
	unsigned char keyStatus;

	do {
		keyStatus = key;

		delayms(DELAY_KEYBOUNCE);

	} while (key != keyStatus);
	
	return keyStatus;

} /* keyIsPressed */


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

	demoKeyStatus = KEY_OFF;
		
	/* Init player selection to invalid */
	playerPickedList = NO_PLAYER;

	/* Stop Audio */
	audioPause();

	/* Play Music */
	audioPlayTrack(SFX_BKG_MUSIC);

} /* initAll */


///****LIST***///////////////////////////
void stateList(void) {
	while (getKey(listKey) == KEY_OFF);					// listKey Off, wait for the listKey pressed

	audioPlayTrack(SFX_KEY_CLICK);							// listKey is ON now
	while (audioIsBusy());
   	audioPlayTrack(SFX_LIST_START);

	while (getKey(listKey) == KEY_ON) {					// listKey is still On
		portList = ledOn(1 << getRandom(RND_LIST));     // Get random no. from List array
		
		delay(DELAY_STD);
	}				  									// listKey Off now
} /* stateList */


void stateListPost(void) {
	int a;

   	a = 0;
	audioPlayTrack(SFX_LIST_SLOWDN);
    while (delaySlowdown[a]!= EOD) {
		playerPickedList = getRandom(RND_LIST);			// Get random no. from List array
		portList = ledOn(1 << playerPickedList);	    // Show picked Name of Constellation

		delay(delaySlowdown[a++]);
	}
} /* end stateListPost */


////****STAR***//////////////////////////////
void stateStar(void) {
	while (getKey(starKey) == KEY_OFF);					// starKey Off, wait for starKey pressed

	audioPlayTrack(SFX_KEY_CLICK);						// starKey is ON now
	while (audioIsBusy());   
  	audioPlayTrack(SFX_STAR_START);
											  	
	while (getKey(starKey) == KEY_ON) {		  		 	// starKey is still ON
		portStar = ledOn(1 << getRandom(RND_STAR));	    // Get random no. from the Star array
		
		delay(DELAY_STD); 
	}													// starKey Off
} /* end stateStar */


void stateStarPost(void) {
	int a;

    a = 0;
	audioPlayTrack(SFX_STAR_SLOWDN);
	while (delaySlowdown[a]!= EOD) {		    
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
    
	audioPlayTrack(SFX_KEY_CLICK3);

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
	audioPlayTrack(SFX_WIN);
#ifdef DEBUG
	P1=0xF0;
#endif
} /* end playerWin */

void playerLose(void) {
	audioPlayTrack(SFX_LOSE);
#ifdef DEBUG
	P1=0xF3;
#endif

} /* end playerWin */
//void playerLose(void){
//	int a;
//	unsigned char ansList;
//
//	audioPlayTrack(SFX_LOSE);
//
//    a = 0;
//	audioPlayTrack(SFX_LIST_SPEEDUP);	
//	while (delaySpeedUp[a]!= EOD) {
//		ansList = getRandom(RND_ANS);			// Get random no. from the Lose array
//		portList = ledOn(1 << ansList); 		// Show randomed Constellation Name
//		delay(delaySpeedUp[a++]);
//	}
//	portList = ledOn(LED_ON);					// Turn On all List LEDs
//#ifdef DEBUG
//	P1=0xF3;
//#endif	
//} /* end playerLose */

	   
void initTimer0(void) {
	isrTimer0Cnt = 0;  	//isrTimer0 interrupt calling counter

	EA = 0;				//Disable all inerrupts

	TMOD = T0_M1;		//Set Timer0 to Mode 1

	TR0 =  0;			//Disable Timer0

	TH0=(65536-50000) >> 8;  	//50,000 timer counts
	TL0=(65536-50000) & 0xff; 	//50,000 timer counts

	ET0 = 1; 					//Enable Timer0 Interrupt
	EA  = 1;					//Enable all interrupts

	TR0 = TIMER_RUN;			//Start Timer0
} /* initTimer */


void main(void){	
	initTimer0(); 					// Star timer 0 for status LED
	serialInit();
	audioInit();

	for(;;) {		
		initAll(); 					// Dark all LEDs and get port ready for switch I/P
		stateList();				// Wait for the Player to pick the Name of Constellation
		stateListPost();		   	// Player picked the Name of Constellation
		stateStar();				// Wait for the Player to pick the Constellation
		stateStarPost();			// Player picked the Constellation

		if (stateMatch() == PLAYER_WIN) 	playerWin();
		else			  					playerLose();

		audioPlayTrack(SFX_SHOW_ANS);

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


void isrTimer0(void) interrupt 1 using 2 {
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
