### Quick Setup Guide:

1. **Connect the Huidu WF2 (ESP32-S3) to Your Computer:**
   - Use a USB cable to connect the Huidu WF2 (ESP32-S3) to your computer.
   
2. **Build and Upload the Program:**
   - Open the program in your Arduino IDE or PlatformIO.
   - Select the correct board (ESP32-S3) and the appropriate port for your device.
   - Click "Build" and then "Upload" to send the program to the ESP32-S3.

3. **Run the Program:**
   - Once uploaded, the program will not display anything initially.
   - The display will only show scrolling text when it receives a JSON configuration via the serial interface.

### Customizing the Display (via JSON):

To display text, you must send a JSON configuration to the device through the serial monitor.

#### Example JSON to display text:
```json
{
  "text": "Hello, World!",
  "direction": "left",
  "color": [255, 0, 0],
  "border_color": [0, 0, 255],
  "border": true,
  "fontsize": 2,
  "repeat": 3,
  "speed": 100
}
```

### How to Send JSON :

1. Open the Serial Monitor (set the baud rate to 115200).
2. Send the JSON string (like the example above) through the serial monitor.

Once the JSON is received, the text will scroll on the Huidu WF2 based on the settings in the JSON.
