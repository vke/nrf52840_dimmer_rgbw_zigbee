#if ! defined ZB_ZCL_POWER_CONFIG_H_
#define ZB_ZCL_POWER_CONFIG_H_

#include "zcl/zb_zcl_common.h"
//#include "zcl/zb_zcl_power_config.h"

enum zb_zcl_power_config_attr_e
{
  /** @brief MainsVoltage attribute, ZCL spec 3.3.2.2.1.1 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_ID                 = 0x0000,
  /** @brief MainsFrequency attribute, ZCL spec 3.3.2.2.1.2 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID               = 0x0001,
  /** @brief MainsAlarmMask attribute, ZCL spec 3.3.2.2.2.1 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_ALARM_MASK_ID              = 0x0010,
  /** @brief MainsVoltageMinThreshold attribute, ZCL spec 3.3.2.2.2.2 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MIN_THRESHOLD      = 0x0011,
  /** @brief MainsVoltageMaxThreshold attribute, ZCL spec 3.3.2.2.2.3 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_VOLTAGE_MAX_THRESHOLD      = 0x0012,
  /** @brief MainsVoltageDwellTripPoint attribute, ZCL spec 3.3.2.2.2.4 */
  ZB_ZCL_ATTR_POWER_CONFIG_MAINS_DWELL_TRIP_POINT           = 0x0013,

  /** @brief BatteryVoltage attribute, ZCL spec 3.3.2.2.3.1 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_ID               = 0x0020,
  /*! @brief BatteryPercentageRemaining attribute, ZCL spec 3.3.2.2.3.2 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_REMAINING_ID  = 0x0021,

  /** The BatteryManufacturer attribute is a maximum of 16 bytes in length
   *  and specifies the name of the battery manufacturer as a character string. */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_MANUFACTURER_ID          = 0x0030,
  /** @brief BatterySize attribute, ZCL spec 3.3.2.2.4.2 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_SIZE_ID                  = 0x0031,
  /** The BatteryAHrRating attribute is 16 bits in length and specifies
   *  the Ampere-hour rating of the battery, measured in units of 10mAHr. */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_A_HR_RATING_ID           = 0x0032,
  /** @brief BatteryQuantity attribute, ZCL spec 3.3.2.2.4.4 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_QUANTITY_ID              = 0x0033,
  /** @brief BatteryRatedVoltage attribute, ZCL spec 3.3.2.2.4.5 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_RATED_VOLTAGE_ID         = 0x0034,
  /** @brief BatteryAlarmMask attribute, ZCL spec 3.3.2.2.4.6 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_MASK_ID            = 0x0035,
  /** @brief BatteryVoltageMinThreshold attribute, ZCL spec 3.3.2.2.4.7 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_MIN_THRESHOLD_ID = 0x0036,
    /*! @brief BatteryVoltageThreshold1 attribute, ZCL spec 3.3.2.2.4.8 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD1_ID        = 0x0037,
  /*! @brief BatteryVoltageThreshold2 attribute, ZCL spec 3.3.2.2.4.8 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD2_ID        = 0x0038,
  /*! @brief BatteryVoltageThreshold3 attribute, ZCL spec 3.3.2.2.4.8 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_VOLTAGE_THRESHOLD3_ID        = 0x0039,

  /*! @brief BatteryPercentageMinThreshold attribute, ZCL spec 3.3.2.2.4.9 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_MIN_THRESHOLD_ID  = 0x003a,

  /*! @brief BatteryPercentageThreshold1 attribute, ZCL spec 3.3.2.2.4.10 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD1_ID     = 0x003b,
  /*! @brief BatteryPercentageThreshold2 attribute, ZCL spec 3.3.2.2.4.10 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD2_ID     = 0x003c,
  /*! @brief BatteryPercentageThreshold3 attribute, ZCL spec 3.3.2.2.4.10 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_PERCENTAGE_THRESHOLD3_ID     = 0x003d,

  /*! @brief BatteryAlarmState attribute, ZCL spec 3.3.2.2.4.11 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY_ALARM_STATE_ID               = 0x003e,

  /*! @brief Battery Information 2 attribute set, ZCL spec 3.3.2.2.5 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_ID = 0x0040,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_REMAINING_ID     = 0x0041,

  /*! @brief Battery Settings 2 attribute set, ZCL spec 3.3.2.2.6 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_MANUFACTURER_ID          = 0x0050,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_SIZE_ID = 0x0051,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_A_HR_RATING_ID           = 0x0052,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_QUANTITY_ID = 0x0053,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_RATED_VOLTAGE_ID = 0x0054,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_ALARM_MASK_ID = 0x0055,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_MIN_THRESHOLD_ID = 0x0056,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_THRESHOLD1_ID        = 0x0057,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_THRESHOLD2_ID        = 0x0058,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_VOLTAGE_THRESHOLD3_ID        = 0x0059,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_MIN_THRESHOLD_ID  = 0x005a,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_THRESHOLD1_ID     = 0x005b,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_THRESHOLD2_ID     = 0x005c,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_PERCENTAGE_THRESHOLD3_ID     = 0x005d,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY2_ALARM_STATE_ID               = 0x005e,

  /*! @brief Battery Information 3 attribute set, ZCL spec 3.3.2.2.7 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_ID = 0x0060,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_REMAINING_ID  = 0x0061,

  /*! @brief Battery Settings 3 attribute set, ZCL spec 3.3.2.2.8 */
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_MANUFACTURER_ID          = 0x0070,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_SIZE_ID = 0x0071,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_A_HR_RATING_ID           = 0x0072,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_QUANTITY_ID = 0x0073,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_RATED_VOLTAGE_ID = 0x0074,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_ALARM_MASK_ID = 0x0075,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_MIN_THRESHOLD_ID = 0x0076,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_THRESHOLD1_ID        = 0x0077,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_THRESHOLD2_ID        = 0x0078,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_VOLTAGE_THRESHOLD3_ID        = 0x0079,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_MIN_THRESHOLD_ID  = 0x007a,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_THRESHOLD1_ID     = 0x007b,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_THRESHOLD2_ID     = 0x007c,
  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_PERCENTAGE_THRESHOLD3_ID     = 0x007d,

  ZB_ZCL_ATTR_POWER_CONFIG_BATTERY3_ALARM_STATE_ID               = 0x007e,
  /** @cond internals_doc */
  /* Custom attributes */
  ZB_ZCL_ATTR_POWER_CONFIG_CUSTOM_BATTERY_VOLTAGE_SHARP_ID  = 0x8020
  /*! @}
   *  @endcond */ /* internals_doc */
};


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
            (void *)data_ptr                                                                \
    }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID(data_ptr)            \
    {                                                                                           \
        ZB_ZCL_ATTR_POWER_CONFIG_MAINS_FREQUENCY_ID,                                            \
            ZB_ZCL_ATTR_TYPE_U8,                                                                \
            ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                        \
            (void *)data_ptr                                                                \
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
