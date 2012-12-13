#include <msp430.h>
//#include "msp430g2452.h"

#define shamash BIT0
#define day1 BIT1
#define day2 BIT2
#define day3 BIT4
#define day4 BIT5
#define day5 BIT6
#define day6 BIT7
#define day7 BIT0
#define day8 BIT1


#define btn BIT3

#define flickerLen 16

const unsigned char FLICKERPATTERN[] = { 14, 5, 11, 14, 8, 12, 3, 15, 1, 13, 18, 12, 7, 15, 16, 2};

int x, y;

int day = -1;
int activated = 0;
unsigned int milliseconds = 0;
unsigned int minutes = 0;
unsigned int hours = 0;
unsigned int flickerOffset = 0;

int flickerCount[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void nextDay(void);
void flicker(void);
int nextCount(void);
void sleep(void);

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    // Set CPU and DCO clock to 1 MHz
	BCSCTL1 = CALBC1_1MHZ; // Set range
	DCOCTL = CALDCO_1MHZ; // SMCLK = DCO = 1MHz

	// Set up timer
    TACCTL0 = OUT;
	TACTL = TASSEL_2; // SMCLK, timer off (for power consumption)

    P1DIR = 0xFF;
	P1DIR &= ~btn;
	P1IES |= btn;
	P1IFG &= ~btn;
	P1IE |= btn;
	P1OUT = 0x00;

	P2DIR = 0xFF;
	P2OUT = 0x00;

    __bis_SR_register(CPUOFF + GIE); // interrupts enabled
	
	return 0;
}


void nextDay(void){
	day++;
	if (day > 8) {
		day = -1;
	}

	if (day < 0) {
		sleep();
	}
	else if (day == 0){
		P1OUT = shamash;
		P2OUT = 0x00;
	    TACTL = TASSEL_2 + MC_2; // SMCLK, start in continuous mode
	    // Set frequency
	    TACCR0 = 1000;
	    TACCTL0 = OUTMOD0 + CCIE;
	}
	else {
		P1OUT = shamash;
		P2OUT = 0x00;
	}

	if (day >= 1)
		P1OUT |= day1;
	if (day >= 2)
		P1OUT |= day2;
	if (day >= 3)
		P1OUT |= day3;
	if (day >= 4)
		P1OUT |= day4;
	if (day >= 5)
		P1OUT |= day5;
	if (day >= 6)
		P1OUT |= day6;
	if (day >= 7)
		P2OUT |= day7;
	if (day >= 8)
		P2OUT |= day8;

	activated = (P2OUT << 8) | P1OUT;
}

int nextCount() {
	int val = FLICKERPATTERN[flickerOffset++];
	if (flickerOffset >= flickerLen)
		flickerOffset = 0;
	return val;
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
	if((P1IFG & btn) == btn) {
		P1IFG &= ~btn; // Clear RXD IFG (interrupt flag)

		nextDay();
	}

}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
	int a = activated & 0xFF;
	int b = activated >> 8;

	if ((a & shamash) == shamash) {
		if (flickerCount[0] == 0) {
			flickerCount[0] = nextCount();
			P1OUT &= ~shamash;
		}
		else {
			flickerCount[0]--;
			P1OUT |= shamash;
		}
	}

	if (((a & day1)) == day1) {
		if (flickerCount[1] == 0) {
			flickerCount[1] = nextCount();
			P1OUT &= ~day1;
		}
		else {
			flickerCount[1]--;
			P1OUT |= day1;
		}
	}
	if ((a & day2) == day2) {
		if (flickerCount[2] == 0) {
			flickerCount[2] = nextCount();
			P1OUT &= ~day2;
		}
		else {
			flickerCount[2]--;
			P1OUT |= day2;
		}
	}
	if ((a & day3) == day3) {
		if (flickerCount[3] == 0) {
			flickerCount[3] = nextCount();
			P1OUT &= ~day3;
		}
		else {
			flickerCount[3]--;
			P1OUT |= day3;
		}
	}
	if ((a & day4) == day4) {
		if (flickerCount[4] == 0) {
			flickerCount[4] = nextCount();
			P1OUT &= ~day4;
		}
		else {
			flickerCount[4]--;
			P1OUT |= day4;
		}
	}
	if ((a & day5) == day5) {
		x = a;
		y = day5;
		if (flickerCount[5] == 0) {
			flickerCount[5] = nextCount();
			P1OUT &= ~day5;
		}
		else {
			flickerCount[5]--;
			P1OUT |= day5;
		}
	}
	if ((a & day6) == day6) {
		if (flickerCount[6] == 0) {
			flickerCount[6] = nextCount();
			P1OUT &= ~day6;
		}
		else {
			flickerCount[6]--;
			P1OUT |= day6;
		}
	}
	if ((b & day7) == day7) {
		if (flickerCount[7] == 0) {
			flickerCount[7] = nextCount();
			P2OUT &= ~day7;
		}
		else {
			flickerCount[7]--;
			P2OUT |= day7;
		}
	}
	if ((b & day8) == day8) {
		if (flickerCount[8] == 0) {
			flickerCount[8] = nextCount();
			P2OUT &= ~day8;
		}
		else {
			flickerCount[8]--;
			P2OUT |= day8;
		}
	}

	milliseconds++;
	if (milliseconds == 900) {
		minutes++;
		milliseconds = 0;
	}

	if (minutes == 360) {
		sleep();
	}

}

void sleep() {
	day = -1;
	P1OUT = 0x00;
	P2OUT = 0x00;
	TACTL = TASSEL_2; // SMCLK, timer off (for power consumption)
	milliseconds = 0;
	minutes = 0;
	hours = 0;
}

