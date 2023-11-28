#pragma once
#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"

#include <map>

namespace esphome {
namespace mr24hpc1 {

#define FRAME_BUF_MAX_SIZE 128
#define PRODUCT_BUF_MAX_SIZE 32

#define FRAME_HEADER1_VALUE 0x53
#define FRAME_HEADER2_VALUE 0x59
#define FRAME_TAIL1_VALUE 0x54
#define FRAME_TAIL2_VALUE 0x43

#define FRAME_CONTROL_WORD_INDEX 2
#define FRAME_COMMAND_WORD_INDEX 3
#define FRAME_DATA_INDEX 6

enum
{
    FRAME_IDLE,
    FRAME_HEADER2,
    FRAME_CTL_WORLD,
    FRAME_CMD_WORLD,
    FRAME_DATA_LEN_H,
    FRAME_DATA_LEN_L,
    FRAME_DATA_BYTES,
    FRAME_DATA_CRC,
    FRAME_TAIL1,
    FRAME_TAIL2,
};

enum
{
    STANDARD_FUNCTION_QUERY_PRODUCT_MODE = 0,
    STANDARD_FUNCTION_QUERY_PRODUCT_ID,
    STANDARD_FUNCTION_QUERY_FIRMWARE_VERDION,
    STANDARD_FUNCTION_QUERY_HARDWARE_MODE,
    STANDARD_FUNCTION_QUERY_HEARTBEAT_STATE,
    STANDARD_FUNCTION_QUERY_HUMAN_STATUS,
    STANDARD_FUNCTION_QUERY_KEEPAWAY_STATUS,
    STANDARD_FUNCTION_QUERY_SCENE_MODE,
    STANDARD_FUNCTION_QUERY_SENSITIVITY,
    STANDARD_FUNCTION_QUERY_UNMANNED_TIME,
    // STANDARD_FUNCTION_QUERY_MOV_TARGET_DETECTION_MAX_DISTANCE,
    // STANDARD_FUNCTION_QUERY_STATIC_TARGET_DETECTION_MAX_DISTANCE,
    STANDARD_FUNCTION_QUERY_RADAR_OUTPUT_INFORMATION_SWITCH,

    CUSTOM_FUNCTION_QUERY_RADAR_OUTPUT_INFORMATION_SWITCH,
    CUSTOM_FUNCTION_QUERY_PRESENCE_OF_DETECTION_RANGE,
    CUSTOM_FUNCTION_QUERY_JUDGMENT_THRESHOLD_EXISTS,
    CUSTOM_FUNCTION_QUERY_MOTION_AMPLITUDE_TRIGGER_THRESHOLD,
    CUSTOM_FUNCTION_QUERY_PRESENCE_OF_PERCEPTION_BOUNDARY,
    CUSTOM_FUNCTION_QUERY_MOTION_TRIGGER_BOUNDARY,
    CUSTOM_FUNCTION_QUERY_MOTION_TRIGGER_TIME,
    CUSTOM_FUNCTION_QUERY_MOVEMENT_TO_REST_TIME,
    CUSTOM_FUNCTION_QUERY_TIME_OF_ENTER_UNMANNED,
    CUSTOM_FUNCTION_MAX,
};

enum
{
    OUTPUT_SWITCH_INIT,
    OUTPUT_SWTICH_ON,
    OUTPUT_SWTICH_OFF,
};

static const std::map<std::string, uint8_t> SCENEMODE_ENUM_TO_INT{
  {"None", 0x00},
  {"Living Room", 0x01},
  {"Bedroom", 0x02},
  {"Washroom", 0x03},
  {"Area Detection", 0x04}
};

static const std::map<std::string, uint8_t> UNMANDTIME_ENUM_TO_INT{
  {"None", 0x00},
  {"10s", 0x01},
  {"30s", 0x02},
  {"1min", 0x03},
  {"2min", 0x04},
  {"5min", 0x05},
  {"10min", 0x06},
  {"30min", 0x07},
  {"60min", 0x08}
};

static const std::map<std::string, uint8_t> BOUNDARY_ENUM_TO_INT{
  {"0.5m", 0x01},
  {"1.0m", 0x02},
  {"1.5m", 0x03},
  {"2.0m", 0x04},
  {"2.5m", 0x05},
  {"3.0m", 0x06},
  {"3.5m", 0x07},
  {"4.0m", 0x08},
  {"4.5m", 0x09},
  {"5.0m", 0x0a},
};

static const char* s_heartbeat_str[2] = {"Abnormal", "Normal"};
static const char* s_scene_str[5] = {"None", "Living Room", "Bedroom", "Washroom", "Area Detection"};
static bool s_someoneExists_str[2] = {false, true};
static const char* s_motion_status_str[3] = {"None", "Motionless", "Active"};
static const char* s_keep_away_str[3] = {"None", "Close", "Away"};
static const char* s_unmanned_time_str[9] = {"None", "10s", "30s", "1min", "2min", "5min", "10min", "30min", "60min"};
static const char* s_boundary_str[10] = {"0.5m", "1.0m", "1.5m", "2.0m", "2.5m", "3.0m", "3.5m", "4.0m", "4.5m", "5.0m"}; // uint: m
static float s_presence_of_detection_range_str[7] = {0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0};  // uint: m

static uint8_t s_output_info_switch_flag = OUTPUT_SWITCH_INIT;

static uint8_t sg_recv_data_state = FRAME_IDLE;
static uint8_t sg_frame_len = 0;
static uint8_t sg_data_len = 0;
static uint8_t sg_frame_buf[FRAME_BUF_MAX_SIZE] = {0};
static uint8_t sg_frame_prase_buf[FRAME_BUF_MAX_SIZE] = {0};
static bool sg_init_flag = false;
static uint32_t sg_motion_trigger_time_bak;
static uint32_t sg_move_to_rest_time_bak;
static uint32_t sg_enter_unmanned_time_bak;
static uint8_t sg_heartbeat_flag = 255;
static uint8_t s_power_on_status = 0;

class mr24hpc1Component : public PollingComponent, public uart::UARTDevice {      // The class name must be the name defined by text_sensor.py
#ifdef USE_TEXT_SENSOR
  SUB_TEXT_SENSOR(heartbeat_state)
  SUB_TEXT_SENSOR(product_model)
  SUB_TEXT_SENSOR(product_id)
  SUB_TEXT_SENSOR(hardware_model)
  SUB_TEXT_SENSOR(firware_version)
  SUB_TEXT_SENSOR(keep_away)
  SUB_TEXT_SENSOR(motion_status)
#endif
#ifdef USE_BINARY_SENSOR
  SUB_BINARY_SENSOR(someoneExists)
#endif
#ifdef USE_SENSOR
  SUB_SENSOR(custom_presence_of_detection)
  SUB_SENSOR(movementSigns)
  SUB_SENSOR(custom_motion_distance)
  SUB_SENSOR(custom_spatial_static_value)
  SUB_SENSOR(custom_spatial_motion_value)
  SUB_SENSOR(custom_motion_speed)
#endif
#ifdef USE_SWITCH
  SUB_SWITCH(underly_open_function)
#endif
#ifdef USE_BUTTON
  SUB_BUTTON(reset)
  SUB_BUTTON(custom_set_end)
#endif
#ifdef USE_SELECT
  SUB_SELECT(scene_mode)
  SUB_SELECT(unman_time)
  SUB_SELECT(existence_boundary)
  SUB_SELECT(motion_boundary)
#endif
#ifdef USE_NUMBER
  SUB_NUMBER(sensitivity)
  SUB_NUMBER(custom_mode)
#endif

  private:
    char c_product_mode[PRODUCT_BUF_MAX_SIZE + 1];
    char c_product_id[PRODUCT_BUF_MAX_SIZE + 1];
    char c_hardware_model[PRODUCT_BUF_MAX_SIZE + 1];
    char c_firmware_version[PRODUCT_BUF_MAX_SIZE + 1];
  public:
    mr24hpc1Component() : PollingComponent(8000) {}
    float get_setup_priority() const override { return esphome::setup_priority::LATE; }
    void setup() override;
    void update() override;
    void dump_config() override;
    void loop() override;
    void R24_split_data_frame(uint8_t value);
    void R24_parse_data_frame(uint8_t *data, uint8_t len);
    void R24_frame_parse_open_underlying_information(uint8_t *data);
    void R24_frame_parse_work_status(uint8_t *data);
    void R24_frame_parse_product_Information(uint8_t *data);
    void R24_frame_parse_human_information(uint8_t *data);
    void send_query(uint8_t *query, size_t string_length);
    void get_heartbeat_packet(void);
    void get_radar_output_information_switch(void);
    void get_product_mode(void);
    void get_product_id(void);
    void get_hardware_model(void);
    void get_firmware_version(void);
    void get_human_status(void);
    void get_keep_away(void);
    void get_scene_mode(void);
    void get_sensitivity(void);
    void get_unmanned_time(void);
    void get_custom_mode(void);
    void get_existence_boundary(void);
    void get_motion_boundary(void);
    void set_scene_mode(const std::string &state);
    void set_underlying_open_function(bool enable);
    void set_sensitivity(uint8_t value);
    void set_unman_time(const std::string &time);
    void set_custom_mode(uint8_t mode);
    void set_custom_end_mode(void);
    void set_existence_boundary(const std::string &value);
    void set_motion_boundary(const std::string &value);
};

}  // namespace mr24hpc1
}  // namespace esphome