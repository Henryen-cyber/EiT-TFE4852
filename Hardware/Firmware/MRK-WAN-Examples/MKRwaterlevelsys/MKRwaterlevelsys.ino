#include "arduino_secrets.h"
#include <MKRWAN.h>
#include "NewPing.h";
#include <DHT.h>

#define trigPin 7
#define echoPin 8
#define maxDistance 400
#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

NewPing sonar(trigPin, echoPin, maxDistance);


String response = "";

float duration, distance;
float hum;
float temp;
float soundsp;
float soundcm;

int iterations = 10;
int ledgreen = 12;
int ledred = 11;

LoRaModem modem;

String appEui = SECRET_APP_EUI_2;
String appKey = SECRET_APP_KEY_2;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //while (!Serial);

  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  //pinMode(buttonPin, INPUT); 
  pinMode(ledgreen, OUTPUT);
  pinMode(ledred, OUTPUT);
  //digitalWrite(ledgreen, HIGH);
  digitalWrite(ledred, HIGH);
  
  dht.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  modem.begin(EU868);
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }
  digitalWrite(ledred, LOW);
  digitalWrite(ledgreen, HIGH);
  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independently by this setting the modem will
  // not allow to send more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.
}


void(* resetFunc) (void) = 0; //declare reset function @ address 0

void loop() {
  
int z= 0;

  for (int i=0; i<120; i++) {
    Serial.print("Counter =");
    z=z+1;
    Serial.println(z);
    // Read sensor values and multiply by 100 to effictively have 2 decimals
     uint16_t humidity = dht.readHumidity(false) * 100;
    // uint8_t humidity = dht.readHumidity(false) * 10;

    // false: Celsius (default)
    // true: Farenheit
     uint16_t temperature = dht.readTemperature(false) * 100;
    // uint8_t temperature = dht.readTemperature(false) * 10;

    hum = dht.readHumidity();
    temp = dht.readTemperature();

    soundsp = 331.4 + (0.606 * temp) + (0.0124 * hum);

    soundcm = soundsp / 10000;

    duration = sonar.ping_median(iterations);
    distance = (duration / 2) * soundcm;

     uint16_t dist = ((duration / 2) * soundcm) * 100;
    // uint8_t dist = ((duration / 2) * soundcm) * 100;

    Serial.print("Temperature");
    Serial.print(temp);
    Serial.print(",");
    Serial.print("Humidity");
    Serial.print(hum);
    Serial.print(",");
    
    Serial.print("Water Level = ");
    if (distance >= 400 || distance <= 2) {
      Serial.println("Out of Range");
      distance = 0;
    }
    else {
      Serial.print(distance / 100);
      Serial.println( " m");
      delay(500);
    }

//set up the uplink nessage
    String msg = String(distance/100);
  
    Serial.print("Sending: " + msg + " - ");
    for (unsigned int i = 0; i < msg.length(); i++) {
      Serial.print(msg[i] >> 4, HEX);
      Serial.print(msg[i] & 0xF, HEX);
      Serial.print(" ");
      }
    Serial.println();
   
    int err;
    
    modem.beginPacket();
    modem.print(msg);
    err = modem.endPacket(false);
    if (err > 0) {
      Serial.println("Message sent correctly!");
    } else {
      Serial.println("Error sending message :(");
      Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
      Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
    }
    delay(3000); //Change the value to increase or decrease the interval
    if (!modem.available()) {
      Serial.println("No downlink message received at this time.");
      return;
    }
    char rcv[64];
//    int i = 0;
    while (modem.available()) {
      rcv[i++] = (char)modem.read();
    }
    Serial.print("Received: ");
    for (unsigned int j = 0; j < i; j++) {
      Serial.print(rcv[j] >> 4, HEX);
      Serial.print(rcv[j] & 0xF, HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  resetFunc();  //call reset

}

void array_to_string(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 8) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';

}
