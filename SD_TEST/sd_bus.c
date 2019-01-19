#include "defines.h"
#include <p24FJ128GA010.h>

#define TRIS_LED  TRISEbits.TRISE4
#define LED  LATEbits.LATE4


#define CMD_state()		PORTDbits.RD1 //��������� ��������� ����� � ������� ������
#define DAT0_state()	PORTDbits.RD0 //��������� ����� DAT0


char            ocr[4],
		csr[4], 
		rca[4], 
		cid[15], 
		csd[15], 
		crc_st[1]; 

void load_to_bus(char send,char bus_type) { //���������� ����, ������� ����� ��������� � �� ����� �����
        char k;
        if(bus_type==CMD_BUS)
				CMD_low(); 
			else
				DAT0_low();
		for(k=0;k<BYTE_WIDTH;k++)	{ //���������� �������� ������ ���
			
			if ((send&0x80)!=0x00){ //���� ������� ��� �� 0
				if(bus_type==CMD_BUS)
					CMD_high(); //����� ���������� �� ����� 1
				else
					DAT0_high();
			}
			send = send<<1; //�������� ������������ ���� �� 1
			tick(); //���� �������� � ������ �����
		}
}

static void reg_fill(char *reg_name, //�������� ������� � ������� ����� ����������
					char *reg_length, //����� ����� �������
					char *responce){ //������, � ������� �������� ���������� ������
	    short i;
		for( i=0;i<reg_length;i++)
			reg_name[i]=responce[i+1];
}

char  receive(	char *responce, //������, � ������� ��������� �����
				unsigned short responce_length, //����� ������, ��� ������ 514 ����!!!!!!!!!!!!!!!!!!!
				char responce_type, //��� ������
				char bus_type) { //�����, �� ������� ����������� �����
        int i,j;
		char start_bit=0; //���� ������ ���������� ����
		char buffer=0; //��������� �������
		for(i=0;i<511;i++){ //511 ������ ���� ������ ���������� ����
			CLK_high(); //���� �������� �����
			if (((!(CMD_state()))&&(bus_type==CMD_BUS))||		//���� ����� CMD ������� 
				((!(DAT0_state()))&&(bus_type==DAT_BUS))) {		//��� ����� DAT0 �������
				CLK_low(); //���� ������ �����
				start_bit=1; //������������� ���� ������ ���������� ����
				break; //������� �� �����
			}
			CLK_low(); //���� ������ �����, ����� �������� ��������� �����
		}
		if(start_bit){ //���� �������� ��� ��� ������
			for( i=0;i<responce_length;i++)	{ //��������� ����� �� *** ����
				//buffer=0;
				for( j=0;j<BYTE_WIDTH;j++)	{ //������ ���� ������� �� *** ���
					if(bus_type==CMD_BUS) if((i==0)&&(j==0)) continue;	//���� ��� ����� CMD �� ���������� ������ ���, ��� ��� �� ���������. ����� ��� �������� ���������
					buffer=buffer<<1; //����� ����������� ������ �������� �� 1 ���, ����� ��������� ��, ��� ������� �� ���������� ��������
					CLK_high(); //�������� �����
					if(bus_type==CMD_BUS){ //���� ��������� �� ����� CMD 
						if (CMD_state()) buffer|=1; // � ��������� ����� � ������ ������ 1 - �� ������� 1 � ������
					}
					else{ //���� ��������� �� ����� DAT0
						if (DAT0_state()) buffer|=1; // � ��������� DAT0 1 - ������� � ������ 1.
					}
					CLK_low();
				}
			
				responce[i]=buffer; //������� �������� ���� � ������
			}
			for ( i=0;i<10;i++) //����� ������ ������ �������� ������������
							tick(); //���� 8+ ������
			if(bus_type==CMD_BUS){ //���� ����� ������ - CMD 
				char crc_r=0,crc_i=0; //����������� ����� ����������, ����������� ����� �����������
				crc_r=responce[responce_length-1]; //������� ���������� ���������� ����� � ����������
				
				if(responce[0]==0x3F) crc_i=0xFF; //����������,�� ���� ������� ������� � ����� 0x3F, � CRC ������� ������ ���� ����� 0xFF
				else crc_i=CRC7(responce,responce_length); //� ����� ������ ������, ������� ����������� �����
				
				if(crc_r==crc_i){ //���������� ���������� ����������� ����� � �����������
					// ���� ��� ���������
					char *reg_addr,reg_len=0;
					
					// � ����������� �� ���� ������ �������� � ����� ������ �������� ����� � ����� ������������ ������
					if(responce_type==R1){ 
						reg_addr=&csr;
						reg_len=CSR_LENGTH;
					}
					if(responce_type==R3){
						reg_addr=&ocr;
						reg_len=OCR_LENGTH;
					}
					if(responce_type==R2_CID){
						reg_addr=&cid;
						reg_len=CID_LENGTH;
					}
					if(responce_type==R2_CSD){
						reg_addr=&csd;
						reg_len=CSD_LENGTH;
					}
					if(responce_type==R6){
						reg_addr=&rca;
						reg_len=RCA_LENGTH;
					}
					
		
					reg_fill(reg_addr,reg_len,responce); //������������ ������ � ������
					
					return STATUS_OK; //������� �������� �������
				}
				return CRC_RECEIVE_ERR; //������ �������� CRC ��� ������ �� ����� CMD, ���� �������� � ����������� �������� CRC  �� ���������
			}
			else{  //��� ������ ���� �������� CRC ��� ������ �� ����� DAT0
			/*
				if(responce_type==CRC_ST){
					crc_st[0]=responce[0];
					
					if (crc_st[0]==CRC16_CHK_OK)
						return STATUS_OK;
					else if(crc_st==CRC16_CHK_ERR)
						return CRC16_WRITE_ERR;
					else
						return WRITE_ERROR;
				}
				*/					
			}
			return STATUS_OK; //���� ���, ���� ������� �� ����� ������ ������ ���������� �������� ���
			
//			return CRC_RECEIVE_ERR;
		}
		return START_BIT_ERR;	//���� �� ������� ��������� ���
}

char send_command(	char code, //��� �������
					char arg3, //���������
					char arg2,
					char arg1,
					char arg0,
					char csum){ //��� ����� ����������� �����
            int i;
			char command[BYTE6];//������� ��� ��� ������ � ������, ��� ����� ������� ���������
			command[0]=code;
			command[1]=arg3;
			command[2]=arg2;
			command[3]=arg1;
			command[4]=arg0;
			command[5]=0; //�������� ����� ��������� CRC 
			csum=CRC7(command,BYTE6); 
			command[5]=csum; //������� CRC � ��������� ����
			CMD_to_output(); //����������� ����� CMD �� �����
			for(i=0;i<BYTE6;i++){ //�������� 6 ���� �������
				load_to_bus(command[i],CMD_BUS);
			}
			tick(); //������� ����
			CMD_to_input(); //����������� ����� �� ����
			char responce_length=BYTE6; //�� ��������� ����� ������
			char receive_status=0; //����� ����� ��������� ��� ������
			if((code==CMD2)||(code==CMD9)||(code==CMD10)) // ���� ��� �������
				responce_length=BYTE17; //�� ����� ����� 17 ����
			char responce[responce_length]; //��������� ����, � ������� �������� �������� ������
			char responce_type=R1; // ��-��������� ��� ������ R1
			if(code==CMD41) responce_type=R3; //�������� � ������ ���� ������
			if(code==CMD3) responce_type=R6;
			if((code==CMD2)||(code==CMD10)) responce_type=R2_CID;
			if(code==CMD9) responce_type=R2_CSD;
			for ( i=0;i<responce_length;i++) //�������� ������, ���� �������� �����
				responce[i]=0;
			receive_status=receive(responce,responce_length,responce_type,CMD_BUS); //��������� �����
			

			return receive_status; //���������� ��� ������

}
char SD_card_init(){

	char card_initiated=0;
	int i;
	for( i=0;i<512;i++) {
		tick();
		}

	send_command(CMD8,0,0,0x01,0xAA,0x87);
	for( i=0;i<10;i++) {
		send_command(CMD55,0,0,0,0,0x65);
		send_command(CMD41,0x40,0x10,0,0,0xCD);
		if(ocr[0]&0x80){LED=1; return INIT_SUCCESS;}
	}
	return INIT_FAILED;
}

char receive_cid_number(){
		return send_command(CMD2,0,0,0,0,0);
}
	
char receive_relative_addr(){
		return send_command(CMD3,0,0,0,0,0);
}

char select_card(){
		return send_command(CMD7,rca[0],rca[1],rca[2],rca[3],0);
}

char read_512_bytes(char addr3,char addr2,char addr1,char addr0){
		char status;
		status=send_command(CMD17,addr3,addr2,addr1,addr0,0);
		if(!status)
			return receive(cid,514,0xFF,DAT_BUS);
		else
			return status;
}





void load_to_bus1(char send,char bus_type) { //���������� ����, ������� ����� ��������� � �� ����� �����
        char k;

       
        
		for(k=0;k<BYTE_WIDTH;k++)	{ //���������� �������� ������ ���
                    if(bus_type==CMD_BUS)
				CMD_low();
			else
				DAT0_low();
                        delay_CLK();
			if ((send&0x80)!=0x00){ //���� ������� ��� �� 0
				if(bus_type==CMD_BUS)
					CMD_high(); //����� ���������� �� ����� 1
				else
					DAT0_high();
			}
			send = send<<1; //�������� ������������ ���� �� 1
			tick(); //���� �������� � ������ �����
		}
        }
