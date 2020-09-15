//#include <task.h>

//#include <apps.h>
//#include <accel.h>
//#include <msp-math.h>
#include "scheduler.h"
#include "app.h"

unsigned _v_seed;

// Number of samples to discard before recording training set
#define NUM_WARMUP_SAMPLES 3
#define ACCEL_WINDOW_SIZE 3
#define MODEL_SIZE 16
#define SAMPLE_NOISE_FLOOR 10 // TODO: made up value

// Number of classifications to complete in one experiment
#define SAMPLES_TO_COLLECT 64

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} threeAxis_t;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t z;
#ifdef __clang__
    uint8_t padding; // clang crashes with type size mismatch assert failure
#endif
} threeAxis_t_8;

typedef threeAxis_t_8 accelReading;
typedef accelReading accelWindow[ACCEL_WINDOW_SIZE];

typedef struct {
    unsigned meanmag;
    unsigned stddevmag;
} features_t;

typedef enum {
    CLASS_STATIONARY,
    CLASS_MOVING,
} class_t;

typedef enum {
    MODE_IDLE = 3,
    MODE_TRAIN_STATIONARY = 2,
    MODE_TRAIN_MOVING = 1,
    MODE_RECOGNIZE = 0, // default
} run_mode_t;


/**
 * 1. TASK declaration here.
 */
TEB(task_init);            //-(0)
TEB(task_selectMode);      //-(1)
TEB(task_sample);          //-(3)
TEB(task_idle);            //-(10)

/**
 * 2. Shared variable declaration here. (164 bytes)
 */
__shared(
#if !LAYOUT


		//unsigned multiple(_v_discardedSamplesCount);          //-[2]:2
		uint16_t multiple(_v_pinState); 
		uint8_t flag7;
		features_t multarray(_v_model_moving,MODEL_SIZE);
		uint8_t flag9;
		features_t multarray(_v_model_stationary,MODEL_SIZE);
		uint8_t flag10;
		unsigned multiple(_v_seed);
		uint8_t flag1;
		unsigned multiple(_v_totalCount);
		uint8_t flag3;
    unsigned multiple(_v_movingCount);
		uint8_t flag4;
		unsigned multiple(_v_stationaryCount);
		uint8_t flag5;
		//accelReading multarray(_v_window,ACCEL_WINDOW_SIZE);
    //accelReading multarray(_v_window,ACCEL_WINDOW_SIZE);
		unsigned multiple(_v_samplesInWindow);
		uint8_t flag2;

		unsigned multiple(_v_count);
		uint8_t flag8;

		unsigned multiple(_v_trainingSetSize);
		 uint8_t flag6;

		class_t multiple(_v_class);
		features_t multiple(_v_features);
		run_mode_t multiple(_v_mode);


#else

unsigned multiple(_v_seed);                           //-[12]:2
uint8_t flag1;
unsigned multiple(_v_samplesInWindow);  
uint8_t flag2;
unsigned multiple(_v_totalCount);                     //-[4]:2
uint8_t flag3;
 unsigned multiple(_v_movingCount);            //-[10]:2
uint8_t flag4;
unsigned multiple(_v_stationaryCount);
uint8_t flag5;
unsigned multiple(_v_trainingSetSize);
uint8_t flag6;
features_t multarray(_v_model_stationary,MODEL_SIZE); //-[14]:64 
uint8_t flag10;
uint16_t multiple(_v_pinState); 
uint8_t flag7;
features_t multarray(_v_model_moving,MODEL_SIZE); 
uint8_t flag9;
unsigned multiple(_v_count); 
uint8_t flag8;
    //-[15]:648?
class_t multiple(_v_class);
//accelReading multarray(_v_window,ACCEL_WINDOW_SIZE);  //-[7]:12
//uint8_t flag2;

features_t multiple(_v_features);
run_mode_t multiple(_v_mode);  //last for sram1
				//SRAM/2

//unsigned multiple(_v_discardedSamplesCount);       
       //sram/1
#endif
)

/**
 * 3. TASK definition here.
 */
TEB(task_init){//-->0, NOTE: R() || W(1,12,13) 8us 
    __SET(_v_pinState) = MODE_IDLE;
    __SET(_v_count) = 0;
    __SET(_v_seed) = 1;
    //__delay_cycles(100);
    NEXT(1);
}

TEB(task_selectMode){//-->1, NOTE: R(1,13) || W(1,2,3,10,11,13)    10us 
	
    unsigned _v_discardedSamplesCount;
    uint16_t pin_state = 1;
		unsigned count_temp = __GET(_v_count);
		++count_temp;
    if(count_temp >= 2) pin_state = 2;
    if(count_temp >= 3) pin_state = 0;
		uint8_t pin_flag = 0;
    // Don't re-launch training after finishing training
    if ((pin_state == MODE_TRAIN_STATIONARY || pin_state == MODE_TRAIN_MOVING) && pin_state == __GET(_v_pinState)) {
        pin_state = MODE_IDLE;
    } else {
			pin_flag = 1;
        //__SET(_v_pinState) = pin_state;
    }
		uint8_t idx;
    threeAxis_t_8 sample;
    unsigned seed = __GET(_v_seed);
    switch(pin_state) {
        case MODE_TRAIN_STATIONARY: //2
            (_v_discardedSamplesCount) = 0;
            __SET(_v_mode) = MODE_TRAIN_STATIONARY;
            __SET(_v_class) = CLASS_STATIONARY;
            __SET(_v_samplesInWindow) = 0;
            while(1){
                if ((_v_discardedSamplesCount) < NUM_WARMUP_SAMPLES) {
                    sample.x = (seed*17)%85;
                    sample.y = (seed*17*17)%85;
                    sample.z = (seed*17*17*17)%85;
											__SET_FLAG(flag1);
                    __SET(_v_seed) = ++seed;
                    ++(_v_discardedSamplesCount);
                }else{
                    __SET(_v_trainingSetSize) = 0;
                    break;
                }
            }
            idx = 2;
						break;
        case MODE_TRAIN_MOVING: //1
            (_v_discardedSamplesCount) = 0;
            __SET(_v_mode) = MODE_TRAIN_MOVING;
            __SET(_v_class) = CLASS_MOVING;
            __SET(_v_samplesInWindow) = 0;
            while(1){
                if ((_v_discardedSamplesCount) < NUM_WARMUP_SAMPLES) {
                    sample.x = (seed*17)%85;
                    sample.y = (seed*17*17)%85;
                    sample.z = (seed*17*17*17)%85;
											__SET_FLAG(flag1);
                    __SET(_v_seed) = ++seed;
                    ++(_v_discardedSamplesCount);
                }else{
                    __SET(_v_trainingSetSize) = 0;
                    break;
                }
            }
            idx = 2;
						break;
        case MODE_RECOGNIZE:    //0
            __SET(_v_mode) = MODE_RECOGNIZE;
            __SET(_v_movingCount) = 0;
            __SET(_v_stationaryCount) = 0;
            __SET(_v_totalCount) = 0;
            __SET(_v_samplesInWindow) = 0;
            idx = 2;
						break;
        default:
            idx = 3;
    }
		__SET_FLAG(flag7);
		__SET(_v_pinState) = pin_flag;
		__SET_FLAG(flag8);
		++__SET(_v_count);
		NEXT(idx);
}



TEB(task_sample){//-->2, NOTE: R(10,12) || W(7,10,12)  40us~45us      
    accelReading sample;
    unsigned i;
		accelReading _v_window[ACCEL_WINDOW_SIZE];
    while(1){
        if (__GET(_v_samplesInWindow) < ACCEL_WINDOW_SIZE){
            unsigned seed = __GET(_v_seed);
            sample.x = (seed*17)%85;
            sample.y = (seed*17*17)%85;
            sample.z = (seed*17*17*17)%85;
							__SET_FLAG(flag2);
            
            (_v_window[__GET(_v_samplesInWindow)].x) = sample.x;
            (_v_window[__GET(_v_samplesInWindow)].y) = sample.y;
            (_v_window[__GET(_v_samplesInWindow)].z) = sample.z;
						__SET_FLAG(flag1);
							__SET(_v_seed) = ++seed;
						__SET_FLAG(flag2);
            ++__SET(_v_samplesInWindow);
        }else{
            for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
                if ((_v_window[i].x) < SAMPLE_NOISE_FLOOR ||
                (_v_window[i].y) < SAMPLE_NOISE_FLOOR ||
                (_v_window[i].z) < SAMPLE_NOISE_FLOOR) {

                (_v_window[i].x) = ((_v_window[i].x) > SAMPLE_NOISE_FLOOR) ? (_v_window[i].x) : 0;
                (_v_window[i].y) = ((_v_window[i].y) > SAMPLE_NOISE_FLOOR) ? (_v_window[i].y) : 0;
                (_v_window[i].z) = ((_v_window[i].z) > SAMPLE_NOISE_FLOOR) ? (_v_window[i].z) : 0;
                }
            }
            break;
        }
    }
    //NEXT(3);
    accelReading mean, stddev;
    mean.x = mean.y = mean.z = 0;
    stddev.x = stddev.y = stddev.z = 0;
    features_t features;

    //int i;
    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        mean.x += (_v_window[i].x);
        mean.y += (_v_window[i].y);
        mean.z += (_v_window[i].z);
    }
    mean.x >>= 2;
    mean.y >>= 2;
    mean.z >>= 2;

    //accelReading sample;

    for (i = 0; i < ACCEL_WINDOW_SIZE; i++) {
        sample.x = (_v_window[i].x);
        sample.y = (_v_window[i].y);
        sample.z = (_v_window[i].z);

        stddev.x += (sample.x > mean.x) ? (sample.x - mean.x)
            : (mean.x - sample.x);
        stddev.y += (sample.y > mean.y) ? (sample.y - mean.y)
            : (mean.y - sample.y);
        stddev.z += (sample.z > mean.z) ? (sample.z - mean.z)
            : (mean.z - sample.z);
    }
    stddev.x >>= 2;
    stddev.y >>= 2;
    stddev.z >>= 2;

    unsigned meanmag = mean.x*mean.x + mean.y*mean.y + mean.z*mean.z;
    unsigned stddevmag = stddev.x*stddev.x + stddev.y*stddev.y + stddev.z*stddev.z;
    features.meanmag   = sqrt16(meanmag);
    features.stddevmag = sqrt16(stddevmag);
		

    switch (__GET(_v_mode)) {
        case MODE_TRAIN_STATIONARY:
        case MODE_TRAIN_MOVING:
					    meanmag = features.meanmag;//__GET(_v_features.meanmag);
						stddevmag = features.stddevmag;//__GET(_v_features.stddevmag);
                int move_less_error = 0;
                int stat_less_error = 0;
                int i;

                long int meanmag;
                long int stddevmag;

     __SET(_v_features.meanmag) = features.meanmag;
      __SET(_v_features.stddevmag) = features.stddevmag;
    __SET(_v_class) = (move_less_error > stat_less_error) ? CLASS_MOVING : CLASS_STATIONARY;
    features_t ms, mm;

    for (i = 0; i < MODEL_SIZE; ++i) {
        ms.meanmag = __GET(_v_model_stationary[i].meanmag);
        ms.stddevmag = __GET(_v_model_stationary[i].stddevmag);
        mm.meanmag = __GET(_v_model_moving[i].meanmag);
        mm.stddevmag = __GET(_v_model_moving[i].stddevmag);
        //__delay_cycles(100);
        long int stat_mean_err = (ms.meanmag > meanmag)
            ? (ms.meanmag - meanmag)
            : (meanmag - ms.meanmag);

        long int stat_sd_err = (ms.stddevmag > stddevmag)
            ? (ms.stddevmag - stddevmag)
            : (stddevmag - ms.stddevmag);

        long int move_mean_err = (mm.meanmag > meanmag)
            ? (mm.meanmag - meanmag)
            : (meanmag - mm.meanmag);

        long int move_sd_err = (mm.stddevmag > stddevmag)
            ? (mm.stddevmag - stddevmag)
            : (stddevmag - mm.stddevmag);

        if (move_mean_err < stat_mean_err) {
            move_less_error++;
        } else {
            stat_less_error++;
        }

        if (move_sd_err < stat_sd_err) {
            move_less_error++;
        } else {
            stat_less_error++;
        }
    }


    unsigned resultStationaryPct;
    unsigned resultMovingPct;
    unsigned sum;
		__SET_FLAG(flag3);
    ++__SET(_v_totalCount);

    switch (__GET(_v_class)) {
        case CLASS_MOVING:
					__SET_FLAG(flag4);
            ++__SET(_v_movingCount);
            break;
        case CLASS_STATIONARY:
					__SET_FLAG(flag5);
            ++__SET(_v_stationaryCount);
            break;
    }

    if (__GET(_v_totalCount) == SAMPLES_TO_COLLECT) {
        resultStationaryPct = __GET(_v_stationaryCount) * 100 / __GET(_v_totalCount);
        resultMovingPct = __GET(_v_movingCount) * 100 / __GET(_v_totalCount);
        sum = __GET(_v_stationaryCount) + __GET(_v_movingCount);
        NEXT(3);
    } else {
        NEXT(2);
        //NEXT(3);
    }
        case MODE_RECOGNIZE:
            __SET(_v_features.meanmag) = features.meanmag;
            __SET(_v_features.stddevmag) = features.stddevmag;
            //NEXT(5);
            switch (__GET(_v_class)) {

            case CLASS_MOVING:
							__SET_FLAG(flag9);
                __SET(_v_model_moving[__GET(_v_trainingSetSize)].meanmag) = __GET(_v_features.meanmag);
                __SET(_v_model_moving[__GET(_v_trainingSetSize)].stddevmag) = __GET(_v_features.stddevmag);
                break;
						case CLASS_STATIONARY:
							__SET_FLAG(flag10);
                __SET(_v_model_stationary[__GET(_v_trainingSetSize)].meanmag) = __GET(_v_features.meanmag);
                __SET(_v_model_stationary[__GET(_v_trainingSetSize)].stddevmag) = __GET(_v_features.stddevmag);
                break;
            }
						__SET_FLAG(flag6);
            ++__SET(_v_trainingSetSize);
            if (__GET(_v_trainingSetSize) < MODEL_SIZE) {
                //NEXT(3);
                NEXT(2);
            } else {
                NEXT(3);
            }
            default:
            // TODO: abort
            break;
    }
}


TEB(task_idle){//-->6, NOTE: R() || W()
    if (__GET(_v_count) < 3) {
        __SET(_v_totalCount)=0;
        NEXT(1);
    }else{
        __SET(_v_totalCount)=0;
        NEXT(0);
    }
}

/**
 * 0. Benchmark app Init here.
 */


void ar_tran_init()
{
        TASK(1,task_init);
        TEB_INIT( task_init,   0  );         //0
        TEB_INIT( task_selectMode,  1);          //1
        TEB_INIT( task_sample,     2);          //3
        TEB_INIT( task_idle,       3 );          //10
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
