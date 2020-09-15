
#include "app.h"
#include <math.h>
#include "scheduler.h"

#define MSG "hellohello"
#define MSG_LEN 5
#define ROUND   2

char * msgPt = MSG;

/**
 * 1. TASK declaration here.
 */
TEB(initTask);      //0
TEB(ce_1);          //1
TEB(encrypt_init);  //2
TEB(encrypt_inner_loop);//3

TEB(decrypt_print);     //4

/**
 * 2. Shared variable declaration here.
 */
__shared(
#if !LAYOUT
		long int multiple(de_j);  
		uint8_t flag1; 
		long int multiple(de_ct);
		long int multiple(de_cnt);
		uint8_t flag3; 
    //25
		long int multarray(temp,10);
		uint8_t flag4;
		long int multiple(en_cnt);
		uint8_t flag5;
 		long int multiple(k);
		uint8_t flag6; 
		long int multiple(i); 
		uint8_t flag7; 
    long int multarray(d,10);     //10
		long int multiple(j);		//4
			long int multiple(de_pt);
		long int multiple(de_k);
		uint8_t flag2; 
		long int multarray(e,10);	
		long int multiple(en_k);
		long int multiple(en_j);
    long int multiple(en_pt);
    long int multiple(en_ct);
    long int multiple(en_key);    //16
		long int multiple(p);          //1
    long int multiple(q);
		long int multiple(n);
    long int multiple(t);

    long int multiple(de_key);    //22
		long int multiple(flag);
		long int multarray(m,10);
		long int multarray(en,10);    //13

#else
		long int multiple(de_j);  
		uint8_t flag1; 
		long int multiple(de_k);
		uint8_t flag2; 
    //25
		long int multiple(de_cnt);
		uint8_t flag3;
		long int multiple(i); 
		uint8_t flag7; 
		long int multiple(en_cnt);
		uint8_t flag5;
		long int multarray(temp,10);
		uint8_t flag4;	
		long int multiple(k);
		uint8_t flag6;
    long int multarray(d,10);     //10
		long int multiple(j);		//4
		long int multiple(en_k);
		long int multiple(en_j);
    long int multiple(en_pt);
    long int multiple(en_ct);
    long int multiple(en_key);    //16	
		long int multiple(de_pt);
    long int multiple(de_ct);
    long int multiple(de_key);    //22		
		long int multiple(p);          //1
    long int multiple(q);
		long int multiple(n);
    long int multiple(t);
		long int multiple(flag);
		long int multarray(e,10);
		long int multarray(en,10);    //13
		long int multarray(m,10);

     //19
#endif
)

/**
 * 3. TASK definition here.
 */
TEB(initTask){//0   1~3
    int in_p = 29;
    int in_q = 47;
    int ii = 0;

    __SET(p)= in_p;
    __SET(q)= in_q;
    __SET(n)= in_p * in_q;
    __SET(t)= (in_p-1) * (in_q-1);
    __SET(i)=1;
    __SET(k)=0;
    __SET(flag)=0;
    for (ii = 0; ii < MSG_LEN; ii++) {
        __SET(m[ii]) = *(msgPt+ii);
    }
    NEXT(1);
}

TEB(ce_1){//1
    uint8_t tempFlag = 0;
    uint8_t roundCnter = 0;
    long int kk=1, __cry;
    int c;
    while(1){
        roundCnter++;
			__SET_FLAG(flag7);
    while(1){
				
        __SET(i)++; // start with i=2
        if(__GET(i) >= __GET(t)){
            NEXT(2);
        }
        if(__GET(t) % __GET(i)){
            c=sqrt16(__GET(i));
            __SET(j) = c;
            for(c=2; c <= __GET( j) ;c++){
                if( __GET(i) % c==0){
                __SET(flag)=0;
                tempFlag = 1;
                }
            }
            if(tempFlag){
                tempFlag = 0;
                break;
            }
            __SET(flag) = 1;
            long int in_i = __GET(i);
            if( __GET(flag) == 1 && in_i != __GET(p) && in_i != __GET(q) ){
                __SET(e[__GET(k)]) = in_i ;
            } else {
                break;
            }
            while(1) {
                kk=kk +  __GET(t);
                if(kk % __GET( e[__GET(k)] ) ==0){
                    __cry = (kk/ __GET( e[ __GET(k) ]) );
                    __SET(flag) = __cry;
                    break;
                }
            }
            __cry = __GET(flag);
            if(__cry > 0){
                __SET(d[ __GET(k) ]) =__cry;
							__SET_FLAG(flag6);
                __SET(k)++;
            }

            if (__GET(k) >= 9) {
                NEXT(2);
            }
        }
    }
    if(roundCnter>ROUND){ 
        NEXT(1);
    }
    }
}

TEB(encrypt_init){//2 
    long int __cry;
    __cry = __GET(m[ __GET(en_cnt) ]) ;
    __SET(en_pt) = __cry;
    __SET(en_pt) -=96;
    __SET(en_k)  = 1;
    __SET(en_j)  = 0;
    __cry = __GET(e[0]) ;
    __SET(en_key) = __cry;

    while(__GET(en_j) < __GET(en_key)){
        __cry = __GET(en_k) * __GET(en_pt);
        __SET(en_k) = __cry;
        __cry = __GET(en_k) % __GET(n);
        __SET(en_k) = __cry;
        __SET(en_j)++;
    }
    __cry = __GET(en_k);
    __SET(temp[ __GET(en_cnt) ]) = __cry;
    __cry = __GET(en_k) + 96;
    __SET(en_ct) = __cry;
    __cry = __GET(en_ct);
    __SET(en[ __GET(en_cnt) ]) = __cry;

    if (__GET(en_cnt) < MSG_LEN) {
				__SET_FLAG(flag5);
        __SET(en_cnt)++;
        NEXT(2);
    } else {
        __SET(en[ __GET(en_cnt) ]) = -1;
        __SET(de_k)  = 1;
        __SET(de_j)  = 0;
        __cry =__GET(d[0]);
        __SET(de_key) = __cry;
        NEXT(3);
    }
}

TEB(decrypt_inner_loop){//3
    long int __cry;
	  long int temp_cnt = __GET(de_cnt);
    uint8_t tempFlag = 0;
    __cry =  __GET(temp[temp_cnt]);
    __SET(de_ct) = __cry;

    uint8_t tskIdx;
    uint8_t tmpIter = 30;   //TODO define it 30
	
	 long int temp_k= __GET(de_k) ;
		long int temp_j = __GET(de_j);

    tskIdx = 3;
    while(tmpIter){
        tmpIter--;
        if(temp_j >= __GET(de_key)){
            __cry = temp_k + 96;
            __SET(de_pt) = __cry;
            __SET(m[temp_cnt ]) = __cry;
            if (__GET(en[temp_cnt ]) != -1) {
                temp_cnt++;
                tskIdx = 3;
                tempFlag = 1;
            } else {
                tskIdx = 0;
            }
            break;
        }
        __cry = temp_k * __GET(de_ct);
        //__SET(de_k) = __cry;
        __cry = __cry % __GET(n);
        temp_k = __cry;
        temp_j++;
    }

    if(tempFlag){
        tempFlag = 0;
        __cry = __GET(en_k);
			__SET_FLAG(flag4);
        __SET(temp[ __GET(en_cnt) ]) = __cry;
        __cry = __GET(en_k) + 96;
        __SET(en_ct) = __cry;
        __cry = __GET(en_ct);
        __SET(en[ __GET(en_cnt) ]) = __cry;
        __SET(en[ __GET(en_cnt) ]) = -1;
			__SET_FLAG(flag2);
        __SET(de_k)  = 1;
			__SET_FLAG(flag1);
        __SET(de_j)  = 0;
        __cry =__GET(d[0]);
        __SET(de_key) = __cry;
    }
		else{
			__SET_FLAG(flag2);
			__SET(de_k) = temp_k;
			__SET_FLAG(flag1);
			__SET(de_j) = temp_j;
		}
		__SET_FLAG(flag3);
		__SET(de_cnt)=temp_cnt;
    NEXT(tskIdx);
}

TEB(decrypt_print){//5
    NEXT(0);
}

void rsa_mod_init(){
    TASK(1,initTask);

    TEB_INIT(initTask,      0,      __GET_OFFSET(flag1));       //0
    TEB_INIT(ce_1,            1,    __GET_OFFSET(flag1)); //1 
    TEB_INIT( encrypt_init,        2,    __GET_OFFSET(flag2));      //7
    TEB_INIT( decrypt_inner_loop,  3,    __GET_OFFSET(flag3));    //8
    
    TEB_INIT( decrypt_print,       4,    __GET_OFFSET(flag1));     //10.
			FLAG_DECLEAR(0,__GET_OFFSET(flag1));
		FLAG_DECLEAR(1,__GET_OFFSET(flag2));
		FLAG_DECLEAR(2,__GET_OFFSET(flag3));
		FLAG_DECLEAR(3,__GET_OFFSET(flag4));
			FLAG_DECLEAR(4,__GET_OFFSET(flag5));
		FLAG_DECLEAR(5,__GET_OFFSET(flag6));
	FLAG_DECLEAR(6,__GET_OFFSET(flag7));
}



