#include "esphome.h"
#include <string>
#include "crc16.h"
#include "protocol_values.h"

#define LOGGER "mmWave"
#define DATA_OFFSET 4

typedef union {
  float f;
  uint8_t buff[4];
} byte_2_float;


class HumanPresenceRadar : public Component, public UARTDevice {
  public:
  HumanPresenceRadar(UARTComponent *parent) : UARTDevice(parent) {}
  Sensor *body_movement = new Sensor();
  TextSensor *heart_beat_sensor = new TextSensor();
  TextSensor *approching_away_sensor = new TextSensor();
  TextSensor *device_id = new TextSensor();
  TextSensor *sw_version = new TextSensor();
  TextSensor *hw_version = new TextSensor();
  TextSensor *protocol_version = new TextSensor();
  TextSensor *scene_settings = new TextSensor();
  uint8_t buffer[64];
  size_t msg_len;

  float get_setup_priority() const override {
    return esphome::setup_priority::AFTER_WIFI;
  }

  void setup() override {
    // set_update_interval(500);
  }

  String bytes_2_string(uint8_t *buff, size_t size){
    return reinterpret_cast<char*>(buff);
  }
  
  void send_command(uint8_t *buff, size_t data_size){
    size_t total_size = 5+data_size; // 8 bytes are necessary
    uint8_t cmd_buff[total_size];

    if(cmd_buff == nullptr) {
      ESP_LOGE(LOGGER, "Can't allocate memory for sending the command");
      return;
    }

    cmd_buff[0] = 0x55;
    cmd_buff[1] = 4 + data_size;
    cmd_buff[2] = 0;
    for (size_t i = 0; i < data_size; i++){
      cmd_buff[3+i] = buff[i];
    }
    crc_t crc = us_CalculateCrc16(cmd_buff, 3 + data_size);
    cmd_buff[total_size-2] = crc.crc_lo;
    cmd_buff[total_size-1] = crc.crc_hi;

    ESP_LOGI(LOGGER, "Sending data to the radar");
    for (size_t i = 0; i < total_size; i++){
      ESP_LOGI(LOGGER, "0x%02x", cmd_buff[i]);
    }
    bytes_2_string(cmd_buff, total_size);
    write_array(cmd_buff, total_size);
    ESP_LOGI(LOGGER, "Data is sent");
  }

  void heart_beat(){
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
        ESP_LOGW(LOGGER, "Unknow heart beat status: 0x02x", buffer[5]);
        break;
    }
  }

  void other_info(){
    switch (buffer[3]){
      case HEART_BEAT:
        heart_beat();
        break;
      case ABNORMAL_RESET:
        ESP_LOGE(LOGGER, "Abnormal reset of the radar: 0x%02x", buffer[4]);
        break;
      default:
        ESP_LOGW(LOGGER, "Unrecognize option of other radar information: 0x%02x", buffer[3]);
        break;
    }
  }

  void motor_signs(){
    // ESP_LOGD(LOGGER, "Motor signs: 0x%02x 0x%02x 0x%02x 0x%02x",
    //           buffer[4], buffer[5], buffer[6], buffer[7]);
    byte_2_float flt;
    extract_data(4, flt.buff);
    body_movement->publish_state(flt.f);
  }

  void approching_away_state() {
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
        ESP_LOGW(LOGGER, "Unrecognized status in approaching away state: 0x%02x", buffer[6]);
        break;
    }
  }

  void report_radar_info(){
    switch (buffer[3]){ // Function address 2
      case ENV_STATUS:
        ESP_LOGD(LOGGER, "Env statu code: 0x%02x", buffer[5]);
        target_present->publish_state(buffer[5] != NONE);
        break;
      case APPROACHING_AWAY_STATE:
        approching_away_state();
        break;
      case MOTOR_SIGNS:
        motor_signs();
        break;
      default:
        ESP_LOGW(LOGGER, "Unknow function address 2: 0x%02x", buffer[3]);
        break;
    }
  }

  void proactive_report(){
    switch (buffer[2]){ //. Function address 1
          case REPORT_MODULE_INFO:
            process_module_info();
          case REPORT_RADAR_INFO: 
            report_radar_info();
            break;
          case OTHER_INFO:
            other_info();
            break;
          default:
            ESP_LOGW(LOGGER, "Unknow function addres 1: 0x%02x", buffer[2]);
            break;
        }
  }

  void extract_data(size_t data_size, uint8_t*output){
    for (size_t i = 0; i < data_size; i++){
      output[i] = buffer[DATA_OFFSET + i];
    }
  }

  void process_info(size_t data_size, const char* msg, auto* sen){
    uint8_t data[data_size];
    extract_data(data_size, data);
    ESP_LOGD(LOGGER, "Received HEX data:");
    for (size_t i = 0; i < data_size; i++){
      ESP_LOGD(LOGGER, "0x%02x", data[i]);
    }
    String value = bytes_2_string(data, data_size);
    ESP_LOGD(LOGGER, "%s: %s", msg, value.c_str());
    if (sen != nullptr){
      sen->publish_state(value.c_str());
    }
  }

  void process_module_info(){
    ESP_LOGD(LOGGER, "Processing radar module information");
    size_t data_size;
    switch (buffer[3]) {
      case DEVICE_ID:
        process_info(12, "Device ID", device_id);
        break;
      case SW_VERSION:
        process_info(10, "Software version", sw_version);
        break;
      case HW_VERSION:
        data_size = 8;
        if (DATA_OFFSET + data_size > msg_len-2){
            ESP_LOGE(LOGGER, 
            "Not enough data in the received message. "
            "Message length is %n, expected data size", msg_len, data_size);
            data_size = msg_len - DATA_OFFSET - 2;
            ESP_LOGD(LOGGER, "Tryin to addjust data size to %n", data_size);
          }
        process_info(8, "Hardware version", hw_version);
        break;
      case PROTOCOL_VERSION:
        data_size = 8;
        ESP_LOGD(LOGGER, "Data received with protocol info");
        for (size_t i = 0; i < msg_len; i++){
          ESP_LOGD(LOGGER, "0x%02x", buffer[i]);
        }
        if (DATA_OFFSET + data_size > msg_len-2){
          ESP_LOGE(LOGGER, 
          "Not enough data in the received message. "
          "Message length is %n, expected data size", msg_len, data_size);
          data_size = msg_len - DATA_OFFSET - 2;
          ESP_LOGD(LOGGER, "Tryin to addjust data size to %n", data_size);
        }
        process_info(data_size, "Protocol version", protocol_version);
        break;
      default:
        ESP_LOGE(LOGGER, "Unknow second function code for module information: 0x%02x", buffer[3]);
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

  void process_sys_info(){
    ESP_LOGD(LOGGER, "Reporting system information");
    switch (buffer[3]){
      case TRASHOLD_GEAR:
        ESP_LOGD(LOGGER, "Current trashold gear: %d", buffer[4]);
        break;
      case SCENE_SETTINGS:
        process_scene_settings(buffer[4]);
        break;
      default:
        ESP_LOGE(LOGGER, "Unknow code for system information: 0x%02x", buffer[3]);
        break;
    } 
  }

  void passive_report(){
    switch (buffer[2]){
      case REPORT_MODULE_INFO:
        process_module_info();
        break;
      case REPORT_RADAR_INFO:
        switch (buffer[3]){ // Function address 2
          case ENV_STATUS:
            ESP_LOGD(LOGGER, "Env statu code: 0x%02x", buffer[5]);
            target_present->publish_state(buffer[5] != NONE);
            break;
          case MOTOR_SIGNS:
            motor_signs();
            break;
          default:
            ESP_LOGD(LOGGER, "Unknow function address 2: 0x%02x", buffer[3]);
            break;
        }
        break;
      case REPORT_SYS_INFO:
        process_sys_info();
        break;
      default:
        ESP_LOGE(LOGGER, "Unknow function code for passive report: 0x%02x", buffer[2]);
        break;
    }
  }
  
  // =================== PARSING MAIN FUNCTION ===================
  void state_judjment(size_t length) {
    msg_len = length - 1;
    read_array(buffer, msg_len); // Result array would contain data from length low byte to CRC high value including it
    if (buffer[0] != 0){
      ESP_LOGE(LOGGER, "Length low byte is not 0: 0x%02x", buffer[0]);
    }
    switch (buffer[1]){ // Main command
      case PROACTIVE_REPORT:
        proactive_report();
        break;
      case PASSIVE_REPORT:
        passive_report();
        break;
      default:
        ESP_LOGW(LOGGER, "Unknow command: 0x%02x", buffer[1]);
        break;
    }
  }
  // =================== COMMANDS ===================
  void reboot(){
    uint8_t cmd[3] = {0x02, 0x05, 0x04};
    send_command(cmd, 3);
  }

  void get_radar_device_id(){
    uint8_t cmd_list[3] = {READ_CMD, MARKING_SEARCH, DEVICE_ID};
    send_command(cmd_list, 3);
  }

  void get_radar_sw_version(){
    uint8_t cmd_list[3] = {READ_CMD, MARKING_SEARCH, SW_VERSION};
    send_command(cmd_list, 3);
  }

  // This shlt doesn't work
  // void get_radar_protocol_version(){
  //   uint8_t cmd_list[3] = {READ_CMD, MARKING_SEARCH, PROTOCOL_VERSION};
  //   send_command(cmd_list, 3);
  // }

  // void get_radar_hw_version(){
  //   uint8_t cmd_list[3] = {READ_CMD, MARKING_SEARCH, HW_VERSION};
  //   send_command(cmd_list, 3);
  // }


  // =================== MAIN LOOP ===================
  void loop() override {
    if (available()) {

      if (read() == MESSAGE_HEAD) { state_judjment((size_t)read()); }
    }
  }
};
