#include <p24FJ256GB206.h>
#include "defines.h"


#define CLK_TRIS    	TRISDbits.TRISD2		
#define CLK_OUT  		LATDbits.LATD2
#define CMD_TRIS    	TRISDbits.TRISD1	
#define CMD_OUT     	LATDbits.LATD1
#define DAT0_TRIS    	TRISDbits.TRISD0
#define DAT0_OUT     	LATDbits.LATD0
#define TRIS_LED  		TRISEbits.TRISE4
#define LED  LATEbits.LATE4

//#define CI_state()		PINC&_BV(CI) //считывает линию card insert

   void bus_init(void){
        TRIS_LED=0;
        LED=1;
        
        ANSD=0x0000;
        TRISD=0xFFFF;
        PORTD=0x0000;
		LATD=0x0000;	//сброс настроек конфигурации входа/выхода для всех линий
		CLK_TRIS=0 ;	//CLK на выход
		CLK_low();	//пассивный уровень - низкий
        
  
		
}
   void CLK_low(void){
		CLK_OUT=0;// Опускаем CLK
		delay_CLK();//задержка чтобы снизить тактовую частоту  
		
}

   void CLK_high(void){
		CLK_OUT=1;// Поднимаем CLK
		delay_CLK();//задержка чтобы снизить тактовую частоту  
		
}
   void CMD_to_output(void) {
		CMD_TRIS=0; // Переключить линию CMD на выход
	    CMD_OUT=1;  //задать высокий, уровень, так как он является пассивным
}
   void CMD_low(void) {
	    CMD_OUT=0;
}

 void  CMD_high(void) {
	    CMD_OUT=1;
}
 void DAT0_low(void) {
	    DAT0_OUT =0;
}
 void DAT0_high(void) {
	    DAT0_OUT =1;
}
   void CMD_to_input(void) {
		CMD_TRIS=1; //переключить CMD на вход
        CMD_OUT=0;
}

   void DAT0_to_output(void) {
	    DAT0_TRIS=0;	 //переключение линии DAT0 на выход
        DAT0_OUT=1; 	//пассивное состояние высокое
}

  void tick(void) {
	   	 CLK_OUT=1; //поднятие уровня на тактовой линии
		 delay_CLK(); //задержка 
         CLK_OUT=0 ; //опустить уровень
	   	 delay_CLK(); //задежка 
}
   void DAT0_to_input(void) { 
	    DAT0_TRIS=0; //переключение линии DAT0 на вход
}
 void  delay_CLK(void){
	   _T1IF = 0x0;
	   TMR1 = 0;
	   PR1 =0x1;//0x8 задержка в 1
	   T1CON = 0x8000;           
       while (_T1IF == 0);
	   T1CON = 0x0;

}
  void delay_Start(void){
	   _T1IF = 0x0;
	   TMR1 = 0;
	   PR1 = 0xffff;
	   T1CON = 0x8020;           
       while (_T1IF == 0);
	   T1CON = 0x0;  
	  //использовалась только чтобы задать задержку после старта контроллера
}