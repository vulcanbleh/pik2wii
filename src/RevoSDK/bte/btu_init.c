#include "RevoSDK/bte/btu.h"

/* Original source:
 * bluedroid <android.googlesource.com/platform/external/bluetooth/bluedroid>
 * stack/btu/btu_init.c
 */

/******************************************************************************
 *
 *  Copyright (C) 2000-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/* Includes changes by muff1n1634 */

/*******************************************************************************
 * headers
 */

#include "types.h"
#include <string.h>

#include "RevoSDK/bte/bt_types.h"
#include "RevoSDK/bte/data_types.h"

#include "RevoSDK/bte/btm_int.h"
#include "RevoSDK/bte/hcidefs.h"
#include "RevoSDK/bte/l2c_int.h"
#include "RevoSDK/bte/sdp_int.h"

/*******************************************************************************
 * variables
 */

// .sdata2
BD_ADDR const BT_BD_ANY = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/*******************************************************************************
 * functions
 */

void btu_init_core(void) {
    btm_init();
    l2c_init();
    sdp_init();

    btm_discovery_db_init();
}

void BTE_Init(void) {
    memset(&btu_cb, 0, sizeof btu_cb);

    btu_cb.hcit_acl_pkt_size = BTU_DEFAULT_DATA_SIZE + HCI_DATA_PREAMBLE_SIZE;
    btu_cb.controller_cmd_window = 1;
}

UINT16 BTU_AclPktSize(void) {
    return btu_cb.hcit_acl_pkt_size;
}
