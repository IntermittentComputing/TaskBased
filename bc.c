#include "app.h"
#include "scheduler.h"
#define SEED 4L
#define ITER 900
#define CHAR_BIT 8
#define LENGTH 256
static char bits[LENGTH] =
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,  /* 0   - 15  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 16  - 31  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 32  - 47  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 48  - 63  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 64  - 79  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 80  - 95  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 96  - 111 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 112 - 127 */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 128 - 143 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 144 - 159 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 160 - 175 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 176 - 191 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 192 - 207 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 208 - 223 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 224 - 239 */
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8   /* 240 - 255 */
};

// Tasks.
TEB(init);
TEB(select_func);
TEB(bit_count);
TEB(bitcount);
TEB(ntbl_bitcnt);
TEB(ntbl_bitcount);
TEB(BW_btbl_bitcount);
TEB(AR_btbl_bitcount);
TEB(bit_shifter);
TEB(end);
TEB(func_incre);

// Task-shared protected variables.
__shared(
#if !LAYOUT

 unsigned multiple(_v_n_0);
 uint8_t flag2;
 unsigned multiple(_v_n_1);
 uint8_t flag3;
 unsigned multiple(_v_n_2);
 uint8_t flag4;
 unsigned multiple(_v_n_3);
 uint8_t flag5;
 unsigned multiple(_v_n_4);
 uint8_t flag6;
 unsigned multiple(_v_n_5);
 uint8_t flag7;
 uint32_t multiple(_v_seed);
 uint8_t flag1;
char multarray(bit_array,LENGTH);
 unsigned multiple(_v_iter);
 uint8_t flag10;
 unsigned multiple(_v_func); //16
 uint8_t flag9;
  unsigned multiple(_v_n_6);
 uint8_t flag8;
 #else
 unsigned multiple(_v_iter);
 uint8_t flag10;
 unsigned multiple(_v_n_0);
 uint8_t flag2;
 unsigned multiple(_v_n_1);
 uint8_t flag3;
 unsigned multiple(_v_n_2);
 uint8_t flag4;
 unsigned multiple(_v_n_3);
 uint8_t flag5;
 unsigned multiple(_v_n_4);
 uint8_t flag6;
 unsigned multiple(_v_n_5);
 uint8_t flag7;
 unsigned multiple(_v_n_6);
 uint8_t flag8;
 uint32_t multiple(_v_seed);
 uint8_t flag1;
 unsigned multiple(_v_func); //16
 uint8_t flag9;
char multarray(bit_array,LENGTH);

 #endif


 )


TEB(init)//0
{
#if TSK_SIZ || EXECUTION_TIME
    cp_reset();
#endif

#if ENABLE_PRF
    full_run_started = 1;
#endif
    //__SET_FLAG(DMAflag);
    __SET(_v_func) = 0;
    __SET(_v_n_0) = 0;
    __SET(_v_n_1) = 0;
    __SET(_v_n_2) = 0;
    __SET(_v_n_3) = 0;
    __SET(_v_n_4) = 0;
    __SET(_v_n_5) = 0;
    __SET(_v_n_6) = 0;
	  __SET(_v_iter) = 0;
	for(int i = 0; i < LENGTH; i++)
	{
		__SET(bit_array[i]) = bits[i];
	}
	
	__SET(_v_seed) = (uint32_t) SEED; 
    NEXT(1);

#if TSK_SIZ
    cp_sendRes("task_init \0");
#endif
}


TEB(select_func)//1
{
#if TSK_SIZ
    cp_reset();
#endif
    //__SET_FLAG(DMAflag1);
    // for testing, seed is always the same

    if (__GET(_v_func) == 0) {
					//__SET_FLAG(flag1);
        //NEXT(2);
				uint32_t tmp_seed = __GET(_v_seed);
				unsigned temp_itr = 0;
				unsigned temp = 0;
				unsigned n_temp = __GET(_v_n_0);
				while( temp_itr < ITER/6){			
				if (tmp_seed) do
						temp++;
				while (0 != (tmp_seed = tmp_seed & (tmp_seed - 1)));			
					n_temp+=temp;
				//__SET_FLAG(DMAflag1);
				temp_itr++;
				}
				__SET_FLAG(flag2);
				__SET(_v_n_0) = n_temp;
				__SET_FLAG(flag1);
				__SET(_v_seed) = tmp_seed + 13;
				__SET_FLAG(flag10);
				__SET(_v_iter) += temp_itr;
				if(__GET(_v_iter)>= ITER)
				{
					NEXT(2);
				}
			 NEXT(1);
    }
    else if (__GET(_v_func) == 1) {
					//__SET_FLAG(flag1);
        //__SET(_v_func)++;
			    //__SET(_v_iter) = 0;
        //NEXT(3);
				unsigned temp_itr = 0;
				uint32_t tmp_seed = __GET(_v_seed);
				unsigned n_temp = __GET(_v_n_1);
				while(temp_itr < ITER/9){

						//__SET_FLAG(DMAflag1);	
				 // __SET(_v_seed) = tmp_seed + 13;

					tmp_seed = ((tmp_seed & 0xAAAAAAAAL) >>  1) + (tmp_seed & 0x55555555L);
					tmp_seed = ((tmp_seed & 0xCCCCCCCCL) >>  2) + (tmp_seed & 0x33333333L);
					tmp_seed = ((tmp_seed & 0xF0F0F0F0L) >>  4) + (tmp_seed & 0x0F0F0F0FL);
					tmp_seed = ((tmp_seed & 0xFF00FF00L) >>  8) + (tmp_seed & 0x00FF00FFL);
					tmp_seed = ((tmp_seed & 0xFFFF0000L) >> 16) + (tmp_seed & 0x0000FFFFL);
						
					
						n_temp += (int)tmp_seed;
					temp_itr++;
					}
				__SET_FLAG(flag3);
				__SET(_v_n_1) += n_temp;
					__SET_FLAG(flag1);
				__SET(_v_seed) = tmp_seed + 13;
				__SET(_v_iter) += temp_itr;
				if(__GET(_v_iter)>= ITER)
				{
					NEXT(2);
				}
							NEXT(1);
			
    }
    else if (__GET(_v_func) == 2) {
        //__SET(_v_func)++;
       // NEXT(4);    // TRICK ALERT!
			uint32_t tmp_seed = __GET(_v_seed);
					unsigned temp_itr = 0;
				unsigned n_temp = __GET(_v_n_2);
				while(temp_itr < ITER/20){
					//uint32_t tmp_seed = __GET(_v_seed);
					
					 n_temp+= non_recursive_cnt(tmp_seed);
									
					//__SET(_v_seed) = tmp_seed + 13;

					temp_itr++;
					}
				__SET_FLAG(flag4);
				__SET(_v_n_2) =n_temp;
					//__SET_FLAG(flag1);
					__SET_FLAG(flag1);
					__SET(_v_seed) = tmp_seed + 13;
				__SET(_v_iter) += temp_itr;
				if(__GET(_v_iter)>= ITER)
				{
					NEXT(2);
				}
        NEXT(1);

    }
    else if (__GET(_v_func) == 3) {
        //__SET(_v_func)++;
        //NEXT(5);
				uint32_t tmp_seed = __GET(_v_seed);
					unsigned temp_itr = 0;
			unsigned n_temp = __GET(_v_n_3);
				 while(temp_itr < ITER/15){
					uint16_t __cry = __GET(_v_seed);
					n_temp +=
							__GET(bit_array)[ (int) (__cry & 0x0000000FUL)] +
							__GET(bit_array)[ (int)((__cry & 0x000000F0UL) >> 4) ] +
							__GET(bit_array)[ (int)((__cry & 0x00000F00UL) >> 8) ] +
							__GET(bit_array)[ (int)((__cry & 0x0000F000UL) >> 12)] +
							__GET(bit_array)[ (int)((__cry & 0x000F0000UL) >> 16)] +
							__GET(bit_array)[ (int)((__cry & 0x00F00000UL) >> 20)] +
							__GET(bit_array)[ (int)((__cry & 0x0F000000UL) >> 24)] +
							__GET(bit_array)[ (int)((__cry & 0xF0000000UL) >> 28)];

					// TRICK ALERT!
									//__SET_FLAG(DMAflag1);			
				
					temp_itr++;
					}
				 __SET_FLAG(flag5);
				__SET(_v_n_3) = n_temp;
					__SET_FLAG(flag1);
					__SET(_v_seed) = tmp_seed + 13;
					__SET(_v_iter) += temp_itr;
				if(__GET(_v_iter)>= ITER)
				{
					NEXT(2);
				}
							NEXT(1);			
    }
    else if (__GET(_v_func) == 4) {
        //__SET(_v_func)++;
        //NEXT(6);
				union {
						unsigned char ch[4];
						long y;
				} U;
				uint32_t tmp_seed = __GET(_v_seed);
				unsigned temp_itr = 0;
				unsigned n_temp = __GET(_v_n_4);
				while(temp_itr < ITER/9){
				U.y = __GET(_v_seed);
				n_temp += __GET(bit_array)[ U.ch[0] ] + __GET(bit_array)[ U.ch[1] ] +
						__GET(bit_array)[ U.ch[3] ] + __GET(bit_array)[ U.ch[2] ];

				// TRICK ALERT!
				//uint32_t tmp_seed = __GET(_v_seed);
								//__SET_FLAG(DMAflag1);
				//__SET(_v_seed) = tmp_seed + 13;
				temp_itr++;
				}
				__SET_FLAG(flag6);
				__SET(_v_n_4) = n_temp;
				__SET_FLAG(flag1);
				__SET(_v_seed) = tmp_seed + 13;
				__SET(_v_iter) += temp_itr;
				if(__GET(_v_iter)>= ITER)
				{
					NEXT(2);
				}
							NEXT(1);		

						
			
    }
    else if (__GET(_v_func) == 5) {
        //__SET(_v_func)++;
        //NEXT(7);
				uint32_t tmp_seed = __GET(_v_seed);
				unsigned temp_itr =0;
			unsigned n_temp = __GET(_v_n_5);
			while(temp_itr < ITER/9){
				unsigned char * Ptr = (unsigned char *) &__GET(_v_seed);
				int Accu;

				Accu  = __GET(bit_array)[ *Ptr++ ];
				Accu += __GET(bit_array)[ *Ptr++ ];
				Accu += __GET(bit_array)[ *Ptr++ ];
				Accu += __GET(bit_array)[ *Ptr ];
				n_temp+= Accu;

				// TRICK ALERT!
				//uint32_t tmp_seed = __GET(_v_seed);     _
				temp_itr++;
			}
			__SET_FLAG(flag7);
				__SET(_v_n_5) = n_temp;
				__SET_FLAG(flag1);
				__SET(_v_seed) = tmp_seed + 13;
							__SET(_v_iter) += temp_itr;
				if(__GET(_v_iter)>= ITER)
				{
						NEXT(2);
				}
							NEXT(1);

    }
    else if (__GET(_v_func) == 6) {
        //__SET(_v_func)++;
        //NEXT(8);
					unsigned i, nn;
					uint32_t tmp_seed = __GET(_v_seed);
				unsigned temp_itr = 0;
			unsigned n_temp = __GET(_v_n_6);
				while(temp_itr< ITER/10){
				//uint32_t tmp_seed = __GET(_v_seed);

				for (i = nn = 0; tmp_seed && (i < (sizeof(long) * CHAR_BIT)); ++i, tmp_seed >>= 1)
						nn += (unsigned)(tmp_seed & 1L);
				
				n_temp += nn;
				// TRICK ALERT!
				tmp_seed += 13;
				temp_itr++;
				}
				__SET_FLAG(flag8);
				__SET(_v_n_6) = n_temp;
				__SET_FLAG(flag1);
				__SET(_v_seed) = tmp_seed + 13;
				__SET(_v_iter) += temp_itr;
				if(__GET(_v_iter)>= ITER)
				{
							NEXT(2);
				}
							NEXT(1);
				}
    else {
        NEXT(0);
    }

#if TSK_SIZ
    cp_sendRes("task_select_func \0");
#endif
}
TEB(func_incre)
{
	__SET(_v_seed) = (uint32_t) SEED; 
	__SET_FLAG(flag9);
	__SET(_v_func)++;
	__SET(_v_iter) = 0;
	NEXT(1);
}







int recursive_cnt(uint32_t x)
{
    int cnt = bits[(int)(x & 0x0000000FL)];

    if (0L != (x >>= 4))
        cnt += recursive_cnt(x);

    return cnt;
}

int non_recursive_cnt(uint32_t x)
{
    int cnt = bits[(int)(x & 0x0000000FL)];

    while (0L != (x >>= 4)) {
        cnt += bits[(int)(x & 0x0000000FL)];
    }

    return cnt;
}



void bc_init(void)
{
    TASK(1,init);

    //may_war_set_bc[0][1],teb_breaking_bc[0]);

    TEB_INIT(init, 0);
    TEB_INIT(select_func,1);
    TEB_INIT(func_incre,2);
		FLAG_DECLEAR(0,__GET_OFFSET(flag1));
		FLAG_DECLEAR(1,__GET_OFFSET(flag2));
		FLAG_DECLEAR(2,__GET_OFFSET(flag3));
		FLAG_DECLEAR(3,__GET_OFFSET(flag4));
			FLAG_DECLEAR(4,__GET_OFFSET(flag5));
		FLAG_DECLEAR(5,__GET_OFFSET(flag6));
		FLAG_DECLEAR(6,__GET_OFFSET(flag7));
		FLAG_DECLEAR(7,__GET_OFFSET(flag8));
		FLAG_DECLEAR(8,__GET_OFFSET(flag9));
		FLAG_DECLEAR(9,__GET_OFFSET(flag10));
}