#define TIME_TO_LIVE 5
#define HEADING_SIZE 4
#define FLAG_SIZE 4
#define MODE_SIZE 10
#define ANGLE_SIZE 5

void initialize_status(void);

int get_active_alarms(int *active_alarms);
int get_depth_below_transducer_in_feet_times_10(int *feet_times_10); // depth in 0.1 foot units
//enum DISTANCE_UNITS get_distance_units(void);

typedef struct {
  int rpm;
  int pitch_percent;
} ENGINE_STATUS;

int get_engine_status(ENGINE_ID engine_id, ENGINE_STATUS *engine_status);
int get_apparent_wind_angle(int *degrees);
int get_apparent_wind_speed_in_knots_times_10(int *knots_times_10);
int get_heading(int *degrees);
//int get_heading_reference(ANGLE_REFERENCE *referebce);
int get_water_speed_in_knots_times_100(int *knots_times_100);
int get_average_water_speed_in_knots_times_100(int *knots_times_100);
int get_turn_direction(TURN_DIRECTION *turn_direction);
int get_rudder_position_in_degrees_right(int *degrees_right);
int get_course_over_ground(int *degrees);
int get_course_over_ground_reference(ANGLE_REFERENCE *reference);
int get_speed_over_ground_in_knots_times_100(int *knots_times_100);
int get_trip_mileage_in_nautical_miles_times_100(int *nautical_miles_times_100);
int get_total_mileage_in_nautical_miles_times_10(int *nautical_miles_times_10);
int get_water_temperature_in_degrees_celsius_times_10(int *degrees_celsius_times_10);
int get_lamp_intensity(int *intensity);

typedef struct {
  LATITUDE_HEMISPHERE hemisphere_latitude;
  int degrees_latitude;
  int minutes_latitude_times_1000;
  LONGITUDE_HEMISPHERE hemisphere_longitude;
  int degrees_longitude;
  int minutes_longitude_times_1000;
} POSITION;

int get_position(POSITION *position);
int get_compass_variation_in_degrees_west(int *degrees_west);

typedef struct {
  int signal_quality_valid;
  int signal_quality;
  int position_error_valid;
  int position_error;
  int antenna_height;
  int satellite_count_valid;
  int satellite_count;
  int geoseparation;
  int dgps_age_valid;
  int dgps_age;
  int dgps_station_id_valid;
  int dgps_station_id;
} GPS_FIX_QUALITY;

int get_gps_fix_quality(GPS_FIX_QUALITY *fix_quality);

typedef struct {
  int waypoint_name_valid;
  char waypoint_name_last_4[5];
  int waypoint_position_valid;
  POSITION waypoint_position;
  int waypoint_bearing_and_range_valid;
  int waypoint_bearing;
  ANGLE_REFERENCE waypoint_bearing_reference;
  int waypoint_range_in_nautical_miles_times_100;
  int cross_track_error_valid;
  int cross_track_error_in_nautical_miles_times_100;
} NAVIGATION_STATUS;

int get_navigation(NAVIGATION_STATUS *navigation_status);

typedef struct {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
} DATE_AND_TIME;

int get_gmt_date_and_time(DATE_AND_TIME *gmt_date_and_time);

typedef struct {
  int target_heading;
  AUTOPILOT_MODE mode;
  AUTOPILOT_RESPONSE_LEVEL response_level;
  int rudder_gain;
} AUTOPILOT_STATUS;

int get_autopilot(AUTOPILOT_STATUS *new_ap_status);
