#define MESSAGE_HEAD 0x55
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

// Values approachin away
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