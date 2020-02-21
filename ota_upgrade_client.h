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
#if ! defined ZB_HA_OTA_UPGRADE_CLIENT_DEVICE_H
#define ZB_HA_OTA_UPGRADE_CLIENT_DEVICE_H

/**
 *  @defgroup ZB_HA_OTA_UPGRADE_CLIENT_DEVICE OTA Upgrade client device.
 *  @ingroup ZB_ZCL_OTA_UPGRADE_CLIENT
 *  @addtogroup ZB_HA_OTA_UPGRADE_CLIENT_DEVICE
 *  HA OTA Upgrade client device.
 *  @{
    @details
    OTA Upgrade client device has 2 clusters: \n
        - @ref ZB_ZCL_BASIC \n
        - @ref ZB_ZCL_OTA_UPGRADE_CLIENT
*/

#define ZB_HA_DEVICE_VER_OTA_UPGRADE_CLIENT         0       /**< Device version */
#define ZB_HA_OTA_UPGRADE_CLIENT_DEVICE_ID         0xfff0  /**< Device ID */

#define ZB_HA_OTA_UPGRADE_CLIENT_IN_CLUSTER_NUM    1 /**< OTA Upgrade client test input clusters number. */
#define ZB_HA_OTA_UPGRADE_CLIENT_OUT_CLUSTER_NUM   1 /**< OTA Upgrade client test output clusters number. */

/**
 *  @brief Declare cluster list for OTA Upgrade client device.
 *  @param cluster_list_name [IN] - cluster list variable name.
 *  @param basic_attr_list [IN] - attribute list for Basic cluster.
 *  @param ota_upgrade_attr_list [OUT] - attribute list for OTA Upgrade client cluster
 */
#define ZB_HA_DECLARE_OTA_UPGRADE_CLIENT_CLUSTER_LIST(          \
  cluster_list_name,                                            \
  basic_attr_list,                                              \
  ota_upgrade_attr_list)                                        \
  zb_zcl_cluster_desc_t cluster_list_name[] =                   \
  {                                                             \
    ZB_ZCL_CLUSTER_DESC(                                        \
      ZB_ZCL_CLUSTER_ID_BASIC,                                  \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),        \
      (basic_attr_list),                                        \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
      ZB_ZCL_MANUF_CODE_INVALID                                 \
      ),                                                        \
    ZB_ZCL_CLUSTER_DESC(                                        \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE,                            \
      ZB_ZCL_ARRAY_SIZE(ota_upgrade_attr_list, zb_zcl_attr_t),  \
      (ota_upgrade_attr_list),                                  \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                               \
      ZB_ZCL_MANUF_CODE_INVALID                                 \
      )                                                         \
  }

/**
 *  @brief Declare simple descriptor for OTA Upgrade client device.
 *  @param ep_name - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param in_clust_num [IN] - number of supported input clusters.
 *  @param out_clust_num [IN] - number of supported output clusters.
 *  @note in_clust_num, out_clust_num should be defined by numeric constants, not variables or any
 *  definitions, because these values are used to form simple descriptor type name.
 */
#define ZB_ZCL_DECLARE_OTA_UPGRADE_CLIENT_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)   \
  /*ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num); - struct zb_af_simple_desc_1_1_t declare by default */   \
  ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)   \
      simple_desc_##ep_name =                           \
  {                                                     \
    ep_id,                                              \
    ZB_AF_HA_PROFILE_ID,                                \
    ZB_HA_OTA_UPGRADE_CLIENT_DEVICE_ID,                 \
    ZB_HA_DEVICE_VER_OTA_UPGRADE_CLIENT,                \
    0,                                                  \
    in_clust_num,                                       \
    out_clust_num,                                      \
    {                                                   \
      ZB_ZCL_CLUSTER_ID_BASIC,                          \
      ZB_ZCL_CLUSTER_ID_OTA_UPGRADE                     \
    }                                                   \
  }

/**
 *  @brief Declare endpoint for OTA Upgrade client device.
 *  @param ep_name [IN] - endpoint variable name.
 *  @param ep_id [IN] - endpoint ID.
 *  @param cluster_list [IN] - endpoint cluster list.
 */
#define ZB_HA_DECLARE_OTA_UPGRADE_CLIENT_EP(ep_name, ep_id, cluster_list) \
  ZB_ZCL_DECLARE_OTA_UPGRADE_CLIENT_SIMPLE_DESC(                        \
    ep_name,                                                            \
    ep_id,                                                              \
    ZB_HA_OTA_UPGRADE_CLIENT_IN_CLUSTER_NUM,                            \
    ZB_HA_OTA_UPGRADE_CLIENT_OUT_CLUSTER_NUM);                          \
  ZB_AF_DECLARE_ENDPOINT_DESC(                                          \
    ep_name,                                                            \
    ep_id,                                                              \
    ZB_AF_HA_PROFILE_ID,                                                \
    0,                                                                  \
    NULL,                                                               \
    ZB_ZCL_ARRAY_SIZE(                                                  \
      cluster_list,                                                     \
      zb_zcl_cluster_desc_t),                                           \
    cluster_list,                                                       \
    (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                   \
    0, NULL, /* No reporting ctx */                                     \
    0, NULL)

#define ZB_HA_DECLARE_OTA_UPGRADE_CLIENT_CTX(device_ctx, ep_name)             \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)

/**
 *  @}
 */

#endif /* ! defined ZB_HA_OTA_UPGRADE_CLIENT_DEVICE_H */
