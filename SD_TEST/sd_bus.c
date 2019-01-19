#include "defines.h"
#include <p24FJ128GA010.h>

#define TRIS_LED  TRISEbits.TRISE4
#define LED  LATEbits.LATE4


#define CMD_state()		PORTDbits.RD1 //считывает состояние линии в текущий момент
#define DAT0_state()	PORTDbits.RD0 //считывает линию DAT0


char            ocr[4],
		csr[4], 
		rca[4], 
		cid[15], 
		csd[15], 
		crc_st[1]; 

void load_to_bus(char send,char bus_type) { //передаются байт, который нужно загрузить и на какую линию
        char k;
        if(bus_type==CMD_BUS)
				CMD_low(); 
			else
				DAT0_low();
		for(k=0;k<BYTE_WIDTH;k++)	{ //поочередно передаем каждый бит
			
			if ((send&0x80)!=0x00){ //если старший бит не 0
				if(bus_type==CMD_BUS)
					CMD_high(); //тогда выставляем на линии 1
				else
					DAT0_high();
			}
			send = send<<1; //сдвигаем передаваемый байт на 1
			tick(); //даем передний и задний фронт
		}
}

static void reg_fill(char *reg_name, //название массива в который будем записывать
					char *reg_length, //длина этого массива
					char *responce){ //массив, в котором хранятся полученные данные
	    short i;
		for( i=0;i<reg_length;i++)
			reg_name[i]=responce[i+1];
}

char  receive(	char *responce, //массив, в который заносится ответ
				unsigned short responce_length, //длина ответа, для данных 514 байт!!!!!!!!!!!!!!!!!!!
				char responce_type, //вид ответа
				char bus_type) { //линия, по которой принимается ответ
        int i,j;
		char start_bit=0; //флаг приема стартового бита
		char buffer=0; //буфферный регистр
		for(i=0;i<511;i++){ //511 тактов ждем приема стартового бита
			CLK_high(); //даем передний фронт
			if (((!(CMD_state()))&&(bus_type==CMD_BUS))||		//если линия CMD просела 
				((!(DAT0_state()))&&(bus_type==DAT_BUS))) {		//или линия DAT0 просела
				CLK_low(); //даем задний фронт
				start_bit=1; //устанавливаем флаг приема стартового бита
				break; //выходим из цикла
			}
			CLK_low(); //даем задний фронт, после проверки состояния линии
		}
		if(start_bit){ //если стартоый бит был принят
			for( i=0;i<responce_length;i++)	{ //принимаем ответ из *** байт
				//buffer=0;
				for( j=0;j<BYTE_WIDTH;j++)	{ //каждый байт состоит из *** бит
					if(bus_type==CMD_BUS) if((i==0)&&(j==0)) continue;	//если это линия CMD то пропускаем первый бит, так как он стартовый. Нужно для удобства обработки
					buffer=buffer<<1; //перед изменнением данных сдвигаем на 1 бит, чтобы сохранить то, что приняли на предыдущей итерации
					CLK_high(); //передний фронт
					if(bus_type==CMD_BUS){ //если принимаем по линии CMD 
						if (CMD_state()) buffer|=1; // и состояние линии в данный момент 1 - то заносим 1 в буффер
					}
					else{ //если принимаем по линии DAT0
						if (DAT0_state()) buffer|=1; // и состояние DAT0 1 - заносим в буффер 1.
					}
					CLK_low();
				}
			
				responce[i]=buffer; //заносим принятый байт в массив
			}
			for ( i=0;i<10;i++) //после приема данных согласно документации
							tick(); //даем 8+ тактов
			if(bus_type==CMD_BUS){ //если линия приема - CMD 
				char crc_r=0,crc_i=0; //контрольная сумма полученная, контрольная сумма вычисленная
				crc_r=responce[responce_length-1]; //заносим полученную котрольную сумму в переменную
				
				if(responce[0]==0x3F) crc_i=0xFF; //ИСКЛЮЧЕНИЕ,то если принята команда с кодом 0x3F, её CRC считаем должен быть равен 0xFF
				else crc_i=CRC7(responce,responce_length); //в любом другом случае, считаем контрольную сумму
				
				if(crc_r==crc_i){ //сравниваем полученную контрольную сумму и вычисленную
					// если они совпадают
					char *reg_addr,reg_len=0;
					
					// в зависимости от типа ответа выбираем в какой массив записать ответ и длину записываемых данных
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
					
		
					reg_fill(reg_addr,reg_len,responce); //осуществляем запись в массив
					
					return STATUS_OK; //усепшно завешаем функцию
				}
				return CRC_RECEIVE_ERR; //ошибка проверки CRC при приеме по линии CMD, если принятое и вычисленное значение CRC  не совпадают
			}
			else{  //тут должна быть проверка CRC для приема по линии DAT0
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
			return STATUS_OK; //пока что, если приняли по линии данных просто возвращаем успешный код
			
//			return CRC_RECEIVE_ERR;
		}
		return START_BIT_ERR;	//если не получен стартовый бит
}

char send_command(	char code, //код команды
					char arg3, //аргументы
					char arg2,
					char arg1,
					char arg0,
					char csum){ //тут будет контрольная сумма
            int i;
			char command[BYTE6];//заносим все эти данные в массив, для более простой обработки
			command[0]=code;
			command[1]=arg3;
			command[2]=arg2;
			command[3]=arg1;
			command[4]=arg0;
			command[5]=0; //обнуляем чтобы вычислить CRC 
			csum=CRC7(command,BYTE6); 
			command[5]=csum; //заносим CRC в последний байт
			CMD_to_output(); //переключаем линию CMD на выход
			for(i=0;i<BYTE6;i++){ //передаем 6 байт команды
				load_to_bus(command[i],CMD_BUS);
			}
			tick(); //пусктой такт
			CMD_to_input(); //переключаем линию на вход
			char responce_length=BYTE6; //по умолчанию длина ответа
			char receive_status=0; //здесь будет храниться код ошибки
			if((code==CMD2)||(code==CMD9)||(code==CMD10)) // если эти команды
				responce_length=BYTE17; //то длина ответ 17 байт
			char responce[responce_length]; //объявляем цикл, в который поступят принятые данные
			char responce_type=R1; // по-умолчанию тип ответа R1
			if(code==CMD41) responce_type=R3; //возможны и другие типы ответа
			if(code==CMD3) responce_type=R6;
			if((code==CMD2)||(code==CMD10)) responce_type=R2_CID;
			if(code==CMD9) responce_type=R2_CSD;
			for ( i=0;i<responce_length;i++) //обнуляем массив, куда поступит ответ
				responce[i]=0;
			receive_status=receive(responce,responce_length,responce_type,CMD_BUS); //принимаем даные
			

			return receive_status; //возвращаем код ошибки

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





void load_to_bus1(char send,char bus_type) { //передаются байт, который нужно загрузить и на какую линию
        char k;

       
        
		for(k=0;k<BYTE_WIDTH;k++)	{ //поочередно передаем каждый бит
                    if(bus_type==CMD_BUS)
				CMD_low();
			else
				DAT0_low();
                        delay_CLK();
			if ((send&0x80)!=0x00){ //если старший бит не 0
				if(bus_type==CMD_BUS)
					CMD_high(); //тогда выставляем на линии 1
				else
					DAT0_high();
			}
			send = send<<1; //сдвигаем передаваемый байт на 1
			tick(); //даем передний и задний фронт
		}
        }
