#include "gba.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;
u32 vBlankCounter = 0;

void waitForVBlank(void) {
	while (*SCANLINECOUNTER > 160); 
	while (*SCANLINECOUNTER < 160);
}

static int __qran_seed= 42;
static int qran(void) {
    __qran_seed= 1664525*__qran_seed+1013904223;
    return (__qran_seed>>16) & 0x7FFF;
}

int randint(int min, int max) {
    return (qran()*(max-min)>>15)+min;
}

void setPixel(int x, int y, u16 color) {
    // TA-TODO: IMPLEMENT
	videoBuffer[OFFSET(x, y, 240)] = color;
}

void drawRectDMA(int x, int y, int width, int height, volatile u16 color) {
    // TA-TODO: IMPLEMENT
	for (int r = 0; r < height; r++) {	
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(x + r, y, 240)];
		DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

void drawFullScreenImageDMA(const u16 *image) {
    // TA-TODO: IMPLEMENT
	for (int i = 0; i < HEIGHT; i++) {
		DMA[3].src = image + i * WIDTH;
		DMA[3].dst = videoBuffer + OFFSET(i, 0, WIDTH);
		DMA[3].cnt = DMA_ON | WIDTH;
	}
    
}

void drawImageDMA(int x, int y, int width, int height, const u16 *image) {
    // TA-TODO: IMPLEMENT
	for (int i = 0; i < height; i++) {
		DMA[3].src = image + i * width;
		DMA[3].dst = videoBuffer + OFFSET(x + i, y, WIDTH);
		DMA[3].cnt = DMA_ON | width;
	}
}

void fillScreenDMA(volatile u16 color) {
    // TA-TODO: IMPLEMENT
	for (int i = 0; i < HEIGHT; i++) {
		DMA[3].src = &color;
		DMA[3].dst = &videoBuffer[OFFSET(i, 0, 240)];
		DMA[3].cnt = WIDTH | DMA_ON | DMA_SOURCE_FIXED;
	}
}

void drawChar(int col, int row, char ch, u16 color) {
    for(int r = 0; r<8; r++) {
        for(int c=0; c<6; c++) {
            if(fontdata_6x8[OFFSET(r, c, 6) + ch*48]) {
                setPixel(row+r, col+c, color);
            }
        }
    }
}

void drawString(int col, int row, char *str, u16 color) {
    while(*str) {
        drawChar(col, row, *str++, color);
        col += 6;
    }
}

void drawCenteredString(int x, int y, int width, int height, char *str, u16 color) {
    u32 len = 0;
    char *strCpy = str;
    while (*strCpy) {
        len++;
        strCpy++;
    }

    u32 strWidth = 6 * len;
    u32 strHeight = 8;

    int col = x + ((width - strWidth) >> 1);
    int row = y + ((height - strHeight) >> 1);
    drawString(col, row, str, color);
}
