#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/gpio.h"

#include "buttons.h"
#include "config.h"
#include "shared_state.h"

#include <string.h>

enum
{
	HAT_UP = 0,
	HAT_UPRIGHT,
	HAT_RIGHT,
	HAT_DOWNRIGHT,
	HAT_DOWN,
	HAT_DOWNLEFT,
	HAT_LEFT,
	HAT_UPLEFT,
	HAT_NONE = 0x0F
};

void buttons_init(void)
{
	for (unsigned i = 0; i < sizeof(kButtonPins); i++)
	{
		uint8_t pin = kButtonPins[i];
		gpio_init(pin);
		gpio_set_dir(pin, GPIO_IN);
		gpio_pull_up(pin);
	}
}

static inline bool pressed(uint8_t pin)
{
	return gpio_get(pin) == 0;
}

void buttons_task(void)
{
	static uint8_t seq = 0;
	static uint64_t s2_down_since = 0;

	bool s2 = pressed(GPIO_S2);
#ifdef GPIO_S2_ALT
	s2 = s2 || pressed(GPIO_S2_ALT);
#endif

	if (s2)
	{
		uint64_t now = time_us_64();
		if (s2_down_since == 0)
			s2_down_since = now;
		else if (now - s2_down_since >= BOOTSEL_HOLD_US)
		{
			reset_usb_boot(0, 0);
		}
	}
	else
	{
		s2_down_since = 0;
	}

	bool up = pressed(GPIO_UP) || pressed(GPIO_UP_ALT);
#ifdef GPIO_UP_ALT2
	up = up || pressed(GPIO_UP_ALT2);
#endif
	bool down = pressed(GPIO_DOWN);
	bool left = pressed(GPIO_LEFT), right = pressed(GPIO_RIGHT);
	uint8_t dpad = HAT_NONE;
	if (up && right)
		dpad = HAT_UPRIGHT;
	else if (down && right)
		dpad = HAT_DOWNRIGHT;
	else if (down && left)
		dpad = HAT_DOWNLEFT;
	else if (up && left)
		dpad = HAT_UPLEFT;
	else if (up)
		dpad = HAT_UP;
	else if (right)
		dpad = HAT_RIGHT;
	else if (down)
		dpad = HAT_DOWN;
	else if (left)
		dpad = HAT_LEFT;

	uint8_t b0 = dpad & 0x0F;
	if (pressed(GPIO_B3))
		b0 |= (1 << 4); // West  / Square
	if (pressed(GPIO_B1))
		b0 |= (1 << 5); // South / Cross
	if (pressed(GPIO_B2))
		b0 |= (1 << 6); // East  / Circle
	if (pressed(GPIO_B4))
		b0 |= (1 << 7); // North / Triangle

	uint8_t b1 = 0;
	if (pressed(GPIO_L1))
		b1 |= (1 << 0);
	if (pressed(GPIO_R1))
		b1 |= (1 << 1);
	if (pressed(GPIO_L2))
		b1 |= (1 << 2);
	if (pressed(GPIO_R2))
		b1 |= (1 << 3);
	bool s1 = pressed(GPIO_S1);
#ifdef GPIO_S1_ALT
	s1 = s1 || pressed(GPIO_S1_ALT);
#endif
	if (s1)
		b1 |= (1 << 4); // Select / Share
	if (s2)
		b1 |= (1 << 5); // Start
	if (pressed(GPIO_L3))
		b1 |= (1 << 6);
	bool r3 = pressed(GPIO_R3);
#ifdef GPIO_R3_ALT
	r3 = r3 || pressed(GPIO_R3_ALT);
#endif
	if (r3)
		b1 |= (1 << 7);

	uint8_t b2 = 0;
	bool a1 = pressed(GPIO_A1);
#ifdef GPIO_A1_ALT
	a1 = a1 || pressed(GPIO_A1_ALT);
#endif
	if (a1)
		b2 |= (1 << 0); // Home / PS
	if (pressed(GPIO_A2))
		b2 |= (1 << 1); // touchpad click

	uint8_t hdr[12];
	hdr[0] = 0x01;													 // report id (also first "key" byte for the dongle)
	hdr[1] = 0x80;													 // lx centered
	hdr[2] = 0x80;													 // ly
	hdr[3] = 0x80;													 // rx
	hdr[4] = 0x80;													 // ry
	hdr[5] = pressed(GPIO_L2) ? 0xFF : 0x00; // lt
	hdr[6] = pressed(GPIO_R2) ? 0xFF : 0x00; // rt
	hdr[7] = seq;														 // sequence number
	hdr[8] = b0;
	hdr[9] = b1;
	hdr[10] = b2;
	hdr[11] = 0x00;

	// Detect change (ignore the seq byte for the change test).
	bool changed = false;
	for (int i = 0; i < 12; i++)
	{
		if (i == 7)
			continue;
		if (hdr[i] != g_state.input12[i])
		{
			changed = true;
			break;
		}
	}

	if (changed)
		seq++;

	hdr[7] = seq;

	for (int i = 0; i < 12; i++)
		g_state.input12[i] = hdr[i];
}
