/**
 * \file LED_Test.h
 * \brief Constants and functions for controlling LEDs on a AT90USBKey
 * 
 * \mainpage Constants and functions for controlling the state of the onboard
 *  LEDs on the AT90USBKey. 
 *
 * \author Alexander M. Hoole
 * \date October 2006
 */

void init_LED_PORTL_pin0(void);
void init_LED_PORTL_pin2(void);
void init_LED_PORTL_pin1(void);
void init_LED_PORTL_pin5(void);
void init_LED_PORTL_pin6(void);
void init_LED_PORTL_pin7(void);
void enable_LED(unsigned int mask);
void disable_LEDs(unsigned int mask);
