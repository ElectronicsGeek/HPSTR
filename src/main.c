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
static TextLayer *s_hour_layer; // DEV: Box Dimensions: 21,44,55,70
static TextLayer *s_minute_layer; // DEV: Box Dimensions: 89,44,123,70

static void main_window_load() {
  // Create hipster image and place on BitmapLayer
  s_hipster_image = gbitmap_create_with_resource(RESOURCE_ID_HIPSTER);
  s_image_layer = bitmap_layer_create(GRect(0,0,144, 168));
  bitmap_layer_set_bitmap(s_image_layer, s_hipster_image);
  layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_image_layer));
  
  // Place text above the image to tell the time
  
  // Hour Layer
  s_hour_layer = text_layer_create(GRect(21,38,34,28));
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorWhite);
  text_layer_set_font(s_hour_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
  text_layer_set_text(s_hour_layer, "00");
  
  // Minute Layer
  s_minute_layer = text_layer_create(GRect(89,38,34,28));
  text_layer_set_background_color(s_minute_layer, GColorClear);
  text_layer_set_text_color(s_minute_layer, GColorWhite);
  text_layer_set_font(s_minute_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);
  text_layer_set_text(s_minute_layer, "00");
  
  // Add TextLayers to main window
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_hour_layer));
  layer_add_child(window_get_root_layer(s_main_window), text_layer_get_layer(s_minute_layer));
}

static void main_window_unload() {
  // Unload all elements in main window
  text_layer_destroy(s_minute_layer);
  text_layer_destroy(s_hour_layer);
  gbitmap_destroy(s_hipster_image);
  bitmap_layer_destroy(s_image_layer);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char hour_buffer[] = "00";
  static char minute_buffer[] = "00";
  
  // Handle hours
  if(clock_is_24h_style() == true) {
    // 24 hour format
    strftime(hour_buffer, sizeof("00"), "%H", tick_time);
  }
  else {
    // 12 hour format
    strftime(hour_buffer, sizeof("00"), "%I", tick_time);
  }
  
  // Handle minutes
  strftime(minute_buffer, sizeof("00"), "%M", tick_time);
  
  text_layer_set_text(s_hour_layer, hour_buffer);
  text_layer_set_text(s_minute_layer, minute_buffer);
  
 }

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
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
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}