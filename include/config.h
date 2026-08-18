// Board config for testing on the "SplitFox Pro" board (Speedy Labs split).
// Pin mapping ported from GP2040-CE commit 4130799 (configs/SplitFoxPro/BoardConfig.h).
//
// NOTE ON ROLES: GP2040-CE runs the buttons AND presents the DualSense over
// GPIO 2/3 (its USB *peripheral*). This firmware instead presents the DualSense
// over the RP2040's *native* USB, and uses a PIO USB *host* port to talk to the
// Mayflash S5 dongle. On SplitFox Pro, GPIO 2/3 are the broken-out USB data
// pins, so the S5 dongle host port is placed there (matching SplitFox polarity:
// D+ = GPIO3, D- = GPIO2 -- reversed vs the Pico-PIO-USB default).
//
// SPLIT BOARD: SplitFox Pro maps several physical switches (two hand clusters)
// to the same logical button. Those extra pins are exposed here as *_ALT
// defines and OR-combined in buttons.cpp.
#pragma once

#include <stdint.h>

// SplitFox: USB_PERIPHERAL_PIN_DPLUS 3 + USB_PERIPHERAL_PIN_ORDER 1
//   => D+ = GPIO 3, D- = GPIO 2 (reversed order).
// Pico-PIO-USB defaults to D- = D+ + 1, so main.cpp flips the pinout to DMDP.
#define PIO_USB_DP_PIN 3
#define PIO_USB_PINOUT_REVERSED 1

// SplitFox rgb pin, single WS2812 chain on GPIO 21:
//   LIGHT_DATA defines 18 lights * LEDS_PER_PIXEL 2 = 36 pixels.
// This firmware floods the whole strip with the lightbar color.
#define BOARD_LEDS_PIN 21
#define LED_COUNT 36

// SplitFox display: i2c0 on SDA=0 / SCL=1
#define DISPLAY_I2C i2c0
#define DISPLAY_SDA_PIN 0
#define DISPLAY_SCL_PIN 1
#define DISPLAY_I2C_ADDR 0x3C
#define DISPLAY_I2C_HZ 1000000

// SplitFox Pro primary button GPIO map (physical pin -> button)
#define GPIO_UP 28
#define GPIO_DOWN 27
#define GPIO_RIGHT 29
#define GPIO_LEFT 26
#define GPIO_B1 9  // Cross
#define GPIO_B2 14 // Circle
#define GPIO_R2 15
#define GPIO_L2 18
#define GPIO_B3 10 // Square
#define GPIO_B4 11 // Triangle
#define GPIO_R1 12
#define GPIO_L1 13
#define GPIO_S1 24 // Select / Share
#define GPIO_S2 25 // Start  (also: HOLD => BOOTSEL)
#define GPIO_L3 4
#define GPIO_R3 22
#define GPIO_A1 23 // PS / Home
#define GPIO_A2 20 // Touchpad click

// Alternate pins: the second hand cluster maps these extra switches to the
// same logical buttons. buttons.cpp OR-combines each *_ALT with its base pin.
#define GPIO_UP_ALT 19
#define GPIO_UP_ALT2 16
#define GPIO_R3_ALT 17
#define GPIO_A1_ALT 6
#define GPIO_S1_ALT 7
#define GPIO_S2_ALT 8

// How long S2 must be held to trigger BOOTSEL
#define BOOTSEL_HOLD_US (1000u * 1000u)

#define DONGLE_BT_ENABLED_DEFAULT 0
#define GPIO_BT_TOGGLE GPIO_S1

// Hmm, perhaps this should be in buttons.cpp
static const uint8_t kButtonPins[] = {
		GPIO_UP,
		GPIO_DOWN,
		GPIO_RIGHT,
		GPIO_LEFT,
		GPIO_B1,
		GPIO_B2,
		GPIO_R2,
		GPIO_L2,
		GPIO_B3,
		GPIO_B4,
		GPIO_R1,
		GPIO_L1,
		GPIO_S1,
		GPIO_S2,
		GPIO_L3,
		GPIO_R3,
		GPIO_A1,
		GPIO_A2,
		GPIO_UP_ALT,
		GPIO_UP_ALT2,
		GPIO_R3_ALT,
		GPIO_A1_ALT,
		GPIO_S1_ALT,
		GPIO_S2_ALT,
};
