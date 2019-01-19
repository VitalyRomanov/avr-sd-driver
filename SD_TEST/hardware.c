#include <p24FJ128GA010.h>
#include "defines.h"


#define CLK_TRIS    	TRISDbits.TRISD2		
#define CLK_OUT  	LATDbits.LATD2
#define CMD_TRIS    	TRISDbits.TRISD1	
#define CMD_OUT     	LATDbits.LATD1
#define DAT0_TRIS    	TRISDbits.TRISD0
#define DAT0_OUT     	LATDbits.LATD0

//#define CI_state()		PINC&_BV(CI) //��������� ����� card insert

   void bus_init(void){
		LATD=0x0;	//����� �������� ������������ �����/������ ��� ���� �����
		CLK_TRIS=0 ;	//CLK �� �����
		CLK_low();	//��������� ������� - ������
		
}
   void CLK_low(void){
		CLK_OUT=0;// �������� CLK
		delay_CLK();//�������� ����� ������� �������� �������  
		
}

   void CLK_high(void){
		CLK_OUT=1;// ��������� CLK
		delay_CLK();//�������� ����� ������� �������� �������  
		
}
   void CMD_to_output(void) {
		CMD_TRIS=0; // ����������� ����� CMD �� �����
	    CMD_OUT=1;  //������ �������, �������, ��� ��� �� �������� ���������
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
		CMD_TRIS=1; //����������� CMD �� ����
}

   void DAT0_to_output(void) {
	    DAT0_TRIS=0;	 //������������ ����� DAT0 �� �����
        DAT0_OUT=1; 	//��������� ��������� �������
}

  void tick(void) {
	   	 CLK_OUT=1; //�������� ������ �� �������� �����
		 delay_CLK();
                 delay_CLK();//��������
                 CLK_OUT=0 ; //�������� �������
	   	 delay_CLK(); //�������
                 delay_CLK();
}

   void DAT0_to_input(void) { 
	    DAT0_TRIS=0; //������������ ����� DAT0 �� ����
}
 void  delay_CLK(void){
		 _T1IF = 0x0;
	   TMR1 = 0;
	   PR1 = 0x8;//0x8 �������� � 1
	   T1CON = 0x8000;           
       while (_T1IF == 0);
	   T1CON = 0x0;

}
  void delay_Start(void){
	   _T1IF = 0x0;
	   TMR1 = 0;
	   PR1 = 0xfff;
	   T1CON = 0x8020;           
       while (_T1IF == 0);
	   T1CON = 0x0;  
	  //�������������� ������ ����� ������ �������� ����� ������ �����������
}