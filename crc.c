#include "scheduler.h"
#include "app.h"

#define RAISE_PIN

#define DATA_LEN 200
const unsigned int CRC_Init = 0xFFFF;
const unsigned int CRC_Input[] = {
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // 10   these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
																	0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,             //20
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           //  30these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
																	0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,            //40
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
																	0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
																	0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691,
                                  0x0fc0, 0x1096, 0x5042, 0x0010,           // 16 random 16-bit numbers
                                  0x7ff7, 0xf86a, 0xb58e, 0x7651,           // these numbers can be
                                  0x8b88, 0x0679, 0x0123, 0x9599,           // modified if desired
                                  0xc58c, 0xd1e2, 0xe144, 0xb691
};

__shared(
 unsigned int multiple(SW_Results);   
uint8_t flag1;// Holds results
 unsigned int multiple(cnt);
 uint8_t flag2;
uint32_t multiple(full_run_started_crc);
uint32_t multiple(step);
//unsigned int multiple(CCITT);
 )

// Debug defines and flags
#define DEBUG_PORT 3
#define DEBUG_PIN  5

//uint8_t full_run_started_crc = 0;


#define TASK_NUM 4

TEB(initTask);
TEB(firstByte);
TEB(task_finish);

unsigned int CCITT_Update(unsigned int init, unsigned int input);



TEB(initTask)
{

    __SET(full_run_started_crc) = 1;
    //__SET_FLAG(DMAflag);
    __SET(cnt) = 0;
    __SET(SW_Results) = CRC_Init;
		__SET(step) = 100;

    NEXT(1);
}

TEB(firstByte)//1
{
    // First input lower byte
		 unsigned int i = 0;
	unsigned int CCITT;
		while( i < DATA_LEN/__GET(step)){
				//__SET_FLAG(flag);
				//__SET(SW_Results) = CCITT_Update(__GET(SW_Results), CRC_Input[__GET(cnt)] & 0xFF);
					(CCITT) = (unsigned char) (__GET(SW_Results) >> 8) | (__GET(SW_Results) << 8);
				(CCITT) ^= CRC_Input[__GET(cnt)] & 0xFF;
				(CCITT) ^= (unsigned char) ((CCITT) & 0xFF) >> 4;
				(CCITT) ^= ((CCITT) << 8) << 4;
				(CCITT) ^= (((CCITT) & 0xFF) << 4) << 1;
				__SET_FLAG(flag1);
				__SET(SW_Results) =  (CCITT);
			
			
				// Then input upper byte
				__SET(SW_Results) = CCITT_Update(__GET(SW_Results), (CRC_Input[__GET(cnt)] >> 8) & 0xFF);
				i++;
		}
		__SET_FLAG(flag2);
    __SET(cnt) = i+ __GET(cnt);

   /* msp_gpio_set(DEBUG_PORT, DEBUG_PIN);
    coala_partial_commit();
    msp_gpio_clear(DEBUG_PORT, DEBUG_PIN);
    msp_gpio_set(DEBUG_PORT, DEBUG_PIN);
    coala_partial_commit();
    msp_gpio_clear(DEBUG_PORT, DEBUG_PIN);*/
    if (__GET(cnt) < DATA_LEN) {
        NEXT(1);
        //return;
    }
    NEXT(2);
}

TEB(task_finish)
{
    //coala_force_commit(); // force a commit after this task

    if (__GET(full_run_started_crc)) {
        // msp_gpio_spike(DEBUG_PORT, DEBUG_PIN);
        __SET(full_run_started_crc) = 0;
    }
    //__SET_FLAG(DMAflagStart);
    NEXT(0);
}

// Software algorithm - CCITT CRC16 code
unsigned int CCITT_Update(unsigned int init, unsigned int input)
{
    unsigned int CCITT = (unsigned char) (init >> 8) | (init << 8);
    CCITT ^= input;
    CCITT ^= (unsigned char) (CCITT & 0xFF) >> 4;
    CCITT ^= (CCITT << 8) << 4;
    CCITT ^= ((CCITT & 0xFF) << 4) << 1;
    return CCITT;
}

void crc_init(void)
{
    TASK(1,initTask);


   TEB_INIT(initTask,0);
   TEB_INIT(firstByte,1);
   TEB_INIT(task_finish,2);
	
			FLAG_DECLEAR(0,__GET_OFFSET(flag1));
		FLAG_DECLEAR(1,__GET_OFFSET(flag2));
}
