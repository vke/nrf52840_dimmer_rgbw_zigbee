#if ! defined ZB_ZCL_POWER_CONFIG_H_
#define ZB_ZCL_POWER_CONFIG_H_

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_power_config.h"

// 0x0000 MainsVoltage uint16
// 0x0001 MainsFrequency uint8

#define ZB_ZCL_DECLARE_POWER_CONFIG_ATTRIB_LIST(attr_list,                                      \
                            mains_voltage,                                                      \
                            mains_frequency)                                                    \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID, (mains_voltage))            \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID, (mains_frequency))        \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID(data_ptr)              \
    {                                                                                           \
        ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID,                                              \
            ZB_ZCL_ATTR_TYPE_U16,                                                               \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                        \
            (zb_voidp_t)data_ptr                                                                \
    }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID(data_ptr)            \
    {                                                                                           \
        ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID,                                            \
            ZB_ZCL_ATTR_TYPE_U8,                                                                \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                        \
            (zb_voidp_t)data_ptr                                                                \
    }

typedef struct zb_zcl_power_config_attrs_s
{
    zb_uint16_t mains_voltage;
    zb_uint8_t  mains_frequency;
} zb_zcl_power_config_attrs_t;

#define ZB_ZCL_DECLARE_POWER_CONFIG_ATTR_LIST(attr_list, attrs)           \
    ZB_ZCL_DECLARE_POWER_CONFIG_ATTRIB_LIST(attr_list,                    \
                                            &attrs.mains_voltage,         \
                                            &attrs.mains_frequency)

#define ZB_ZCL_CLUSTER_ID_POWER_CONFIG_SERVER_ROLE_INIT ((zb_zcl_cluster_init_t)NULL)
#define ZB_ZCL_CLUSTER_ID_POWER_CONFIG_CLIENT_ROLE_INIT ((zb_zcl_cluster_init_t)NULL)

#endif /* defined ZB_ZCL_POWER_CONFIG_H_ */
