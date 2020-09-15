#include "scheduler.h"
#include "app.h"
#include <stdlib.h>

#define NUM_BUCKETS 128 // must be a power of 2
#define NUM_INSERTS (NUM_BUCKETS / 4) // shoot for 25% occupancy
#define NUM_LOOKUPS NUM_INSERTS
#define MAX_RELOCATIONS 8
#define BUFFER_SIZE 32

typedef uint16_t value_t;
typedef uint16_t hash_t;
typedef uint16_t fingerprint_t;
typedef uint16_t index_t; // bucket index

typedef struct _insert_count {
    unsigned insert_count;
    unsigned inserted_count;
} insert_count_t;

typedef struct _lookup_count {
    unsigned lookup_count;
    unsigned member_count;
} lookup_count_t;

// Tasks.
TEB(init);
TEB(generate_key);
TEB(calc_indexes);
TEB(calc_indexes_index_1);
TEB(calc_indexes_index_2);
TEB(insert);
TEB(add);
TEB(relocate);
TEB(insert_done);
TEB(lookup);
TEB(lookup_search);
TEB(lookup_done);
TEB(print_stats);
// NOT USED.
//TEB(task_init_array);

// Task-shared protected variables.
__shared(
#if !LAYOUT
 index_t multiple(_v_index);
 uint64_t multiple(_v_key);
 uint8_t flag1;
 void* _v_next_task;
 fingerprint_t multiple(_v_fingerprint);
 //uint8_t flag4;
 value_t multiple(_v_index1);
  //uint8_t flag7;
 value_t multiple(_v_index2);
 value_t multiple(_v_relocation_count);
  uint8_t flag8;
 value_t multiple(_v_insert_count);
  uint8_t flag2;
 value_t multiple(_v_inserted_count);
 uint8_t flag3;
 value_t multiple(_v_lookup_count);
 uint8_t flag5;
 value_t multiple(_v_member_count);
 uint8_t flag6;
 uint64_t multiple(_v_success);
 uint8_t multiple(_v_member);
 fingerprint_t multarray(_v_filter,NUM_BUCKETS);
 uint8_t flag9;
#else
 value_t multiple(_v_key);
  uint8_t flag1;
 value_t multiple(_v_insert_count);
 uint8_t flag2;
  value_t multiple(_v_inserted_count);
	uint8_t flag3;
	fingerprint_t multiple(_v_fingerprint);
	//uint8_t flag4;
 value_t multiple(_v_lookup_count);
 uint8_t flag5;
  value_t multiple(_v_member_count);
	 uint8_t flag6;
	 value_t multiple(_v_index1);
	 //uint8_t flag7;
	 	value_t multiple(_v_relocation_count);
   uint8_t flag8;
	 	fingerprint_t multarray(_v_filter,NUM_BUCKETS);
  uint8_t flag9;
 index_t multiple(_v_index);
 value_t multiple(_v_index2);
   void* _v_next_task;
 uint64_t multiple(_v_success);
 uint8_t multiple(_v_member);

#endif
 )


static value_t init_key = 0x0001; // seeds the pseudo-random sequence of keys

static hash_t djb_hash(uint8_t* data, unsigned len)
{
    uint16_t hash = 5381;
    unsigned int i;

    for (i = 0; i < len; data++, i++)
        hash = ((hash << 5) + hash) + (*data);

    return hash & 0xFFFF;
}

static index_t hash_to_index(fingerprint_t fp)
{
    hash_t hash = djb_hash((uint8_t *) &fp, sizeof(fingerprint_t));
    return hash & (NUM_BUCKETS - 1); // NUM_BUCKETS must be power of 2
}

static fingerprint_t hash_to_fingerprint(value_t key)
{
    return djb_hash((uint8_t *) &key, sizeof(value_t));
}


TEB(init)//0
{

    unsigned i;
    for (i = 0; i < NUM_BUCKETS ; ++i) {
        __SET(_v_filter[i]) = 0;
    }

    __SET(_v_insert_count) = 0;
    __SET(_v_lookup_count) = 0;
    __SET(_v_inserted_count) = 0;
    __SET(_v_member_count) = 0;
    __SET(_v_key) = init_key;
    __SET(_v_next_task) = insert;

    uint16_t __cry;
    __cry = (__GET(_v_key) + 1) * 17;
    void* next_task = __GET(_v_next_task);
    if (next_task == insert) {
			    __SET(_v_key) = __cry;
					NEXT(6);
    } else if (next_task == lookup) {
			    __SET(_v_key) = __cry;
					NEXT(10);
    } else {
        while(1); // Debugging pu__GETose
    }


}


TEB(init_array)//1
{

    unsigned i;
    //__SET_FLAG(DMAflag2);
    for (i = 0; i < BUFFER_SIZE - 1; ++i) {
        __SET(_v_filter[i + __GET(_v_index)*(BUFFER_SIZE-1)]) = 0;
    }

    __SET(_v_index)++;
    if (__GET(_v_index) == NUM_BUCKETS / (BUFFER_SIZE - 1)) {
        NEXT(2);
    }
    else {
        NEXT(1);
    }

}


TEB(generate_key)//2
{
		uint16_t __cry;
    __cry = (__GET(_v_key) + 1) * 17;
    void* next_task = __GET(_v_next_task);
					
    if (next_task == insert) {
			__SET_FLAG(flag1);
			    __SET(_v_key) = __cry;
        NEXT(6);
			  
    } else if (next_task == lookup) {
			__SET_FLAG(flag1);
			    __SET(_v_key) = __cry;
       NEXT(10);
    }
		 else if (next_task == lookup_search) {
        NEXT(11);//task_lookup_search
    }
		else if (next_task == insert) {
        NEXT(7);//task_lookup_search
		}		
		else {
        while(1); // Debugging pu__GETose
    }

}


TEB(calc_indexes)//3
{
    uint16_t __cry;
    __cry = hash_to_fingerprint(__GET(_v_key));
    __SET(_v_fingerprint) = __cry;
	  //NEXT(4);
    __cry = hash_to_index(__GET(_v_key));
    __SET(_v_index1) = __cry;
	    index_t fp_hash = hash_to_index(__GET(_v_fingerprint));
    __cry = __GET(_v_index1) ^ fp_hash;
    __SET(_v_index2) = __cry;
    void* next_task = __GET(_v_next_task);
    if (next_task == add) {
        NEXT(7);//task_add
    } else if (next_task == lookup_search) {
        NEXT(11);//task_lookup_search
    } else if (next_task == lookup) {
        NEXT(10);//task_lookup_search
		}
		else if (next_task == insert) {
        NEXT(7);//task_lookup_search
		}
		else {
        while(1); // Debugging pu__GETose
    }

}


TEB(calc_indexes_index_1)//4
{
    uint16_t __cry;
    __cry = hash_to_index(__GET(_v_key));
    __SET(_v_index1) = __cry;
    //NEXT(5);
	    index_t fp_hash = hash_to_index(__GET(_v_fingerprint));
    //uint16_t __cry;
    __cry = __GET(_v_index1) ^ fp_hash;
    __SET(_v_index2) = __cry;

    void* next_task = __GET(_v_next_task);

    if (next_task == add) {
        NEXT(7);//task_add
    } else if (next_task == lookup_search) {
        NEXT(11);//task_lookup_search
    } else {
        while(1); // Debugging pu__GETose
    }

}


TEB(calc_indexes_index_2)//5
{

    index_t fp_hash = hash_to_index(__GET(_v_fingerprint));
    uint16_t __cry;
    __cry = __GET(_v_index1) ^ fp_hash;
    __SET(_v_index2) = __cry;

    void* next_task = __GET(_v_next_task);

    if (next_task == add) {
        NEXT(7);//task_add
    } else if (next_task == lookup_search) {
        NEXT(11);//task_lookup_search
    } else {
        while(1); // Debugging pu__GETose
    }

}


// This task is redundant.
// Alpaca never needs this but since Chain code had it, leaving it for fair comparison.
TEB(insert)//6
{
    __SET(_v_next_task) = add;
   NEXT(3);//task_calc_indexes
	  uint16_t __cry;
    __cry = hash_to_fingerprint(__GET(_v_key));
    __SET(_v_fingerprint) = __cry;
		NEXT(4);
	   
}


TEB(add)//7
{

    uint16_t __cry;
    uint16_t __cry_idx = __GET(_v_index1);
    uint16_t __cry_idx2 = __GET(_v_index2);

    if (!__GET(_v_filter[__cry_idx])) {
        __SET(_v_success) = 1;
        __cry = __GET(_v_fingerprint);
			    __SET_FLAG(flag9);
        __SET(_v_filter[__cry_idx]) = __cry;
        NEXT(9);//task_insert_done
    } else {
        if (!__GET(_v_filter[__cry_idx2])) {
            __SET(_v_success) = 1;
            __cry = __GET(_v_fingerprint);
					    __SET_FLAG(flag9);
            __SET(_v_filter[__cry_idx2])  = __cry;
            NEXT(9);//task_insert_done
        } else { // evict one of the two entries
            fingerprint_t fp_victim;
            index_t index_victim;

            if (rand() % 2) {
                index_victim = __cry_idx;
                fp_victim = __GET(_v_filter[__cry_idx]);
            } else {
                index_victim = __cry_idx2;
                fp_victim = __GET(_v_filter[__cry_idx2]);
            }

            // Evict the victim
            __cry = __GET(_v_fingerprint);
						__SET_FLAG(flag9);
            __SET(_v_filter[index_victim])  = __cry;
            __SET(_v_index1) = index_victim;
            __SET(_v_fingerprint) = fp_victim;
            __SET(_v_relocation_count) = 0;

            NEXT(8);//relocate dummy node
        }
    }

}


TEB(relocate)//8
{

    uint16_t __cry;
    fingerprint_t fp_victim = __GET(_v_fingerprint);
    index_t fp_hash_victim = hash_to_index(fp_victim);
    index_t index2_victim = __GET(_v_index1) ^ fp_hash_victim;
    if (!__GET(_v_filter[index2_victim])) { // slot was free
        __SET(_v_success) = 1;
			    __SET_FLAG(flag9);
        __SET(_v_filter[index2_victim]) = fp_victim;
        NEXT(9);//task_insert_done
    } else { // slot was occupied, rellocate the next victim
        if (__GET(_v_relocation_count) >= MAX_RELOCATIONS) { // insert failed
            __SET(_v_success) = 0;
            NEXT(9);//task_insert_done
            //return;
        }
				 __SET_FLAG(flag8);
        __SET(_v_relocation_count)++;
        __SET(_v_index1) = index2_victim;
        __cry = __GET(_v_filter[index2_victim]);
        __SET(_v_fingerprint) = __cry;
				__SET_FLAG(flag9);
        __SET(_v_filter[index2_victim]) = fp_victim;
        NEXT(8);//task_relocate
    }

}


TEB(insert_done)//9
{

    uint16_t __cry;
		uint16_t temp;
		uint16_t key_temp;
		uint16_t inserted_temp;
    temp = __GET(_v_insert_count);
		temp++;
    __cry = __GET(_v_inserted_count);
    __cry+= __GET(_v_success);
			inserted_temp	= __cry;
    if (temp < NUM_INSERTS) {
        __SET(_v_next_task) = insert;
        //NEXT(2);//task_generate_key
    } else {
        __SET(_v_next_task) = lookup;
        __SET(_v_key) = init_key;
        //task_generate_key
    }
		__SET_FLAG(flag2);
		__SET(_v_insert_count) = temp;
		__SET_FLAG(flag3);
		__SET(_v_inserted_count) = __cry;
		NEXT(2);


}


TEB(lookup)//10
{

    __SET(_v_next_task) = lookup_search;
    NEXT(3);//task_calc_indexes

}


TEB(lookup_search)//11
{

    if (__GET(_v_filter[__GET(_v_index1)]) == __GET(_v_fingerprint)) {
        __SET(_v_member) = 1;
    } else {
        if (__GET(_v_filter[__GET(_v_index2)]) == __GET(_v_fingerprint)) {
            __SET(_v_member) = 1;
        } else {
            __SET(_v_member) = 0;
        }
    }
		 uint16_t __cry;
    __SET_FLAG(flag5);
    __SET(_v_lookup_count)++;
		__SET_FLAG(flag6);
    __cry = __GET(_v_member_count) ;
    __cry+= __GET(_v_member);
    __SET(_v_member_count)  = __cry;

    if (__GET(_v_lookup_count) < NUM_LOOKUPS) {
        __SET(_v_next_task) = lookup;
        NEXT(2);
    } else {
        NEXT(13);
    }


}


TEB(lookup_done)
{


    uint16_t __cry;
    __SET_FLAG(flag1);
    __SET(_v_lookup_count)++;
    __cry = __GET(_v_member_count) ;
    __cry+= __GET(_v_member);
    __SET(_v_member_count)  = __cry;

    if (__GET(_v_lookup_count) < NUM_LOOKUPS) {
        __SET(_v_next_task) = lookup;
        NEXT(2);
    } else {
        NEXT(13);
    }

}


TEB(print_stats)//13
{
    // Print output here
     unsigned i;
		uint32_t temp;
     for (i = 0; i < NUM_BUCKETS; ++i) {
         //printf("%04x ", __GET(_v_filter[i]));
					temp=	__GET(_v_filter[i]);
         if (i > 0 && (i + 1) % 8 == 0){
             //printf("\n");
						temp = 0;
         }
     }

    NEXT(0);
}





void cuckoo_init(void)
{
    TASK(1,init);

    TEB_INIT(init, 0);
    TEB_INIT(init_array, 1);//not used
    TEB_INIT(generate_key, 2);
    TEB_INIT(calc_indexes, 3);
    TEB_INIT(calc_indexes_index_1,4);
    TEB_INIT(calc_indexes_index_2, 5);
    TEB_INIT(insert, 6);
    TEB_INIT(add, 7);
    TEB_INIT(relocate, 8);
    TEB_INIT(insert_done, 9);
    TEB_INIT(lookup, 10);
    TEB_INIT(lookup_search, 11);
    TEB_INIT(lookup_done, 12);
    TEB_INIT(print_stats, 13);
	
	FLAG_DECLEAR(0,__GET_OFFSET(flag1));
		FLAG_DECLEAR(1,__GET_OFFSET(flag2));
		FLAG_DECLEAR(2,__GET_OFFSET(flag3));
			FLAG_DECLEAR(3,__GET_OFFSET(flag5));
		FLAG_DECLEAR(4,__GET_OFFSET(flag6));
		FLAG_DECLEAR(5,__GET_OFFSET(flag8));
		FLAG_DECLEAR(6,__GET_OFFSET(flag9));

}