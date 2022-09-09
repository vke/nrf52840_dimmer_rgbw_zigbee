#if ! defined ZB_ZCL_DEVICE_TEMPERATURE_H
#define ZB_ZCL_DEVICE_TEMPERATURE_H

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

enum zb_zcl_device_temperature_attr_e
{
    ZB_ZCL_ATTR_DEVICE_TEMPERATURE_CURRENT_TEMPERATURE_ID = 0x0000,
    ZB_ZCL_ATTR_DEVICE_TEMPERATURE_MIN_TEMP_EXPERIENCED_ID = 0x0001,
    ZB_ZCL_ATTR_DEVICE_TEMPERATURE_MAX_TEMP_EXPERIENCED_ID = 0x0002,
    ZB_ZCL_ATTR_DEVICE_TEMPERATURE_OVER_TEMP_TOTAL_DWELL_ID = 0x0003
};

#define ZB_ZCL_DEVICE_TEMPERATURE_INVALID_VALUE (0xffff)
#define ZB_ZCL_DEVICE_TEMPERATURE_DEFAULT_TEMPERATURE_VALUE ZB_ZCL_DEVICE_TEMPERATURE_INVALID_VALUE
#define ZB_ZCL_DEVICE_TEMPERATURE_MIN_TEMPERATURE_VALUE (-200)
#define ZB_ZCL_DEVICE_TEMPERATURE_MAX_TEMPERATURE_VALUE (200)

#define ZB_ZCL_DECLARE_DEVICE_TEMPERATURE_ATTRIB_LIST(attr_list, current_temperature,                                    \
                                                      min_temp_experienced, max_temp_experienced, over_temp_total_dwell) \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                                          \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMPERATURE_CURRENT_TEMPERATURE_ID, (current_temperature))                   \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMPERATURE_MIN_TEMP_EXPERIENCED_ID, (min_temp_experienced))                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMPERATURE_MAX_TEMP_EXPERIENCED_ID, (max_temp_experienced))                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMPERATURE_OVER_TEMP_TOTAL_DWELL_ID, (over_temp_total_dwell))               \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMPERATURE_CURRENT_TEMPERATURE_ID(data_ptr) \
    {                                                                                          \
        ZB_ZCL_ATTR_DEVICE_TEMPERATURE_CURRENT_TEMPERATURE_ID,                                 \
            ZB_ZCL_ATTR_TYPE_S16,                                                              \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                       \
            (void *)data_ptr                                                               \
    }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMPERATURE_MIN_TEMP_EXPERIENCED_ID(data_ptr) \
    {                                                                                           \
        ZB_ZCL_ATTR_DEVICE_TEMPERATURE_MIN_TEMP_EXPERIENCED_ID,                                 \
            ZB_ZCL_ATTR_TYPE_S16,                                                               \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                        \
            (void *)data_ptr                                                                \
    }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMPERATURE_MAX_TEMP_EXPERIENCED_ID(data_ptr) \
    {                                                                                           \
        ZB_ZCL_ATTR_DEVICE_TEMPERATURE_MAX_TEMP_EXPERIENCED_ID,                                 \
            ZB_ZCL_ATTR_TYPE_S16,                                                               \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                        \
            (void *)data_ptr                                                                \
    }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMPERATURE_OVER_TEMP_TOTAL_DWELL_ID(data_ptr) \
    {                                                                                            \
        ZB_ZCL_ATTR_DEVICE_TEMPERATURE_OVER_TEMP_TOTAL_DWELL_ID,                                 \
            ZB_ZCL_ATTR_TYPE_U16,                                                                \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                         \
            (void *)data_ptr                                                                 \
    }

typedef struct zb_zcl_device_temperature_attrs_s
{
    zb_int16_t current_temperature;
    zb_int16_t min_temp_experienced;
    zb_int16_t max_temp_experienced;
    zb_uint16_t over_temp_total_dwell;

} zb_zcl_device_temperature_attrs_t;

#define ZB_ZCL_DECLARE_DEVICE_TEMPERATURE_ATTR_LIST(attr_list, attrs)          \
    ZB_ZCL_DECLARE_DEVICE_TEMPERATURE_ATTRIB_LIST(attr_list,                   \
                                                  &attrs.current_temperature,  \
                                                  &attrs.min_temp_experienced, \
                                                  &attrs.max_temp_experienced, \
                                                  &attrs.over_temp_total_dwell)

void zb_zcl_device_temperature_init_server(void);
void zb_zcl_device_temperature_init_client(void);
#define ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG_SERVER_ROLE_INIT zb_zcl_device_temperature_init_server
#define ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG_CLIENT_ROLE_INIT zb_zcl_device_temperature_init_client

#endif /* defined ZB_ZCL_DEVICE_TEMPERATURE_H */
