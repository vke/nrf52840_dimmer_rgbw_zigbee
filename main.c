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

#include "sdk_config.h"
#include "zb_error_handler.h"
#include "zb_ha_dimmable_light.h"
#include "zb_mem_config_med.h"
#include "zb_nrf52_internal.h"
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zigbee_helpers.h"

#include "zigbee_dfu_transport.h"
#include "nrf_dfu_settings.h"

#include "zboss_api_af_addons.h"

#include "app_timer.h"
#include "boards.h"
#include "bsp.h"
#include "nrf_drv_pwm.h"
#include "nrf_drv_saadc.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_delay.h"
#include "nrf_temp.h"

#include "zb_zcl_device_temperature.h"
#include "zb_zcl_electrical_measurement.h"
#include "zb_zcl_power_config.h"
#include "ep_dimmer_control.h"
#include "ep_dimmable_light.h"

#define MAX_CHILDREN                  32                      //
#define IEEE_CHANNEL_MASK             (1l << ZIGBEE_CHANNEL)  //
#define HA_DIMMABLE_LIGHT_ENDPOINT_R  10                      //
#define HA_DIMMABLE_LIGHT_ENDPOINT_G  11                      //
#define HA_DIMMABLE_LIGHT_ENDPOINT_B  12                      //
#define HA_DIMMABLE_LIGHT_ENDPOINT_W  13                      //
#define DIMMER_CONTROL_ENDPOINT       14                      //
#define ERASE_PERSISTENT_CONFIG       ZB_FALSE                //
#define DIMMER_PWM_INSTANCE           NRF_DRV_PWM_INSTANCE(0) //
#define DIMMER_PWM_VALUE_MAX          255                     //

#define DIMMER_EFEKTA_BOARD_DBG       0                       // 1 - use on-board leds (debug mode), 0 - use gpio header (production mode)

#define BULB_INIT_BASIC_APP_VERSION   01                                  /**< Version of the application software (1 byte). */
#define BULB_INIT_BASIC_STACK_VERSION 10                                  /**< Version of the implementation of the ZigBee stack (1 byte). */
#define BULB_INIT_BASIC_HW_VERSION    11                                  /**< Version of the hardware of the device (1 byte). */
#define BULB_INIT_BASIC_MANUF_NAME    "vke"                               /**< Manufacturer name (32 bytes). */
#define BULB_INIT_BASIC_MODEL_ID      "RGBW Dimmer 1.2"                   /**< Model number assigned by manufacturer (32-bytes long string). */
#define BULB_INIT_BASIC_DATE_CODE     "20200503"                          /**< First 8 bytes specify the date of manufacturer of the device in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific. */
#define BULB_INIT_BASIC_POWER_SOURCE  ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
#define BULB_INIT_BASIC_LOCATION_DESC ""                                  /**< Describes the physical location of the device (16 bytes). May be modified during commisioning process. */
#define BULB_INIT_BASIC_PH_ENV        ZB_ZCL_BASIC_ENV_UNSPECIFIED        /**< Describes the type of physical environment. For possible values see section 3.2.2.2.10 of ZCL specification. */
#define BULB_INIT_BASIC_SW_VER        "1.2.0.7"

#define IDENTIFY_MODE_BSP_EVT         BSP_EVENT_KEY_0    //
#define ZIGBEE_NETWORK_STATE_LED      BSP_BOARD_LED_0    //
#define ERASE_PERSISTENT_CONFIG_PIN   BSP_BUTTON_0       //
#define OTA_ACTVITY_LED               BSP_BOARD_LED_1    //

#define INTERNAL_TEMPERATURE_MEAS_INTERVAL   APP_TIMER_TICKS(1000)                   /**< Internal temperature measurement interval (ticks). This value corresponds to 1 second. */
#define BATTERY_LEVEL_MEAS_INTERVAL          APP_TIMER_TICKS(1000)                   /**< Battery level measurement interval (ticks). This value corresponds to 1 second. */

#define ADC_12V_DIVISOR                      37033
#define ADC_12V_MULTIPLIER                   45
#define ADC_12V_DIVISOR_MV                   4629
#define ADC_12V_MULTIPLIER_MV                5625

#define ADC_CHANNELS                         2
#define ADC_SAMPLES_PER_CHANNEL              32

// ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS * ADC_PRE_SCALING_COMPENSATION * 50) / ADC_RES_14BIT / 11))
// ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_14BIT) * ADC_PRE_SCALING_COMPENSATION)
#define ADC_3V3_RESULT_IN_0_1_VOLTS(ADC_VALUE) ((((ADC_VALUE) * 9) / 4096))
#define ADC_12V_RESULT_IN_MILLI_VOLTS(ADC_VALUE) ((((ADC_VALUE) * ADC_12V_MULTIPLIER_MV) / ADC_12V_DIVISOR_MV))


#define OTA_UPGRADE_TEST_MANUFACTURER 123
#define OTA_UPGRADE_TEST_IMAGE_TYPE   321
#define OTA_UPGRADE_DATA_BLOCK_SIZE    BACKGROUND_DFU_DEFAULT_BLOCK_SIZE

#if (NRF_DFU_HW_VERSION > 0xFFFFUL)
#error Incorrect Hardware Version value in NRF_DFU_HW_VERSION
#endif

#if DIMMER_EFEKTA_BOARD_DBG
#define DIMMER_CHANNEL_PIN_R          LED2_R // pwm channel 0
#define DIMMER_CHANNEL_PIN_G          LED2_G // pwm channel 1
#define DIMMER_CHANNEL_PIN_B          LED2_B // pwm channel 2
#define DIMMER_CHANNEL_PIN_W          LED2_DW // pwm channel 3
#define DIMMER_PWM_INVERSION          0
#else // DIMMER_EFEKTA_BOARD_DBG
#define DIMMER_CHANNEL_PIN_R          LED2_DR // pwm channel 0
#define DIMMER_CHANNEL_PIN_G          LED2_DG // pwm channel 1
#define DIMMER_CHANNEL_PIN_B          LED2_DB // pwm channel 2
#define DIMMER_CHANNEL_PIN_W          LED2_DW // pwm channel 3
#define DIMMER_PWM_INVERSION          0 // NRF_DRV_PWM_PIN_INVERTED
#endif // DIMMER_EFEKTA_BOARD_DBG

APP_TIMER_DEF(m_battery_timer_id); /**< Battery measurement timer. */
APP_TIMER_DEF(m_internal_temperature_timer_id); /**< Internal temperature measurement timer. */
static nrf_saadc_value_t adc_buf[ADC_CHANNELS * ADC_SAMPLES_PER_CHANNEL];

static nrf_drv_pwm_t m_led_pwm = DIMMER_PWM_INSTANCE;
static nrf_pwm_values_individual_t m_led_values;
static nrf_pwm_sequence_t const m_led_seq =
{
        .values.p_individual = &m_led_values,
        .length = NRF_PWM_VALUES_LENGTH(m_led_values),
        .repeats = 0,
        .end_delay = 0
};

#if !defined ZB_ROUTER_ROLE
#error Define ZB_ROUTER_ROLE to compile light bulb (Router) source code.
#endif

typedef struct
{
    zb_bool_t on_off;
    zb_bool_t global_scene_ctrl;
    zb_uint16_t on_time;
    zb_uint16_t off_wait_time;
} bulb_device_on_off_attr_t;

typedef struct
{
    zb_uint8_t current_level;
    zb_uint16_t remaining_time;
} bulb_device_level_control_attr_t;

typedef struct
{
    zb_uint8_t scene_count;
    zb_uint8_t current_scene;
    zb_uint8_t scene_valid;
    zb_uint8_t name_support;
    zb_uint16_t current_group;
} bulb_device_scenes_attr_t;

typedef struct
{
    zb_uint8_t name_support;
} bulb_device_groups_attr_t;

typedef struct
{
    uint8_t ep_id;
    uint8_t pwm_channel;

    zb_zcl_basic_attrs_ext_t          basic_attr;
    zb_zcl_identify_attrs_t           identify_attr;
    bulb_device_scenes_attr_t         scenes_attr;
    bulb_device_groups_attr_t         groups_attr;
    bulb_device_on_off_attr_t         on_off_attr;
    bulb_device_level_control_attr_t  level_control_attr;
} bulb_device_ctx_t;

static bulb_device_ctx_t m_dev_ctx_r;
static bulb_device_ctx_t m_dev_ctx_g;
static bulb_device_ctx_t m_dev_ctx_b;
static bulb_device_ctx_t m_dev_ctx_w;

// Dimmer control ep structs
typedef struct
{
    zb_int16_t current_temperature;
} dimmer_control_device_temperature_attr_t;

typedef struct
{
    zb_ieee_addr_t upgrade_server;
    zb_uint32_t    file_offset;
    zb_uint32_t    file_version;
    zb_uint16_t    stack_version;
    zb_uint32_t    downloaded_file_ver;
    zb_uint32_t    downloaded_stack_ver;
    zb_uint8_t     image_status;
    zb_uint16_t    manufacturer;
    zb_uint16_t    image_type;
    zb_uint16_t    min_block_reque;
    zb_uint16_t    image_stamp;
    zb_uint16_t    server_addr;
    zb_uint8_t     server_ep;
} dimmer_control_ota_upgrade_attr_t;

typedef struct
{
    zb_zcl_basic_attrs_ext_t              basic_attr;
    zb_zcl_power_config_attrs_t           power_config_attr;
    zb_zcl_identify_attrs_t               identify_attr;
    zb_zcl_device_temperature_attrs_t     device_temperature_attr;
    zb_zcl_electrical_measurement_attrs_t electrical_measurement_attr;
    dimmer_control_ota_upgrade_attr_t     ota_attr;
} dimmer_control_ctx_t;

APP_TIMER_DEF(m_ota_server_discovery_timer);
static zb_zcl_ota_upgrade_client_periodical_discovery_ctx_t m_discovery_ctx;

static dimmer_control_ctx_t m_dimmer_control_ctx;

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(dimmer_control_basic_attr_list,
                                     &m_dimmer_control_ctx.basic_attr.zcl_version,
                                     &m_dimmer_control_ctx.basic_attr.app_version,
                                     &m_dimmer_control_ctx.basic_attr.stack_version,
                                     &m_dimmer_control_ctx.basic_attr.hw_version,
                                     m_dimmer_control_ctx.basic_attr.mf_name,
                                     m_dimmer_control_ctx.basic_attr.model_id,
                                     m_dimmer_control_ctx.basic_attr.date_code,
                                     &m_dimmer_control_ctx.basic_attr.power_source,
                                     m_dimmer_control_ctx.basic_attr.location_id,
                                     &m_dimmer_control_ctx.basic_attr.ph_env,
                                     m_dimmer_control_ctx.basic_attr.sw_ver);

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(dimmer_control_identify_attr_list,
                                    &m_dimmer_control_ctx.identify_attr.identify_time);

ZB_ZCL_DECLARE_DEVICE_TEMPERATURE_ATTR_LIST(dimmer_control_device_temperature_attr_list,
                                            m_dimmer_control_ctx.device_temperature_attr);

ZB_ZCL_DECLARE_ELECTRICAL_MEASUREMENT_ATTR_LIST(dimmer_control_electrical_measurement_attr_list,
                                                m_dimmer_control_ctx.electrical_measurement_attr);

ZB_ZCL_DECLARE_POWER_CONFIG_ATTR_LIST(dimmer_control_power_config_attr_list,
                                      m_dimmer_control_ctx.power_config_attr);

ZB_ZCL_DECLARE_OTA_UPGRADE_ATTRIB_LIST(dimmer_control_ota_upgrade_attr_list,
                                       m_dimmer_control_ctx.ota_attr.upgrade_server,
                                       &m_dimmer_control_ctx.ota_attr.file_offset,
                                       &m_dimmer_control_ctx.ota_attr.file_version,
                                       &m_dimmer_control_ctx.ota_attr.stack_version,
                                       &m_dimmer_control_ctx.ota_attr.downloaded_file_ver,
                                       &m_dimmer_control_ctx.ota_attr.downloaded_stack_ver,
                                       &m_dimmer_control_ctx.ota_attr.image_status,
                                       &m_dimmer_control_ctx.ota_attr.manufacturer,
                                       &m_dimmer_control_ctx.ota_attr.image_type,
                                       &m_dimmer_control_ctx.ota_attr.min_block_reque,
                                       &m_dimmer_control_ctx.ota_attr.image_stamp,
                                       &m_dimmer_control_ctx.ota_attr.server_addr,
                                       &m_dimmer_control_ctx.ota_attr.server_ep,
                                       (uint16_t)NRF_DFU_HW_VERSION,
                                       OTA_UPGRADE_DATA_BLOCK_SIZE,
                                       ZB_ZCL_OTA_UPGRADE_QUERY_TIMER_COUNT_DEF);

ZB_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(m_dev_ctx_r, m_dev_clusters_r);
ZB_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(m_dev_ctx_g, m_dev_clusters_g);
ZB_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(m_dev_ctx_b, m_dev_clusters_b);
ZB_DECLARE_DIMMABLE_LIGHT_CLUSTER_LIST(m_dev_ctx_w, m_dev_clusters_w);

ZB_HA_DECLARE_DIMMER_CONTROL_CLUSTER_LIST(dimmer_control_clusters,
                                          dimmer_control_basic_attr_list,
                                          dimmer_control_power_config_attr_list,
                                          dimmer_control_identify_attr_list,
                                          dimmer_control_device_temperature_attr_list,
                                          dimmer_control_electrical_measurement_attr_list,
                                          dimmer_control_ota_upgrade_attr_list);

ZB_HA_DECLARE_LIGHT_EP(dimmable_light_ep_r, HA_DIMMABLE_LIGHT_ENDPOINT_R, m_dev_clusters_r);
ZB_HA_DECLARE_LIGHT_EP(dimmable_light_ep_g, HA_DIMMABLE_LIGHT_ENDPOINT_G, m_dev_clusters_g);
ZB_HA_DECLARE_LIGHT_EP(dimmable_light_ep_b, HA_DIMMABLE_LIGHT_ENDPOINT_B, m_dev_clusters_b);
ZB_HA_DECLARE_LIGHT_EP(dimmable_light_ep_w, HA_DIMMABLE_LIGHT_ENDPOINT_W, m_dev_clusters_w);

ZB_HA_DECLARE_DIMMER_CONTROL_EP(dimmer_control_ep, DIMMER_CONTROL_ENDPOINT, dimmer_control_clusters);

ZBOSS_DECLARE_DEVICE_CTX_EP_VA(dimmable_light_ctx, &dimmable_light_ep_r, &dimmable_light_ep_g, &dimmable_light_ep_b, &dimmable_light_ep_w, &dimmer_control_ep);

void update_voltage_attributes_callback(zb_uint8_t param)
{
    ZB_ZCL_SET_ATTRIBUTE(DIMMER_CONTROL_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID,
                         (zb_uint8_t *)&m_dimmer_control_ctx.electrical_measurement_attr.dc_voltage,
                         ZB_FALSE);

    ZB_ZCL_SET_ATTRIBUTE(DIMMER_CONTROL_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID,
                         (zb_uint8_t *)&m_dimmer_control_ctx.power_config_attr.mains_voltage,
                         ZB_FALSE);

    NRF_LOG_INFO("12V: %d / %d, 3V3: %d",
                 m_dimmer_control_ctx.electrical_measurement_attr.dc_voltage,
                 ADC_12V_RESULT_IN_MILLI_VOLTS(((int32_t)m_dimmer_control_ctx.electrical_measurement_attr.dc_voltage)),
                 m_dimmer_control_ctx.power_config_attr.mains_voltage);
}

void saadc_event_handler(nrf_drv_saadc_evt_t const *p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        uint32_t err_code;

        int32_t sums[ADC_CHANNELS];
        for (int i = 0; i < ADC_CHANNELS; i++) {
            sums[i] = 0;
            for (int j = 0; j < ADC_SAMPLES_PER_CHANNEL; j++) {
                sums[i] += p_event->data.done.p_buffer[j * 2 + i];
            }
            sums[i] = sums[i] / ADC_SAMPLES_PER_CHANNEL;
        }

        m_dimmer_control_ctx.electrical_measurement_attr.dc_voltage = (int16_t)(sums[1]);

        if (sums[0] < 0)
            sums[0] = 0;
        m_dimmer_control_ctx.power_config_attr.mains_voltage = (zb_uint16_t)(ADC_3V3_RESULT_IN_0_1_VOLTS(sums[0]));

        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, ADC_CHANNELS * ADC_SAMPLES_PER_CHANNEL);
        APP_ERROR_CHECK(err_code);

        ZB_SCHEDULE_APP_CALLBACK(update_voltage_attributes_callback, 0);
    }
    else
    {
        NRF_LOG_INFO("saadc unhandled event: %d", p_event->type);
    }
}

static void adc_configure(void)
{
    ret_code_t err_code = nrf_drv_saadc_init(NULL, saadc_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_saadc_calibrate_offset();
    APP_ERROR_CHECK(err_code);

    while (nrfx_saadc_is_busy());

    nrf_saadc_channel_config_t config0 = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_VDD);
    config0.acq_time = NRF_SAADC_ACQTIME_40US;
    err_code = nrf_drv_saadc_channel_init(0, &config0);
    APP_ERROR_CHECK(err_code);

    nrf_saadc_channel_config_t config1 = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN4);
    config1.acq_time = NRF_SAADC_ACQTIME_40US;
    err_code = nrf_drv_saadc_channel_init(1, &config1);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(adc_buf, ADC_CHANNELS * ADC_SAMPLES_PER_CHANNEL);
    APP_ERROR_CHECK(err_code);
}

static void battery_level_meas_timeout_handler(void *p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code;

    err_code = nrf_drv_saadc_sample();
    APP_ERROR_CHECK(err_code);
}

void update_internal_temperature_attribute_callback(zb_uint8_t param)
{
    NRF_LOG_INFO("Internal temperature: %d", m_dimmer_control_ctx.device_temperature_attr.current_temperature);

    ZB_ZCL_SET_ATTRIBUTE(DIMMER_CONTROL_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_DEVICE_TEMPERATURE_CURRENT_TEMPERATURE_ID,
                         (zb_uint8_t *)&m_dimmer_control_ctx.device_temperature_attr.current_temperature,
                         ZB_FALSE);
}

static void internal_temperature_meas_timeout_handler(void *p_context)
{
    UNUSED_PARAMETER(p_context);

    NRF_TEMP->TASKS_START = 1;
    /* Busy wait while temperature measurement is not finished. */
    while (NRF_TEMP->EVENTS_DATARDY == 0)
    {
        // Do nothing.
    }
    NRF_TEMP->EVENTS_DATARDY = 0;

    int32_t temp = nrf_temp_read() / 4;

    NRF_TEMP->TASKS_STOP = 1;

    if (temp >= -200 && temp <= 200) {
        m_dimmer_control_ctx.device_temperature_attr.current_temperature = (int16_t)temp;
        ZB_SCHEDULE_APP_CALLBACK(update_internal_temperature_attribute_callback, 0);
    } else {
        m_dimmer_control_ctx.device_temperature_attr.current_temperature = ZB_ZCL_DEVICE_TEMPERATURE_INVALID_VALUE;
    }
}

bulb_device_ctx_t *find_ctx_by_ep_id(zb_uint8_t ep)
{
    if (ep == HA_DIMMABLE_LIGHT_ENDPOINT_R)
        return &m_dev_ctx_r;
    else if (ep == HA_DIMMABLE_LIGHT_ENDPOINT_G)
        return &m_dev_ctx_g;
    else if (ep == HA_DIMMABLE_LIGHT_ENDPOINT_B)
        return &m_dev_ctx_b;
    else if (ep == HA_DIMMABLE_LIGHT_ENDPOINT_W)
        return &m_dev_ctx_w;
    return NULL;
}

static void timer_init(void)
{
    ret_code_t error_code = app_timer_init();
    APP_ERROR_CHECK(error_code);

    // Battery timer
    error_code = app_timer_create(&m_battery_timer_id, APP_TIMER_MODE_REPEATED, battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(error_code);

    // Internal temperature timer
    error_code = app_timer_create(&m_internal_temperature_timer_id, APP_TIMER_MODE_REPEATED, internal_temperature_meas_timeout_handler);
    APP_ERROR_CHECK(error_code);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

// Dimmer control endpoint attributes initialization
static void dimmer_control_attr_init(void)
{
    /* Basic cluster attributes data */
    fill_basic_attrs(&m_dimmer_control_ctx.basic_attr);

    /* OTA cluster attributes data */
    zb_ieee_addr_t addr = ZB_ZCL_OTA_UPGRADE_SERVER_DEF_VALUE;
    ZB_MEMCPY(m_dimmer_control_ctx.ota_attr.upgrade_server, addr, sizeof(zb_ieee_addr_t));
    m_dimmer_control_ctx.ota_attr.file_offset = ZB_ZCL_OTA_UPGRADE_FILE_OFFSET_DEF_VALUE;
    m_dimmer_control_ctx.ota_attr.file_version = s_dfu_settings.app_version;
    m_dimmer_control_ctx.ota_attr.stack_version = ZB_ZCL_OTA_UPGRADE_FILE_HEADER_STACK_PRO;
    m_dimmer_control_ctx.ota_attr.downloaded_file_ver = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_FILE_VERSION_DEF_VALUE;
    m_dimmer_control_ctx.ota_attr.downloaded_stack_ver = ZB_ZCL_OTA_UPGRADE_DOWNLOADED_STACK_DEF_VALUE;
    m_dimmer_control_ctx.ota_attr.image_status = ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_DEF_VALUE;
    m_dimmer_control_ctx.ota_attr.manufacturer = OTA_UPGRADE_TEST_MANUFACTURER;
    m_dimmer_control_ctx.ota_attr.image_type = OTA_UPGRADE_TEST_IMAGE_TYPE;
    m_dimmer_control_ctx.ota_attr.min_block_reque = 0;
    m_dimmer_control_ctx.ota_attr.image_stamp = ZB_ZCL_OTA_UPGRADE_IMAGE_STAMP_MIN_VALUE;

    m_dimmer_control_ctx.device_temperature_attr.current_temperature = ZB_ZCL_DEVICE_TEMPERATURE_INVALID_VALUE;
    m_dimmer_control_ctx.device_temperature_attr.min_temp_experienced = ZB_ZCL_DEVICE_TEMPERATURE_INVALID_VALUE;
    m_dimmer_control_ctx.device_temperature_attr.max_temp_experienced = ZB_ZCL_DEVICE_TEMPERATURE_INVALID_VALUE;
    m_dimmer_control_ctx.device_temperature_attr.over_temp_total_dwell = ZB_ZCL_DEVICE_TEMPERATURE_INVALID_VALUE;

    ZB_ZCL_SET_ATTRIBUTE(DIMMER_CONTROL_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_DEVICE_TEMPERATURE_CURRENT_TEMPERATURE_ID,
                         (zb_uint8_t *)&m_dimmer_control_ctx.device_temperature_attr.current_temperature,
                         ZB_FALSE);

    m_dimmer_control_ctx.electrical_measurement_attr.measurement_type = ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_MEASUREMENT;
    m_dimmer_control_ctx.electrical_measurement_attr.dc_voltage = ZB_ZCL_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_DEFAULT_VALUE;
    m_dimmer_control_ctx.electrical_measurement_attr.dc_voltage_multiplier = ADC_12V_MULTIPLIER;
    m_dimmer_control_ctx.electrical_measurement_attr.dc_voltage_divisor = ADC_12V_DIVISOR;

    ZB_ZCL_SET_ATTRIBUTE(DIMMER_CONTROL_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID,
                         (zb_uint8_t *)&m_dimmer_control_ctx.electrical_measurement_attr.dc_voltage,
                         ZB_FALSE);

    m_dimmer_control_ctx.power_config_attr.mains_frequency = 0;
    m_dimmer_control_ctx.power_config_attr.mains_voltage = 0;

    ZB_ZCL_SET_ATTRIBUTE(DIMMER_CONTROL_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID,
                         (zb_uint8_t *)&m_dimmer_control_ctx.power_config_attr.mains_frequency,
                         ZB_FALSE);

    ZB_ZCL_SET_ATTRIBUTE(DIMMER_CONTROL_ENDPOINT,
                         ZB_ZCL_CLUSTER_ID_POWER_CONFIG,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID,
                         (zb_uint8_t *)&m_dimmer_control_ctx.power_config_attr.mains_voltage,
                         ZB_FALSE);
}

static void reboot_application(zb_uint8_t param)
{
    UNUSED_VARIABLE(param);
    NRF_LOG_FINAL_FLUSH();

#if NRF_MODULE_ENABLED(NRF_LOG_BACKEND_RTT)
    // To allow the buffer to be flushed by the host.
    nrf_delay_ms(100);
#endif

    NVIC_SystemReset();
}

static void pwm_set_brightness(bulb_device_ctx_t *p_dimmable_light_ctx, zb_uint16_t brightness)
{
    uint16_t *p_channels = (uint16_t *)&m_led_values;

    p_channels[p_dimmable_light_ctx->pwm_channel] = DIMMER_PWM_VALUE_MAX - brightness;
}

static void level_control_set_value(bulb_device_ctx_t *p_dimmable_light_ctx, zb_uint16_t new_level)
{
    NRF_LOG_INFO("Set level value: %i for endpoint %hu", new_level, p_dimmable_light_ctx->ep_id);

    ZB_ZCL_SET_ATTRIBUTE(p_dimmable_light_ctx->ep_id,
                         ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID,
                         (zb_uint8_t *)&new_level,
                         ZB_FALSE);

    if (new_level == 0)
    {
        zb_uint8_t value = ZB_FALSE;
        ZB_ZCL_SET_ATTRIBUTE(p_dimmable_light_ctx->ep_id,
                             ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ZB_ZCL_CLUSTER_SERVER_ROLE,
                             ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                             &value,
                             ZB_FALSE);
    }
    else
    {
        zb_uint8_t value = ZB_TRUE;
        ZB_ZCL_SET_ATTRIBUTE(p_dimmable_light_ctx->ep_id,
                             ZB_ZCL_CLUSTER_ID_ON_OFF,
                             ZB_ZCL_CLUSTER_SERVER_ROLE,
                             ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                             &value,
                             ZB_FALSE);
    }

    pwm_set_brightness(p_dimmable_light_ctx, p_dimmable_light_ctx->level_control_attr.current_level);
}

static void on_off_set_value(bulb_device_ctx_t *p_dimmable_light_ctx, zb_bool_t on)
{
    NRF_LOG_INFO("Set ON/OFF value: %i on endpoint: %hu", on, p_dimmable_light_ctx->ep_id);

    ZB_ZCL_SET_ATTRIBUTE(p_dimmable_light_ctx->ep_id,
                         ZB_ZCL_CLUSTER_ID_ON_OFF,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         (zb_uint8_t *)&on,
                         ZB_FALSE);

    if (on)
    {
        level_control_set_value(p_dimmable_light_ctx, p_dimmable_light_ctx->level_control_attr.current_level);
    }
    else
    {
        // FIXME:
        p_dimmable_light_ctx->level_control_attr.current_level = 0;
        level_control_set_value(p_dimmable_light_ctx, p_dimmable_light_ctx->level_control_attr.current_level);
    }
}

static void buttons_handler(bsp_event_t evt)
{
    zb_ret_t zb_err_code;

    switch (evt)
    {
    case IDENTIFY_MODE_BSP_EVT:
        if (m_dev_ctx_w.identify_attr.identify_time == ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE)
        {
            NRF_LOG_INFO("Bulb put in identifying mode");
            zb_err_code = zb_bdb_finding_binding_target(HA_DIMMABLE_LIGHT_ENDPOINT_W);
            ZB_ERROR_CHECK(zb_err_code);
        }
        else
        {
            NRF_LOG_INFO("Cancel F&B target procedure");
            zb_bdb_finding_binding_target_cancel();
        }
        break;

    default:
        NRF_LOG_INFO("Unhandled BSP Event received: %d", evt);
        break;
    }
}

static void board_init(void)
{
    ret_code_t err_code;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, buttons_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_pwm_config_t const led_pwm_config =
        {
            .output_pins =
                {
                    DIMMER_CHANNEL_PIN_R | DIMMER_PWM_INVERSION, // channel 0
                    DIMMER_CHANNEL_PIN_G | DIMMER_PWM_INVERSION, // channel 1
                    DIMMER_CHANNEL_PIN_B | DIMMER_PWM_INVERSION, // channel 2
                    DIMMER_CHANNEL_PIN_W | DIMMER_PWM_INVERSION, // channel 3
                },
            .irq_priority = APP_IRQ_PRIORITY_LOWEST,
            .base_clock = NRF_PWM_CLK_1MHz,
            .count_mode = NRF_PWM_MODE_UP,
            .top_value = DIMMER_PWM_VALUE_MAX,
            .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
            .step_mode = NRF_PWM_STEP_AUTO
        };

    uint16_t *p_channels = (uint16_t *)&m_led_values;

    p_channels[0] = DIMMER_PWM_VALUE_MAX;
    p_channels[1] = DIMMER_PWM_VALUE_MAX;
    p_channels[2] = DIMMER_PWM_VALUE_MAX;
    p_channels[3] = DIMMER_PWM_VALUE_MAX;

    err_code = nrf_drv_pwm_init(&m_led_pwm, &led_pwm_config, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_pwm_simple_playback(&m_led_pwm, &m_led_seq, 1, NRF_DRV_PWM_FLAG_LOOP);
    APP_ERROR_CHECK(err_code);
}

void fill_basic_attrs(zb_zcl_basic_attrs_ext_t *p_basic_attrs_ext)
{
    p_basic_attrs_ext->zcl_version = ZB_ZCL_VERSION;
    p_basic_attrs_ext->app_version = BULB_INIT_BASIC_APP_VERSION;
    p_basic_attrs_ext->stack_version = BULB_INIT_BASIC_STACK_VERSION;
    p_basic_attrs_ext->hw_version = BULB_INIT_BASIC_HW_VERSION;

    ZB_ZCL_SET_STRING_VAL(p_basic_attrs_ext->mf_name,
                          BULB_INIT_BASIC_MANUF_NAME,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MANUF_NAME));
    ZB_ZCL_SET_STRING_VAL(p_basic_attrs_ext->model_id,
                          BULB_INIT_BASIC_MODEL_ID,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MODEL_ID));
    ZB_ZCL_SET_STRING_VAL(p_basic_attrs_ext->date_code,
                          BULB_INIT_BASIC_DATE_CODE,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_DATE_CODE));

    p_basic_attrs_ext->power_source = BULB_INIT_BASIC_POWER_SOURCE;

    ZB_ZCL_SET_STRING_VAL(p_basic_attrs_ext->location_id,
                          BULB_INIT_BASIC_LOCATION_DESC,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_LOCATION_DESC));

    p_basic_attrs_ext->ph_env = BULB_INIT_BASIC_PH_ENV;

    ZB_ZCL_SET_STRING_VAL(p_basic_attrs_ext->sw_ver,
                          BULB_INIT_BASIC_SW_VER,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_SW_VER));
}

static void dimmable_light_clusters_attr_init(bulb_device_ctx_t *p_dimmable_light_ctx)
{
    fill_basic_attrs(&p_dimmable_light_ctx->basic_attr);

    p_dimmable_light_ctx->identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

    if (p_dimmable_light_ctx->ep_id == HA_DIMMABLE_LIGHT_ENDPOINT_W)
      p_dimmable_light_ctx->on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;
    else
      p_dimmable_light_ctx->on_off_attr.on_off = (zb_bool_t)ZB_ZCL_ON_OFF_IS_OFF;

    p_dimmable_light_ctx->on_off_attr.global_scene_ctrl = ZB_TRUE;
    p_dimmable_light_ctx->on_off_attr.on_time = 0;
    p_dimmable_light_ctx->on_off_attr.off_wait_time = 0;

    if (p_dimmable_light_ctx->ep_id == HA_DIMMABLE_LIGHT_ENDPOINT_W)
      p_dimmable_light_ctx->level_control_attr.current_level = ZB_ZCL_LEVEL_CONTROL_LEVEL_MAX_VALUE;
    else
      p_dimmable_light_ctx->level_control_attr.current_level = ZB_ZCL_LEVEL_CONTROL_LEVEL_MIN_VALUE;

    p_dimmable_light_ctx->level_control_attr.remaining_time = ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE;

    ZB_ZCL_SET_ATTRIBUTE(p_dimmable_light_ctx->ep_id,
                         ZB_ZCL_CLUSTER_ID_ON_OFF,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         (zb_uint8_t *)&p_dimmable_light_ctx->on_off_attr.on_off,
                         ZB_FALSE);
    ZB_ZCL_SET_ATTRIBUTE(p_dimmable_light_ctx->ep_id,
                         ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,
                         ZB_ZCL_CLUSTER_SERVER_ROLE,
                         ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID,
                         (zb_uint8_t *)&p_dimmable_light_ctx->level_control_attr.current_level,
                         ZB_FALSE);
}

void zb_dimmable_light_init_ctx(bulb_device_ctx_t *p_dimmable_light_ctx, uint8_t ep_id, uint8_t pwm_channel)
{
    memset(p_dimmable_light_ctx, 0, sizeof(bulb_device_ctx_t));

    p_dimmable_light_ctx->ep_id = ep_id;
    p_dimmable_light_ctx->pwm_channel = pwm_channel;

    dimmable_light_clusters_attr_init(p_dimmable_light_ctx);

    // FIXME: set in dimmable_light_clusters_attr_init
    level_control_set_value(p_dimmable_light_ctx, p_dimmable_light_ctx->level_control_attr.current_level);
}

zb_void_t zb_osif_go_idle(zb_void_t)
{
    zb_osif_wait_for_event();
}

zb_ret_t zb_dimmer_light_set_attribute(bulb_device_ctx_t *p_dimmable_light_ctx, zb_zcl_set_attr_value_param_t *p_savp)
{
    zb_ret_t ret = RET_NOT_IMPLEMENTED;

    if (p_savp->cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
    {
        if (p_savp->attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
        {
            uint8_t value = p_savp->values.data8;
            on_off_set_value(p_dimmable_light_ctx, (zb_bool_t)value);
            ret = RET_OK;
            NRF_LOG_INFO("on/off attribute setting to %hd", value);
        }
    }
    else if (p_savp->cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL)
    {
        if (p_savp->attr_id == ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID)
        {
            uint16_t value = p_savp->values.data16;
            level_control_set_value(p_dimmable_light_ctx, value);
            ret = RET_OK;
            NRF_LOG_INFO("level control attribute setting to %hd", value);
        }
    }
    else
    {
        NRF_LOG_INFO("Unhandled cluster attribute id: %d", p_savp->cluster_id);
    }

    return ret;
}

static zb_void_t zcl_device_cb(zb_bufid_t bufid)
{
    zb_uint8_t cluster_id;
    zb_uint8_t attr_id;
    zb_zcl_device_callback_param_t *p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    NRF_LOG_INFO("Received ZCL callback %hd on endpoint %hu", p_device_cb_param->device_cb_id, p_device_cb_param->endpoint);

    p_device_cb_param->status = RET_OK;

    switch (p_device_cb_param->device_cb_id)
    {
        case ZB_ZCL_LEVEL_CONTROL_SET_VALUE_CB_ID:
        {
            NRF_LOG_INFO("Level control setting to %d for ep %d",
                        p_device_cb_param->cb_param.level_control_set_value_param.new_value,
                        p_device_cb_param->endpoint);
            bulb_device_ctx_t *p_dimmable_light_ctx = find_ctx_by_ep_id(p_device_cb_param->endpoint);
            if (p_dimmable_light_ctx)
            {
                level_control_set_value(p_dimmable_light_ctx, p_device_cb_param->cb_param.level_control_set_value_param.new_value);
            }
            else
            {
                NRF_LOG_WARNING("Context for endpoint %hu not found", p_device_cb_param->endpoint);
                p_device_cb_param->status = RET_ERROR;
            }
            break;
        }

        case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
        {
            bulb_device_ctx_t *p_dimmable_light_ctx = find_ctx_by_ep_id(p_device_cb_param->endpoint);
            if (p_dimmable_light_ctx)
            {
                zb_dimmer_light_set_attribute(p_dimmable_light_ctx, &p_device_cb_param->cb_param.set_attr_value_param);
            }
            else
            {
                NRF_LOG_WARNING("Context for endpoint %hu not found", p_device_cb_param->endpoint);
                p_device_cb_param->status = RET_ERROR;
            }
            break;
        }

        case ZB_ZCL_OTA_UPGRADE_VALUE_CB_ID:
        {
            zb_zcl_ota_upgrade_value_param_t *p_ota_upgrade_value = &(p_device_cb_param->cb_param.ota_value_param);

            switch (p_ota_upgrade_value->upgrade_status)
            {
            case ZB_ZCL_OTA_UPGRADE_STATUS_START:
                /* Check if OTA client is in the middle of image download.
                If so, silently ignore the second QueryNextImageResponse packet from OTA server. */
                if (zb_zcl_ota_upgrade_get_ota_status(p_device_cb_param->endpoint) != ZB_ZCL_OTA_UPGRADE_IMAGE_STATUS_NORMAL)
                {
                    p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_BUSY;
                }

                /* Check if we're not downgrading. If we do, let's politely say no since we do not support that. */
                else if (p_ota_upgrade_value->upgrade.start.file_version > m_dimmer_control_ctx.ota_attr.file_version)
                {
                    p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
                }
                else
                {
                    p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_ABORT;
                }
                break;

            case ZB_ZCL_OTA_UPGRADE_STATUS_RECEIVE:
                /* Process image block. */
                p_ota_upgrade_value->upgrade_status = zb_process_chunk(p_ota_upgrade_value, bufid);
                bsp_board_led_invert(OTA_ACTVITY_LED);
                break;

            case ZB_ZCL_OTA_UPGRADE_STATUS_CHECK:
                p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
                break;

            case ZB_ZCL_OTA_UPGRADE_STATUS_APPLY:
                bsp_board_led_on(OTA_ACTVITY_LED);
                p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_OK;
                break;

            case ZB_ZCL_OTA_UPGRADE_STATUS_FINISH:
                /* It is time to upgrade FW. We use callback so the stack can have time to i.e. send response etc */
                UNUSED_RETURN_VALUE(ZB_SCHEDULE_APP_CALLBACK(reboot_application, 0));
                break;

            case ZB_ZCL_OTA_UPGRADE_STATUS_ABORT:
                NRF_LOG_INFO("Zigbee DFU Aborted");
                p_ota_upgrade_value->upgrade_status = ZB_ZCL_OTA_UPGRADE_STATUS_ABORT;
                bsp_board_led_off(OTA_ACTVITY_LED);
                zb_abort_dfu();
                break;

            case ZB_ZCL_OTA_UPGRADE_STATUS_SERVER_NOT_FOUND:
                NRF_LOG_INFO("OTA Upgrade server not found");
                break;

            default:
                break;
            }
            break;
        }

    default:
        p_device_cb_param->status = RET_ERROR;
        break;
    }

    NRF_LOG_INFO("zcl_device_cb status: %hd", p_device_cb_param->status);
}

void zboss_signal_handler(zb_bufid_t bufid)
{
    zb_zdo_app_signal_hdr_t *p_sg_p = NULL;
    zb_zdo_app_signal_type_t sig = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t status = ZB_GET_APP_SIGNAL_STATUS(bufid);
    zb_bool_t comm_status;

    switch (sig)
    {
    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        NRF_LOG_INFO("ZB_BDB_SIGNAL_DEVICE_FIRST_START: %d", status);
        break;
    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
        NRF_LOG_INFO("ZB_BDB_SIGNAL_DEVICE_REBOOT: %d", status);
        if (status == RET_OK)
        {
            ret_code_t err_code = zb_zcl_ota_upgrade_client_with_periodical_discovery_start(&m_discovery_ctx);
            APP_ERROR_CHECK(err_code);
        }
        break;
    case ZB_COMMON_SIGNAL_CAN_SLEEP:
        NRF_LOG_INFO("ZB_COMMON_SIGNAL_CAN_SLEEP: %d", status);
        break;
    case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
        NRF_LOG_INFO("ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY: %d", status);
        break;
    case ZB_ZDO_SIGNAL_LEAVE:
        NRF_LOG_INFO("ZB_ZDO_SIGNAL_LEAVE: %d", status);
        break;
    case ZB_ZDO_SIGNAL_ERROR:
        NRF_LOG_INFO("ZB_ZDO_SIGNAL_ERROR: %d", status);
        break;
    case ZB_ZDO_SIGNAL_DEFAULT_START:
        NRF_LOG_INFO("ZB_ZDO_SIGNAL_DEFAULT_START: %d", status);
        break;
    case ZB_ZDO_SIGNAL_SKIP_STARTUP:
        NRF_LOG_INFO("ZB_ZDO_SIGNAL_SKIP_STARTUP: %d", status);
        break;
    case ZB_ZDO_SIGNAL_DEVICE_ANNCE:
        NRF_LOG_INFO("ZB_ZDO_SIGNAL_DEVICE_ANNCE: %d", status);
        break;
    case ZB_BDB_SIGNAL_STEERING:
        NRF_LOG_INFO("ZB_BDB_SIGNAL_STEERING: %d", status);
        if (status == RET_OK)
        {
            ret_code_t err_code = zb_zcl_ota_upgrade_client_with_periodical_discovery_start(&m_discovery_ctx);
            APP_ERROR_CHECK(err_code);
        }
        break;
    case ZB_BDB_SIGNAL_FORMATION:
        NRF_LOG_INFO("ZB_BDB_SIGNAL_FORMATION: %d", status);
        break;
    case ZB_BDB_SIGNAL_FINDING_AND_BINDING_TARGET_FINISHED:
        NRF_LOG_INFO("ZB_BDB_SIGNAL_FINDING_AND_BINDING_TARGET_FINISHED: %d", status);
        break;
    case ZB_BDB_SIGNAL_FINDING_AND_BINDING_INITIATOR_FINISHED:
        NRF_LOG_INFO("ZB_BDB_SIGNAL_FINDING_AND_BINDING_INITIATOR_FINISHED: %d", status);
        break;
    case ZB_ZGP_SIGNAL_COMMISSIONING:
        NRF_LOG_INFO("ZB_ZGP_SIGNAL_COMMISSIONING: %d", status);
        break;
    case ZB_NWK_SIGNAL_NO_ACTIVE_LINKS_LEFT:
        NRF_LOG_INFO("ZB_NWK_SIGNAL_NO_ACTIVE_LINKS_LEFT: %d", status);
        break;
    case ZB_ZDO_SIGNAL_DEVICE_AUTHORIZED:
        NRF_LOG_INFO("ZB_ZDO_SIGNAL_DEVICE_AUTHORIZED: %d", status);
        break;
    case ZB_NWK_SIGNAL_DEVICE_ASSOCIATED:
        NRF_LOG_INFO("ZB_NWK_SIGNAL_DEVICE_ASSOCIATED: %d", status);
        break;
    case ZB_ZDO_SIGNAL_LEAVE_INDICATION:
        NRF_LOG_INFO("ZB_ZDO_SIGNAL_LEAVE_INDICATION: %d", status);
        break;
    case ZB_BDB_SIGNAL_WWAH_REJOIN_STARTED:
        NRF_LOG_INFO("ZB_BDB_SIGNAL_WWAH_REJOIN_STARTED: %d", status);
        break;
    case ZB_ZDO_SIGNAL_DEVICE_UPDATE:
        NRF_LOG_INFO("ZB_ZDO_SIGNAL_DEVICE_UPDATE: %d", status);
        break;
    case ZB_NWK_SIGNAL_PANID_CONFLICT_DETECTED:
        NRF_LOG_INFO("ZB_NWK_SIGNAL_PANID_CONFLICT_DETECTED: %d", status);
        break;
    case ZB_NLME_STATUS_INDICATION:
        NRF_LOG_INFO("ZB_NLME_STATUS_INDICATION: %d", status);
        if (status == RET_OK) {
            zb_zdo_signal_nlme_status_indication_params_t *p_nlme_status_params =
                ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_nlme_status_indication_params_t);

            NRF_LOG_INFO("NLME status: %d, nwk addr: 0x%04hx, unknown command id: %d",
                         p_nlme_status_params->nlme_status.status,
                         p_nlme_status_params->nlme_status.network_addr,
                         p_nlme_status_params->nlme_status.unknown_command_id);
        }
        break;
    default:
        NRF_LOG_INFO("Unhandled signal %d. Status: %d", sig, status);
        break;
    }

    zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);

    ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
    if (bufid)
        zb_buf_free(bufid);
}

zb_bool_t check_erase_persistent_storage()
{
    nrf_gpio_cfg_input(ERASE_PERSISTENT_CONFIG_PIN, NRF_GPIO_PIN_PULLUP);

    volatile uint32_t pin_state;

    for (int i = 0; i < 10; i++)
        pin_state = nrf_gpio_pin_read(ERASE_PERSISTENT_CONFIG_PIN);

    nrf_gpio_cfg_default(ERASE_PERSISTENT_CONFIG_PIN);

    return (pin_state == 0) ? ZB_TRUE : ZB_FALSE;
}

zb_uint8_t zcl_ep_handler(zb_bufid_t bufid)
{
    zb_zcl_parsed_hdr_t *p_cmd_info = ZB_BUF_GET_PARAM(bufid, zb_zcl_parsed_hdr_t);
    NRF_LOG_INFO("EP ZCL Handler1: src: 0x%04hx/%d, dst: 0x%04hx/%d",
                 p_cmd_info->addr_data.common_data.source.u.short_addr,
                 p_cmd_info->addr_data.common_data.src_endpoint,
                 p_cmd_info->addr_data.common_data.dst_addr,
                 p_cmd_info->addr_data.common_data.dst_endpoint
    );
    NRF_LOG_INFO("EP ZCL Handler2: cmd: %d, dir: %d, cluster: %d, seq: %d",
                 p_cmd_info->cmd_id,
                 p_cmd_info->cmd_direction,
                 p_cmd_info->cluster_id,
                 p_cmd_info->seq_number
    );
    return ZB_FALSE;
}

int main(void)
{
    zb_ret_t zb_err_code;
    zb_ieee_addr_t ieee_addr;
    zb_bool_t erase_persistent_storage;
    ret_code_t err_code;

    erase_persistent_storage = ERASE_PERSISTENT_CONFIG;
    if (erase_persistent_storage != ZB_TRUE)
        erase_persistent_storage = check_erase_persistent_storage();

    timer_init();
    log_init();
    board_init();

    adc_configure();

    ZB_SET_TRACE_LEVEL(ZIGBEE_TRACE_LEVEL);
    ZB_SET_TRACE_MASK(ZIGBEE_TRACE_MASK);
    ZB_SET_TRAF_DUMP_OFF();

    zb_dfu_init(DIMMER_CONTROL_ENDPOINT);

    ZB_INIT("dimmer_rgbw");

    zb_osif_get_ieee_eui64(ieee_addr);
    zb_set_long_address(ieee_addr);

    zb_set_network_router_role(IEEE_CHANNEL_MASK);
    zb_set_max_children(MAX_CHILDREN);
    zigbee_erase_persistent_storage(erase_persistent_storage);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));

    ZB_AF_REGISTER_DEVICE_CTX(&dimmable_light_ctx);

    ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

    init_multi_ep_fixers();

    // FIXME: commented out due to init_multi_ep_fixers() call
//    ZB_AF_SET_ENDPOINT_HANDLER(HA_DIMMABLE_LIGHT_ENDPOINT_R, zcl_ep_handler);
//    ZB_AF_SET_ENDPOINT_HANDLER(HA_DIMMABLE_LIGHT_ENDPOINT_G, zcl_ep_handler);
//    ZB_AF_SET_ENDPOINT_HANDLER(HA_DIMMABLE_LIGHT_ENDPOINT_B, zcl_ep_handler);
//    ZB_AF_SET_ENDPOINT_HANDLER(HA_DIMMABLE_LIGHT_ENDPOINT_W, zcl_ep_handler);
//    ZB_AF_SET_ENDPOINT_HANDLER(DIMMER_CONTROL_ENDPOINT, zcl_ep_handler);

    dimmer_control_attr_init();

    zb_dimmable_light_init_ctx(&m_dev_ctx_r, HA_DIMMABLE_LIGHT_ENDPOINT_R, 0);
    zb_dimmable_light_init_ctx(&m_dev_ctx_g, HA_DIMMABLE_LIGHT_ENDPOINT_G, 1);
    zb_dimmable_light_init_ctx(&m_dev_ctx_b, HA_DIMMABLE_LIGHT_ENDPOINT_B, 2);
    zb_dimmable_light_init_ctx(&m_dev_ctx_w, HA_DIMMABLE_LIGHT_ENDPOINT_W, 3);

    nrf_802154_tx_power_set(8);

    nrf_temp_init();

    zb_err_code = zb_zcl_ota_upgrade_client_with_periodical_discovery_init(&m_discovery_ctx, &m_ota_server_discovery_timer, DIMMER_CONTROL_ENDPOINT);
    APP_ERROR_CHECK(zb_err_code);

    zb_err_code = zboss_start_no_autostart();
    ZB_ERROR_CHECK(zb_err_code);

    err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL / ADC_SAMPLES_PER_CHANNEL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_internal_temperature_timer_id, INTERNAL_TEMPERATURE_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    while (1)
    {
        zboss_main_loop_iteration();
        app_sched_execute();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    }
}