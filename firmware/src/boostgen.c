#include "LPC17xx.h"
#include "boostgen.h"

// We want approx 59kHz with 90% duty cycle, 0.4mA * 20 = 0.08A
// http://www.ladyada.net/library/diyboostcalc.html

#define PWMENA6 (1<<14)

// Setup PWM channel 6, single edge
// Table 443:
//  Set by Match 0
//  Reset by Match 6
void BoostGen_Setup(void)
{
    // PWM1.6 is P1.26
    // PINSEL3 21:20 = 10
    LPC_PINCON->PINSEL3 &= ~(3<<20);
    LPC_PINCON->PINSEL3 |= 2<<20;
    
    // Disable pull-up and pull-down (10)
    LPC_PINCON->PINMODE3 &= ~(3<<20);
    LPC_PINCON->PINMODE3 |= 2<<20;

    // PCONP.PCPWM1 = 1, power to PWM
    LPC_SC->PCONP |= 1<<6;

    // Select PWM clock = main clock/8 (11 = /8, 01 = mclk)
    LPC_SC->PCLKSEL0 |= (3<<12);

    LPC_PWM1->MR0 = 254;    // PWM rate
    LPC_PWM1->MR6 = 200;    // Duty cycle

    LPC_PWM1->MCR |= 1 << 1; // Reset on PWMMR0: the PWMTC will be reset if PWMMR0 matches it

    // Enable Timer/Counter: Counter enable, PWM enable
    LPC_PWM1->TCR |= (1 << 0) | (1 << 3);   

    // Select single edged mode
    LPC_PWM1->PCR &= ~(1<<6);

    LPC_PWM1->PCR |= PWMENA6;
}

void BoostGen_SetParam(int freq, int duty)
{
    LPC_PWM1->MR0 = freq;    // PWM rate
    LPC_PWM1->MR6 = duty;    // Duty cycle
    LPC_PWM1->LER |= (1<<0) | (1<<6);   // Latch enable for MR0 and MR6
}

void BoostGen_Enable(int enable) 
{
    if (enable) {
        LPC_PWM1->PCR |= PWMENA6;
    } else {
        LPC_PWM1->PCR &= ~PWMENA6;
    }
}
