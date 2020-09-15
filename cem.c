#include "scheduler.h"
#include "app.h"

#define NIL 0 // like NULL, but for indexes, not real pointers

#define DICT_SIZE         64
#define BLOCK_SIZE         32

#define NUM_LETTERS_IN_SAMPLE        2
#define LETTER_MASK             0x000F
#define LETTER_SIZE_BITS             8
#define NUM_LETTERS (LETTER_MASK + 1)


typedef uint16_t index_t;
typedef uint16_t letter_t;
typedef uint16_t sample_t;

// NOTE: can't use pointers, since need to ChSync, etc
typedef struct _node_t {
    letter_t letter; // 'letter' of the alphabet
    index_t sibling; // this node is a member of the parent's children list
    index_t child;   // link-list of children
} node_t;

// Tasks.
TEB(init);
TEB(init_dict);
TEB(sample);
TEB(measure_temp);
TEB(letterize);
TEB(compress);
TEB(find_sibling);
TEB(add_node);
TEB(add_insert);
TEB(append_compressed);
TEB(print);
TEB(samplecount);


__shared(   //3520
#if !LAYOUT

 letter_t multiple(_v_letter);
  unsigned multiple(_v_letter_idx);
	 uint8_t flag1;
	   node_t multarray(_v_compressed_data,BLOCK_SIZE);
 sample_t multiple(_v_prev_sample);
 uint8_t flag2;
 index_t multiple(_v_out_len);
  uint8_t flag5;

 index_t multiple(_v_node_count);
   uint8_t flag4;
 sample_t multiple(_v_sample);
 index_t multiple(_v_sample_count);
    uint8_t flag6;
 index_t multiple(_v_sibling);
  uint8_t flag3;
 index_t multiple(_v_child);
 index_t multiple(_v_parent);
 index_t multiple(_v_parent_next);
 node_t multiple(_v_parent_node);
 node_t multiple(_v_sibling_node);
 sample_t multiple(_v_symbol);
	 node_t multarray(_v_dict,DICT_SIZE);

 #else
    unsigned multiple(_v_letter_idx);
		uint8_t flag1;
		 	sample_t multiple(_v_prev_sample);
	 uint8_t flag2;
 index_t multiple(_v_sibling);
  uint8_t flag3;
	  index_t multiple(_v_node_count);
		  uint8_t flag4;
 index_t multiple(_v_out_len);
 uint8_t flag5;
  	index_t multiple(_v_sample_count);
 uint8_t flag6;
   node_t multarray(_v_compressed_data,BLOCK_SIZE);
 	  letter_t multiple(_v_letter);

  index_t multiple(_v_child);
	 	node_t multarray(_v_dict,DICT_SIZE);
	  index_t multiple(_v_symbol);
 node_t multiple(_v_parent_node);
 index_t multiple(_v_parent_next);
 index_t multiple(_v_parent);
  sample_t multiple(_v_sample);
 node_t multiple(_v_sibling_node);

 
 #endif
)


static sample_t acquire_sample(letter_t prev_sample)
{
    letter_t sample = (prev_sample + 1) & 0x03;
    return sample;
}


TEB(init)//0
{
    //__SET_FLAG(DMAflag1);
    __SET(_v_parent_next) = 0;
    __SET(_v_out_len) = 0;
    __SET(_v_letter) = 0;
    __SET(_v_prev_sample) = 0;
    __SET(_v_letter_idx) = 0;
    __SET(_v_sample_count) = 1;
		__SET(_v_node_count) = 0;

    //NEXT(1);
			uint16_t i = __GET(_v_letter);
		while(i < NUM_LETTERS){

    //__SET_FLAG(DMAflag2);
    __SET(_v_dict[i].letter) = i ;
    __SET(_v_dict[i].sibling) =  NIL;
    __SET(_v_dict[i].child) = NIL;
			i++;
  			
		}
		//__SET_FLAG(flag2);
		 __SET(_v_letter) = i;
     __SET(_v_node_count) =  NUM_LETTERS;
		 NEXT(2);
 
}


TEB(init_dict)
{
		uint16_t i = __GET(_v_letter);
		while(i < NUM_LETTERS){

    //__SET_FLAG(DMAflag2);
    __SET(_v_dict[i].letter) = i ;
    __SET(_v_dict[i].sibling) =  NIL;
    __SET(_v_dict[i].child) = NIL;
  			
		}
		__SET_FLAG(flag2);
		 __SET(_v_letter) = i;
     __SET(_v_node_count) =  NUM_LETTERS;
		 NEXT(2);


}

TEB(sample)//2
{

    unsigned next_letter_idx = __GET(_v_letter_idx) + 1;
    if (next_letter_idx == NUM_LETTERS_IN_SAMPLE)
        next_letter_idx = 0;
    //__SET_FLAG(DMAflag);

    if (__GET(_v_letter_idx) == 0) {
				__SET_FLAG(flag1);
        __SET(_v_letter_idx) = next_letter_idx;
        NEXT(3);
    } else {
				__SET_FLAG(flag1);
        __SET(_v_letter_idx) = next_letter_idx;
        NEXT(4);
    }

}


TEB(measure_temp)//3
{

    sample_t prev_sample;
    prev_sample = __GET(_v_prev_sample);

    sample_t sample = acquire_sample(prev_sample);
    prev_sample = sample;
    //__SET_FLAG(DMAflag);
		__SET_FLAG(flag2);
    __SET(_v_prev_sample) = prev_sample;
    __SET(_v_sample) = sample;
    NEXT(4);
}


TEB(letterize)//4
{

    unsigned letter_idx = __GET(_v_letter_idx);
    if (letter_idx == 0)
        letter_idx = NUM_LETTERS_IN_SAMPLE;
    else
        letter_idx--;

    unsigned letter_shift = LETTER_SIZE_BITS * letter_idx;
    letter_t letter = (__GET(_v_sample) & (LETTER_MASK << letter_shift)) >> letter_shift;
    //__SET_FLAG(DMAflag);
    __SET(_v_letter) = letter;
    index_t parent = __GET(_v_parent_next);
		NEXT(5);
}


TEB(compress)//5
{

    // pointer into the dictionary tree; starts at a root's child
    index_t parent = __GET(_v_parent_next);

    uint16_t __cry;

    __cry = __GET(_v_dict[parent].child);
    __SET(_v_sibling) = __cry ;
    __cry = __GET(_v_dict[parent].letter);
    __SET(_v_parent_node.letter) =  __cry;
    __cry = __GET(_v_dict[parent].sibling);
    __SET(_v_parent_node.sibling) = __cry;
    __cry = __GET(_v_dict[parent].child);
    __SET(_v_parent_node.child) = __cry;
    __SET(_v_parent) = parent;
    __cry = __GET(_v_dict[parent].child);
    __SET(_v_child) = __cry;
    NEXT(11);


}


TEB(find_sibling)//6
{

    if (__GET(_v_sibling) != NIL) {
        int i = __GET(_v_sibling);
        //__SET_FLAG(DMAflag1);
        uint16_t __cry = __GET(_v_letter);
        if (__GET(_v_dict[i].letter) == __cry ) { // found

            __cry = __GET(_v_sibling);

            __SET(_v_parent_next) = __cry;

            NEXT(4);

        } else { // continue traversing the siblings
            if(__GET(_v_dict[i].sibling) != 0){
                __cry = __GET(_v_dict[i].sibling);
								__SET_FLAG(flag3);
                __SET(_v_sibling) = __cry;
                NEXT(6);

            }
        }

    }
    index_t starting_node_idx = (index_t)__GET(_v_letter);
    __SET(_v_parent_next) = starting_node_idx;

    if (__GET(_v_child) == NIL) {
        NEXT(8);
    }
    else {
        NEXT(7);
    }

}


TEB(add_node)//7
{

    int i = __GET(_v_sibling);

    //__SET_FLAG(DMAflag1);
    if (__GET(_v_dict[i].sibling) != NIL) {
        index_t next_sibling = __GET(_v_dict[i].sibling);
				__SET_FLAG(flag3);
        __SET(_v_sibling) = next_sibling;
        NEXT(7);

    } else { // found last sibling in the list

        uint16_t __cry;

        __cry = __GET(_v_dict[i].letter);
        __SET(_v_sibling_node.letter) = __cry;
        __cry = __GET(_v_dict[i].sibling);
        __SET(_v_sibling_node.sibling) = __cry;
        __cry = __GET(_v_dict[i].child);
        __SET(_v_sibling_node.child) = __cry;

        NEXT(8);
    }

}


TEB(add_insert)//8
{
    if (__GET(_v_node_count) == DICT_SIZE) { // wipe the table if full
        //while (1);
    }

    index_t child = __GET(_v_node_count);
    uint16_t __cry;
    if (__GET(_v_parent_node.child) == NIL) { // the only child

        int i = __GET(_v_parent);

        __cry = __GET(_v_parent_node.letter);
        __SET(_v_dict[i].letter) = __cry;
        __cry  = __GET(_v_parent_node.sibling);
        __SET(_v_dict[i].sibling) = __cry;
        __cry = child;
        __SET(_v_dict[i].child) = __cry;

    } else { // a sibling

        index_t last_sibling = __GET(_v_sibling);

        __cry = __GET(_v_sibling_node.letter);
        __SET(_v_dict[last_sibling].letter) = __cry;
        __cry = child;
        __SET(_v_dict[last_sibling].sibling) = __cry;
        __cry  = __GET(_v_sibling_node.child);
        __SET(_v_dict[last_sibling].child) = __cry;
    }
    __cry = __GET(_v_letter);
    __SET(_v_dict[child].letter) = __cry;
    __SET(_v_dict[child].sibling) = NIL;
    __SET(_v_dict[child].child) = NIL;
    __cry = __GET(_v_parent);
    __SET(_v_symbol) = __cry;
			__SET_FLAG(flag4);
    __SET(_v_node_count)++;
    NEXT(9);

}


TEB(append_compressed)//9
{
    uint16_t __cry;
    int i = __GET(_v_out_len);
    __cry = __GET(_v_symbol);
    //__SET_FLAG(DMAflag);
    __SET(_v_compressed_data[i].letter) = __cry;
	__SET_FLAG(flag5);
    __SET(_v_out_len)++;

    if ( (__GET(_v_out_len)) == BLOCK_SIZE) {
        NEXT(10);
    } else {
        NEXT(2);
    }
}


TEB(print)//10
{
#if TSK_SIZ
    cp_reset();
#endif

    unsigned i;
	//unsigned temp;
    for (i = 0; i < BLOCK_SIZE; ++i) {
      index_t index = __GET(_v_compressed_data[i].letter);
     }
    //printf("\n");
    NEXT(0);

}


TEB(samplecount)
{		
	__SET_FLAG(flag6);
		__SET(_v_sample_count)++;
    NEXT(6);


}
//}



void cem_init(void)
{
    TASK(1,init);

    TEB_INIT(init, 0);
    TEB_INIT(init_dict, 1);
    TEB_INIT(sample, 2);
    TEB_INIT(measure_temp, 3);
    TEB_INIT(letterize, 4);
    TEB_INIT(compress, 5);
    TEB_INIT(find_sibling, 6);
    TEB_INIT(add_node, 7);
    TEB_INIT(add_insert, 8);
    TEB_INIT(append_compressed, 9);
    TEB_INIT(print, 10);
		TEB_INIT(samplecount, 11);
	
				FLAG_DECLEAR(0,__GET_OFFSET(flag1));
		FLAG_DECLEAR(1,__GET_OFFSET(flag2));
		FLAG_DECLEAR(2,__GET_OFFSET(flag3));
		FLAG_DECLEAR(3,__GET_OFFSET(flag4));
			FLAG_DECLEAR(4,__GET_OFFSET(flag5));
		FLAG_DECLEAR(5,__GET_OFFSET(flag6));



	
}