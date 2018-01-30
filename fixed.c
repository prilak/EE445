// filename ******** fixed.h ************** 
// possible header file for Lab 1 Spring 2018
// feel free to change the specific syntax of your system
// Michael Lawrence
// 01/24/2018

#include <stdint.h>
#include "ST7735.h"


// Helper functions
// void ST7735_OutString(char *ptr);

/****************ST7735_sDecOut2***************
 converts fixed point number to LCD
 format signed 32-bit with resolution 0.01
 range -99.99 to +99.99
 Inputs:  signed 32-bit integer part of fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
 12345    " **.**"
  2345    " 23.45"  
 -8100    "-81.00"
  -102    " -1.02" 
    31    "  0.31" 
-12345    "-**.**"
 */ 
void ST7735_sDecOut2(int32_t n){
	
	// Out of bounds
	if(n <= -10000){
		ST7735_OutString("-**.**");
		return;
	} else if(n >= 10000){
		ST7735_OutString(" **.**");
		return;
	}
	
	char str[6];
	if(n >= 0){
		str[0] = ' ';
	} else {
		str[0] = '-';
		n *= -1;
	} 
	for(int i = 5; i > 0; i--){// convert n to string
		if(i == 3){
			str[i] = '.';
			continue;
		}
		
		str[i] = (n % 10) + 48;
		if(i == 1 && str[i] == '0') {// deals with 0 before .
			if(str[i-1] == '-'){
				str[i] = '-';
				str[i-1] = ' ';
			} else {
				str[i] = ' ';
			}
			
			
		}
		n /= 10;
		
		
		
		
	}
	//str[6] = '\n';
	
	ST7735_OutString(str);
}

void convertIntToString(uint32_t n, char* str){
	for(int i = 2; i >= 0; i--){
		str[i] = (n % 10) + 48;
		n /= 10;
	}
	
	// adjusts to 00 and 000 cases
	if(str[0] == '0'){
		str[0] = ' ';
		if(str[1] == '0'){
			str[1] = ' ';
		}
	}
}
/**************ST7735_uBinOut6***************
 unsigned 32-bit binary fixed-point with a resolution of 1/64. 
 The full-scale range is from 0 to 999.99. 
 If the integer part is larger than 63999, it signifies an error. 
 The ST7735_uBinOut6 function takes an unsigned 32-bit integer part 
 of the binary fixed-point number and outputs the fixed-point value on the LCD
 Inputs:  unsigned 32-bit integer part of binary fixed-point number
 Outputs: none
 send exactly 6 characters to the LCD 
Parameter LCD display
     0	  "  0.00"
     1	  "  0.01"
    16    "  0.25"
    25	  "  0.39"
   125	  "  1.95"
   128	  "  2.00"
  1250	  " 19.53"
  7500	  "117.19"
 63999	  "999.99"
 64000	  "***.**"
*/
void ST7735_uBinOut6(uint32_t n){
	// edge cases
	if(n >= 64000){
		ST7735_OutString("***.**");
		return;
	}
	
	char str[6];
	
	uint32_t rem = 0;
	rem = n % 64;
	n /= 64;
	
	convertIntToString(n, str);
	
	str[3] = '.';
	
	// for decimal portion
	
	for(int i = 4; i < 6; i++){
		rem *= 10;
		str[i] = (rem / 64) + 48;
		rem = rem % 64;
		
		//for rounding
		if(i==5){
			rem *= 10;
			if(rem / 64 >= 5){
				str[i]++;
			}
		}
		
		
	}
	
	
	
	
	ST7735_OutString(str);
}	



/**************ST7735_XYplotInit***************
 Specify the X and Y axes for an x-y scatter plot
 Draw the title and clear the plot area
 Inputs:  title  ASCII string to label the plot, null-termination
          minX   smallest X data value allowed, resolution= 0.001
          maxX   largest X data value allowed, resolution= 0.001
          minY   smallest Y data value allowed, resolution= 0.001
          maxY   largest Y data value allowed, resolution= 0.001
 Outputs: none
 assumes minX < maxX, and miny < maxY
*/
int32_t Xmx;
int32_t Xmn;
int32_t Ymx;
int32_t Ymn;
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_FillScreen(0);// clear to black
	ST7735_SetCursor(0,0);
	ST7735_OutString(title);
	
	Xmn = minX;
	Xmx = maxX;
	Ymn = minY;
	Ymx = maxY;
	
	
	ST7735_FillRect((0), (20), (127), (255), ST7735_WHITE);
	ST7735_SetCursor(maxX, maxY);
}

/**************ST7735_XYplot***************
 Plot an array of (x,y) data
 Inputs:  num    number of data points in the two arrays
          bufX   array of 32-bit fixed-point data, resolution= 0.001
          bufY   array of 32-bit fixed-point data, resolution= 0.001
 Outputs: none
 assumes ST7735_XYplotInit has been previously called
 neglect any points outside the minX maxY minY maxY bounds
*/
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	int32_t Xrange = Xmx - Xmn;
	int32_t Yrange = Ymx - Ymn;
	for(int i = 0; i < num; i++){
		int32_t x = bufX[i];
		int32_t y = bufY[i];
		
		if(x<Xmn) x=Xmn;
		if(x>Xmx) x=Xmx;
		if(y<Ymn) y=Ymn;
		if(y>Ymx) y=Ymx;
	
		// X goes from 0 to 127
		// j goes from 159 to 32
		// y=Ymax maps to j=32
		// y=Ymin maps to j=159
		int j = 127 - (127 * (Xmx - x))/Xrange;
		int k = 32+(127*(Ymx-y))/Yrange;
		
		if(j<0) j = 0;
		if(j>127) j = 127;
		if(k<32) k = 32;
		if(k>159) k = 159;
		ST7735_DrawPixel(j,   k,   ST7735_BLUE);
		ST7735_DrawPixel(j+1, k,   ST7735_BLUE);
		ST7735_DrawPixel(j,   k+1, ST7735_BLUE);
		ST7735_DrawPixel(j+1, k+1, ST7735_BLUE);
		
	}
	
 
}



