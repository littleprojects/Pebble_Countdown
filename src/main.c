//////////////////////////////////////////////////////////////////// 
//
// PEBBLE countdown watchface V1.0             04-2015
//
// TODO:
// inverte, mit Automode, 
//
// Ideen: vielleicht eine eigene Schrifft für größere Zahlen, Wakeup
//
// vibe ist verschoben 12h = 13h -1s könnte man einfach auf 11h setzen = 12h-1s
//
////////////////////////////////////////////////////////////////////

#include <pebble.h>
#include <PDUtils.h>
	
#define SETTINGS_KEY 95
  
static Window *s_main_window;
Window *window;

static TextLayer *s_1_layer3;
static TextLayer *s_1_layer4;
static TextLayer *s_1_layer1;
static TextLayer *s_2_layer1;
static TextLayer *s_3_layer1;
static TextLayer *s_1_layer2;
static TextLayer *s_2_layer2;
static TextLayer *s_3_layer2;
static TextLayer *s_4_layer2;
static TextLayer *s_1_layer0;
static TextLayer *s_2_layer0;

static AppSync sync;
static uint8_t sync_buffer[256]; //256/128

static int valueRead, valueWritten;

enum {
  KEY_MODE      = 0,
  KEY_DAY_START = 1,
  KEY_DAY_END   = 2,
	KEY_VIBE  		= 3,
	KEY_YEAR			= 4,
	KEY_MON				= 5,
	KEY_DAY				= 6,
	KEY_HOUR			= 7,
	KEY_MIN				= 8,
	KEY_SEC				= 9,
	KEY_TEXT1			= 0xa,
	KEY_TEXT2			= 0xb
};

typedef struct persist {
  //settings
	int Mode;
  int Day_start;
  int Day_end;
  bool is_day;
	bool vibe;
	//target date
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;
	char text1[21];
	char text2[21];//text2[21];
	
	bool change;
} __attribute__((__packed__)) persist;

persist settings = {
  .Mode 			= 0,
  .Day_start 	= 8,
  .Day_end 		= 21,
	.is_day			= true,
	.vibe				= true,
	.change			= true,
	
	//target date
	.year				= 2016,
	.mon				= 6,
	.day 				= 8,
	.hour				= 10,
	.min				= 0,
	.sec				= 0,
	.text1			= "Krissis birthday",
	.text2			= "is just in:"
};

int old_year 	= 0;
int old_day		= 0;
int old_hour	= 0;
int old_min		= 0;
int old_sec		= 0;

static void loadPersistentSettings() {  
  valueRead = persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

static void savePersistentSettings() {
  valueWritten = persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

//------------------------------------------- update time & vibe

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
 
	int now = (int)p_mktime(localtime(&temp));
	
	struct tm *end_time = localtime(&temp);
	
	//set targetdate
	end_time->tm_year 	= settings.year -1900;
	end_time->tm_mon 		= settings.mon -1;
	end_time->tm_mday 	= settings.day ;
	end_time->tm_hour 	= settings.hour;
  end_time->tm_min 		= settings.min;
	end_time->tm_sec 		= settings.sec;
	
	int end = (int)p_mktime(end_time);
	
	int raw_time = 140000;
	int year	= 0;
	int day 	= 0;
	int hour 	= 0;
	int min		= 44;
	int sec		= 44;
	
	if(end > now){
		raw_time =  end - now;
	}else{
		raw_time =  now - end;
	}
	
	if(raw_time < 0){
		raw_time = raw_time * -1;
	}
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "rawtime: %d", raw_time);
	
	sec							= raw_time % 60;
  int temp_time 	= (raw_time - sec)/60;
	min 						= temp_time % 60;
  temp_time 			= (temp_time - min)/60;
	hour						= temp_time % 24;
	temp_time 			= (temp_time - hour) /24;
	day							= temp_time;
	year						= day / 365;
	day							= day - (year * 365);
	
	//test text fill
	//char s_day[] = "Days ";
	//char s_hour[]= "h";
	//char s_min[] = "min";
	//char s_sec[] = "sec";
	//char text1[] = "to Giacomos and"; 
	//char text2[] = "Krissis Bday";
	
  // Create a long-lived buffer
	static char buffer1_1[sizeof("0000")];
	static char buffer1_2[sizeof("00000000")];
	static char buffer2_1[sizeof("000")];
	static char buffer2_2[sizeof("00000")];
	static char buffer2_3[sizeof("000")];
	static char buffer2_4[sizeof("0000")];
	static char buffer0_1[sizeof("000")];
	static char buffer0_2[sizeof("0000")];
	static char buffer3_1[sizeof("00001000010000100001")];
	static char buffer4_1[sizeof("00001000010000100001")];
	bool oversize = false;
	//bool change		= false;
	
	if(year > 0){
	//year
		snprintf(buffer1_1, sizeof(buffer1_1), "%d", year);
		if(year > 1){snprintf(buffer1_2, sizeof(buffer1_2), "Years");}else{snprintf(buffer1_2, sizeof(buffer1_2), "Year");}
		
		snprintf(buffer2_1, sizeof(buffer2_1), "%d", day);		
		snprintf(buffer2_2, sizeof(buffer2_2), "days");
		
		snprintf(buffer2_3, sizeof(buffer2_1), "%d", hour);	
		snprintf(buffer2_4, sizeof(buffer2_4), "h");
		
		snprintf(buffer0_1, sizeof(buffer0_1), "%d", min);
		snprintf(buffer0_2, sizeof(buffer0_2), "min");
		
		if(year > 99){oversize = true;}
		if(year != old_year){settings.change = true;}
	}else if(day > 0){
	//day
		snprintf(buffer1_1, sizeof(buffer1_1), "%d", day);
		if(day > 1){snprintf(buffer1_2, sizeof(buffer1_2), "Days");}else{snprintf(buffer1_2, sizeof(buffer1_2), "Day");}		
		
		snprintf(buffer2_1, sizeof(buffer2_1), "%d", hour);	
		snprintf(buffer2_2, sizeof(buffer2_2), "h");
		
		snprintf(buffer2_3, sizeof(buffer2_3), "%d", min);	
		snprintf(buffer2_4, sizeof(buffer2_4), "min");	
		
		snprintf(buffer0_1, sizeof(buffer0_1), "%d", sec);
		snprintf(buffer0_2, sizeof(buffer0_2), "sec");
		
		if(day > 99){oversize = true;}
		if(day != old_day){settings.change = true;}
	}else if(hour > 0){
	//hour
		snprintf(buffer1_1, sizeof(buffer1_1), "%d", hour);
		if(hour > 1){snprintf(buffer1_2, sizeof(buffer1_2), "Hours");}else{snprintf(buffer1_2, sizeof(buffer1_2), "Hour");}
		
		snprintf(buffer2_1, sizeof(buffer2_1), "%d", min);
		snprintf(buffer2_2, sizeof(buffer2_2), "min");
		
		snprintf(buffer2_3, sizeof(buffer2_3), "%d", sec);	
		snprintf(buffer2_4, sizeof(buffer2_4), "sec");
		
		snprintf(buffer0_1, sizeof(buffer0_1), " ");
		snprintf(buffer0_2, sizeof(buffer0_2), " ");
	}else if(min > 0){
	//min
		snprintf(buffer1_1, sizeof(buffer1_1), "%d", min);
		if(min > 1){snprintf(buffer1_2, sizeof(buffer1_2), "Minutes");}else{snprintf(buffer1_2, sizeof(buffer1_2), "Minute");}
		
		snprintf(buffer2_1, sizeof(buffer2_1), "%d", sec);
		snprintf(buffer2_2, sizeof(buffer2_2), "sec");
		
		snprintf(buffer2_3, sizeof(buffer2_3), " ");
		snprintf(buffer2_4, sizeof(buffer2_4), " ");	
		
		snprintf(buffer0_1, sizeof(buffer0_1), " ");
		snprintf(buffer0_2, sizeof(buffer0_2), " ");
	}else{
	//sec
		snprintf(buffer1_1, sizeof(buffer1_1), "%d", sec);
		snprintf(buffer1_2, sizeof(buffer1_2), "Sec");
		
		snprintf(buffer2_1, sizeof(buffer2_1), " ");
		snprintf(buffer2_2, sizeof(buffer2_2), " ");
		snprintf(buffer2_3, sizeof(buffer2_3), " ");
		snprintf(buffer2_4, sizeof(buffer2_4), " ");
		snprintf(buffer0_1, sizeof(buffer0_1), " ");
		snprintf(buffer0_2, sizeof(buffer0_2), " ");
	}
	
	//write the textlines
	//time_t temp2 = time(NULL); 
  struct tm *tick_time = localtime(&temp);
	strftime(buffer3_1, sizeof(settings.text1), settings.text1, tick_time);
	strftime(buffer4_1, sizeof(settings.text1), settings.text2, tick_time);
	//snprintf(buffer3_1, sizeof(settings.text1), "%s", settings.text1);
	//snprintf(buffer4_1, sizeof(settings.text2), "%s", settings.text2);

	if(settings.change){	//settings.change
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "change %d", oversize);
		if(oversize){	//set small number
			//text_layer_destroy(s_1_layer1);
			//s_1_layer1 = text_layer_create(GRect(00, 56, 70,  50));	//Line1 big nummber
			//text_layer_set_background_color(s_1_layer1, GColorClear);
			//text_layer_set_text_color(s_1_layer1, GColorBlack);
			//text_layer_set_text_alignment(s_1_layer1, GTextAlignmentRight);
			//text_layer_set_font(s_1_layer1, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS)); //Bitham 42 Light FONT_KEY_GOTHIC_28_BOLD
			//layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_1_layer1));
		}else{		//set Big number	
			//s_1_layer1 = text_layer_create(GRect(00, 46, 70,  50));	//Line1 big nummber
			//text_layer_set_background_color(s_1_layer1, GColorClear);
			//text_layer_set_text_color(s_1_layer1, GColorBlack);
			//text_layer_set_text_alignment(s_1_layer1, GTextAlignmentRight);
			//text_layer_set_font(s_1_layer1, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
		}
		settings.change = false;
		//layer_set_update_proc(text_layer_get_layer(s_1_layer1), s_1_layer1);
	}
	
  // Display this time on the TextLayer
	
	if(oversize){ //small number
		//text_layer_set_size(s_1_layer1, GSize(0,0));
		//text_layer_set_size(s_3_layer1, GSize(70,50));
		text_layer_set_text(s_1_layer1, "");
		text_layer_set_text(s_3_layer1, buffer1_1);
	}else{ //big number
		//text_layer_set_size(s_3_layer1, GSize(0,0));
		//text_layer_set_size(s_1_layer1, GSize(70,50));
		text_layer_set_text(s_3_layer1, "");
		text_layer_set_text(s_1_layer1, buffer1_1);	
	}
	
	//text_layer_set_text(s_1_layer1, buffer1_1);	
	text_layer_set_text(s_2_layer1, buffer1_2);
	text_layer_set_text(s_1_layer2, buffer2_1);
	text_layer_set_text(s_2_layer2, buffer2_2);
	text_layer_set_text(s_3_layer2, buffer2_3);
	text_layer_set_text(s_4_layer2, buffer2_4);
	text_layer_set_text(s_1_layer3, buffer3_1);
	text_layer_set_text(s_1_layer4, buffer4_1);
	text_layer_set_text(s_1_layer0, buffer0_1);
	text_layer_set_text(s_2_layer0, buffer0_2);
	
	
	
	//vibe
	if(settings.vibe){ //settings.vibe
	
		//use raw_time
		//vibe secound
		if(raw_time == 0){vibes_long_pulse();}			//0 sec left
		if(raw_time == 1){vibes_short_pulse();}			//1 sec
		if(raw_time == 2){vibes_short_pulse();}			//2 sec
		if(raw_time == 3){vibes_short_pulse();}
		if(raw_time == 4){vibes_short_pulse();}
		if(raw_time == 5){vibes_short_pulse();}
		if(raw_time == 10){vibes_short_pulse();}
		if(raw_time == 30){vibes_short_pulse();}		//30 sec
		
		//vibe minutes
		if(raw_time == 60){vibes_short_pulse();}		//1 min
		if(raw_time == 120){vibes_short_pulse();}		//2 min
		if(raw_time == 300){vibes_short_pulse();}		//5 min
		if(raw_time == 600){vibes_short_pulse();}		//10 min
		if(raw_time == 1800){vibes_short_pulse();}	//30 min
		
		//vibe hour
		if(raw_time == 3600){vibes_short_pulse();}	//1 h
		if(raw_time == 21600){vibes_short_pulse();}	//6 h
		if(raw_time == 43200){vibes_short_pulse();}	//12 h
		
		//vibe days
		if(raw_time == 86400){vibes_short_pulse();}	//1 day
		if(raw_time == 604800){vibes_short_pulse();}	//7 days
		if(raw_time == 1209600){vibes_short_pulse();}	//14 days
		if(raw_time == 2678400){vibes_short_pulse();}	//31 days
		if(raw_time == 8640000){vibes_short_pulse();} //100 days
		
		
		/* //old incorret vibe calc
		//day
		if(year == 0){
			if(day == 100 && day != old_day){
				 vibes_short_pulse();
				 //vibes_long_pulse();
			}
			if(day == 10 && day != old_day){
				 vibes_short_pulse();
				 //vibes_long_pulse();
			}
			if(day == 1 && day != old_day){
				 vibes_short_pulse();
				 //vibes_long_pulse();
			}
	//hour
			if(day == 0){
				if(hour == 12 && hour != old_hour){
					 vibes_short_pulse();
					 //vibes_long_pulse();
				}
				if(hour == 6 && hour != old_hour){
					 vibes_short_pulse();
					 //vibes_long_pulse();
				}
				if(hour == 1 && hour != old_hour){
					 vibes_short_pulse();
					 //vibes_long_pulse();
				}
	//min
				if(hour == 0){
					if(min == 30 && min != old_min){
						 vibes_short_pulse();
						 //vibes_long_pulse();
					}
					if(min == 10 && min != old_min){
						 vibes_short_pulse();
						 //vibes_long_pulse();
					}
					if(min == 5 && min != old_min){
						 vibes_short_pulse();
						 //vibes_long_pulse();
					}
					if(min == 1 && min != old_min){
						 vibes_short_pulse();
						 //vibes_long_pulse();
					}
	//sec
					if(min == 0){
						if(sec == 30){
							vibes_short_pulse();
						}
						if(sec == 10){
							vibes_short_pulse();
						}
						if(sec == 5){
							vibes_short_pulse();
						}
						if(sec == 4){
							vibes_short_pulse();
						}
						if(sec == 3){
							vibes_short_pulse();
						}
						if(sec == 2){
							vibes_short_pulse();
						}
						if(sec == 1){
							vibes_long_pulse();
						}						
					}
				}
			}
		}
		*/
	}
		
	//not used anymore
	//old_year 	= year;
	//old_day		= day;
	//old_hour	= hour;
	//old_min		= min;
	//old_sec		= sec;
}

//------------------------------------------------------- Sync callback

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY: %d", (int)key);
	
	switch (key) {
    case KEY_MODE:
    	if(strcmp(new_tuple->value->cstring, "day") == 0){settings.Mode = 0;} 
    	else if(strcmp(new_tuple->value->cstring, "night")==0){settings.Mode = 1;}
    	else if(strcmp(new_tuple->value->cstring, "auto")== 0){settings.Mode = 2;}
      break;
    case KEY_DAY_START:
      settings.Day_start = (int)new_tuple->value->int32;
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Day_start Time: %d", (int)settings.Day_start);
    	break;
    case KEY_DAY_END:
      settings.Day_end = (int)new_tuple->value->int32;
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Day_end Time: %d", settings.Day_end);
    	break;
		case KEY_VIBE:
			if(new_tuple->value->int32 == 1){settings.vibe = false;}
			if(new_tuple->value->int32 == 0){settings.vibe = true;}
			break;
		case KEY_YEAR:
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "year: %d", (int)new_tuple->value->int32);
			settings.year = (int)new_tuple->value->int32;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "year: %d", settings.year);
			settings.change = true;
			break;
		case KEY_MON:
			settings.mon = (int)new_tuple->value->int32;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "mon: %d", settings.mon);
			break;
		case KEY_DAY:
			settings.day = (int)new_tuple->value->int32;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "day: %d", settings.day);
			break;
		case KEY_HOUR:
			settings.hour = (int)new_tuple->value->int32;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "hour: %d", settings.hour);
			break;		
		case KEY_MIN:
			settings.min = (int)new_tuple->value->int32;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "min: %d", settings.min);
			break;
		case KEY_SEC:
			settings.sec = (int)new_tuple->value->int32;
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "sec: %d", settings.sec);
			break;
		case KEY_TEXT1:
			strcpy(settings.text1, new_tuple->value->cstring);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "text1: %s", settings.text1);
			break;
		case KEY_TEXT2:
			strcpy(settings.text2, new_tuple->value->cstring);
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "text2: %s", new_tuple->value->cstring);	
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "text2: %s", settings.text2);
			break;
  }
	
	savePersistentSettings();
	update_time();
}

static void main_window_load(Window *window) {
  // Create time TextLayer
	/*
	s_1_layer0 = text_layer_create(GRect(10,  5, 144, 30));	//Text line0
	s_1_layer1 = text_layer_create(GRect(10, 26, 90,  50));	//Line1 big nummber
	s_2_layer1 = text_layer_create(GRect(70, 45, 50,  50));	//Line1 text
	s_1_layer2 = text_layer_create(GRect(10, 75, 30,  30));	//#2 Number
	s_2_layer2 = text_layer_create(GRect(35, 85, 30,  30));	//#2 Text
	s_3_layer2 = text_layer_create(GRect(70, 75, 30,  30));	//#3 Number
	s_4_layer2 = text_layer_create(GRect(95, 85, 30,  30));	//#3 Text
	s_1_layer3 = text_layer_create(GRect(10, 115, 144,30));	//Text line1
	s_1_layer4 = text_layer_create(GRect(10, 135, 144,30));	//Text line2
	*/
	
	//with 70, start 74
	s_1_layer3 = text_layer_create(GRect(00, 00, 144, 30));	//Text line1
	s_1_layer4 = text_layer_create(GRect(00, 20, 144, 30));	//Text line2	
	s_1_layer1 = text_layer_create(GRect(00, 46, 70,  50));	//Line1 big nummber
	s_3_layer1 = text_layer_create(GRect(00, 56, 70,  50));	//Line1 big nummber small
	s_2_layer1 = text_layer_create(GRect(74, 66, 60,  50));	//Line1 text
	s_1_layer2 = text_layer_create(GRect(00, 85, 70,  30));	//#2 Number
	s_2_layer2 = text_layer_create(GRect(74, 95, 70,  30));	//#2 Text
	s_3_layer2 = text_layer_create(GRect(00, 110, 70, 30));	//#3 Number
	s_4_layer2 = text_layer_create(GRect(74, 120, 70, 30));	//#3 Text
	s_1_layer0 = text_layer_create(GRect(00, 135, 70, 30));	//#4 Number
	s_2_layer0 = text_layer_create(GRect(74, 145, 70, 30));	//#4 Text

	
  //text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_background_color(s_1_layer1, GColorClear);
	text_layer_set_background_color(s_2_layer1, GColorClear);
	text_layer_set_background_color(s_3_layer1, GColorClear);
	text_layer_set_background_color(s_3_layer2, GColorClear);
	text_layer_set_background_color(s_2_layer2, GColorClear);
	text_layer_set_background_color(s_3_layer2, GColorClear);
	text_layer_set_background_color(s_4_layer2, GColorClear);
	text_layer_set_background_color(s_1_layer3, GColorClear);
	text_layer_set_background_color(s_1_layer4, GColorClear);
	text_layer_set_background_color(s_1_layer0, GColorClear);
	text_layer_set_background_color(s_2_layer0, GColorClear);
	
  //text_layer_set_text_color(s_time_layer, GColorBlack);
	text_layer_set_text_color(s_1_layer1, GColorBlack);
	text_layer_set_text_color(s_2_layer1, GColorBlack);
	text_layer_set_text_color(s_3_layer1, GColorBlack);
	text_layer_set_text_color(s_1_layer2, GColorBlack);
	text_layer_set_text_color(s_2_layer2, GColorBlack);
	text_layer_set_text_color(s_3_layer2, GColorBlack);
	text_layer_set_text_color(s_4_layer2, GColorBlack);
	text_layer_set_text_color(s_1_layer3, GColorBlack);
	text_layer_set_text_color(s_1_layer4, GColorBlack);
	text_layer_set_text_color(s_1_layer0, GColorBlack);
	text_layer_set_text_color(s_2_layer0, GColorBlack);
	
  //text_layer_set_text(s_time_layer, "00:00");
	text_layer_set_text(s_1_layer1, "22");
	text_layer_set_text(s_2_layer1, "Days");
	text_layer_set_text(s_3_layer1, "");
	text_layer_set_text(s_1_layer2, "22");
	text_layer_set_text(s_2_layer2, "h");
	text_layer_set_text(s_3_layer2, "55");
	text_layer_set_text(s_4_layer2, "min");
	text_layer_set_text(s_1_layer3, "to Giacomos and");	
	text_layer_set_text(s_1_layer4, "Krissi's Bday");	
	text_layer_set_text(s_1_layer0, "22");
	text_layer_set_text(s_2_layer0, "sec");

  // Improve the layout to be more like a watchface
  //text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_font(s_1_layer1, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_font(s_3_layer1, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
	text_layer_set_font(s_2_layer1, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_font(s_1_layer2, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_font(s_2_layer2, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_font(s_3_layer2, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_font(s_4_layer2, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_font(s_1_layer3, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_font(s_1_layer4, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_font(s_1_layer0, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_font(s_2_layer0, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  
	//text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	/*
	text_layer_set_text_alignment(s_1_layer0, GTextAlignmentLeft);	
	text_layer_set_text_alignment(s_1_layer1, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_2_layer1, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_1_layer2, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_2_layer2, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_3_layer2, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_4_layer2, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_1_layer3, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_1_layer4, GTextAlignmentLeft);
	*/
		
	text_layer_set_text_alignment(s_1_layer1, GTextAlignmentRight);
	text_layer_set_text_alignment(s_3_layer1, GTextAlignmentRight);
	text_layer_set_text_alignment(s_2_layer1, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_1_layer2, GTextAlignmentRight);
	text_layer_set_text_alignment(s_2_layer2, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_3_layer2, GTextAlignmentRight);
	text_layer_set_text_alignment(s_4_layer2, GTextAlignmentLeft);
	text_layer_set_text_alignment(s_1_layer3, GTextAlignmentCenter);
	text_layer_set_text_alignment(s_1_layer4, GTextAlignmentCenter);
	text_layer_set_text_alignment(s_1_layer0, GTextAlignmentRight);
	text_layer_set_text_alignment(s_2_layer0, GTextAlignmentLeft);
	
	 // Make sure the time is displayed from the start
  update_time();

  // Add it as a child layer to the Window's root layer	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_2_layer1));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_1_layer2));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_2_layer2));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_1_layer3));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_1_layer4));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_1_layer0));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_2_layer0)); 
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_1_layer1));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_3_layer1));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_3_layer2));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_4_layer2));
 
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  //text_layer_destroy(s_time_layer);
	text_layer_destroy(s_1_layer0);
	text_layer_destroy(s_2_layer0);
	text_layer_destroy(s_1_layer1);
	text_layer_destroy(s_2_layer1);
	text_layer_destroy(s_3_layer1);
	text_layer_destroy(s_1_layer2);
	text_layer_destroy(s_2_layer2);
	text_layer_destroy(s_3_layer2);
	text_layer_destroy(s_4_layer2);
	text_layer_destroy(s_1_layer3);
	text_layer_destroy(s_1_layer4);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
	
	//load setting
	loadPersistentSettings();	

	
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  	
	//eingangsvariablen -> nicht mehr vergessen
  Tuplet initial_values[] = {
    TupletInteger(KEY_MODE, settings.Mode),
    TupletInteger(KEY_DAY_START, settings.Day_start),
    TupletInteger(KEY_DAY_END, settings.Day_end),
		TupletInteger(KEY_VIBE, settings.vibe),
		
		TupletInteger(KEY_YEAR, settings.year),
		TupletInteger(KEY_MON, 	settings.mon),
		TupletInteger(KEY_DAY, 	settings.day),
		TupletInteger(KEY_HOUR, settings.hour),
		TupletInteger(KEY_MIN, 	settings.min),
		TupletInteger(KEY_SEC, 	settings.sec),
		TupletCString(KEY_TEXT1, settings.text1),
		TupletCString(KEY_TEXT2, settings.text2)
  };

	const int inbound_size = 256;//256;//128;
  const int outbound_size = 256;//256;//128;  
  app_message_open(inbound_size, outbound_size); 
	
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_tuple_changed_callback, NULL, NULL);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  
	//save setting
	savePersistentSettings();
	app_sync_deinit(&sync);
	
	// Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
