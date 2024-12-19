#include <HardwareSerial.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <ArduinoJson.h>

// Pin definitions
#define WF2_X1_R1_PIN 2
#define WF2_X1_R2_PIN 3
#define WF2_X1_G1_PIN 6
#define WF2_X1_G2_PIN 7
#define WF2_X1_B1_PIN 10
#define WF2_X1_B2_PIN 11
#define WF2_X1_E_PIN 21
#define WF2_X2_R1_PIN 4
#define WF2_X2_R2_PIN 5
#define WF2_X2_G1_PIN 8
#define WF2_X2_G2_PIN 9
#define WF2_X2_B1_PIN 12
#define WF2_X2_B2_PIN 13
#define WF2_X2_E_PIN -1 // Currently unknown, so X2 port will not work (yet) with 1/32 scan panels
#define WF2_A_PIN 39
#define WF2_B_PIN 38
#define WF2_C_PIN 37
#define WF2_D_PIN 36
#define WF2_OE_PIN 35
#define WF2_CLK_PIN 34
#define WF2_LAT_PIN 33

// LED matrix resolution and chain configuration
#define PANEL_RES_X 32 // Width of the panel (32 columns)
#define PANEL_RES_Y 16 // Height of the panel (16 rows)
#define PANEL_CHAIN 1  // Number of panels in the chain

// Matrix panel configuration
HUB75_I2S_CFG::i2s_pins _pins_x1 = {WF2_X1_R1_PIN, WF2_X1_G1_PIN, WF2_X1_B1_PIN, WF2_X1_R2_PIN, WF2_X1_G2_PIN, WF2_X1_B2_PIN, WF2_A_PIN, WF2_B_PIN, WF2_C_PIN, WF2_D_PIN, WF2_X1_E_PIN, WF2_LAT_PIN, WF2_OE_PIN, WF2_CLK_PIN};
HUB75_I2S_CFG::i2s_pins _pins_x2 = {WF2_X2_R1_PIN, WF2_X2_G1_PIN, WF2_X2_B1_PIN, WF2_X2_R2_PIN, WF2_X2_G2_PIN, WF2_X2_B2_PIN, WF2_A_PIN, WF2_B_PIN, WF2_C_PIN, WF2_D_PIN, WF2_X2_E_PIN, WF2_LAT_PIN, WF2_OE_PIN, WF2_CLK_PIN};
HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN, _pins_x1);
MatrixPanel_I2S_DMA dma_display(mxconfig);

// Global variables for text settings
String direction = "left";                                  // Direction of text movement ('left' or 'right')
uint16_t textColor = dma_display.color565(255, 255, 255);   // Default text color (white)
uint16_t borderColor = dma_display.color565(255, 255, 255); // Default border color (white)
bool border = false;                                        // Flag for showing a border
uint8_t fontsize = 2;                                       // Font size (default: 2)
int repeat = 1;                                             // Number of repetitions for the text
int speed = 50;                                             // Speed of text movement

// Function to calculate the width of the text based on the font size
int16_t getTextWidth(String text, uint8_t fontsize)
{
  int16_t charWidth = 6; // Width of one character (font size 1)
  return text.length() * charWidth * fontsize;
}

// Setup function for initializing the display and serial communication
void setup()
{
  Serial.begin(115200);            // Start serial communication at 115200 baud rate
  dma_display.begin();             // Initialize the LED matrix display
  dma_display.setBrightness8(125); // Set the display brightness
  dma_display.setTextWrap(false);  // Disable text wrapping
}

// Function to display scrolling text with the specified settings
void displayText(String text, int repeat, int speed)
{
  dma_display.clearScreen();           // Clear the display
  dma_display.setTextColor(textColor); // Set the text color
  dma_display.setTextSize(fontsize);   // Set the font size

  int16_t textWidth = getTextWidth(text, fontsize); // Get the width of the text

  // Adjust width for border spacing (2px padding around text)
  int16_t borderPadding = 4; // 2px padding on each side
  if (border)
  {
    textWidth += borderPadding; // Add padding for border
  }

  int16_t x = PANEL_RES_X;                            // Initial x position (start from the right side of the screen)
  int16_t y = (PANEL_RES_Y - (8 * fontsize)) / 2 + 1; // Center the text vertically

  // Start text animation
  bool isMoving = true;
  int movingTime = 0;

  while (isMoving)
  {
    dma_display.clearScreen(); // Clear the screen each time to update
    if (border)
    {
      // Draw border with padding
      dma_display.drawRect(1, 0, PANEL_RES_X - 1, PANEL_RES_Y, borderColor); // Draw border
    }

    // Adjust the x position to account for border padding
    int16_t textX = x - borderPadding / 2;

    dma_display.setCursor(textX, y); // Set cursor position for text
    dma_display.print(text);         // Display the text

    // Move the text based on the direction
    if (direction == "left")
    {
      x -= 1; // Move left
      if (x < -textWidth)
      {
        x = PANEL_RES_X; // Reset the text position when it moves off-screen
        movingTime++;
        if (movingTime == repeat)
        {
          isMoving = false;          // Stop after the specified number of repetitions
          x = PANEL_RES_X;           // Reset position
          dma_display.clearScreen(); // Clear the screen
        }
      }
    }
    if (direction == "right")
    {
      x += 1; // Move right
      if (x > PANEL_RES_X)
      {
        x = -textWidth; // Reset the position when text moves off-screen
        movingTime++;
        if (movingTime == repeat)
        {
          isMoving = false;          // Stop after the specified number of repetitions
          x = PANEL_RES_X;           // Reset position
          dma_display.clearScreen(); // Clear the screen
        }
      }
    }

    delay(speed | 50); // Delay to control the speed of text movement
    if (Serial.available() > 0)
      break; // Stop the animation if new data is received via serial
  }
}

// Function to parse the JSON configuration sent via serial
void parseJson(String json)
{
  // Create a JSON document to store the parsed data
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.println("JSON Parse Error");
    return; // Return if there was an error parsing the JSON
  }

  // Parse individual fields from the JSON document
  String text = doc["text"] | "";        // Text to display
  direction = doc["direction"] | "left"; // Direction of the scrolling text ('left' or 'right')
  JsonArray color = doc["color"];        // RGB values for the text color
  textColor = dma_display.color565(color[0] | 255, color[1] | 255, color[2] | 255);
  JsonArray bColor = doc["border_color"]; // RGB values for the border color
  borderColor = dma_display.color565(bColor[0] | 255, bColor[1] | 255, bColor[2] | 255);
  border = doc["border"] | false; // Enable or disable the border
  fontsize = doc["fontsize"] | 1; // Font size (default: 1)
  repeat = doc["repeat"] | 1;     // Number of repetitions
  speed = doc["speed"] | 50;      // Speed of the scrolling text (in ms)

  // If the text is non-empty, call the function to display the text
  if (text != "")
  {
    displayText(text, repeat, speed);
  }
}

// Main loop function to listen for JSON input from the serial interface
void loop()
{
  if (Serial.available() > 0)
  {
    // Read the incoming JSON string from the serial interface
    String json = Serial.readStringUntil('\n');
    parseJson(json); // Parse and apply the JSON configuration
  }
}
