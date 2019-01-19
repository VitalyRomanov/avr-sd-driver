#include <p24FJ128GA010.h>
#include "defines.h"

//_CONFIG1( FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)

//_CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)


int main( void ) {
delay_Start();
//ANSD=0x0000;
TRISD=0xFFFF;

bus_init();
while(1){
 load_to_bus(0xAA, CMD_BUS);
}
SD_card_init(); //инициализация
receive_cid_number(); //чтение идентификатора, без этого работать не будет
receive_relative_addr(); //получение адреса карты
select_card(); //выбор карты по этому адресу
read_512_bytes(0,0,0,0);

}