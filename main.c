#include <p24FJ256GB206.h>
#include "defines.h"

//_CONFIG1( FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)

//_CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)

//_CONFIG1( FWDTEN_OFF & GWRP_OFF & GCP_OFF & JTAGEN_OFF & ICS_PGx3 )
//_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_ON & POSCMOD_HS & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_NODIV & IESO_OFF & IOL1WAY_OFF );
#define CD()	PORTDbits.RD9
#define TRIS_CD()	TRISDbits.TRISD9

int main( void ) {
delay_Start();
 
 TRIS_CD()=1;
while(!CD());
bus_init(); 
SD_card_init(); //инициализация
receive_cid_number(); //чтение идентификатора, без этого работать не будет
receive_relative_addr(); //получение адреса карты
select_card(); //выбор карты по этому адресу
read_512_bytes(0,0,0,0);

}