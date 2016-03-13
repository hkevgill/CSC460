#include <avr/io.h>
#include "LED_Test.h"
/**
 * \file LED_Test.c
 * \brief Small set of test functions for controlling LEDs on a AT90USBKey
 * 
 * \mainpage Simple set of functions to control the state of the onboard
 *  LEDs on the AT90USBKey. 
 *
 * \author Alexander M. Hoole
 * \date October 2006
 */

void init_LED_PORTL_pin0(void) {
    DDRL |= _BV(DDL0);
}

void init_LED_PORTL_pin1(void) {
    DDRL |= _BV(DDL1);
}

void init_LED_PORTL_pin2(void) {
    DDRL |= _BV(DDL2);  // Set DDL2 bit of the DDRL register
}

void init_LED_PORTL_pin5(void) {
    DDRL |= _BV(DDL5);  // Set DDL5 bit of the DDRL register
}

void init_LED_PORTL_pin6(void) {
    DDRL |= _BV(DDL6);  // Set DDL6 bit of the DDRL register
}

void enable_LED(unsigned int mask) {
    /* set pin 47 high to turn led on */
    PORTL |= _BV(mask);
}

void disable_LED(unsigned int mask) {
    PORTL &= ~_BV(mask);
}

void toggle_LED(unsigned int mask) {
    PORTL ^= _BV(mask);
}