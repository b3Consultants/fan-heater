#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>;

// Update these with values suitable for your network.
const char* ssid = "elpropio";
const char* password = "d798fd798f";
//const char* ssid = "LAS CHICHIS";
//const char* password = "140805092105";
//const char* mqtt_server = "192.168.1.11";
const char* mqtt_server = "192.168.0.148";
const char* clientID = "fanheater-controller";
const char* sensor_name = "";
const char* separator = ":";

//Variables
int chk;

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
char data[50];
char* heater;
char* fan;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topico, byte* payload, unsigned int length) {
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;
  String topic = String(topico);

  Serial.print("Message arrived on topic: [");
  Serial.print(topic);
  Serial.print("], ");
  Serial.println(message);

  if(topic == "testing" && message == "testing"){
    client.publish("testing-response", "alive");
  }
  
  if(topic == "heater" && message == "on"){
    digitalWrite(D1, LOW);    
  }
  if(topic == "heater" && message == "off"){
    digitalWrite(D1, HIGH);    
  }
  if(topic == "fan" && message == "on"){
    digitalWrite(D2, LOW);    
  }
  if(topic == "fan" && message == "off"){
    digitalWrite(D2, HIGH);    
  }
  
  if(topic == "status" && message == "data"){
    
   // -------- SENSOR 1 -------------------------
    memset(msg, 0, sizeof(msg));
    memset(data, 0, sizeof(data));
    if(digitalRead(D1) == LOW){
      heater = "on";
    }else{
      heater = "off";
    }
    strcat(msg, clientID);
    strcat(msg, separator);
    sensor_name = "heater";
    strcat(msg, sensor_name);
    strcat(msg, separator);
    strcat(msg, heater);
    client.publish("status-response", msg);
    // -------------------------------------------
    
    // -------- SENSOR 2 -------------------------
    memset(msg, 0, sizeof(msg));
    memset(data, 0, sizeof(data));
    if(digitalRead(D2) == LOW){
      fan = "on";
    }else{
      fan = "off";
    }
    strcat(msg, clientID);
    strcat(msg, separator);
    sensor_name = "fan";
    strcat(msg, sensor_name);
    strcat(msg, separator);
    strcat(msg, fan);
    client.publish("status-response", msg);
    // -------------------------------------------
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("conexions", clientID);
      // ... and resubscribe
      client.subscribe("fan");
      client.subscribe("heater");
      client.subscribe("status");
      client.subscribe("testing");
      // ---------------- Subscruptions
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  // digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
}


void loop() {     
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
