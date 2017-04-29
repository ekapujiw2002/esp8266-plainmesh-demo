// arduino header file
#include <Arduino.h>

// json lib
#include <ArduinoJson.h>

#include "caljd.h"

// mesh lib
#include <painlessMesh.h>

#define LED D0 // GPIO number of connected LED, ON ESP-12 IS GPIO2

// delay info
#define BLINK_PERIOD 3000000  // microseconds until cycle repeat
#define BLINK_DURATION 100000 // microseconds LED is on for

// mesh info
#define MESH_SSID "MESHXXX"
#define MESH_PASSWORD "messingxxx"
#define MESH_PORT 12345

// global var
painlessMesh meshx;
SimpleList<uint32_t> nodesx;
uint32_t sendMessageTime = 0;
bool calc_delay = false;

/**
 * when other node send us some data
 * @method receivedCallback
 * @param  from             the node is sender
 * @param  msg              the data
 */
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("startHere: Received from %08X msg=%s\n", from, msg.c_str());
}

/**
 * when a new node connect to us
 * @method newConnectionCallback
 * @param  nodeId                the node id making connection
 */
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

/**
 * when the connection is changing
 * @method changedConnectionCallback
 */
void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", meshx.subConnectionJson().c_str());

  nodesx = meshx.getNodeList();

  Serial.printf("Num nodes: %d\n", nodesx.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodesx.begin();
  while (node != nodesx.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

/**
 * when we got new time adjustment inside the mesh
 * @method nodeTimeAdjustedCallback
 * @param  offset                   offset value
 */
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", meshx.getNodeTime(), offset);
}

/**
 * delay to a node
 * @method delayReceivedCallback
 * @param  from                  node id destination
 * @param  delay                 the delay in microseconds
 */
void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

/**
 * setup all here
 * @method setup
 */
void setup() {

  Serial.begin(115200);

  Serial.println(cal_to_jd(2017, 4, 26, 16, 43, 0), 6);

  // pinMode(LED, OUTPUT);
  //
  // Serial.println("MESH INIT START");
  //
  // // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC |
  // // COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  // meshx.setDebugMsgTypes(
  //     ERROR); // set before init() so that you can see startup messages
  //
  // // initialize mesh network
  // meshx.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  // meshx.onReceive(&receivedCallback);
  // meshx.onNewConnection(&newConnectionCallback);
  // meshx.onChangedConnections(&changedConnectionCallback);
  // meshx.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  // meshx.onNodeDelayReceived(&delayReceivedCallback);
  //
  // randomSeed(analogRead(A0));
  //
  // Serial.println("MESH INIT DONE");
}

/**
 * main program loop
 * @method loop
 */
void loop() {
  // bool error;
  //
  // // update mesh network info
  // meshx.update();
  //
  // // run the blinky
  // bool onFlag = true;
  // uint32_t cycleTime = meshx.getNodeTime() % BLINK_PERIOD;
  // for (uint8_t i = 0; i < (meshx.getNodeList().size() + 1); i++) {
  //   uint32_t onTime = BLINK_DURATION * i * 2;
  //
  //   if (cycleTime > onTime && cycleTime < onTime + BLINK_DURATION)
  //     onFlag = false;
  // }
  // digitalWrite(LED, onFlag);
  //
  // // get next random time for send message
  // if (sendMessageTime == 0) {
  //   sendMessageTime = meshx.getNodeTime() + random(1000000, 5000000);
  // }
  //
  // // if the time is ripe, send everyone a message!
  // if (sendMessageTime != 0 &&
  //     (int)sendMessageTime - (int)meshx.getNodeTime() <
  //         0) { // Cast to int in case of time rollover
  //   String msg = "Hello from node ";
  //   msg += String(meshx.getNodeId(), 16);
  //   error = meshx.sendBroadcast(msg + " myFreeMemory: " +
  //                               String(ESP.getFreeHeap()));
  //   sendMessageTime = 0;
  //
  //   if (calc_delay) {
  //     SimpleList<uint32_t>::iterator node = nodesx.begin();
  //     while (node != nodesx.end()) {
  //       meshx.startDelayMeas(*node);
  //       node++;
  //     }
  //     calc_delay = false;
  //   }
  // }
}
