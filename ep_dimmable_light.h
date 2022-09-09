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
#if !defined ZB_HA_DIMMABLE_LIGHT_EP_H
#define ZB_HA_DIMMABLE_LIGHT_EP_H

#define ZB_ZCL_DECLARE_HA_DIMMABLE_LIGHT_SIMPLE_DESC_VA(ep_name, ep_id, in_clust_num, out_clust_num) \
    ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                                 \
    ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name)                                  \
    simple_desc_##ep_name =                                                                          \
        {                                                                                            \
            ep_id,                                                                                   \
            ZB_AF_HA_PROFILE_ID,                                                                     \
            ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,                                                          \
            ZB_HA_DEVICE_VER_DIMMABLE_LIGHT,                                                         \
            0,                                                                                       \
            in_clust_num,                                                                            \
            out_clust_num,                                                                           \
            {                                                                                        \
                ZB_ZCL_CLUSTER_ID_BASIC,                                                             \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                          \
                ZB_ZCL_CLUSTER_ID_GROUPS,                                                            \
                ZB_ZCL_CLUSTER_ID_ON_OFF,                                                            \
                ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                     \
            }}

#define ZB_HA_DECLARE_LIGHT_EP(ep_name, ep_id, cluster_list)                                                                               \
    ZB_ZCL_DECLARE_HA_DIMMABLE_LIGHT_SIMPLE_DESC_VA(ep_name,                                                                               \
                                                    ep_id,                                                                                 \
                                                    ZB_HA_DIMMABLE_LIGHT_IN_CLUSTER_NUM,                                                   \
                                                    ZB_HA_DIMMABLE_LIGHT_OUT_CLUSTER_NUM);                                                 \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info##ep_name, ZB_HA_DIMMABLE_LIGHT_REPORT_ATTR_COUNT);                                   \
    ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(cvc_alarm_info##ep_name, ZB_HA_DIMMABLE_LIGHT_CVC_ATTR_COUNT);                                  \
    ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                                                                   \
                                ep_id,                                                                                                     \
                                ZB_AF_HA_PROFILE_ID,                                                                                       \
                                0,                                                                                                         \
                                NULL,                                                                                                      \
                                ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                                                    \
                                cluster_list,                                                                                              \
                                (zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,                                                         \
                                ZB_HA_DIMMABLE_LIGHT_REPORT_ATTR_COUNT,                                                                    \
                                reporting_info##ep_name,                                                                                   \
                                ZB_HA_DIMMABLE_LIGHT_CVC_ATTR_COUNT,                                                                       \
                                cvc_alarm_info##ep_name)

#define ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST_VA(attr_list, current_level, remaining_time, ...) \
    zb_zcl_level_control_move_status_t move_status_data_ctx##__VA_ARGS__##_attr_list;              \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                    \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, (current_level))              \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID, (remaining_time))            \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID,                                 \
        (&(move_status_data_ctx##__VA_ARGS__##_attr_list)))                                        \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(dev_ctx_name, dimmable_light_bulb_cluster_list) \
    ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(dev_ctx_name##_identify_attr_list,                       \
        &dev_ctx_name.identify_attr.identify_time);                                              \
    ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(dev_ctx_name####_groups_attr_list,                         \
        &dev_ctx_name.groups_attr.name_support);                                                 \
    ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(dev_ctx_name##_scenes_attr_list,                           \
        &dev_ctx_name.scenes_attr.scene_count,                                                   \
        &dev_ctx_name.scenes_attr.current_scene,                                                 \
        &dev_ctx_name.scenes_attr.current_group,                                                 \
        &dev_ctx_name.scenes_attr.scene_valid,                                                   \
        &dev_ctx_name.scenes_attr.name_support);                                                 \
    ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(dev_ctx_name##_basic_attr_list,                         \
        &dev_ctx_name.basic_attr.zcl_version,                                                    \
        &dev_ctx_name.basic_attr.app_version,                                                    \
        &dev_ctx_name.basic_attr.stack_version,                                                  \
        &dev_ctx_name.basic_attr.hw_version,                                                     \
        dev_ctx_name.basic_attr.mf_name,                                                         \
        dev_ctx_name.basic_attr.model_id,                                                        \
        dev_ctx_name.basic_attr.date_code,                                                       \
        &dev_ctx_name.basic_attr.power_source,                                                   \
        dev_ctx_name.basic_attr.location_id,                                                     \
        &dev_ctx_name.basic_attr.ph_env,                                                         \
        dev_ctx_name.basic_attr.sw_ver);                                                         \
    ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(dev_ctx_name##_on_off_attr_list,                       \
        &dev_ctx_name.on_off_attr.on_off,                                                        \
        &dev_ctx_name.on_off_attr.global_scene_ctrl,                                             \
        &dev_ctx_name.on_off_attr.on_time,                                                       \
        &dev_ctx_name.on_off_attr.off_wait_time);                                                \
    ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST_VA(dev_ctx_name##_level_control_attr_list,          \
        &dev_ctx_name.level_control_attr.current_level,                                          \
        &dev_ctx_name.level_control_attr.remaining_time);                                        \
    ZB_HA_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(dimmable_light_bulb_cluster_list,                  \
        dev_ctx_name##_basic_attr_list,                                                          \
        dev_ctx_name##_identify_attr_list,                                                       \
        dev_ctx_name##_groups_attr_list,                                                         \
        dev_ctx_name##_on_off_attr_list,                                                         \
        dev_ctx_name##_level_control_attr_list);

#endif /* ! defined ZB_HA_DIMMABLE_LIGHT_EP_H */
