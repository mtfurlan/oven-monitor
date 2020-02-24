/**
 * This works mostly by global magics.
 * We have an array of ADC readings, an index, and a running sum.
 * This allows window averaging.
 *
 * There is also a global float temp, reportedTemp, and displayedTemp.
 * temp is the last known temperature, the other two are for figuring out when
 * to update the display or MQTT.
 *
 * Variables you might want to fuck with:
 *   STATUS_INTERVAL: minimum mqtt update frequency (ms)
 *   TEMP_REPORT_SIZE: tempreature change upon which to update MQTT (c).
 *   OVEN_COOL_THRESH: don't turn on the display below this many degrees C
 *   MQTT_ENABLED: comment out if you don't want mqtt
 **/
//comment out if not using MQTT
#define MQTT_ENABLED

//Maximum amount of time between MQTT updates (ms)
#define STATUS_INTERVAL 60000UL
//how big a temperature swing to report to MQTT
#define TEMP_REPORT_SIZE 5

//Don't display if temp is below this
#define OVEN_COOL_THRESH 70

//how big the thermocouple averaging window is
#define ADC_NUM_READINGS 50

//how fast to poll the ADC
#define ADC_INTERVAL 10


#include "oven-monitor.h"
#include "pins.h"

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef MQTT_ENABLED
#include <mqtt-wrapper.h>

uint32_t nextStatus = 0UL;
float reportedTemp = 0;
bool mqttDirty;
struct mqtt_wrapper_options mqtt_options;
enum ConnState connState;
char topicBuf[1024];
#endif

uint32_t nextRead = 0UL;

//For the ADC, we want some smoothing
//So we average the past ADC_NUM_READINGS readings
//adc_init says if we've read enough readings yet
int adc_readings[ADC_NUM_READINGS];
int adc_sum = 0;
int adc_index = 0;
bool adc_init = false;

float temp = 0;
float displayedTemp = 0;

bool displayDirty;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL_PIN, /* data=*/ SDA_PIN);

char buf[1024];

void display() {
  displayedTemp = temp;
  if(temp < OVEN_COOL_THRESH) {
    u8g2.setPowerSave(1);
    return;
  }
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_inr38_mf);

  //put temp in buf
  sprintf(buf, "%.0fC", temp);

  //display buf
  u8g2.drawStr(0, 50, buf);


#ifdef MQTT_ENABLED
  //show mqtt connected status
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  // https://github.com/olikraus/u8g2/wiki/fntgrpunifont
  // bottom of page
  switch(connState) {
    case F_MQTT_CONNECTED:
      u8g2.drawGlyph(119, 10, 0x25C6);	/* dec 9670/hex 25C6 solid diamond */
      break;
    case F_MQTT_DISCONNECTED:
      u8g2.drawGlyph(119, 10, 0x25C8);	/* dec 9672/hex 25C8 dot inside diamond */
      break;
    case WIFI_DISCONNECTED:
      u8g2.drawGlyph(119, 10, 0x25C7);	/* dec 9671/hex 25C7 empty diamond */
      break;
  }
#endif
  u8g2.sendBuffer();
  u8g2.setPowerSave(0);
}

//read ADC into adc_readings, pudate adc_sum/adc_init
void readADC() {
  adc_sum -= adc_readings[adc_index];
  adc_readings[adc_index] = analogRead(THERMOCOUPLE_PIN);
  adc_sum += adc_readings[adc_index];
  adc_index = (adc_index+1) % ADC_NUM_READINGS;

  if(!adc_init && adc_index == 0) {
    //we've wrapped back to 0
    adc_init = true;
  }
}

//calculate temp from adc_sum; update temp and display/mqtt dirty
void calculateTemp() {
#define AREF 3.2
#define ADC_RESOLUTION 10
  if(!adc_init) {
    return;
  }

  float voltage = ((float)adc_sum)/ADC_NUM_READINGS * (AREF / (pow(2, ADC_RESOLUTION)-1));
  temp = (voltage - 1.25) / 0.005;

  //sprintf(buf, "t: %f, v: %f, raw: %f\n", temp, voltage, ((float)adc_sum)/ADC_NUM_READINGS);
  //Serial.print(buf);

#ifdef MQTT_ENABLED
  if(abs(temp-reportedTemp) > TEMP_REPORT_SIZE) {
    mqttDirty = true;
  }
#endif

  if(abs(temp-displayedTemp) > 0) {
    displayDirty = true;
  }
}

#ifdef MQTT_ENABLED
void reportTelemetry(PubSubClient *client) {
  sprintf(topicBuf, "tele/%s/temp", TOPIC);
  //sprintf(buf, "%f", temp);
  dtostrf(temp, 0, 0, buf);
  client->publish(topicBuf, buf);

  reportedTemp = temp;
}

void callback(char* topic, byte* payload, unsigned int length, PubSubClient *client) {
  sprintf(topicBuf, "stat/%s/what", TOPIC);
  client->publish(topicBuf, "bad command");
}

void connectionEvent(PubSubClient* client, enum ConnState state, int reason) {
  connState = state;
  //Skipping displayDirty because sometimes this is called with no looping
  //like for wifi connect and then mqtt connect
  display();
}

void connectSuccess(PubSubClient* client, char* ip) {
  //Are subscribed to cmnd/fullTopic/+
}

void connectedLoop(PubSubClient* client) {
  if(mqttDirty || (long)( millis() - nextStatus ) >= 0) {
    nextStatus = millis() + STATUS_INTERVAL;
    mqttDirty = false;
    reportTelemetry(client);
  }
}
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");


  // --- Display ---
  u8g2.setI2CAddress(0x78);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();

#ifdef MQTT_ENABLED
  // --- MQTT ---
  mqtt_options.connectedLoop = connectedLoop;
  mqtt_options.callback = callback;
  mqtt_options.connectSuccess = connectSuccess;
  mqtt_options.connectionEvent = connectionEvent;
  mqtt_options.ssid = WIFI_SSID;
  mqtt_options.password = WIFI_PASSWORD;
  mqtt_options.mqtt_server = MQTT_SERVER;
  mqtt_options.mqtt_port = MQTT_PORT;
  mqtt_options.host_name = NAME;
  mqtt_options.fullTopic = TOPIC;
  mqtt_options.debug_print = true;
  setup_mqtt(&mqtt_options);
#endif

  //empty adc reading array
  for(int i = 0; i < ADC_NUM_READINGS; ++i) {
    adc_readings[i] = 0;
  }
}


void loop() {
#ifdef MQTT_ENABLED
  loop_mqtt();
#endif
  if( (long)( millis() - nextRead ) >= 0) {
    nextRead = millis() + ADC_INTERVAL;
    readADC();
    calculateTemp();
  }

  //mqtt loop called before this
  if(displayDirty) {
    displayDirty = false;
    display();
  }
}
