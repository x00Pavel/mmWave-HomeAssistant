#include "esphome.h"
// #include <string>

#define MESSAGE_HEAD 0x55
#define LOGGER "mmWave"

// Command code
#define PROACTIVE_REPORT 0x04
#define PASSIVE_REPORT 0x03
#define COPY_ORDER 0x02
#define READ_CMD 0x01

// Function address 1
#define REPORT_RADAR_INFO 0x03
#define REPORT_MODULE_INFO 0x01
#define REPORT_SYS_INFO 0x04
#define OTHER_INFO 0x05
#define REPORT_RADAR_ID 0x01
#define RADAR_SYS_INFO 0x04
#define MARKING_SEARCH 0x01

// Function address 2
#define ENV_STATUS 0x05
#define APPROACHING_AWAY_STATE 0x07
#define MOTOR_SIGNS 0x06
#define HEART_BEAT 0x01
#define DEVICE_ID 0x01
#define SW_VERSION 0x02
#define HW_VERSION 0x03
#define PROTOCOL_VERSION 0x04
#define SCENE 0x10
#define ABNORMAL_RESET 0x02
#define SCENE_SETTINGS 0x10
#define TRASHOLD_GEAR 0x0c

// Values environment status
#define NONE 0xff
#define STATIONAR 0x00
#define MOVING 0x01

// Values approaching away
#define NO_MOVEMENT 0x01
#define CLOSE 0x02
#define MOVING_AWAY 0x03

// Value scene settings
#define SCENE_DEFAULT 0x00
#define SCENE_AREA_DETECTION_TOP_LOADING 0x01
#define SCENE_BATHROOM_TOP_MOUNTED 0x02
#define SCENE_BEADROOM_TOP_LOADING 0x03
#define SCENE_LIVING_ROOM_TOP_MOUNTED 0x04
#define SCENE_OFFICE_TOP_LOADING 0x05
#define SCENE_HOTEL_TOP_LOADING 0x06

#define IGNORE 0x99

typedef union {
  float f;
  uint8_t buff[4];
} byte_2_float;


const unsigned char cuc_CRCHi[256]= {
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40
};

const unsigned char  cuc_CRCLo[256]= {
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
  0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
  0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
  0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
  0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
  0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
  0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
  0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
  0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
  0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
  0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
  0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
  0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
  0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
  0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
  0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
  0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
  0x41, 0x81, 0x80, 0x40
};

unsigned short int us_CalculateCrc16(unsigned char *lpuc_Frame, size_t lus_Len){
  unsigned char luc_CRCHi = 0xFF;
  unsigned char luc_CRCLo = 0xFF;
  int li_Index=0;
  while(lus_Len--){
    li_Index = luc_CRCLo ^ *( lpuc_Frame++);
    luc_CRCLo = (unsigned char)( luc_CRCHi ^ cuc_CRCHi[li_Index]);
    luc_CRCHi = cuc_CRCLo[li_Index];
  }
  return (unsigned short int )(luc_CRCLo << 8 | luc_CRCHi);
}


class HumanPresenceRadar : public Component, public UARTDevice {
  public:
  // constructor
  HumanPresenceRadar(UARTComponent *parent) : UARTDevice(parent) {}
  BinarySensor *target_present = new BinarySensor();
  Sensor *body_movement = new Sensor();
  TextSensor *heart_beat_sensor = new TextSensor();
  TextSensor *approching_away_sensor = new TextSensor();
  TextSensor *device_id = new TextSensor();
  TextSensor *sw_version = new TextSensor();
  TextSensor *hw_version = new TextSensor();
  TextSensor *protocol_version = new TextSensor();
  TextSensor *scene_settings = new TextSensor();
  uint8_t buffer[64];

  float get_setup_priority() const override {
    return esphome::setup_priority::AFTER_WIFI;
  }

  void setup() override {
    // set_update_interval(500);
  }

  String bytes_2_string(uint8_t *buff){
    return reinterpret_cast<char*>(buff);
  }
  
  void send_command(uint8_t *buff, size_t data_size){
    size_t total_size = 8+data_size; // 8 bytes are necessary
    uint8_t cmd_buff[total_size];

    if(cmd_buff == nullptr) {
      ESP_LOGE(LOGGER, "Can't allocate memory for sending the command");
      return;
    }

    cmd_buff[0] = 0x55;
    cmd_buff[1] = 7 + data_size;
    cmd_buff[2] = 0;
    for (size_t i = 0; i < data_size; i++){
      cmd_buff[3+i] = buff[i];
    }
    unsigned short int crc = us_CalculateCrc16(cmd_buff, 3 + data_size);
    cmd_buff[total_size - 1] = (crc & 0xff00) >> 8;
    cmd_buff[total_size] = crc & 0xff;
    ESP_LOGI(LOGGER, "Sending data to the radar: %s", bytes_2_string(cmd_buff));
    write_array(cmd_buff, total_size);
  }

  void heart_beat(uint8_t *buffer){
    ESP_LOGD(LOGGER, "Heart veat statu code: 0x%02x", buffer[5]);
    switch (buffer[5]){
      case 0xff:
        heart_beat_sensor->publish_state("Unoccupied");
        break;
      case 0x00:
        heart_beat_sensor->publish_state("Someone is stationar");
        break;
      case 0x01:
        heart_beat_sensor->publish_state("Someone is exercise");
        break;
      default:
        ESP_LOGD(LOGGER, "Unknow heart beat status: 0x02x", buffer[5]);
        break;
    }
  }

  void other_info(uint8_t *buffer){
    switch (buffer[3]){
      case HEART_BEAT:
        heart_beat(buffer);
        break;
      case ABNORMAL_RESET:
        ESP_LOGE(LOGGER, "Abnormal reset of the radar: 0x%02x", buffer[4]);
        break;
      default:
        ESP_LOGD(LOGGER, "Unrecognize option of other radar information: 0x%02x", buffer[3]);
        break;
    }
  }

  void motor_signs(uint8_t *buffer){
    ESP_LOGD(LOGGER, "Motor signs: 0x%02x 0x%02x 0x%02x 0x%02x",
              buffer[4], buffer[5], buffer[6], buffer[7]);
    byte_2_float flt;
    flt.buff[0] = buffer[4];
    flt.buff[1] = buffer[5];
    flt.buff[2] = buffer[6];
    flt.buff[3] = buffer[7];
    body_movement->publish_state(flt.f);
  }

  void approching_away_state(uint8_t *buffer) {
    ESP_LOGD(LOGGER, "Approaching away state: 0x%02x", buffer[6]);
    switch (buffer[6]){
      case NO_MOVEMENT:
        approching_away_sensor->publish_state("no movemenet");
        break;
      case CLOSE:
        approching_away_sensor->publish_state("someone is closing");
        break;
      case MOVING_AWAY:
        approching_away_sensor->publish_state("someone going away");
        break;
      default:
        ESP_LOGD(LOGGER, "Unrecognized status in approaching away state: 0x%02x", buffer[6]);
        break;
    }
  }

  void report_radar_info(uint8_t *buffer){
    switch (buffer[3]){ // Function address 2
      case ENV_STATUS:
        ESP_LOGD(LOGGER, "Env statu code: 0x%02x", buffer[5]);
        target_present->publish_state(buffer[5] != NONE);
        break;
      case APPROACHING_AWAY_STATE:
        approching_away_state(buffer);
        break;
      case MOTOR_SIGNS:
        motor_signs(buffer);
        break;
      default:
        ESP_LOGD(LOGGER, "Unknow function address 2: 0x%02x", buffer[3]);
        break;
    }
  }

  void proactive_report(uint8_t *buffer){
    switch (buffer[2]){ //. Function address 1
          case REPORT_MODULE_INFO:
            process_module_info(buffer);
          case REPORT_RADAR_INFO: 
            report_radar_info(buffer);
            break;
          case OTHER_INFO:
            other_info(buffer);
            break;
          default:
            ESP_LOGD(LOGGER, "Unknow function addres 1: 0x%02x", buffer[2]);
            break;
        }
  }
  String extract_buff_part(uint8_t *buff, uint8_t start, uint8_t end){
    size_t size = end - start;
    uint8_t bytes[size];
    for (size_t i = 0; i < size; i++){
      bytes[i] = buff[start + i];
    }
    return bytes_2_string(bytes);
  }

  void process_module_info(uint8_t *buff){
    String value;
    switch (buff[3]) {
      case DEVICE_ID:
        value = extract_buff_part(buff, 4, 16);
        ESP_LOGD(LOGGER, "Device ID: %s", value);
        device_id->publish_state(value.c_str());
        break;
      case SW_VERSION:
        value = extract_buff_part(buff, 4, 14);
        ESP_LOGI(LOGGER, "Software version: %s", value);
        sw_version->publish_state(value.c_str());
        break;
      case HW_VERSION:
        value = extract_buff_part(buff, 4, 12);
        ESP_LOGI(LOGGER, "Hardare version: %s", value);
        hw_version->publish_state(value.c_str());
        break;
      case PROTOCOL_VERSION:
        value = extract_buff_part(buff, 4, 12);
        ESP_LOGI(LOGGER, "Protocol version: %s", value);
        protocol_version->publish_state(value.c_str());
        break;
      default:
        ESP_LOGE(LOGGER, "Unknow second function code for module information: 0x%02x", buff[3]);
        break;
    }
  }

  void process_scene_settings(uint8_t scene){
    const char* state = "Unknown scene";
    switch (scene){
      case SCENE_DEFAULT:
        state = "Default scene";
        break;
      case SCENE_AREA_DETECTION_TOP_LOADING:
        state = "Area detection top loading";
        break;
      case SCENE_BATHROOM_TOP_MOUNTED:
        state = "Bathroom top mounted";
        break;
      case SCENE_BEADROOM_TOP_LOADING:
        state = "Beadroom top loading";
        break;
      case SCENE_HOTEL_TOP_LOADING:
        state = "Hotel top loading";
        break;
      case SCENE_LIVING_ROOM_TOP_MOUNTED:
        state = "Living room top mounted";
        break;
      case SCENE_OFFICE_TOP_LOADING:
        state = "Office top loading";
        break;
      default:
        ESP_LOGE(LOGGER, "Unknow scene: 0x%02x", scene);
        break;
    }
    scene_settings->publish_state(state);
    ESP_LOGD(LOGGER, "Scene settings: %s", state);
  }

  void process_sys_info(uint8_t *buff){
    ESP_LOGD(LOGGER, "Reporting system information");
    switch (buffer[3]){
      case TRASHOLD_GEAR:
        ESP_LOGD(LOGGER, "Current trashold gear: %d", buff[4]);
        break;
      case SCENE_SETTINGS:
        process_scene_settings(buff[4]);
        break;
      default:
        ESP_LOGE(LOGGER, "Unknow code for system informatio: 0x%02x", buff[3]);
        break;
    } 
  }

  void passive_report(uint8_t *buffer){
    switch (buffer[2]){
      case REPORT_MODULE_INFO:
        process_module_info(buffer);
        break;
      case REPORT_RADAR_INFO:
        switch (buffer[3]){ // Function address 2
          case ENV_STATUS:
            ESP_LOGD(LOGGER, "Env statu code: 0x%02x", buffer[5]);
            target_present->publish_state(buffer[5] != NONE);
            break;
          case MOTOR_SIGNS:
            motor_signs(buffer);
            break;
          default:
            ESP_LOGD(LOGGER, "Unknow function address 2: 0x%02x", buffer[3]);
            break;
        }
        break;
      case REPORT_SYS_INFO:
        process_sys_info(buffer);
        break;
      default:
        ESP_LOGE(LOGGER, "Unknow function code for passive report: 0x%02x", buffer[2]);
        break;
    }
  }
  
  // =================== PARSING MAIN FUNCTION ===================
  void state_judjment(size_t length) {
    read_array(buffer, length-1);
    switch (buffer[1]){ // Main command
      case PROACTIVE_REPORT:
        proactive_report(buffer);
        break;
      case PASSIVE_REPORT:
        passive_report(buffer);
        break;
      default:
        ESP_LOGD(LOGGER, "Unknow command: 0x%02x", buffer[1]);
        break;
    }
  }
  // =================== COMMANDS ===================
  void reboot(){
    uint8_t cmd[3] = {0x02, 0x05, 0x04};
    send_command(cmd, 3);
  }

  void get_radar_info(){
    for (uint8_t cmd: {DEVICE_ID, SW_VERSION, HW_VERSION, PROTOCOL_VERSION}){
      uint8_t cmd_list[3] = {READ_CMD, MARKING_SEARCH, cmd};
      send_command(cmd_list, 3);
    }
  }

  // =================== MAIN LOOP ===================
  void loop() override {
    if (available()) {
      if (read() == MESSAGE_HEAD) { state_judjment((size_t)read()); }
    }
  }
};
