// #include <Arduino.h>
// #include <ESP32Servo.h>
// #include <PPMReader.h>

// Servo myservo; // create servo object to control a servo
// Servo myservo2;
// // 16 servo objects can be created on the ESP32

// int pos = 0; // variable to store the servo position

// #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
// int servoPin = 17;
// #elif defined(CONFIG_IDF_TARGET_ESP32C3)
// int servoPin = 2;
// int servoPin2 = 3;
// #else
// int servoPin = 18;
// #endif

// byte interruptPin = 4;
// byte channelAmount = 8;
// PPMReader ppm(interruptPin, channelAmount);

// void setup()
// {
//   Serial.begin(115200);
//   // Allow allocation of all timers
//   ESP32PWM::allocateTimer(0);
//   ESP32PWM::allocateTimer(1);
//   ESP32PWM::allocateTimer(2);
//   ESP32PWM::allocateTimer(3);
//   myservo.setPeriodHertz(50); // standard 50 hz servo
//   // myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
//   myservo2.setPeriodHertz(60);            // standard 50 hz servo
//   myservo2.attach(servoPin2, 1000, 2000); // attaches the servo on pin 18 to the servo object

//   Serial.println("Big Cam");
// }

// void loop()
// {

//   for (byte channel = 1; channel <= channelAmount; ++channel)
//   {
//     unsigned value = ppm.latestValidChannelValue(channel, 0);
//     Serial.print(value);
//     if (channel < channelAmount)
//       Serial.print('\t');
//   }
//   Serial.println();
//   delay(20);

//   // for (pos = 0; pos <= 180; pos += 1)
//   // { // goes from 0 degrees to 180 degrees
//   //   // in steps of 1 degree
//   //   myservo.write(pos); // tell servo to go to position in variable 'pos'
//   //   myservo2.write(pos);
//   //   delay(5); // waits 15ms for the servo to reach the position
//   // }
//   // delay(1500);
//   // for (pos = 180; pos >= 0; pos -= 1)
//   // {                     // goes from 180 degrees to 0 degrees
//   //   myservo.write(pos); // tell servo to go to position in variable 'pos'
//   //   myservo2.write(pos);
//   //   delay(5); // waits 15ms for the servo to reach the position
//   // }
//   // delay(1500);
// }

// // Initialize a PPMReader on digital pin 3 with 6 expected channels.

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "sbus.h"

/* SBUS object, reading SBUS */
bfs::SbusRx sbus_rx(&Serial1, 4, 21, true);
/* SBUS object, writing SBUS */
/* SBUS data */
bfs::SbusData data;

AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
AsyncWebServer server(80);

const char *ssid = "Proxxima_297257_2G";
const char *password = "klinsman258";
String ch;

void sendEvents(String evt, String evtData)
{
    events.send(evtData.c_str(), evt.c_str(), random(111111, 999999));
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  /* Begin the SBUS communication */
  sbus_rx.Begin();
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Hello, world"); });
  server.addHandler(&events);
  server.begin();
}

void loop()
{
  if (sbus_rx.Read())
  {
    /* Grab the received data */
    data = sbus_rx.data();
    /* Display the received data */
    ch = "";
    for (int8_t i = 0; i < data.NUM_CH; i++)
    {
      // ch = "canal";
      ch += String(data.ch[i]);
      ch += ",";
      // sendEvents(ch, String(data.ch[i]));
      Serial.print(data.ch[i]);
      Serial.print("\t");
    }
    // /* Display lost frames and failsafe data */
    Serial.print(data.lost_frame);
    Serial.print("\t");
    Serial.println(data.failsafe);
    sendEvents("canal", ch);
  }
}
