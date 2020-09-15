#include "scheduler.h"
#include "sort.h"
#include "app.h"



__shared(
/*#if !LAYOUT
	uint16_t multarray(array,LENGTH);
	uint8_t flag2;
	uint16_t multiple(outer_idx);
	uint8_t flag3;
	uint16_t multiple(inner_idx);
	uint8_t flag1;
#else*/


	uint8_t multiple(flag);
	uint8_t flag2;
		uint16_t multarray(array,LENGTH);
	uint16_t multiple(inner_idx);
	uint16_t multiple(outer_idx);
	uint16_t multiple(max);
	//uint8_t flag3;
	//uint16_t multiple(outer_idx);
//#endif
)






TEB(task_init);
TEB(task_inner_loop);
TEB(task_outer_loop);




/* teb decleration*/


TEB(task_init)//0
{
    __SET(outer_idx) = 0;
    __SET(inner_idx) = 0;

    const uint16_t* array_pt;
	
		//__SET_FLAG(flag);
    //++__SET(iteration);
    
    array_pt = a1;      
    uint16_t idx;
    for (idx = 0; idx < LENGTH; idx++) {
        __SET(array[idx]) = array_pt[idx];
    }

    NEXT(1);
}


TEB(task_inner_loop)//1
{
    uint16_t i, j,  temp,x_i,x_j;
		uint16_t max;
		__SET(inner_idx) = __GET(outer_idx);   
    i = __GET(outer_idx);
		x_i = __GET(array[i]);

		__SET(max) = i;
		for(j = __GET(inner_idx);j<LENGTH;j++)
		{
			if(__GET(array[max]) < __GET(array[j]))
			{
				__SET(max) = j;
			}
		}
		x_j = __GET(array[max]);
		if (x_i < x_j) {
			__SET(flag) = 1;							 
		}
    
    NEXT(2);

}


TEB(task_outer_loop)//2
{
		if(__GET(flag))
		{	
			uint16_t temp;
			temp = __GET(array[__GET(inner_idx)]);
			__GET(array[__GET(inner_idx)]) = __GET(array[__GET(max)]);
			__SET(array[__GET(max)]) = temp;
			__SET_FLAG(flag2);
		}		
		__SET(flag) =0;
    __SET(outer_idx) =  __GET(inner_idx)+1;
    if ( __GET(outer_idx) < LENGTH - 1) {
        NEXT(1);
    } else {
        NEXT(0);
    }
}





void sort_init()
{
    TASK(1,task_init);
    TEB_INIT( task_init,  0, __GET_OFFSET(flag));       //0
    TEB_INIT( task_inner_loop,1,   __GET_OFFSET(flag1)); //1
    TEB_INIT( task_outer_loop, 2,  __GET_OFFSET(flag)); //2
		FLAG_DECLEAR(0,__GET_OFFSET(flag2));
}