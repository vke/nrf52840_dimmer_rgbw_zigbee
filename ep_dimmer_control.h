/**
 * Copyright (c) 2018 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#if !defined ZB_HA_DIMMER_CONTROL_EP_H
#define ZB_HA_DIMMER_CONTROL_EP_H

#define ZB_HA_DEVICE_VER_OTA_UPGRADE_CLIENT 0     /**< Device version */
#define ZB_HA_OTA_UPGRADE_CLIENT_DEVICE_ID  0xfff0 /**< Device ID */

#define ZB_HA_DIMMER_CONTROL_IN_CLUSTER_NUM  5  /**< Dimmer control ep input clusters number. */
#define ZB_HA_DIMMER_CONTROL_OUT_CLUSTER_NUM 1 /**< Dimmer controp ep output clusters number. */

#define ZB_HA_DIMMER_CONTROL_REPORT_ATTR_COUNT 16

#define ZB_HA_DECLARE_DIMMER_CONTROL_CLUSTER_LIST(                                  \
    cluster_list_name,                                                              \
    basic_attr_list,                                                                \
    power_config_attr_list,                                                         \
    identify_attr_list,                                                             \
    device_temp_attr_list,                                                          \
    electrical_measurement_attr_list,                                               \
    ota_upgrade_attr_list)                                                          \
    zb_zcl_cluster_desc_t cluster_list_name[] =                                     \
        {                                                                           \
            ZB_ZCL_CLUSTER_DESC(                                                    \
                ZB_ZCL_CLUSTER_ID_BASIC,                                            \
                ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),                  \
                (basic_attr_list),                                                  \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                         \
                ZB_ZCL_MANUF_CODE_INVALID),                                         \
            ZB_ZCL_CLUSTER_DESC(                                                    \
                ZB_ZCL_CLUSTER_ID_POWER_CONFIG,                                     \
                ZB_ZCL_ARRAY_SIZE(power_config_attr_list, zb_zcl_attr_t),           \
                (power_config_attr_list),                                           \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                         \
                ZB_ZCL_MANUF_CODE_INVALID),                                         \
            ZB_ZCL_CLUSTER_DESC(                                                    \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                                         \
                ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),               \
                (identify_attr_list),                                               \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                         \
                ZB_ZCL_MANUF_CODE_INVALID),                                         \
            ZB_ZCL_CLUSTER_DESC(                                                    \
                ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG,                               \
                ZB_ZCL_ARRAY_SIZE(device_temp_attr_list, zb_zcl_attr_t),            \
                (device_temp_attr_list),                                            \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                         \
                ZB_ZCL_MANUF_CODE_INVALID),                                         \
            ZB_ZCL_CLUSTER_DESC(                                                    \
                ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,                           \
                ZB_ZCL_ARRAY_SIZE(electrical_measurement_attr_list, zb_zcl_attr_t), \
                (electrical_measurement_attr_list),                                 \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                         \
                ZB_ZCL_MANUF_CODE_INVALID),                                         \
            ZB_ZCL_CLUSTER_DESC(                                                    \
                ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                                      \
                ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t),            \
                (ota_upgrade_attr_list),                                            \
                ZB_ZCL_CLUSTER_CLIENT_ROLE,                                         \
                ZB_ZCL_MANUF_CODE_INVALID)}

#define ZB_ZCL_DECLARE_DIMMER_CONTROL_SIMPLE_DESC_VA(ep_name, ep_id, in_clust_num, out_clust_num) \
    ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                              \
    ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name)                               \
    simple_desc_##ep_name =                                                                       \
        {                                                                                         \
            ep_id,                               /* Endpoint */                                   \
            ZB_AF_HA_PROFILE_ID,                 /* Application profile identifier */             \
            ZB_HA_OTA_UPGRADE_CLIENT_DEVICE_ID,  /* Application device identifier */              \
            ZB_HA_DEVICE_VER_OTA_UPGRADE_CLIENT, /* Application device version */                 \
            0,                                   /* Reserved */                                   \
            in_clust_num,                        /* Application input cluster count */            \
            out_clust_num,                       /* Application output cluster count */           \
            {ZB_ZCL_CLUSTER_ID_BASIC,            /* Application input and output cluster list */  \
             ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG,                                                \
             ZB_ZCL_CLUSTER_ID_OTA_UPGRADE}}

#define ZB_HA_DECLARE_DIMMER_CONTROL_EP(ep_name, ep_id, cluster_list)                                                                        \
    ZB_ZCL_DECLARE_DIMMER_CONTROL_SIMPLE_DESC_VA(                                                                                            \
        ep_name,                                                                                                                             \
        ep_id,                                                                                                                               \
        ZB_HA_DIMMER_CONTROL_IN_CLUSTER_NUM,                                                                                                 \
        ZB_HA_DIMMER_CONTROL_OUT_CLUSTER_NUM);                                                                                               \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info##ep_name, ZB_HA_DIMMER_CONTROL_REPORT_ATTR_COUNT);                                     \
    ZB_AF_DECLARE_ENDPOINT_DESC(                                                                                                             \
        ep_name,                                                /* endpoint name */                                                          \
        ep_id,                                                  /* endpoint id */                                                            \
        ZB_AF_HA_PROFILE_ID,                                    /* id of profile deployed on this endpoint */                                \
        0,                                                      /* unused parameter */                                                       \
        NULL,                                                   /* unused parameter */                                                       \
        ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), /* number of clusters deployed on endpoint */                                \
        cluster_list,                                           /* pointer to cluster list */                                                \
        (zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,      /* pointer to simple descriptor */                                           \
        ZB_HA_DIMMER_CONTROL_REPORT_ATTR_COUNT,                                                                                              \
        reporting_info##ep_name,                                                                                                             \
        0,                                                      /* number of level control attributes */                                     \
        NULL                                                    /* level control context variable name (NULL if no level control context) */ \
    )

#endif /* ! defined ZB_HA_DIMMER_CONTROL_EP_H */
