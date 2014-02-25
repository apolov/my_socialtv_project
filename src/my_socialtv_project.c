#include <pebble.h>

Window *window;
TextLayer *question_layer, *op1_layer, *op2_layer, *op3_layer;
char question_buffer[64], op1_buffer[32], op2_buffer[32], op3_buffer[32];

enum {
  KEY_QUESTION = 0,
  KEY_OP1 = 1,
  KEY_OP2 = 2,
  KEY_OP3 = 3,
};
void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
 
}
 
void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
 
}
 
void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
 
}

void process_tuple(Tuple *t)
{
  //Get key
  int key = t->key;

  //Get integer value, if present
  int value = t->value->int32;

  //Get string value, if present
  char string_value[32];
  strcpy(string_value, t->value->cstring);

  //Decide what to do
  switch(key) {
    case KEY_QUESTION:
      //Location received
      snprintf(question_buffer, sizeof("Q: couldbereallylongname"), "Q: %s", string_value);
      text_layer_set_text(question_layer, (char*) &question_buffer);
      break;
    case KEY_OP1:
      //Temperature received
      snprintf(op1_buffer, sizeof("OP1: couldbereallylongname"), "OP1: %s", string_value);
      text_layer_set_text(op1_layer, (char*) &op1_buffer);
      break;
      case KEY_OP2:
      //Temperature received
      snprintf(op2_buffer, sizeof("OP2: couldbelong"), "OP2: %s", string_value);
      text_layer_set_text(op2_layer, (char*) &op2_buffer);
      break;
       case KEY_OP3:
      //Temperature received
      snprintf(op3_buffer, sizeof("OP3: couldbelong"), "OP3: %s", string_value);
      text_layer_set_text(op3_layer, (char*) &op3_buffer);
      break;
  }
}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
  (void) context;
  
  //Get data
  Tuple *t = dict_read_first(iter);
  if(t)
  {
    process_tuple(t);
  }
  
  //Get next
  while(t != NULL)
  {
    t = dict_read_next(iter);
    if(t)
    {
      process_tuple(t);
    }
  }
}

void click_config_provider(void *context) 
{
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}


static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
  TextLayer *layer = text_layer_create(location);
  text_layer_set_text_color(layer, colour);
  text_layer_set_background_color(layer, background);
  text_layer_set_font(layer, fonts_get_system_font(res_id));
  text_layer_set_text_alignment(layer, alignment);

  return layer;
}
 
void window_load(Window *window)
{
  question_layer = init_text_layer(GRect(5, 0, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_14", GTextAlignmentLeft);
  text_layer_set_text(question_layer, "Q");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(question_layer));

  op1_layer = init_text_layer(GRect(5, 30, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
  text_layer_set_text(op1_layer, "OP1:      ");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(op1_layer));

  op2_layer = init_text_layer(GRect(5, 60, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
  text_layer_set_text(op2_layer, "OP2:      ");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(op2_layer));

  op3_layer = init_text_layer(GRect(5, 90, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
  text_layer_set_text(op3_layer, "OP3:      ");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(op3_layer));
}
 
void window_unload(Window *window)
{ 
  text_layer_destroy(question_layer);
  text_layer_destroy(op1_layer);
  text_layer_destroy(op2_layer);
  text_layer_destroy(op3_layer);
}

void send_int(uint8_t key, uint8_t cmd)
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  
  Tuplet value = TupletInteger(key, cmd);
  dict_write_tuplet(iter, &value);
  
  app_message_outbox_send();
}

void tick_callback(struct tm *tick_time, TimeUnits units_changed)
{
  //Every five minutes
  if(tick_time->tm_min % 5 == 0)
  {
    //Send an arbitrary message, the response will be handled by in_received_handler()
    send_int(5, 5);
  }
}
 
void init()
{
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  WindowHandlers handlers = {
    .load = window_load,
    .unload = window_unload
  };
  window_set_window_handlers(window, handlers);

  //Register AppMessage events
  app_message_register_inbox_received(in_received_handler);          
  app_message_open(512, 512);   //Large input and output buffer sizes
  
  //Register to receive minutely updates
  tick_timer_service_subscribe(MINUTE_UNIT, tick_callback);

  window_stack_push(window, true);
}
 
void deinit()
{
  tick_timer_service_unsubscribe();
  
  window_destroy(window);
}
 
int main(void)
{
  init();
  app_event_loop();
  deinit();
}