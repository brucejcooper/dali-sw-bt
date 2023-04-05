/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (c) 2016-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef _USER_CUSTS1_DEF_H_
#define _USER_CUSTS1_DEF_H_

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "attm_db_128.h"

/*
 * DEFINES
 ****************************************************************************************
 */

// Service 1 of the custom server 1
// 730d6403-d934-4244-adbd-b30cdd03987c
#define DEF_SVC1_UUID_128                {0x7c, 0x98, 0x03, 0xdd, 0x0c, 0xb3, 0xbd, 0xad, 0x44, 0x44, 0x34, 0xd9, 0x03, 0x64, 0x0d, 0x73}

// d1 51 ab 07-9e d0-47 56-92 28-31 b1 64 a7 09 b0
#define DEF_SVC1_USERROW_UUID_128     {0xb1, 0x09, 0xa7, 0x64, 0xb1, 0x31, 0x28, 0x92, 0x56, 0x47, 0xd0, 0x9e, 0x07, 0xab, 0x51, 0xd1}
#define DEF_SVC1_USERROW_CHAR_MAX_LEN     1
#define DEF_SVC1_USERROW_USER_DESC "User Config"

#define DEF_SVC1_SWITCH_TARGET_2_UUID_128     {0xb2, 0x09, 0xa7, 0x64, 0xb1, 0x31, 0x28, 0x92, 0x56, 0x47, 0xd0, 0x9e, 0x07, 0xab, 0x51, 0xd1}
#define DEF_SVC1_SWITCH_TARGET_2_CHAR_MAX_LEN     1
#define DEF_SVC1_SWITCH_TARGET_2_USER_DESC "Switch 2 Target"

#define DEF_SVC1_SWITCH_TARGET_3_UUID_128     {0xb3, 0x09, 0xa7, 0x64, 0xb1, 0x31, 0x28, 0x92, 0x56, 0x47, 0xd0, 0x9e, 0x07, 0xab, 0x51, 0xd1}
#define DEF_SVC1_SWITCH_TARGET_3_CHAR_MAX_LEN     1
#define DEF_SVC1_SWITCH_TARGET_3_USER_DESC "Switch 3 Target"

#define DEF_SVC1_SWITCH_TARGET_4_UUID_128     {0xb4, 0x09, 0xa7, 0x64, 0xb1, 0x31, 0x28, 0x92, 0x56, 0x47, 0xd0, 0x9e, 0x07, 0xab, 0x51, 0xd1}
#define DEF_SVC1_SWITCH_TARGET_4_CHAR_MAX_LEN     1
#define DEF_SVC1_SWITCH_TARGET_4_USER_DESC "Switch 4 Target"

#define DEF_SVC1_SWITCH_TARGET_5_UUID_128     {0xb5, 0x09, 0xa7, 0x64, 0xb1, 0x31, 0x28, 0x92, 0x56, 0x47, 0xd0, 0x9e, 0x07, 0xab, 0x51, 0xd1}
#define DEF_SVC1_SWITCH_TARGET_5_CHAR_MAX_LEN     1
#define DEF_SVC1_SWITCH_TARGET_5_USER_DESC "Switch 5 Target"


/// Custom1 Service Data Base Characteristic enum
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,

    SVC1_IDX_USERROW_CHAR,
    SVC1_IDX_USERROW_VAL,
    SVC1_IDX_USERROW_USER_DESC,

    SVC1_IDX_SWITCH_TARGET_2_CHAR,
    SVC1_IDX_SWITCH_TARGET_2_VAL,
    SVC1_IDX_SWITCH_TARGET_2_USER_DESC,

    SVC1_IDX_SWITCH_TARGET_3_CHAR,
    SVC1_IDX_SWITCH_TARGET_3_VAL,
    SVC1_IDX_SWITCH_TARGET_3_USER_DESC,

    SVC1_IDX_SWITCH_TARGET_4_CHAR,
    SVC1_IDX_SWITCH_TARGET_4_VAL,
    SVC1_IDX_SWITCH_TARGET_4_USER_DESC,

    SVC1_IDX_SWITCH_TARGET_5_CHAR,
    SVC1_IDX_SWITCH_TARGET_5_VAL,
    SVC1_IDX_SWITCH_TARGET_5_USER_DESC,



    CUSTS1_IDX_NB
};

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_
