#ifndef __SETTINGS__
#define __SETTINGS__

/* SETTINGS.H
 * Replace the value of a '#define' statement to edit
 * that parameter. If it has no value, then uncomment it
 * to enable it or comment it to disable it.
 */


/* ---DEBUG--- */
//#define VERBOSE
//#define PRINT_RAW_SERIAL
#define DISPLAY_VIDEO 0     /* undef: no video   0: raw input   1: threshold */
//#define DRAW_CONTOURS
#define DRAW_CONVEX_HULL
//#define DRAW_BOUNDING_BOXES



/* ---HADRWARE CONNECTIONS--- */

/* SERIAL CONNECTION TO RIO */
#define SERIAL_PORT "/dev/serial0"
#define BAUDRATE    38400

/* CAMERA */
#define FRAME_WIDTH     640  /* Low Res: 640   HD: 1080 */
#define FRAME_HEIGHT    480  /* Low Res: 480   HD: 720  */
#define MAX_FPS         255

/* LIGHT RING */
#define LED_GPIO_PIN    18
//#define ACTIVATE_LIGHT_RING 0X0000FF00 /* bytes are arranged: WWBBGGRR where W represents white */
#define TARGET_FREQ     WS2811_TARGET_FREQ
#define DMA             5
//#define STRIP_TYPE WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
#define STRIP_TYPE      WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)
#define LED_WIDTH       12
#define LED_HEIGHT      1
#define LED_COUNT       (LED_WIDTH * LED_HEIGHT)



/* ---VISION DETECTION PARAMETERS--- */

/* LOW SEGMENTATION BOUNDARY */
#define LOW_SEG_R 100
#define LOW_SEG_G 100
#define LOW_SEG_B 0

/* HIGH SEGMENTATION BOUNDARY */
#define HIGH_SEG_R 255
#define HIGH_SEG_G 255
#define HIGH_SEG_B 50

/* OBJECT SEGMENTATION PARAMETERS */
#define BOUNDING_BOX_MIN_WIDTH  FRAME_WIDTH  * 0.12
#define BOUNDING_BOX_MIN_HEIGHT FRAME_HEIGHT * 0.12

/* BASELINE BRIGHTNESS RGB */
#define BASE_R 123
#define BASE_G 123
#define BASE_B 123

#define BRIGHTNESS_ADJUSTMENT 0.10


#endif /* __SETTINGS__ */
