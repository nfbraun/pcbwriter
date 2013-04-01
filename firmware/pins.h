#ifndef PCBWRITER_PINS_H
#define PCBWRITER_PINS_H

// DEBUG0_OUT: PB0
#define DEBUG0_OUT_PORT GPIOB
#define DEBUG0_OUT_PIN  GPIO0

// DEBUG1_OUT: PB1
#define DEBUG1_OUT_PORT GPIOB
#define DEBUG1_OUT_PIN  GPIO1

/*	Stepper
	PB11 ORANGE
	PB12 YELLOW
	PB13 BROWN
	PB14 BLACK
 */

#define STEPPER_PORT GPIOB
#define STEPPER_ORANGE_PIN GPIO11
#define STEPPER_YELLOW_PIN GPIO12
#define STEPPER_BROWN_PIN GPIO13
#define STEPPER_BLACK_PIN GPIO14

#endif
