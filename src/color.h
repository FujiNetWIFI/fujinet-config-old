/**
 * CONFIG - color functions
 */

#ifndef COLOR_H
#define COLOR_H

void color_luminanceIncrease(void);
void color_luminanceDecrease(void);
void color_hueIncrease(void);
void color_hueDecrease(void);

#define COLOR_SETTING_NETWORK 0x66
#define COLOR_SETTING_FAILED 0x33
#define COLOR_SETTING_SUCCESSFUL 0xB4
#define COLOR_CHECKING_NETWORK 0x26

#endif /* COLOR_H */
