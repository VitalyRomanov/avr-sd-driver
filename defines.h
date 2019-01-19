
//дефайны номеров команд

#define CMD0	(0x40+0)
#define CMD1	(0x40+1)
#define CMD2	(0x40+2)
#define CMD3	(0x40+3)
#define CMD7	(0x40+7)
#define CMD8	(0x40+8)
#define CMD9	(0x40+9)
#define CMD10	(0x40+10)
#define CMD13	(0x40+13)
#define CMD17	(0x40+17)
#define CMD24	(0x40+24)
#define	CMD41	(0x40+41)
#define CMD55	(0x40+55)
#define CMD58	(0x40+58)
#define CMD59	(0x40+59)





//дефайны контрольных сумм 

#define CMD0_CRC	0x95
#define CMD8_CRC	0x87
#define CMD55_CRC	0x65
#define DUMMY_CRC	0xFF
#define DUMMY_DATA	0xFF
#define CRC7_POLY	0x89
#define CRC16_POLY	0x1021

//это 2 остались со времен SPI

#define READ_READY			0x00
#define DATA_START_BLOCK	0xFE

//различные значения, которые используются в комманде.
//в основном это длины массивов

#define BYTE_WIDTH	8
#define R1_WIDTH	6
#define BYTE6		6
#define BYTE17		17
#define OCR_LENGTH	4
#define CSR_LENGTH	4
#define CID_LENGTH	15
#define CSD_LENGTH	15
#define RCA_LENGTH	4
#define CRC_ST_LEN	1

//дефайны для различных типов ответа

#define R1		1
#define R2_CID	2
#define R2_CSD	4
#define R3		3
#define R6		6
#define CRC_ST	7

//коды ошибок
//их возвращают функции
//send_command, receive

#define STATUS_OK		0
#define START_BIT_ERR	1
#define CRC_RECEIVE_ERR	2
#define CRC16_CHK_OK	0x50
#define CRC16_CHK_ERR	0xB0
#define CRC16_WRITE_ERR	3
#define WRITE_ERROR		4
#define INIT_SUCCESS	0
#define INIT_FAILED		5

//используется в умной функции получения ответа.

#define CMD_BUS		1
#define DAT_BUS		0


