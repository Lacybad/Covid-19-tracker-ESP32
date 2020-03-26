/* *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/ 

/*______Import Libraries_______*/
#include <Arduino.h> 
#include <SPI.h>
#include <WiFi.h>
//#include <ESP8266WiFi.h> //Library for ESP8266
#include <WiFiClientSecure.h>  
#include <ArduinoHttpClient.h>
#include "Adafruit_GFX.h"
//#include "Adafruit_ILI9341.h"
#include <TFT_eSPI.h>

#define FS_NO_GLOBALS
#include <FS.h>




/*______End of Libraries_______*/

/*__Pin definitions for the Arduino MKR__*/
//#define TFT_CS   5
//#define TFT_DC   4
//#define TFT_MOSI 23
//#define TFT_CLK  18
//#define TFT_RST  22
//#define TFT_MISO 19
//#define TFT_LED  15  

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library


  #define TFT_GREY        0x5AEB
  #define TFT_BLACK       0x0000      
  #define TFT_NAVY        0x000F
  #define TFT_DARKGREEN   0x03E0      
  #define TFT_DARKCYAN    0x03EF      
  #define TFT_MAROON      0x7800      
  #define TFT_PURPLE      0x780F      
  #define TFT_OLIVE       0x7BE0      
  #define TFT_LIGHTGREY   0xC618      
  #define TFT_DARKGREY    0x7BEF      
  #define TFT_BLUE        0x001F  
  #define TFT_GREEN       0x07E0
  #define TFT_CYAN        0x07FF
  #define TFT_RED         0xF800
  #define TFT_MAGENTA     0xF81F      
  #define TFT_YELLOW      0xFFE0      
  #define TFT_WHITE       0xFFFF    
  #define TFT_ORANGE      0xFDA0      
  #define TFT_GREENYELLOW 0xB7E0  
  #define TFT_PINK        0xFC9F    

  // Don't change!!!
  //#define TFT_DC  D3
  //#define TFT_CS  D8
  //#define TFT_MOSI D7
  //#define TFT_SCK D5
  //#define TFT_RST D4

  //For ESP32
  //#define TFT_MISO 19
  //#define TFT_MOSI 23
  //#define TFT_SCLK 18
  //#define TFT_CS    15  // Chip select control pin
  //#define TFT_DC    2  // Data Command control pin
  //#define TFT_RST   4  // Reset pin (could connect to RST pin)
  //#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is 

  #define ESP32_PARALLEL

  // The ESP32 and TFT the pins used for testing are:
  #define TFT_CS   33  // Chip select control pin (library pulls permanently low
  #define TFT_DC   15  // Data Command control pin - must use a pin in the range 0-31
  #define TFT_RST  32  // Reset pin, toggles on startup

  #define TFT_WR    4  // Write strobe control pin - must use a pin in the range 0-31
  #define TFT_RD    2  // Read strobe control pin

  #define TFT_D0   12  // Must use pins in the range 0-31 for the data bus
  #define TFT_D1   13  // so a single register write sets/clears all bits.
  #define TFT_D2   26  // Pins can be randomly assigned, this does not affect
  #define TFT_D3   25  // TFT screen update performance.
  #define TFT_D4   17
  #define TFT_D5   16
  #define TFT_D6   27
  #define TFT_D7   14




//#define HAVE_TOUCHPAD
//#define TOUCH_CS 14
//#define TOUCH_IRQ 2 
/*_______End of definitions______*/

/*____Calibrate Touchscreen_____*/
//#define MINPRESSURE 10      // minimum required force for touch event
//#define TS_MINX 370
//#define TS_MINY 470
//#define TS_MAXX 3700
//#define TS_MAXY 3600
/*______End of Calibration______*/


/*____Wifi _____________________*/
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
#define WIFI_SSID "********"       // Enter your SSID here
#define WIFI_PASS "********"    // Enter your WiFi password here
// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 2000; 
/*______End of Wifi______*/


int status = WL_IDLE_STATUS;
int infected=0;
int recovered=0;
int deaths=0;


WiFiClientSecure client;
HttpClient http(client,"www.worldometers.info", 443); 

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // init GPIOs
  //pinMode(TFT_LED, OUTPUT); // define as output for backlight control

  // initialize the TFT
  Serial.println("Init TFT ...");
  tft.begin();    
  if (!SPIFFS.begin()) {  //SPIFFS INICIALIZĂLĂSA
    Serial.println("SPIFFS error!!");
   while (1) yield(); // VĂRAKOZIK AMĂŤG NEM SIKERĂśL
  } 
      
  tft.setRotation(3);   // landscape mode  
  //tft.fillScreen(TFT_BLACK);// clear screen 
  drawBmp("/COVID19.bmp",0, 0);

  tft.setCursor(30,100);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.print("Connecting...");
//  digitalWrite(TFT_LED, LOW);    // LOW to turn backlight on; 


 // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip); 
 

}

void loop() {
 check_country("China");
 delay(2000);
 check_country("Italy");
 delay(2000); 
 check_country("Germany");
 delay(2000); 
 check_country("Spain");
 delay(2000); 
 check_country("Austria");
 delay(2000); 
 check_country("Switzerland");
 delay(2000); 
 check_country("Hungary");
 delay(10000);
 check_country("USA");
 delay(2000);
 check_country("France");
 delay(2000);
 check_country("UK");
 delay(2000);
 check_country("S.Korea");
 delay(2000);
 check_country("Iran");
 delay(2000);
 check_country("Netherlands");
 delay(2000);
 check_country("Belgium");
 delay(2000);
 check_country("Canada");
 delay(2000);
 check_country("Norway");
 delay(2000);
 check_country("Portugal");
 delay(2000);
 check_country("Australia");
 delay(2000);
 check_country("Brazil");
 delay(2000);
 check_country("Sweden");
 delay(2000);
 check_country("Israel");
 delay(2000);
 check_country("Turkey");
 delay(2000);
 check_country("Malaysia");
 delay(2000);
 check_country("Denmark");
 delay(2000);
 check_country("Ireland");
 delay(2000);
 check_country("Poland");
 delay(2000);
 check_country("Greece");
 delay(2000);
 check_country("Philippines");
 delay(2000);
 check_country("India");
 delay(2000);
 check_country("Hong Kong");
 delay(2000);
 check_country("Iraq");
 delay(2000);
 check_country("Algeria");
 delay(2000);
 check_country("Finland");
 delay(2000);
 check_country("Russia");
 delay(2000);
 check_country("Slovenia");
 delay(2000);
 check_country("Croatia");
 delay(2000);
 check_country("Serbia");
 delay(2000);
 check_country("Lithuania");
 delay(2000);
 check_country("Slovakia");
 delay(2000);
 check_country("Latvia");
 delay(2000);
 check_country("Romania");
 delay(2000);
 check_country("Ukraine");
 delay(2000);
 check_country("Belarus");
 delay(2000);
}


void draw_country_screen(String sCountry){
  //tft.fillScreen(TFT_BLACK);// clear screen
  drawBmp("/COVID19.bmp",0, 0);
  // headline
  tft.setCursor(10,10);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(3);
  tft.print(sCountry + ":");

  // infected
  tft.setCursor(10,70);
  tft.setTextColor(TFT_RED);
  tft.print("Infected:");
  tft.setCursor(200,70);
  tft.print(infected);

  // recovered
  tft.setCursor(10,130);
  tft.setTextColor(TFT_GREEN);
  tft.print("Recovered:");
  tft.setCursor(200,130);
  tft.print(recovered);

  // deaths
  tft.setCursor(10,190);
  tft.setTextColor(TFT_BLUE);
  tft.print("Deaths:");
  tft.setCursor(200,190);
  tft.print(deaths); 
      
}

void check_country(String sCountry) {
  int err =0;
  int readcounter = 0;
  int read_value_step = 0;
  String s1 = "";
  String s2 = "";
  
  err = http.get("/coronavirus/country/" + sCountry +"/");
  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      // If you are interesting in the response headers, you
      // can read them here:
      //while(http.headerAvailable())
      //{
      //  String headerName = http.readHeaderName();
      //  String headerValue = http.readHeaderValue();
      //}

      Serial.print("Request data for ");
      Serial.println(sCountry);
    
      // Now we've got to the body, so we can print it out
      unsigned long timeoutStart = millis();
      char c;
      // Whilst we haven't timed out & haven't reached the end of the body
      while ( (http.connected() || http.available()) &&
             (!http.endOfBodyReached()) &&
             ((millis() - timeoutStart) < kNetworkTimeout) )
      {
          if (http.available())
          {
              c = http.read();
              s2 = s2 + c;
              if (readcounter < 255) {
                readcounter++;
              } else {
                readcounter = 0;
                String tempString = "";
                tempString.concat(s1);
                tempString.concat(s2);
                // check infected first 
                if (read_value_step == 0) {                               
                  int place = tempString.indexOf("Coronavirus Cases:");
                  if ((place != -1) && (place < 350)) { 
                    read_value_step = 1;
                    s2 = tempString.substring(place + 15);
                    tempString = s2.substring(s2.indexOf("#aaa") + 6);
                    s1 = tempString.substring(0, (tempString.indexOf("</")));
                    s1.remove(s1.indexOf(","),1);  
                    Serial.print("Coronavirus Cases: ");
                    Serial.println(s1);
                    infected = s1.toInt();
                  }
                  
                }
                // check deaths               
                if (read_value_step == 1) {
                  int place = tempString.indexOf("Deaths:");
                  if ((place != -1) && (place < 350)) { 
                    read_value_step = 2;
                    s2 = tempString.substring(place + 15);
                    tempString = s2.substring(s2.indexOf("<span>") + 6);
                    s1 = tempString.substring(0, (tempString.indexOf("</")));
                    s1.remove(s1.indexOf(","),1);  
                    Serial.print("Deaths: ");
                    Serial.println(s1);
                    deaths = s1.toInt();
                  }
                }                
                // check recovered               
                if (read_value_step == 2) {
                  int place = tempString.indexOf("Recovered:");
                  if ((place != -1) && (place < 350)) {                   
                    s2 = tempString.substring(place + 15);
                    tempString = s2.substring(s2.indexOf("<span>") + 6);
                    s1 = tempString.substring(0, (tempString.indexOf("</")));
                    s1.remove(s1.indexOf(","),1);  
                    Serial.print("Recovered: ");
                    Serial.println(s1);
                    recovered = s1.toInt();
                    draw_country_screen(sCountry);
                    http.stop();
                    return;
                  }
                }                
      
                s1 = s2;
                s2 = ""; 
              }              
              
              // We read something, reset the timeout counter
              timeoutStart = millis();
          }
          else
          {
              // We haven't got any data, so let's pause to allow some to
              // arrive
              delay(kNetworkDelay);
          }
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
// Bodmers BMP image rendering function

void drawBmp(const char *filename, int16_t x, int16_t y) {

  if ((x >= tft.width()) || (y >= tft.height())) return;

  fs::File bmpFS;

  // Open requested file on SD card
  bmpFS = SPIFFS.open(filename, "r");

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
     // Serial.print("Loaded in "); Serial.print(millis() - startTime);
     // Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

