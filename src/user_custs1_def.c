/**
 ****************************************************************************************
 *
 * @file user_custs1_def.c
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

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom server 1 (CUSTS1) profile database definitions.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "co_utils.h"
#include "prf_types.h"
#include "attm_db_128.h"
#include "user_custs1_def.h"
#include "attm.h"

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

// Service 1 of the custom server 1
static const att_svc_desc128_t custs1_svc1                          = DEF_SVC1_UUID_128;

static const uint8_t SVC1_TARGET_1_UUID_128[ATT_UUID_128_LEN]     = DEF_SVC1_USERROW_UUID_128;
static const uint8_t SVC1_TARGET_2_UUID_128[ATT_UUID_128_LEN]     = DEF_SVC1_SWITCH_TARGET_2_UUID_128;
static const uint8_t SVC1_TARGET_3_UUID_128[ATT_UUID_128_LEN]     = DEF_SVC1_SWITCH_TARGET_3_UUID_128;
static const uint8_t SVC1_TARGET_4_UUID_128[ATT_UUID_128_LEN]     = DEF_SVC1_SWITCH_TARGET_4_UUID_128;
static const uint8_t SVC1_TARGET_5_UUID_128[ATT_UUID_128_LEN]     = DEF_SVC1_SWITCH_TARGET_5_UUID_128;

// Attribute specifications
static const uint16_t att_decl_svc       = ATT_DECL_PRIMARY_SERVICE;
static const uint16_t att_decl_char      = ATT_DECL_CHARACTERISTIC;
static const uint16_t att_desc_cfg       = ATT_DESC_CLIENT_CHAR_CFG;
static const uint16_t att_desc_user_desc = ATT_DESC_CHAR_USER_DESCRIPTION;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

const uint8_t custs1_services[]  = {SVC1_IDX_SVC, CUSTS1_IDX_NB};
const uint8_t custs1_services_size = ARRAY_LEN(custs1_services) - 1;
const uint16_t custs1_att_max_nb = CUSTS1_IDX_NB;

uint8_t target1 = 0xFF;
uint8_t target2 = 0xFF;
uint8_t target3 = 0xFF;
uint8_t target4 = 0xFF;
uint8_t target5 = 0xFF;

/// Full CUSTS1 Database Description - Used to add attributes into the database
const struct attm_desc_128 custs1_att_db[CUSTS1_IDX_NB] =
{
    /*************************
     * Service 1 configuration
     *************************
     */

    // Service 1 Declaration
    [SVC1_IDX_SVC]                        = {(uint8_t*)&att_decl_svc, ATT_UUID_128_LEN, PERM(WR, ENABLE), sizeof(custs1_svc1), sizeof(custs1_svc1), (uint8_t*)&custs1_svc1},

    // Switch 1 Characteristic
    [SVC1_IDX_USERROW_CHAR]       = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE), 0, 0, NULL},
    [SVC1_IDX_USERROW_VAL]        = {SVC1_TARGET_1_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), PERM(RI, ENABLE) | DEF_SVC1_USERROW_CHAR_MAX_LEN, 0, NULL}, // 1, &target1},
    [SVC1_IDX_USERROW_USER_DESC]  = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE), sizeof(DEF_SVC1_USERROW_USER_DESC) - 1, sizeof(DEF_SVC1_USERROW_USER_DESC) - 1, (uint8_t*)DEF_SVC1_USERROW_USER_DESC},

    // Switch 2 Characteristic
    [SVC1_IDX_SWITCH_TARGET_2_CHAR]       = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE), 0, 0, NULL},
    [SVC1_IDX_SWITCH_TARGET_2_VAL]        = {SVC1_TARGET_2_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), DEF_SVC1_SWITCH_TARGET_2_CHAR_MAX_LEN, 1, &target2},
    [SVC1_IDX_SWITCH_TARGET_2_USER_DESC]  = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE), sizeof(DEF_SVC1_SWITCH_TARGET_2_USER_DESC) - 1, sizeof(DEF_SVC1_SWITCH_TARGET_2_USER_DESC) - 1, (uint8_t*)DEF_SVC1_SWITCH_TARGET_2_USER_DESC},

    // Switch 3 Characteristic
    [SVC1_IDX_SWITCH_TARGET_3_CHAR]       = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE), 0, 0, NULL},
    [SVC1_IDX_SWITCH_TARGET_3_VAL]        = {SVC1_TARGET_3_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), DEF_SVC1_SWITCH_TARGET_3_CHAR_MAX_LEN, 1, &target3},
    [SVC1_IDX_SWITCH_TARGET_3_USER_DESC]  = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE), sizeof(DEF_SVC1_SWITCH_TARGET_3_USER_DESC) - 1, sizeof(DEF_SVC1_SWITCH_TARGET_3_USER_DESC) - 1, (uint8_t*)DEF_SVC1_SWITCH_TARGET_3_USER_DESC},

    // Switch 4 Characteristic
    [SVC1_IDX_SWITCH_TARGET_4_CHAR]       = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE), 0, 0, NULL},
    [SVC1_IDX_SWITCH_TARGET_4_VAL]        = {SVC1_TARGET_4_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), DEF_SVC1_SWITCH_TARGET_4_CHAR_MAX_LEN, 1, &target4},
    [SVC1_IDX_SWITCH_TARGET_4_USER_DESC]  = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE), sizeof(DEF_SVC1_SWITCH_TARGET_4_USER_DESC) - 1, sizeof(DEF_SVC1_SWITCH_TARGET_4_USER_DESC) - 1, (uint8_t*)DEF_SVC1_SWITCH_TARGET_4_USER_DESC},

    // Switch 5 Characteristic
    [SVC1_IDX_SWITCH_TARGET_5_CHAR]       = {(uint8_t*)&att_decl_char, ATT_UUID_16_LEN, PERM(RD, ENABLE), 0, 0, NULL},
    [SVC1_IDX_SWITCH_TARGET_5_VAL]        = {SVC1_TARGET_5_UUID_128, ATT_UUID_128_LEN, PERM(RD, ENABLE) | PERM(WR, ENABLE) | PERM(WRITE_REQ, ENABLE), DEF_SVC1_SWITCH_TARGET_5_CHAR_MAX_LEN, 1, &target5},
    [SVC1_IDX_SWITCH_TARGET_5_USER_DESC]  = {(uint8_t*)&att_desc_user_desc, ATT_UUID_16_LEN, PERM(RD, ENABLE), sizeof(DEF_SVC1_SWITCH_TARGET_5_USER_DESC) - 1, sizeof(DEF_SVC1_SWITCH_TARGET_5_USER_DESC) - 1, (uint8_t*)DEF_SVC1_SWITCH_TARGET_5_USER_DESC},


};

/// @} USER_CONFIG
