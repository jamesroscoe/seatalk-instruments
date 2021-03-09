#include "../seatalk/logger.h"
#include "timeout.h"
#include "../seatalk_settings.h"
#include "boat_sensor.h"
#include "../seatalk/seatalk_protocol.h"

int sensors_initialized = 0;

#define SENSOR_VARIABLE(NAME) sensor_ ## NAME
#define SENSOR_EXPIRY_TIME(NAME) sensor_##NAME##_expiry
#define SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME) sensor_##NAME##_transmitted
#define VALID_SENSOR(NAME) (sensors_initialized && timeout_still_valid(SENSOR_EXPIRY_TIME(NAME)))
#define SENSOR_VALUE_UNTRANSMITTED(NAME) !SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME)
#define SET_SENSOR_EXPIRY(NAME, VALUE) SENSOR_EXPIRY_TIME(NAME) = VALUE
#define VALIDATE_SENSOR(NAME) restart_timeout(&SENSOR_EXPIRY_TIME(NAME), SENSOR_TIME_TO_LIVE)
#define INVALIDATE_SENSOR(NAME) invalidate_timeout(&SENSOR_EXPIRY_TIME(NAME))
#define UNTRANSMITTED_SENSOR_VALUE(NAME) SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME) = 0
#define TRANSMITTED_SENSOR_VALUE(NAME) SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME) = 1
#define DEFINE_SENSOR_EXPIRY(NAME) timeout SENSOR_EXPIRY_TIME(NAME) = TIMEOUT_DEFAULT
#define DEFINE_SENSOR_VALUE_TRANSMITTED(NAME) int SENSOR_VALUE_TRANSMITTED_VARIABLE(NAME) = 1

#define DEFINE_RAW_SENSOR(NAME, TYPE) TYPE SENSOR_VARIABLE(NAME)

#define DEFINE_READ_ONLY_SENSOR(NAME, TYPE) DEFINE_RAW_SENSOR(NAME, TYPE);\
  DEFINE_SENSOR_EXPIRY(NAME);\
  DEFINE_SENSOR_VALUE_TRANSMITTED(NAME);\
int has_##NAME##_sensor_value_to_transmit(void) {\
  return (VALID_SENSOR(NAME) && SENSOR_VALUE_UNTRANSMITTED(NAME));\
}\
\
int pop_##NAME##_sensor_value(TYPE *value) {\
  if (has_##NAME##_sensor_value_to_transmit()) {\
    *value = SENSOR_VARIABLE(NAME);\
    TRANSMITTED_SENSOR_VALUE(NAME);\
    return 0;\
  }\
  return 1;\
}\
void invalidate_##NAME##_sensor() {\
  INVALIDATE_SENSOR(NAME);\
}

#define DEFINE_SENSOR(NAME, TYPE) DEFINE_READ_ONLY_SENSOR(NAME, TYPE)\
void update_##NAME##_sensor(TYPE NAME) {\
  SENSOR_VARIABLE(NAME) = NAME;\
  VALIDATE_SENSOR(NAME);\
  UNTRANSMITTED_SENSOR_VALUE(NAME);\
  wake_transmitter();\
}

DEFINE_SENSOR(heading, int);
DEFINE_SENSOR(water_speed_in_knots_times_100, int);
DEFINE_SENSOR(apparent_wind_angle, int);
DEFINE_SENSOR(apparent_wind_speed_in_knots_times_10, int);
DEFINE_SENSOR(depth_below_transducer_in_feet_times_10, int);
DEFINE_SENSOR(course_over_ground, int);
DEFINE_SENSOR(speed_over_ground_in_knots_times_100, int);
DEFINE_SENSOR(water_temperature_in_degrees_celsius_times_10, int);
DEFINE_SENSOR(rudder_position_in_degrees_right, int);

void initialize_sensors(void) {
  sensors_initialized = 1;
  INVALIDATE_SENSOR(heading);
  INVALIDATE_SENSOR(water_speed_in_knots_times_100);
  INVALIDATE_SENSOR(apparent_wind_angle);
  INVALIDATE_SENSOR(apparent_wind_speed_in_knots_times_10);
  INVALIDATE_SENSOR(depth_below_transducer_in_feet_times_10);
  INVALIDATE_SENSOR(course_over_ground);
  INVALIDATE_SENSOR(speed_over_ground_in_knots_times_100);
  INVALIDATE_SENSOR(water_temperature_in_degrees_celsius_times_10);
  INVALIDATE_SENSOR(rudder_position_in_degrees_right);
}

// microbit mods
// #include "logger.h"
// #include "timeout.h"
// #include "settings.h"
// #include "boat_sensor.h"
// #include "seatalk_protocol.h"
// #include "../seatalk/seatalk_hardware_layer.h"

// #define IF_SENSOR_PENDING(SEATALK_PORT, NAME, ID_NAME) if (has_##NAME##_sensor_value_to_transmit(seatalk_port)) {\
//   *sensor_id = SENSOR_ID_##ID_NAME;\
// }

// int seatalk_sensor_pending(int seatalk_port, SENSOR_ID *sensor_id) {
//   // need to pass everything through else so need an impossible condition to start
//   if (1 == 2) {
//   }
// #ifdef SENSE_HEADING
//   else IF_SENSOR_PENDING(seatalk_port, heading, HEADING)
// #endif
// #ifdef SENSE_WATER_SPEED
//   else IF_SENSOR_PENDING(seatalk_port, water_speed_in_knots_times_100, WATER_SPEED)
// #endif
// #ifdef SENSE_APPARENT_WIND_ANGLE
//   else IF_SENSOR_PENDING(seatalk_port, apparent_wind_angle, APPARENT_WIND_ANGLE)
// #endif
// #ifdef SENSE_APPARENT_WIND_SPEED
//   else IF_SENSOR_PENDING(seatalk_port, apparent_wind_speed_in_knots_times_10, APPARENT_WIND_SPEED)
// #endif
// #ifdef SENSE_DEPTH
//   else IF_SENSOR_PENDING(seatalk_port, depth_below_transducer_in_feet_times_10, DEPTH_BELOW_TRANSDUCER)
// #endif
// #ifdef SENSE_COURSE_OVER_GROUND
//   else IF_SENSOR_PENDING(seatalk_port, course_over_ground, COURSE_OVER_GROUND)
// #endif
// #ifdef SENSE_SPEED_OVER_GROUND
//   else IF_SENSOR_PENDING(seatalk_port, speed_over_ground_in_knots_times_100, SPEED_OVER_GROUND)
// #endif
// #ifdef SENSE_WATER_TEMPERATURE
//   else IF_SENSOR_PENDING(seatalk_port, water_temperature_in_degrees_celsius_times_10, WATER_TEMPERATURE)
// #endif
// #ifdef SENSE_RUDDER_POSITION
//   else IF_SENSOR_PENDING(seatalk_port, rudder_position_in_degrees_right, RUDDER_POSITION)
// #endif
//   else {
//     *sensor_id = SENSOR_ID_NONE;
//   }
//   return *sensor_id != SENSOR_ID_NONE;
// }

// int sensors_initialized = 0;

// void initialize_sensors(void) {
//   sensors_initialized = 1;
// }

