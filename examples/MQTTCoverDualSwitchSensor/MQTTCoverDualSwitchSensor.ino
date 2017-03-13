#include "mywifi.h"
#include "mymqttbroker.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Cover_DualSwitchSensor.h>
#include "ctype.h"

#define MQTT_GARAGE_1_ROOT_TOPIC "/home/garage/1"
#define MQTT_GARAGE_STATE_TOPIC "/cover/state"
#define MQTT_GARAGE_CMD_TOPIC "/cover/cmd"
#define MQTT_GARAGE_1_STATE_TOPIC MQTT_GARAGE_1_ROOT_TOPIC MQTT_GARAGE_STATE_TOPIC
#define MQTT_GARAGE_1_CMD_TOPIC MQTT_GARAGE_1_ROOT_TOPIC MQTT_GARAGE_CMD_TOPIC

const char *ssid = WLAN_SSID;
const char *password = WLAN_PASS;
const char *mqtt_server = MQTT_SERVER;

char message_buff[20];

WiFiClient espClient;
PubSubClient *client;
Cover::DualSwitchSensor *coverSensor;

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String topicStr(topic);
  topicStr.toLowerCase();

  int i;
  // create character buffer with ending null terminator (string)
  for (i = 0; i < length; i++)
  {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  String msgString = String(message_buff);
  msgString.toLowerCase();

  // Parse the topic & payload and set any cover state or commands
}

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client->connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client->connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD))
    {
      Serial.println("connected");
      // ... and resubscribe
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  // put your setup code here, to run once:
  client = new PubSubClient(espClient);
  coverSensor = new Cover::DualSwitchSensor(13, 14);
  coverSensor->setup();

  Serial.begin(115200);
  setup_wifi();
  client->setServer(mqtt_server, 1883);
  client->setCallback(callback);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!client->connected())
  {
    reconnect();
  }

  // Look for any new messages
  client->loop();

  coverSensor->loop();

  // Should we publish anything?
  if (coverSensor->getStateChanged())
  {
    Cover::State state = coverSensor->getCurrentState();
    if (state == Cover::StateOpen)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "open");
    if (state == Cover::StateOpening)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "opening");
    if (state == Cover::StateClosed)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "closed");
    if (state == Cover::StateClosing)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "closing");
    if (state == Cover::StateUnknown)
      client->publish(MQTT_GARAGE_1_STATE_TOPIC, "unknown");
  }
}
