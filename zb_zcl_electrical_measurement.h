#if ! defined ZB_ZCL_ELECTRICAL_MEASUREMENT_H_
#define ZB_ZCL_ELECTRICAL_MEASUREMENT_H_

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_el_measurement.h"


#define ZB_ZCL_DECLARE_ELECTRICAL_MEASUREMENT_ATTRIB_LIST(attr_list,                                            \
                            measurement_type,                                                                   \
                            dc_voltage,                                                                         \
                            dc_voltage_multiplier,                                                              \
                            dc_voltage_divisor)                                                                 \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID, (measurement_type))            \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_ID, (dc_voltage))                        \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MULTIPLIER_ID, (dc_voltage_multiplier))  \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_DIVISOR_ID, (dc_voltage_divisor))        \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID(data_ptr) \
    {                                                                                           \
        ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID,                                 \
            ZB_ZCL_ATTR_TYPE_32BITMAP,                                                          \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                       \
            (zb_voidp_t)data_ptr                                                                \
    }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_ID(data_ptr)       \
    {                                                                                           \
        ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_ID,                                       \
            ZB_ZCL_ATTR_TYPE_S16,                                                               \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                        \
            (zb_voidp_t)data_ptr                                                                \
    }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MULTIPLIER_ID(data_ptr) \
    {                                                                                                \
        ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_MULTIPLIER_ID,                                 \
            ZB_ZCL_ATTR_TYPE_U16,                                                                    \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                             \
            (zb_voidp_t)data_ptr                                                                     \
    }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_DIVISOR_ID(data_ptr) \
    {                                                                                             \
        ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_DC_VOLTAGE_DIVISOR_ID,                                 \
            ZB_ZCL_ATTR_TYPE_U16,                                                                 \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                          \
            (zb_voidp_t)data_ptr                                                                  \
    }

#define ZB_ZCL_DEVICE_TEMPERATURE_REPORT_ATTR_COUNT 0

typedef struct zb_zcl_electrical_measurement_attrs_s
{
    zb_uint32_t measurement_type;
    zb_int16_t dc_voltage;
    zb_uint16_t dc_voltage_multiplier;
    zb_uint16_t dc_voltage_divisor;
} zb_zcl_electrical_measurement_attrs_t;

#define ZB_ZCL_DECLARE_ELECTRICAL_MEASUREMENT_ATTR_LIST(attr_list, attrs)          \
    ZB_ZCL_DECLARE_ELECTRICAL_MEASUREMENT_ATTRIB_LIST(attr_list,                   \
                                                  &attrs.measurement_type,         \
                                                  &attrs.dc_voltage,               \
                                                  &attrs.dc_voltage_multiplier,    \
                                                  &attrs.dc_voltage_divisor)

#define ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT_SERVER_ROLE_INIT ((zb_zcl_cluster_init_t)NULL)
#define ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT_CLIENT_ROLE_INIT ((zb_zcl_cluster_init_t)NULL)

#endif /* defined ZB_ZCL_ELECTRICAL_MEASUREMENT_H_ */
