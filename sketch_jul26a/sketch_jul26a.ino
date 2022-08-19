  #include <WiFi.h>  
#include <PubSubClient.h>   // This library is an editted version DO NOT INSTALL IT ON YOUR OWN

WiFiClient wifiClient;
PubSubClient client(wifiClient);


char ssid[] = "OpenWrt"; //type your WIFI information inside the quotes
char password[] = "sinasina"; 
const char* mqtt_server = "192.168.1.230";   //Your server IP address
const char* mqtt_username = "";
const char* mqtt_password = "";
const int mqtt_port = 1883;
#define SENSORNAME "TestDevice"  //Sensor name should be unique for every sensor in the network


//----------------------------------------------------------------------------
//  Edit messages in the Serial print or delete them all if you don't want to use Serial
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

//-----------------------------------------------------------------------------
// Connects to mqtt server
void reconnect() {
  Serial.println("Conneting to MQTT Server");
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect(SENSORNAME, mqtt_username, mqtt_password)) {
//    Uncomment next line if you want to subscribe to a server
//      client.subscribe(light_set_topic);
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//-----------------------------------------------------------------------------
char buffer[100];
const char* light_state_topic = "sensor/"; // The topic that sensor is publishing to the broker
int magSensor;  // Contains information about pin
int lightSensor; //Contains information about pin
int magCounter = 0; //Counter associated to Hall Sensor
int lightCounter = 0; //Counter associated to Light Sensor
int magStat = 0; // 0: Door is close , 1: Door is open
int lightStat = 0;//0: Normal, 1:something is detected

void setup(){
  Serial.begin(115200);
  Serial.println("Beginning the program");
  setup_wifi();  // Connects to WiFi
  client.setServer(mqtt_server, mqtt_port); // Defining information about server
  reconnect();  //  Connects to the mqtt broker
  client.publish(light_state_topic, buffer, true);  // Publishing the message to the broker
  delay(500);

  pinMode(0, INPUT); // The pin above GND (Hall Sensor Input)
  pinMode(1, INPUT); // Log-rx pin        (Light Sensor Input)
  
}

void loop(){
  String payld; // Put the message you want to send in this string
  if (WiFi.status() != WL_CONNECTED){ // Checking WiFi status and connecting again if not connected
    setup_wifi();
  }
  if ((WiFi.status() == WL_CONNECTED) && (!client.connected())){ // Checking mqtt broker connection
    reconnect();
  }
  // ---- Write your program after here
  magSensor = digitalRead(0);
  lightSensor = digitalRead(1);

  // Detecting Door
  //Close to open
  if ((magSensor == 0)&&(magStat == 0)) { 
    if (magCounter == 2)
    {
      magStat = 1;
      magCounter = 0;
    }
    else{
      magCounter += 1;
    }
  }
  else{
    //Open to close
    if ((magSensor == 1)&&(magStat == 1)) { 
      if (magCounter == 2)
      {
        magStat = 0;
        magCounter = 0;
      }
      else{
        magCounter += 1;
      }
    }
    else{
      magCounter = 0;
    }
  }

  
  
  payld = String(magStat) + "," +String(lightStat);
  
  payld.toCharArray(buffer, (payld.length() + 1));  // Converting the string to char array 
  client.publish(light_state_topic, buffer, true);  // Sending the message
  delay(1000);
}
