/* 
HPSTR Watchface
Gregory Parker 2015
*/

#include <pebble.h>
  
// Main Window
static Window *s_main_window;

// Hipster Image
static BitmapLayer *s_image_layer;
static GBitmap *s_hipster_image;

// Text to display time with
static TextLayer *s_left_eye_layer; // For displaying either the hour or the day
static TextLayer *s_right_eye_layer; // For displaying either the minute or the month

// Variable to toggle displaying of date upon accelerometer motion
static bool display_date;


static void main_window_load() {
  // Create hipster image and place on BitmapLayer
  s_hipster_image = gbitmap_create_with_resource(RESOURCE_ID_HIPSTER);
  s_image_layer = bitmap_layer_create(GRect(0,0,144, 168));
  bitmap_layer_set_bitmap(s_image_layer, s_hipster_image);
  layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_image_layer));
  
  // Place text above the image to tell the time
  
  // Hour Layer
  s_left_eye_layer = text_layer_create(GRect(21,38,34,28));
  text_layer_set_background_color(s_left_eye_layer, GColorClear);
  text_layer_set_text_color(s_left_eye_layer, GColorWhite);
  text_layer_set_font(s_left_eye_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_left_eye_layer, GTextAlignmentCenter);
  text_layer_set_text(s_left_eye_layer, "00");
  
  // Minute Layer
  s_right_eye_layer = text_layer_create(GRect(89,38,34,28));
  text_layer_set_background_color(s_right_eye_layer, GColorClear);
  text_layer_set_text_color(s_right_eye_layer, GColorWhite);
  text_layer_set_font(s_right_eye_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_right_eye_layer, GTextAlignmentCenter);
  text_layer_set_text(s_right_eye_layer, "00");
  
  // Add TextLayers to main window
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_left_eye_layer));
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_right_eye_layer));
}

static void main_window_unload() {
  // Unload all elements in main window
  text_layer_destroy(s_right_eye_layer);
  text_layer_destroy(s_left_eye_layer);
  gbitmap_destroy(s_hipster_image);
  bitmap_layer_destroy(s_image_layer);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char left_buffer[] = "00";
  static char right_buffer[] = "00";
  
  if(display_date == true) {
    strftime(left_buffer, sizeof("00"), "%d", tick_time); // Put day in buffer
    strftime(right_buffer, sizeof("00"), "%m", tick_time); // Put month in buffer
  }
  else {
    
    // Handle hours
    if(clock_is_24h_style() == true) {
      // 24 hour format
      strftime(left_buffer, sizeof("00"), "%H", tick_time);
    }
    else {
      // 12 hour format
      strftime(left_buffer, sizeof("00"), "%I", tick_time);
    }
  
  // Handle minutes
  strftime(right_buffer, sizeof("00"), "%M", tick_time);
  
  text_layer_set_text(s_left_eye_layer, left_buffer);
  text_layer_set_text(s_right_eye_layer, right_buffer);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void hide_date(void *data) {
  display_date = false;
  
  // Mark layers dirty so they're updated in the update_time() function
  layer_mark_dirty(text_layer_get_layer(s_left_eye_layer));
  layer_mark_dirty(text_layer_get_layer(s_right_eye_layer));
  
  update_time();
}

static void accel_tap(AccelAxisType axis, int32_t direction) {
  display_date = true;
  update_time();
  
  // Mark layers dirty so they're updated in the update_time() function
  layer_mark_dirty(text_layer_get_layer(s_left_eye_layer));
  layer_mark_dirty(text_layer_get_layer(s_right_eye_layer));
  
  // Wait 3 seconds, then call hide_date()
  app_timer_register(3000, hide_date, NULL);
}

static void init(){
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
                            .load = main_window_load,
                             .unload = main_window_unload});
  
  window_set_background_color(s_main_window, GColorBlack);
  
  window_stack_push(s_main_window, true);
  
  // Subscribe to TickTimerService to handle telling the time
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Subscribe to the accelerometer tap service to cover viewing the date
  accel_tap_service_subscribe(accel_tap);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}