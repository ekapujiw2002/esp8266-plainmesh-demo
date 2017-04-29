// arduino header file
#include <Arduino.h>

// json lib
#include <ArduinoJson.h>

// mesh lib
// #define ESP8266
#include <painlessMesh.h>

#define LED D0 // nodemcu default led

// delay info
#define BLINK_PERIOD 3000000  // microseconds until cycle repeat
#define BLINK_DURATION 100000 // microseconds LED is on for

// mesh info
#define MESH_SSID "MESHXXX"
#define MESH_PASSWORD "messingxxx"
#define MESH_PORT 12345

// port serial for interface
#define APP_PORT Serial
#define MAX_MSG_LEN 32

// global var
painlessMesh meshx;
SimpleList<uint32_t> nodesx;
uint32_t sendMessageTime = 0;
bool calc_delay = false;
bool is_new_mesh_message_arrive = false;
String mesh_message_received;

/**
 * when other node send us some data
 * @method receivedCallback
 * @param  from             the node is sender
 * @param  msg              the data
 */
void receivedCallback(uint32_t from, String &msg) {
  meshx.debugMsg(ERROR, "RX[%08X] = %s\n", from, msg.c_str());

  if (!is_new_mesh_message_arrive) {
    mesh_message_received = msg;
    is_new_mesh_message_arrive = true;
  }
}

/**
 * when a new node connect to us
 * @method newConnectionCallback
 * @param  nodeId                the node id making connection
 */
void newConnectionCallback(uint32_t nodeId) {
  meshx.debugMsg(ERROR, "ID[%u / %08X]\n", nodeId, nodeId);
}

/**
 * when the connection is changing
 * @method changedConnectionCallback
 */
void changedConnectionCallback() {
  meshx.debugMsg(ERROR, "CONNECTED TO %s\n", meshx.subConnectionJson().c_str());

  nodesx = meshx.getNodeList();

  meshx.debugMsg(ERROR, "NUM NODES : %d\n", nodesx.size());
  meshx.debugMsg(ERROR, "DEVICES :");

  SimpleList<uint32_t>::iterator node = nodesx.begin();
  while (node != nodesx.end()) {
    meshx.debugMsg(ERROR, " [%u / %08X]", *node, *node);
    node++;
  }
  meshx.debugMsg(ERROR, "\n");
  calc_delay = true;
}

/**
 * when we got new time adjustment inside the mesh
 * @method nodeTimeAdjustedCallback
 * @param  offset                   offset value
 */
void nodeTimeAdjustedCallback(int32_t offset) {
  meshx.debugMsg(ERROR, "TIME ADJUST %u = %d\n", meshx.getNodeTime(), offset);
}

/**
 * delay to a node
 * @method delayReceivedCallback
 * @param  from                  node id destination
 * @param  delay                 the delay in microseconds
 */
void delayReceivedCallback(uint32_t from, int32_t delay) {
  meshx.debugMsg(ERROR, "DELAY NODE [%u / %08X] = %d us\n", from, from, delay);
}

/**
 * blink the led status
 * @method blinkLedStatus
 */
void blinkLedStatus() {
  bool onFlag = true;
  uint32_t cycleTime = meshx.getNodeTime() % BLINK_PERIOD;
  for (uint8_t i = 0; i < (meshx.getNodeList().size() + 1); i++) {
    uint32_t onTime = BLINK_DURATION * i * 2;

    if (cycleTime > onTime && cycleTime < onTime + BLINK_DURATION)
      onFlag = false;
  }
  digitalWrite(LED, onFlag);
}

/**
 * split string char by a delimiter char
 * @method subStr
 * @param  input_string   char input
 * @param  separator      separator char
 * @param  segment_number data index from 1
 * @return                return char pointer
 */
// char *subStr(char *input_string, char *separator, int segment_number) {
//   char *act, *sub, *ptr;
//   static char copy[MAX_STRING_LEN];
//   int i;
//   strcpy(copy, input_string);
//   for (i = 1, act = copy; i <= segment_number; i++, act = NULL) {
//     sub = strtok_r(act, separator, &ptr);
//     if (sub == NULL)
//       break;
//   }
//   return sub;
// }

/**
 * get value from string delimited by char
 * @method getValue
 * @param  data      the string input_string
 * @param  separator char separator data
 * @param  index     index data from 0
 * @return           string
 */
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

/**
 * parse and process the data received. if it is for this node, then pass it to
 * APP_PORT
 * @method parseAndProcessData
 * @param  data_input          String input
 */
void parseAndProcessData(String data_input) {
  // get the designated node id
  String node_id_dest = getValue(data_input, ',', 0);
  String device_id = String(meshx.getNodeId(), 16);
  if (node_id_dest.equalsIgnoreCase(device_id)) {
    // send the data
    APP_PORT.println(data_input);
  }
}

/**
 * process data via APP_PORT, broadcast it
 * format data : node id destination,data\n
 * @method appPortDataProcessing
 */
void appPortDataProcessing() {
  String buffx;

  if (APP_PORT.available() > 0) {
    buffx = APP_PORT.readStringUntil('\n');
    // send to mesh
    if (meshx.sendBroadcast(buffx)) {
      APP_PORT.println(String(meshx.getNodeId(), 16) + String(",SEND OK"));
    } else {
      APP_PORT.println(String(meshx.getNodeId(), 16) + String(",SEND ERROR"));
    }
  }
}

/**
 * setup all here
 * @method setup
 */
void setup() {

  // setup app port
  APP_PORT.begin(115200);

  // set led mode pin
  pinMode(LED, OUTPUT);

  // set speed debug port
  meshx.initDebugPort(115200);

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC |
  // COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  meshx.setDebugMsgTypes(
      ERROR); // set before init() so that you can see startup messages

  // initialize mesh network
  meshx.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  meshx.onReceive(&receivedCallback);
  meshx.onNewConnection(&newConnectionCallback);
  meshx.onChangedConnections(&changedConnectionCallback);
  meshx.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  meshx.onNodeDelayReceived(&delayReceivedCallback);

  randomSeed(analogRead(A0));

  //info the device node id
  APP_PORT.println();
  APP_PORT.println(String(meshx.getNodeId(), 16));
}

/**
 * main program loop
 * @method loop
 */
void loop() {
  // bool error;

  // update mesh network info
  meshx.update();

  // run the blinky
  blinkLedStatus();

  // process command from app port
  appPortDataProcessing();

  // process data got from mesh
  if (is_new_mesh_message_arrive) {
    parseAndProcessData(mesh_message_received);
    is_new_mesh_message_arrive = false;
  }

  // get next random time for send message
  if (sendMessageTime == 0) {
    sendMessageTime = meshx.getNodeTime() + random(1000000, 5000000);
  }

  // if the time is ripe, send everyone a message!
  if (sendMessageTime != 0 && ((int)sendMessageTime - (int)meshx.getNodeTime() <
                               0)) { // Cast to int in case of time rollover

    // String msg = "Hello from node ";
    // msg += String(meshx.getNodeId(), 16);
    // error = meshx.sendBroadcast(msg + " myFreeMemory: " +
    //                             String(ESP.getFreeHeap()));

    sendMessageTime = 0;

    if (calc_delay) {
      SimpleList<uint32_t>::iterator node = nodesx.begin();
      while (node != nodesx.end()) {
        meshx.startDelayMeas(*node);
        node++;
      }
      calc_delay = false;
    }
  }
}
