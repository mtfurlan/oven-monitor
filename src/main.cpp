#include "oven-monitor.h"

#include <Arduino.h>
#include <U8g2lib.h>
#include <mqtt-wrapper.h>

#include "pins.h"

// how often to report temp over mqtt
uint32_t nextStatus = 0UL;
uint32_t statusInterval = 60000UL;

//How much of a change to report over mqtt
int8_t reportChangeSize = 5;

//how often to read the ADC
uint32_t nextRead = 0UL;
uint32_t readInterval = 10UL;

//Don't display if temp is below this
#define OVEN_COOL_THRESH 70

//For the ADC, we want some smoothing
//So we average the past ADC_NUM_READINGS readings
//adc_init says if we've read enough readings yet
#define ADC_NUM_READINGS 50
int adc_readings[ADC_NUM_READINGS];
int adc_sum = 0;
int adc_index = 0;
bool adc_init = false;

//Track temp, and what temp is displayed/sent over mqtt
float temp = 0;
float reportedTemp = 0;
float displayedTemp = 0;

//When to update display or mqtt
bool displayDirty;
bool mqttDirty;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL_PIN, /* data=*/ SDA_PIN);


struct mqtt_wrapper_options mqtt_options;
enum ConnState connState;
char buf[1024];
char topicBuf[1024];

void display() {
  displayedTemp = temp;
  if(temp < OVEN_COOL_THRESH) {
    u8g2.setPowerSave(1);
    return;
  }
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_inr38_mf);

  //display current temp
  sprintf(buf, "%.0fC", temp);
  //dtostrf(temp, 0, 0, buf);

  u8g2.drawStr(0, 50, buf);
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
  u8g2.sendBuffer();
  u8g2.setPowerSave(0);
}

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

  if(abs(temp-reportedTemp) > reportChangeSize) {
    mqttDirty = true;
  }
  if(abs(temp-displayedTemp) > 0) {
    displayDirty = true;
  }
}

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
    nextStatus = millis() + statusInterval;
    mqttDirty = false;
    reportTelemetry(client);
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting");


  // --- Display ---
  u8g2.setI2CAddress(0x78);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();

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

  //empty adc reading array
  for(int i = 0; i < ADC_NUM_READINGS; ++i) {
    adc_readings[i] = 0;
  }
}


void loop() {
  loop_mqtt();
  if( (long)( millis() - nextRead ) >= 0) {
    nextRead = millis() + readInterval;
    readADC();
    calculateTemp();
  }

  //mqtt loop called before this
  if(displayDirty) {
    displayDirty = false;
    display();
  }
}
