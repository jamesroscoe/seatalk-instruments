#include "../seatalk/logger.h"
#include "timeout.h"
#include "../seatalk/seatalk_protocol.h"
#include "boat_status.h"
#include "../seatalk_settings.h"

int status_initialized = 0;

#define STATUS_VARIABLE(NAME) status_ ## NAME
#define STATUS_EXPIRY_TIME(NAME) status_##NAME##_expiry
#define VALID_STATUS(NAME) (status_initialized && timeout_still_valid(STATUS_EXPIRY_TIME(NAME)))
#define SET_STATUS_EXPIRY(NAME, VALUE) STATUS_EXPIRY_TIME(NAME) = VALUE
#define VALIDATE_STATUS(NAME) restart_timeout(&STATUS_EXPIRY_TIME(NAME), STATUS_TIME_TO_LIVE)
#define INVALIDATE_STATUS(NAME) invalidate_timeout(&STATUS_EXPIRY_TIME(NAME))
#define DEFINE_STATUS_EXPIRY(NAME) timeout STATUS_EXPIRY_TIME(NAME) = TIMEOUT_DEFAULT

#define DEFINE_RAW_STATUS(NAME, TYPE) TYPE STATUS_VARIABLE(NAME);\

#define DEFINE_STATUS(NAME, TYPE) DEFINE_RAW_STATUS(NAME, TYPE)\
  DEFINE_STATUS_EXPIRY(NAME);\
int get_##NAME(TYPE *NAME) {\
  if (VALID_STATUS(NAME)) {\
    *NAME = STATUS_VARIABLE(NAME);\
    return 0;\
  }\
  return 1;\
}

#define SET_STATUS(NAME, SOURCE) STATUS_VARIABLE(NAME) = SOURCE;\
//  VALIDATE_STATUS(NAME)

// status properties are designed to be read from the SeaTalk bus but not written out
DEFINE_STATUS(active_alarms, int)

void st_received_wind_alarm(int active_alarms) {
  STATUS_VARIABLE(active_alarms) |= active_alarms;
  VALIDATE_STATUS(active_alarms);
}

void st_received_alarm_acknowledgement(int alarms) {
  STATUS_VARIABLE(active_alarms) &= ~alarms;
  VALIDATE_STATUS(active_alarms);
}

DEFINE_STATUS(depth_below_transducer_in_feet_times_10, int)

void st_received_depth_below_transducer(int feet_times_10, int display_units, int active_alarms, int transducer_defective) {
  SET_STATUS(depth_below_transducer_in_feet_times_10, feet_times_10)
  // update active alarms here
}

//DEFINE_STATUS(distance_units, DISTANCE_UNITS)
DEFINE_STATUS(engine_single, ENGINE_STATUS)
DEFINE_STATUS(engine_port, ENGINE_STATUS)
DEFINE_STATUS(engine_starboard, ENGINE_STATUS)

int get_engine_status(ENGINE_ID engine_id, ENGINE_STATUS *engine_status) {
  switch (engine_id) {
    case ENGINE_ID_SINGLE:
      return get_engine_single(engine_status);
      break;
    case ENGINE_ID_PORT:
      return get_engine_port(engine_status);
      break;
    case ENGINE_ID_STARBOARD:
      return get_engine_starboard(engine_status);
      break;
  }
  return -1;
}

void st_received_engine_rpm_and_pitch(ENGINE_ID engine_id, int rpm, int pitch_percent) {
  switch (engine_id) {
    case ENGINE_ID_SINGLE:
      STATUS_VARIABLE(engine_single).rpm = rpm;
      STATUS_VARIABLE(engine_single).pitch_percent = pitch_percent;
      break;
    case ENGINE_ID_PORT:
      STATUS_VARIABLE(engine_port).rpm = rpm;
      STATUS_VARIABLE(engine_port).pitch_percent = pitch_percent;
      break;
    case ENGINE_ID_STARBOARD:
      STATUS_VARIABLE(engine_starboard).rpm = rpm;
      STATUS_VARIABLE(engine_starboard).pitch_percent = pitch_percent;
      break;
  }
}

DEFINE_STATUS(apparent_wind_angle, int)

void st_received_apparent_wind_angle(int degrees_right_times_2) {
  SET_STATUS(apparent_wind_angle, degrees_right_times_2 / 2);
}

DEFINE_STATUS(apparent_wind_speed_in_knots_times_10, int)

void st_received_apparent_wind_speed(int knots_times_10, int display_in_metric) {
  SET_STATUS(apparent_wind_speed_in_knots_times_10, knots_times_10);
}

DEFINE_STATUS(heading, int)

void st_received_heading(int heading, int locked_heading_active, int locked_heading) {
  SET_STATUS(heading, heading);
}

DEFINE_STATUS(heading_reference, ANGLE_REFERENCE)

// no SeaTalk datagram expected for this

DEFINE_STATUS(water_speed_in_knots_times_100, int)

void st_received_water_speed(int knots_times_10) {
  SET_STATUS(water_speed_in_knots_times_100, knots_times_10 * 10);
}

DEFINE_STATUS(average_water_speed_in_knots_times_100, int)

void st_received_average_water_speed(int knots_1_times_100, int knots_2_times_100, int speed_1_from_sensor, int speed_2_is_average, int average_is_stopped, int display_in_statute_miles) {
  if (speed_1_from_sensor) {
    SET_STATUS(water_speed_in_knots_times_100, knots_1_times_100);
  }
  if (speed_2_is_average) {
    SET_STATUS(average_water_speed_in_knots_times_100, knots_2_times_100);
  }
}

DEFINE_STATUS(turn_direction, TURN_DIRECTION)
DEFINE_STATUS(rudder_position_in_degrees_right, int)

void st_received_heading_and_rudder_position(int heading, TURN_DIRECTION turn_direction, int rudder_position) {
  SET_STATUS(heading, heading);
  SET_STATUS(turn_direction, turn_direction);
  SET_STATUS(rudder_position_in_degrees_right, rudder_position);
}

DEFINE_STATUS(course_over_ground, int)

void st_received_course_over_ground(int true_degrees) {
  SET_STATUS(course_over_ground, true_degrees);
}

DEFINE_STATUS(course_over_ground_reference, ANGLE_REFERENCE)

// no SeaTalk datagram expected for this

DEFINE_STATUS(speed_over_ground_in_knots_times_100, int)

void st_received_speed_over_ground(int knots_times_10) {
  SET_STATUS(speed_over_ground_in_knots_times_100, knots_times_10 * 10);
}

DEFINE_STATUS(trip_mileage_in_nautical_miles_times_100, int)

void st_received_trip_mileage(int nautical_miles_times_100) {
  SET_STATUS(trip_mileage_in_nautical_miles_times_100, nautical_miles_times_100);
}

DEFINE_STATUS(total_mileage_in_nautical_miles_times_10, int)

void st_received_total_mileage(int nautical_miles_times_10) {
  SET_STATUS(total_mileage_in_nautical_miles_times_10, nautical_miles_times_10);
}

void st_received_total_and_trip_mileage(int total_mileage_in_nautical_miles_times_10, int trip_mileage_in_nautical_miles_times_100) {
  SET_STATUS(total_mileage_in_nautical_miles_times_10, total_mileage_in_nautical_miles_times_10);
  SET_STATUS(trip_mileage_in_nautical_miles_times_100, trip_mileage_in_nautical_miles_times_100);
}

DEFINE_STATUS(water_temperature_in_degrees_celsius_times_10, int)

void st_received_water_temperature(int degrees_fahrenheit, int transducer_defective) {
  SET_STATUS(water_temperature_in_degrees_celsius_times_10, ((degrees_fahrenheit - 32) * 5) / 9);
}

void st_received_precise_water_temperature(int degrees_celsius_times_10) {
  SET_STATUS(water_temperature_in_degrees_celsius_times_10, degrees_celsius_times_10);
}

DEFINE_STATUS(speed_distance_units, DISTANCE_UNITS)

void st_received_speed_distance_units(DISTANCE_UNITS distance_units) {
  SET_STATUS(speed_distance_units, distance_units);
}

DEFINE_STATUS(lamp_intensity, int)

void st_received_set_lamp_intensity(int intensity) {
  SET_STATUS(lamp_intensity, intensity);
}

DEFINE_RAW_STATUS(position, POSITION)
DEFINE_STATUS_EXPIRY(position_latitude);
DEFINE_STATUS_EXPIRY(position_longitude);

int get_position(POSITION *position) {
  if (VALID_STATUS(position_latitude) && VALID_STATUS(position_longitude)) {
    *position = STATUS_VARIABLE(position);
    return 0;
  }
  return -1;
}

void st_received_lat_position(LATITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100) {
  STATUS_VARIABLE(position).hemisphere_latitude = hemisphere;
  STATUS_VARIABLE(position).degrees_latitude = degrees;
  STATUS_VARIABLE(position).minutes_latitude_times_1000 = minutes_times_100 * 10;
  VALIDATE_STATUS(position_latitude);
}

void st_received_lon_position(LATITUDE_HEMISPHERE hemisphere, int degrees, int minutes_times_100) {
  STATUS_VARIABLE(position).hemisphere_longitude = hemisphere;
  STATUS_VARIABLE(position).degrees_longitude = degrees;
  STATUS_VARIABLE(position).minutes_longitude_times_1000 = minutes_times_100 * 10;
  VALIDATE_STATUS(position_longitude);
}

void st_received_lat_lon_position(LATITUDE_HEMISPHERE latitude_hemisphere, int degrees_lat, int minutes_lat_times_1000, LONGITUDE_HEMISPHERE longitude_hemisphere, int degrees_lon, int minutes_lon_times_1000) {
  STATUS_VARIABLE(position).hemisphere_latitude = latitude_hemisphere;
  STATUS_VARIABLE(position).degrees_latitude = degrees_lat;
  STATUS_VARIABLE(position).minutes_latitude_times_1000 = minutes_lat_times_1000;
  STATUS_VARIABLE(position).hemisphere_longitude = longitude_hemisphere;
  STATUS_VARIABLE(position).degrees_longitude = degrees_lon;
  STATUS_VARIABLE(position).minutes_longitude_times_1000 = minutes_lon_times_1000;
  VALIDATE_STATUS(position_latitude);
  VALIDATE_STATUS(position_longitude);
}

DEFINE_STATUS(countdown_timer, int)
DEFINE_STATUS(countdown_timer_mode, TIMER_MODE)

void st_received_set_countdown_timer(int hours, int minutes, int seconds, TIMER_MODE mode) {
  SET_STATUS(countdown_timer, hours * 3600 + minutes * 60 + seconds);
  SET_STATUS(countdown_timer_mode, mode);
}

DEFINE_STATUS(compass_variation_in_degrees_west, int);

void st_received_compass_variation(int degrees_west) {
  SET_STATUS(compass_variation_in_degrees_west, degrees_west);
}

DEFINE_STATUS(gps_fix_quality, GPS_FIX_QUALITY);

void st_received_gps_and_dgps_fix_info(int signal_quality_available, int signal_quality, int hdop_available, int hdop, int antenna_height, int satellite_count_available, int satellite_count, int geoseparation, int dgps_age_available, int dgps_age, int dgps_station_id_available, int dgps_station_id) {
  GPS_FIX_QUALITY gps_fix_quality;
  gps_fix_quality.signal_quality_valid = signal_quality_available;
  gps_fix_quality.signal_quality = signal_quality;
  gps_fix_quality.position_error_valid = hdop_available;
  gps_fix_quality.position_error = hdop;
  gps_fix_quality.antenna_height = antenna_height;
  gps_fix_quality.satellite_count_valid = satellite_count_available;
  gps_fix_quality.satellite_count = satellite_count;
  gps_fix_quality.geoseparation = geoseparation;
  gps_fix_quality.dgps_age_valid = dgps_age_available;
  gps_fix_quality.dgps_age = dgps_age;
  gps_fix_quality.dgps_station_id_valid = dgps_station_id_available;
  gps_fix_quality.dgps_station_id = dgps_station_id;
  SET_STATUS(gps_fix_quality, gps_fix_quality);
}

void st_received_satellite_info(int satellite_count, int horizontal_dilution_of_position) {
  STATUS_VARIABLE(gps_fix_quality).satellite_count_valid = 1;
  STATUS_VARIABLE(gps_fix_quality).satellite_count = satellite_count;
  STATUS_VARIABLE(gps_fix_quality).position_error_valid = 1;
  STATUS_VARIABLE(gps_fix_quality).position_error = horizontal_dilution_of_position;
  VALIDATE_STATUS(gps_fix_quality);
}

DEFINE_RAW_STATUS(navigation, NAVIGATION_STATUS);
DEFINE_STATUS_EXPIRY(navigation_name);
DEFINE_STATUS_EXPIRY(navigation);

int get_navigation(NAVIGATION_STATUS *navigation_status) {
  if (VALID_STATUS(navigation_name) && VALID_STATUS(navigation)) {
    *navigation_status = STATUS_VARIABLE(navigation);
    return 0;
  }
  return -1;
}

void st_received_target_waypoint_name(int char1, int char2, int char3, int char4) {
  STATUS_VARIABLE(navigation).waypoint_name_valid = 1;
  STATUS_VARIABLE(navigation).waypoint_name_last_4[0] = char1;
  STATUS_VARIABLE(navigation).waypoint_name_last_4[1] = char2;
  STATUS_VARIABLE(navigation).waypoint_name_last_4[2] = char3;
  STATUS_VARIABLE(navigation).waypoint_name_last_4[3] = char4;
  VALIDATE_STATUS(navigation_name);
}

void st_received_waypoint_navigation(int cross_track_error_present, int cross_track_error_times_100, int waypoint_bearing_present, int waypoint_bearing, int bearing_is_magnetic, int waypoint_distance_present, int waypoint_distance_times_100, int direction_to_steer) {
  STATUS_VARIABLE(navigation).cross_track_error_valid = cross_track_error_present;
  STATUS_VARIABLE(navigation).cross_track_error_in_nautical_miles_times_100 = cross_track_error_times_100;
  STATUS_VARIABLE(navigation).waypoint_bearing_and_range_valid = waypoint_bearing_present && waypoint_distance_present;
  STATUS_VARIABLE(navigation).waypoint_bearing = waypoint_bearing;
  STATUS_VARIABLE(navigation).waypoint_range_in_nautical_miles_times_100 = waypoint_distance_times_100;
  VALIDATE_STATUS(navigation);
}

DEFINE_RAW_STATUS(gmt_date_and_time, DATE_AND_TIME)
DEFINE_STATUS_EXPIRY(gmt_date);
DEFINE_STATUS_EXPIRY(gmt_time);

int get_gmt_date_and_time(DATE_AND_TIME *date_and_time) {
  if (VALID_STATUS(gmt_date) && VALID_STATUS(gmt_time)) {
    *date_and_time = STATUS_VARIABLE(gmt_date_and_time);
    return 0;
  }
  return -1;
}

void st_received_date(int year, int month, int day) {
  STATUS_VARIABLE(gmt_date_and_time).year = year;
  STATUS_VARIABLE(gmt_date_and_time).month = month;
  STATUS_VARIABLE(gmt_date_and_time).day = day;
  VALIDATE_STATUS(gmt_date);
}

void st_received_gmt_time(int hour, int minute, int second) {
  STATUS_VARIABLE(gmt_date_and_time).hour = hour;
  STATUS_VARIABLE(gmt_date_and_time).minute = minute;
  STATUS_VARIABLE(gmt_date_and_time).second = second;
  VALIDATE_STATUS(gmt_time);
}

DEFINE_RAW_STATUS(autopilot, AUTOPILOT_STATUS);
DEFINE_STATUS_EXPIRY(autopilot_mode);
DEFINE_STATUS_EXPIRY(autopilot_response);

int get_autopilot(AUTOPILOT_STATUS *autopilot_status) {
  if (VALID_STATUS(autopilot_mode) && VALID_STATUS(autopilot_response)) {
    *autopilot_status = STATUS_VARIABLE(autopilot);
    return 0;
  }
  return -1;
}

void st_received_autopilot_status(int compass_heading, TURN_DIRECTION turn_direction, int target_heading, AUTOPILOT_MODE mode, int rudder_position, int alarms, int display_flags) {
  SET_STATUS(heading, compass_heading);
  STATUS_VARIABLE(autopilot).target_heading = target_heading;
  STATUS_VARIABLE(autopilot).mode = mode;
  VALIDATE_STATUS(autopilot_mode);
}

void st_received_set_autopilot_response_level(AUTOPILOT_RESPONSE_LEVEL response_level) {
  STATUS_VARIABLE(autopilot).response_level = response_level;
  VALIDATE_STATUS(autopilot_response);
}

DEFINE_STATUS(arrival_perpendicular_passed, int)
DEFINE_STATUS(arrival_circle_entered, int)

void st_received_arrival_info(int perpendicular_passed, int circle_entered, int char_1, int char_2, int char_3, int char_4) {
  SET_STATUS(arrival_perpendicular_passed, perpendicular_passed);
  SET_STATUS(arrival_circle_entered, circle_entered);
}

DEFINE_STATUS(course_computer_failure_type, COURSE_COMPUTER_FAILURE_TYPE)

void st_received_course_computer_failure(COURSE_COMPUTER_FAILURE_TYPE failure_type) {
  SET_STATUS(course_computer_failure_type, failure_type);
}

// ignore autopilot_parameter datagram

void st_received_autopilot_parameter(int parameter, int min_value, int max_value, int value) {
}

// ignore these comamnd datagrams:

void st_received_autopilot_command(ST_AUTOPILOT_COMMAND command) {
}

void st_received_set_autopilot_parameter(int parameter, int value) {
}

void st_received_enter_autopilot_setup(void) {
}

void st_received_set_rudder_gain(int rudder_gain) {
}

void st_received_maxview_keystroke(int key_1, int key_2, int held_longer) {
}

// ignore waypoint list

void st_received_destination_waypoint_info(char last_4, char first_8, int more_records, int last_record) {
}

void initialize_status(void) {
  status_initialized = 1;
  INVALIDATE_STATUS(active_alarms);
  INVALIDATE_STATUS(depth_below_transducer_in_feet_times_10);
  INVALIDATE_STATUS(engine_single);
  INVALIDATE_STATUS(engine_port);
  INVALIDATE_STATUS(engine_starboard);
  INVALIDATE_STATUS(apparent_wind_angle);
  INVALIDATE_STATUS(apparent_wind_speed_in_knots_times_10);
  INVALIDATE_STATUS(heading);
  INVALIDATE_STATUS(heading_reference);
  INVALIDATE_STATUS(turn_direction);
  INVALIDATE_STATUS(water_speed_in_knots_times_100);
  INVALIDATE_STATUS(average_water_speed_in_knots_times_100);
  INVALIDATE_STATUS(rudder_position_in_degrees_right);
  INVALIDATE_STATUS(course_over_ground);
  INVALIDATE_STATUS(course_over_ground_reference);
  INVALIDATE_STATUS(speed_over_ground_in_knots_times_100);
  INVALIDATE_STATUS(trip_mileage_in_nautical_miles_times_100);
  INVALIDATE_STATUS(total_mileage_in_nautical_miles_times_10);
  INVALIDATE_STATUS(water_temperature_in_degrees_celsius_times_10);
//  INVALIDATE_STATUS(lamp_intensity);
  INVALIDATE_STATUS(position_latitude);
  INVALIDATE_STATUS(position_longitude);
  INVALIDATE_STATUS(compass_variation_in_degrees_west);
  INVALIDATE_STATUS(gps_fix_quality);
  STATUS_VARIABLE(gps_fix_quality).signal_quality_valid = 0;
  STATUS_VARIABLE(gps_fix_quality).position_error_valid = 0;
  STATUS_VARIABLE(gps_fix_quality).satellite_count_valid = 0;
  STATUS_VARIABLE(gps_fix_quality).dgps_age_valid = 0;
  STATUS_VARIABLE(gps_fix_quality).dgps_station_id_valid = 0;
  INVALIDATE_STATUS(navigation);
  STATUS_VARIABLE(navigation).waypoint_name_valid = 0;
  STATUS_VARIABLE(navigation).waypoint_position_valid = 0;
  STATUS_VARIABLE(navigation).waypoint_bearing_and_range_valid = 0;
  STATUS_VARIABLE(navigation).cross_track_error_valid = 0;
  INVALIDATE_STATUS(gmt_date);
  INVALIDATE_STATUS(gmt_time);
  INVALIDATE_STATUS(autopilot_mode);
  INVALIDATE_STATUS(autopilot_response);
}
