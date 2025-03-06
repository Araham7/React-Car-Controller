#include <WiFi.h>        // Include the WiFi library for ESP32 to handle WiFi connections
#include <WebSocketsServer.h> // Include the WebSocket server library for real-time communication
#include <ArduinoJson.h> // Include the ArduinoJson library for JSON parsing

// Access Point (AP) credentials
const char* ssid = "DIY-Car";     // Define the SSID (name) of the WiFi Access Point
const char* password = "DIY@1234"; // Define the password for the WiFi Access Point (min 8 characters)

// Create a WebSocket server on port 81
WebSocketsServer webSocket(81); // Initialize a WebSocket server listening on port 81

// Motor control pins
#define pin1 4   // GPIO4 (safe)
#define pin2 16  // GPIO16 (safe)
#define pin3 17  // GPIO17 (safe)
#define pin4 15  // GPIO15 (safe, but ensure it's LOW during boot)

void setup() {
  Serial.begin(9600); // Initialize Serial communication at 9600 baud rate

  // Initialize the pins as outputs
  pinMode(pin1, OUTPUT); // Set pin1 as an output
  pinMode(pin2, OUTPUT); // Set pin2 as an output
  pinMode(pin3, OUTPUT); // Set pin3 as an output
  pinMode(pin4, OUTPUT); // Set pin4 as an output
  pinMode(LED_BUILTIN, OUTPUT); // Set the built-in LED pin as an output

  // Set initial state of pins to LOW
  digitalWrite(pin1, LOW); // Set pin1 to LOW
  digitalWrite(pin2, LOW); // Set pin2 to LOW
  digitalWrite(pin3, LOW); // Set pin3 to LOW
  digitalWrite(pin4, LOW); // Set pin4 to LOW
  digitalWrite(LED_BUILTIN, LOW); // Set the built-in LED to LOW

  // Set ESP32 as Access Point
  WiFi.softAP(ssid, password); // Start the ESP32 as an Access Point with the defined SSID and password
  Serial.println("Access Point Started!"); // Print a message indicating the AP has started
  Serial.print("AP IP Address: "); // Print the AP's IP address label
  Serial.println(WiFi.softAPIP()); // Print the actual IP address of the AP
  Serial.print("AP SSID Address: "); // Print the AP's SSID label
  Serial.println(ssid); // Print the actual SSID of the AP

  // Start WebSocket server
  webSocket.begin(); // Start the WebSocket server
  webSocket.onEvent(webSocketEvent); // Set the WebSocket event handler function
}

void loop() {
  webSocket.loop(); // Continuously handle WebSocket events
}

// WebSocket event handler function
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) { // Check the type of WebSocket event
    case WStype_DISCONNECTED: // Case for when a client disconnects
      Serial.printf("[%u] Disconnected!\n", num); // Print the client number that disconnected
      break;

    case WStype_CONNECTED: // Case for when a client connects
      Serial.printf("[%u] Connected from %s\n", num, webSocket.remoteIP(num).toString().c_str()); // Print the client number and IP address
      break;

    case WStype_TEXT: // Case for when a text message is received
      Serial.printf("[%u] Received: %s\n", num, payload); // Print the received message

      // Ensure the payload is null-terminated
      char payloadStr[length + 1]; // Create a character array to hold the payload
      memcpy(payloadStr, payload, length); // Copy the payload into the character array
      payloadStr[length] = '\0'; // Null-terminate the character array

      // JSON Deserialization
      StaticJsonDocument<200> doc; // Create a JSON document with a capacity of 200 bytes
      DeserializationError error = deserializeJson(doc, payloadStr); // Parse the JSON payload

      if (error) { // Check if JSON parsing failed
        Serial.print("JSON parsing failed: "); // Print an error message
        Serial.println(error.c_str()); // Print the specific error
        webSocket.sendTXT(num, "Error: Invalid JSON"); // Send an error message back to the client
        return; // Exit the function if parsing fails
      }

      // Extracting values from the JSON document
      const char* direction = doc["direction"] | "N/A"; // Extract the "direction" field or default to "N/A"
      int speed = doc["speed"] | 0; // Extract the "speed" field or default to 0
      bool headlightsOn = doc["headlightsOn"] | false; // Extract the "headlightsOn" field or default to false
      bool hornOn = doc["hornOn"] | false; // Extract the "hornOn" field or default to false
      int battery = doc["battery"] | 0; // Extract the "battery" field or default to 0

      // Print extracted values to the Serial monitor
      Serial.println("Parsed Data:"); // Print a label for the parsed data
      Serial.printf("Direction: %s\n", direction); // Print the direction
      Serial.printf("Speed: %d\n", speed); // Print the speed
      Serial.printf("Headlights: %s\n", headlightsOn ? "ON" : "OFF"); // Print headlights status
      Serial.printf("Horn: %s\n", hornOn ? "ON" : "OFF"); // Print horn status
      Serial.printf("Battery: %d%%\n", battery); // Print battery level

      // Control the motors based on the direction
      if (strcmp(direction, "Forward") == 0) { // Check if the direction is "Forward"
        moveForward(); // Call the function to move forward
      } else if (strcmp(direction, "Reverse") == 0) { // Check if the direction is "Reverse"
        moveBackward(); // Call the function to move backward
      } else if (strcmp(direction, "Left") == 0) { // Check if the direction is "Left"
        turnLeft(); // Call the function to turn left
      } else if (strcmp(direction, "Right") == 0) { // Check if the direction is "Right"
        turnRight(); // Call the function to turn right
      } else { // If no valid direction is provided
        neutral(); // Call the function to stop the motors
      }

      // Control the headlights
      digitalWrite(LED_BUILTIN, headlightsOn ? HIGH : LOW); // Set the built-in LED based on the headlights status

      // Echo back the received payload to the client
      webSocket.sendTXT(num, payloadStr); // Send the same payload back to the client
      break;
  }
}

void neutral() {
  // Turn off all pins
  digitalWrite(pin1, LOW); // Set pin1 to LOW
  digitalWrite(pin2, LOW); // Set pin2 to LOW
  digitalWrite(pin3, LOW); // Set pin3 to LOW
  digitalWrite(pin4, LOW); // Set pin4 to LOW
  Serial.println("Neutral: Motors stopped"); // Print a message indicating the motors are stopped
}

void moveForward() {
  // Logic to move forward
  digitalWrite(pin1, HIGH); // Set pin1 to HIGH
  digitalWrite(pin2, LOW); // Set pin2 to LOW
  digitalWrite(pin3, HIGH); // Set pin3 to HIGH
  digitalWrite(pin4, LOW); // Set pin4 to LOW
  Serial.println("Moving Forward"); // Print a message indicating forward movement
}

void moveBackward() {
  // Logic to move backward
  digitalWrite(pin1, LOW); // Set pin1 to LOW
  digitalWrite(pin2, HIGH); // Set pin2 to HIGH
  digitalWrite(pin3, LOW); // Set pin3 to LOW
  digitalWrite(pin4, HIGH); // Set pin4 to HIGH
  Serial.println("Moving Backward"); // Print a message indicating backward movement
}

void turnLeft() {
  // Logic to turn left
  digitalWrite(pin1, LOW); // Set pin1 to LOW
  digitalWrite(pin2, HIGH); // Set pin2 to HIGH
  digitalWrite(pin3, HIGH); // Set pin3 to HIGH
  digitalWrite(pin4, LOW); // Set pin4 to LOW
  Serial.println("Turning Left"); // Print a message indicating a left turn
}

void turnRight() {
  // Logic to turn right
  digitalWrite(pin1, HIGH); // Set pin1 to HIGH
  digitalWrite(pin2, LOW); // Set pin2 to LOW
  digitalWrite(pin3, LOW); // Set pin3 to LOW
  digitalWrite(pin4, HIGH); // Set pin4 to HIGH
  Serial.println("Turning Right"); // Print a message indicating a right turn
}