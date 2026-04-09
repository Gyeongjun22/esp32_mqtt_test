#ifndef __TASK_BLE_H__
#define __TASK_BLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void task_ble_main(void);



/* Attributes State Machine */
enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    HRS_IDX_NB,
};

#endif
