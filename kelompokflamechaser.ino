#include <Wire.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>

// Konfigurasi SD Card
#define SD_CS_PIN 5 // Pin CS untuk modul SD card
File logFile;

// Konfigurasi RTC
RTC_DS3231 rtc;


// Variabel untuk interval waktu dan penanda waktu terakhir
 unsigned long lastLogTime = 0;               // Waktu terakhir data disimpan
const unsigned long logInterval = 10 * 60 * 1000; // Interval log 10 menit (600000 ms)

// Konfigurasi DHT11
#define DHT_PIN 15     // Pin untuk sensor DHT11
#define DHT_TYPE DHT11 // Tipe sensor DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Konfigurasi OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Bitmap image stored in PROGMEM
const unsigned char epd_bitmap_WhatsApp_Image_2024_12_24_at_16 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x55, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0xa2, 0x22, 0x22, 0x22, 0x22, 0x22, 
	0x22, 0x22, 0x22, 0x22, 0x22, 0x24, 0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x3e, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x14, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0xfc, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0xf8, 0x04, 0x08, 0x44, 0x44, 0x44, 0x44, 0x44, 
	0x44, 0x44, 0x44, 0x44, 0x44, 0x00, 0x08, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0xf1, 0x55, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x94, 0x07, 0xe0, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x10, 0x57, 0xfe, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x22, 0x04, 0x2a, 0x00, 0x80, 0x84, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x01, 0x1f, 0x80, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 
	0x22, 0x22, 0x22, 0x22, 0x10, 0x40, 0xa4, 0x00, 0x1f, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x02, 0x00, 0x05, 0x43, 0x80, 0x3f, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x7f, 0x80, 0xbe, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x0f, 0x00, 0x7e, 0x00, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x0f, 0x00, 0x7a, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xf8, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x22, 0x28, 0x00, 0x02, 0x01, 0xf8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x20, 0x01, 0x04, 0x01, 0xf1, 0x80, 0x24, 0x01, 0x08, 0x88, 0x88, 0x88, 
	0x88, 0x88, 0x88, 0x00, 0xa0, 0x46, 0x04, 0x07, 0xe0, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x08, 0x03, 0xe0, 0x04, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x04, 0x21, 0x00, 0x08, 0x07, 0xc0, 0x01, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x80, 0x01, 0x08, 0x10, 0x17, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x08, 0x01, 0x50, 0x10, 0x0f, 0xe0, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x10, 0x60, 0x10, 0x00, 0x20, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x04, 0x10, 0x10, 0xe0, 0x28, 0x00, 0x7e, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x24, 0x00, 0xfc, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x10, 0x08, 0x40, 0x2e, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x76, 0x07, 0xf0, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0a, 0x08, 0x80, 0x0c, 0x57, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x20, 0x80, 0x0d, 0x0f, 0xc8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0x00, 0x02, 0xa4, 0x0a, 0xac, 0x1f, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x08, 0x3f, 0x0e, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x7e, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x10, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0xf8, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x13, 0xf0, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x40, 0x00, 0x00, 0x00, 0x10, 0x27, 0xe0, 0x00, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6f, 0xc0, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x5f, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x0a, 0x02, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20
};

void setup() {
  // Inisialisasi komunikasi serial
  Serial.begin(9600);
  while (!Serial) {
    ; // Tunggu koneksi serial untuk port USB native
  }

  // Inisialisasi kartu SD
  Serial.print("Menginisialisasi kartu SD...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Inisialisasi kartu SD gagal!");
    while (1);
  }
  Serial.println("Kartu SD berhasil diinisialisasi.");

  // Membuat atau membuka file log
  logFile = SD.open("/data_log.txt", FILE_APPEND);
  if (logFile) {
    logFile.println("Waktu, Suhu (C), Kelembapan (%)"); // Header file
    logFile.close();
    Serial.println("File log berhasil dibuat/dibuka.");
  } else {
    Serial.println("Gagal membuka file log.");
  }

  // Inisialisasi RTC
  if (!rtc.begin()) {
    Serial.println("RTC tidak terdeteksi!");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC kehilangan daya, mengatur ulang waktu.");
    rtc.adjust(DateTime(F(_DATE), F(TIME_)));
  }

  // Inisialisasi OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Pastikan alamat I2C sesuai
    Serial.println(F("OLED tidak terdeteksi!"));
    while (1);
  }
  
  display.clearDisplay();
  
  // Show startup frame or image only once at the beginning
  display.drawBitmap(0, 0, epd_bitmap_WhatsApp_Image_2024_12_24_at_16, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.display();
  delay(3000);  // Show the frame for 3 seconds

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Inisialisasi DHT11
  dht.begin();
}

void loop() {
  
    // Waktu sekarang dalam milidetik
    unsigned long currentTime = millis();
    
  // Membaca data dari DHT11
  float temperature = dht.readTemperature(); // Membaca suhu dalam Celcius
  float humidity = dht.readHumidity();       // Membaca kelembapan dalam persen

  // Validasi pembacaan sensor
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Gagal membaca data dari sensor DHT11!");
    return;
  }

  // Menentukan status suhu dengan batas 30°C
  String tempStatus = (temperature >= 30.00) ? "Panas" : "Normal";

  // Membaca waktu dari RTC
  DateTime now = rtc.now();
  char timeString[20];
  sprintf(timeString, "%02d/%02d/%04d %02d:%02d:%02d",
          now.day(), now.month(), now.year(),
          now.hour(), now.minute(), now.second());

 if (currentTime - lastLogTime >= logInterval) {
        // Perbarui waktu terakhir data disimpan
        lastLogTime = currentTime;
         // Menulis data ke file SD
  logFile = SD.open("/data_log.txt", FILE_APPEND);
  if (logFile) {
      // Format penyimpanan: Waktu, Suhu, Kelembapan
      logFile.print(timeString);  // Menyimpan tanggal dan waktu
      logFile.print(", ");
      logFile.print("suhu=");
      logFile.print(temperature);  // Menyimpan suhu
      logFile.print(", ");
      logFile.print("kelembapan=");
      logFile.println(humidity);  // Menyimpan kelembapan
      logFile.close();

      // Serial monitor output
      Serial.print("Data disimpan: ");
      Serial.print(timeString);
      Serial.print(", suhu=");
      Serial.print(temperature);
      Serial.print(", kelembapan=");
      Serial.println(humidity);
    } else {
      Serial.println("Gagal membuka file untuk menulis.");
    }
  }

  // Tunggu sebentar sebelum pembacaan berikutnya

  // Menampilkan informasi ke OLED
  display.clearDisplay();

  // Menampilkan judul "SENSOR DHT11" di tengah layar
  int16_t x1, y1;
  uint16_t width, height;

  display.setTextSize(1);
  display.getTextBounds("SENSOR DHT11", 0, 0, &x1, &y1, &width, &height);
  display.setCursor((SCREEN_WIDTH - width) / 2, 0);
  display.println("SENSOR DHT11");

  // Menampilkan suhu di tengah layar
  String tempString = "Suhu: " + String(temperature, 1) + " °C";
  display.getTextBounds(tempString, 0, 0, &x1, &y1, &width, &height);
  display.setCursor((SCREEN_WIDTH - width) / 2, 15);
  display.println(tempString);

  // Menampilkan kelembapan di tengah layar
  String humString = "Kelembapan: " + String(humidity, 1) + " %";
  display.getTextBounds(humString, 0, 0, &x1, &y1, &width, &height);
  display.setCursor((SCREEN_WIDTH - width) / 2, 30);
  display.println(humString);

  // Menampilkan status suhu di tengah layar
  String statusString = "Status: " + tempStatus;
  display.getTextBounds(statusString, 0, 0, &x1, &y1, &width, &height);
  display.setCursor((SCREEN_WIDTH - width) / 2, 45);
  display.println(statusString);

  String timeString2 = String(timeString);
  display.getTextBounds(timeString2, 0, 0, &x1, &y1, &width, &height);
  display.setCursor((SCREEN_WIDTH - width) / 2, 55);
  display.println(timeString2);

  display.display();

  // Tunggu 1 detik sebelum membaca lagi
  delay(1000);
}