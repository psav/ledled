#include <Wire.h>

#include <DS1307RTC.h>
#include <FastLED.h>
#include <IRremote.h>
#define LED_PIN     8
#define NUM_LEDS    144
int RECV_PIN = 2;

int LED_OFFSET = 144 / 2;

CRGB leds[NUM_LEDS];

IRrecv irrecv(RECV_PIN);

decode_results results;

//used to be int, but think long is needed
time_t start_time;
float mod = 60.0f / NUM_LEDS;
float mod12 = 12.0f / NUM_LEDS;
float modpsec = 1000.0f / NUM_LEDS;

float real_sec;
float real_min;
float real_psec;

//Subsecond
int psec_led;
int psec_led2;
float psec_bright;
float psec_pre_led;
int psec_led_old = 0;
int psec_led2_old = 0;

//Seconds
int sec_led;
int sec_led2;
float sec_bright;
float sec_pre_led;
int sec_led_old = 0;
int sec_led2_old = 0;

//Minutes
int min_led;
int min_led2;
float min_bright;
float min_pre_led;
int min_led_old = 0;
int min_led2_old = 0;

//Hours
int hour_led;
int hour_led2;
float hour_bright;
float hour_pre_led;
int hour_led_old = 0;
int hour_led2_old = 0;

int bright_scale = 1;

unsigned long cur_mil;
float mills;
int mills_and_time;

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);
  Serial.println(start_time);
  //RTC.set(1556619998);
  start_time = RTC.get();
  Serial.println(start_time);
  irrecv.enableIRIn();
}

void loop() {
  leds[psec_led_old] = CRGB(0, 0, 0);
  leds[psec_led2_old] = CRGB(0, 0, 0);
  leds[sec_led_old] = CRGB(0, 0, 0);
  leds[sec_led2_old] = CRGB(0, 0, 0);
  leds[min_led_old] = CRGB(0, 0, 0);
  leds[min_led2_old] = CRGB(0, 0, 0);
  leds[hour_led_old] = CRGB(0, 0, 0);
  leds[hour_led2_old] = CRGB(0, 0, 0);
  
  cur_mil = millis();
  mills = float(cur_mil % 1000) / 1000.0f;
  mills_and_time = cur_mil / 1000.0f;

  //SubSeconds
  real_psec = mills / 1000.0f;
  psec_pre_led = mills * NUM_LEDS;
  psec_led = ((int)psec_pre_led + LED_OFFSET) % (NUM_LEDS - 1);
  psec_bright = psec_pre_led - psec_led;
  psec_led2 = psec_led + 1 % (NUM_LEDS - 1);

  leds[psec_led] = CRGB::Red;
  leds[psec_led2] = CRGB::Red;
  leds[psec_led2].nscale8(psec_bright * 256);
  leds[psec_led].nscale8((1.0f - psec_bright) * 256);
  leds[psec_led] /= bright_scale;
  leds[psec_led2] /= bright_scale;

  //Seconds
  real_sec = ((float)second(start_time + mills_and_time) + mills) / 60.0f;
  sec_pre_led = ((float)second(start_time + mills_and_time) + mills) / mod;
  sec_led = ((int)sec_pre_led + LED_OFFSET) % (NUM_LEDS - 1);
  sec_bright = sec_pre_led - sec_led;
  sec_led2 = sec_led + 1 % (NUM_LEDS - 1);

  leds[sec_led] = CRGB::Purple;
  leds[sec_led2] = CRGB::Purple;
  leds[sec_led2].nscale8(sec_bright * 256);
  leds[sec_led].nscale8((1.0f - sec_bright) * 256);
  leds[sec_led] /= bright_scale;
  leds[sec_led2] /= bright_scale;

  //Minutes
  real_min = ((float)minute(start_time + mills_and_time) + real_sec) / 60.0f;
  min_pre_led = ((float)minute(start_time + mills_and_time) + real_sec) / mod;
  min_led = ((int)min_pre_led + LED_OFFSET) % (NUM_LEDS - 1);
  min_bright = min_pre_led - min_led;
  min_led2 = min_led + 1 % (NUM_LEDS - 1);

  leds[min_led] += CRGB::SkyBlue;
  leds[min_led2] += CRGB::SkyBlue;
  leds[min_led2].nscale8(min_bright * 256);
  leds[min_led].nscale8((1.0f - min_bright) * 256);
  leds[min_led] /= bright_scale;
  leds[min_led2] /= bright_scale;

  //Hours
  hour_pre_led = (((float)hour(start_time + mills_and_time) + real_min));
  if (hour_pre_led >= 12.0){
      hour_pre_led -= 12;
  }
  //Serial.println(hour_pre_led);
  hour_pre_led = hour_pre_led / mod12;
  //Serial.println(hour_pre_led);
  hour_led = ((int)hour_pre_led + LED_OFFSET) % (NUM_LEDS - 1);
  hour_bright = hour_pre_led - hour_led;
  hour_led2 = hour_led + 1 % (NUM_LEDS - 1);

  leds[hour_led] += CRGB::DarkOrange;
  leds[hour_led2] += CRGB::DarkOrange;
  leds[hour_led2].nscale8(hour_bright * 256);
  leds[hour_led].nscale8((1.0f - hour_bright) * 256);
  leds[hour_led] /= bright_scale;
  leds[hour_led2] /= bright_scale;

  //for (int i=0; i <= 3; i++){
  //  leds[int((float)i / 4.0 * NUM_LEDS)] = CRGB::White;
  //  leds[int((float)i / 4.0 * NUM_LEDS)] /= 60;
  //}

  //if (irrecv.decode(&results)) {
  //  Serial.println(results.value, HEX);
  //  irrecv.resume(); // Receive the next value
  //}
  psec_led_old = psec_led;
  psec_led2_old = psec_led2;
  sec_led_old = sec_led;
  sec_led2_old = sec_led2;
  min_led_old = min_led;
  min_led2_old = min_led2;
  hour_led_old = hour_led;
  hour_led2_old = hour_led2;

  FastLED.show();
  
  //delay(20);  

  //Serial.println((float)minute(start_time + mills_and_time));
  //Serial.println((float)second(start_time + mills_and_time));
  //Serial.println(start_time);
}
