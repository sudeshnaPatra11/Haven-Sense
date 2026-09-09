#include <Wire.h>
#include <DHT.h>
#include <Adafruit_MLX90614.h>
#include <MPU6050.h>
#include <Adafruit_NeoPixel.h>
#include "driver/i2s.h"
#include <DFRobotDFPlayerMini.h>

// ================= SENSOR PINS =================

#define DHTPIN 4
#define DHTTYPE DHT22

#define FSR_PIN 34
#define PIR_PIN 14

#define TRIG_PIN 32
#define ECHO_PIN 33

// ================= NEOPIXEL =================

#define LED_PIN 27
#define NUM_LEDS 10

// ================= I2S MICROPHONE =================

#define I2S_WS 13
#define I2S_SD 35
#define I2S_SCK 12

// ================= DFPLAYER =================

#define DF_RX 16
#define DF_TX 17

// ================= OBJECTS =================

DHT dht(DHTPIN, DHTTYPE);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MPU6050 mpu;

Adafruit_NeoPixel strip(
  NUM_LEDS,
  LED_PIN,
  NEO_GRB + NEO_KHZ800
);

HardwareSerial dfSerial(1);
DFRobotDFPlayerMini dfPlayer;

// ================= SENSOR VARIABLES =================

float temperature;
float humidity;
float bodyTemp;
float distance;

int pressureValue;
int soundLevel;
int motionDetected;

// ================= I2S SETUP =================

void setupI2S()
{
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(
    I2S_NUM_0,
    &i2s_config,
    0,
    NULL
  );

  i2s_set_pin(
    I2S_NUM_0,
    &pin_config
  );
}

// ================= READ MICROPHONE =================

int readSoundLevel()
{
  int32_t sample = 0;
  size_t bytesRead = 0;

  i2s_read(
    I2S_NUM_0,
    &sample,
    sizeof(sample),
    &bytesRead,
    portMAX_DELAY
  );

  sample = abs(sample);

  return sample / 1000;
}

// ================= SET LED COLOR =================

void setColor(int r, int g, int b)
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    strip.setPixelColor(
      i,
      strip.Color(r, g, b)
    );
  }

  strip.show();
}

// ================= SETUP =================

void setup()
{
  Serial.begin(115200);

  // Sensor pins
  pinMode(PIR_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // I2C
  Wire.begin();

  // DHT22
  dht.begin();

  // MLX90614
  mlx.begin();

  // MPU6050
  mpu.initialize();

  // I2S microphone
  setupI2S();

  // NeoPixel
  strip.begin();
  strip.show();

  // Startup LED
  setColor(0, 255, 0);
  delay(1000);
  setColor(0, 0, 0);

  // DFPlayer
  dfSerial.begin(
    9600,
    SERIAL_8N1,
    DF_RX,
    DF_TX
  );

  if (dfPlayer.begin(dfSerial))
  {
    Serial.println("DFPlayer connected");

    dfPlayer.volume(20);
    dfPlayer.EQ(DFPLAYER_EQ_NORMAL);
  }
  else
  {
    Serial.println("DFPlayer connection failed");
  }

  Serial.println();
  Serial.println("================================");
  Serial.println("     HAVEN SENSE SYSTEM");
  Serial.println("================================");
  Serial.println("System Started");
}

// ================= MAIN LOOP =================

void loop()
{
  readSensors();

  int predictedEmotion = detectEmotion();

  applyEmotion(predictedEmotion);

  printStatus(predictedEmotion);

  delay(3000);
}

// ================= READ ALL SENSORS =================

void readSensors()
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("DHT22 ERROR");
  }

  bodyTemp = mlx.readObjectTempC();

  pressureValue = analogRead(FSR_PIN);

  motionDetected = digitalRead(PIR_PIN);

  distance = readDistance();

  soundLevel = readSoundLevel();
}

// ================= ULTRASONIC =================

float readDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );

  if(duration == 0)
  {
    return 0;
  }

  return duration * 0.034 / 2;
}

// ================= EMOTION DETECTION =================
//
// 0 = Calm
// 1 = Stress
// 2 = Anxiety
// 3 = Distress
//

int detectEmotion()
{
  if(soundLevel > 2000)
  {
    return 2;
  }

  if(pressureValue > 2000)
  {
    return 1;
  }

  if(motionDetected == 0)
  {
    return 3;
  }

  return 0;
}

// ================= EMOTION RESPONSE =================

void applyEmotion(int emotion)
{
  switch(emotion)
  {
    // CALM
    case 0:

      setColor(0, 0, 255);

      // Track 1 = calm audio
      dfPlayer.play(1);

      Serial.println("Emotion: CALM");

      break;


    // STRESS
    case 1:

      setColor(255, 255, 0);

      // Track 2 = stress audio
      dfPlayer.play(2);

      Serial.println("Emotion: STRESS");

      break;


    // ANXIETY
    case 2:

      setColor(255, 0, 0);

      // Track 3 = anxiety audio
      dfPlayer.play(3);

      Serial.println("Emotion: ANXIETY");

      break;


    // DISTRESS
    case 3:

      setColor(80, 0, 120);

      // Track 4 = distress audio
      dfPlayer.play(4);

      Serial.println("Emotion: DISTRESS");

      break;
  }
}

// ================= SERIAL STATUS =================

void printStatus(int emotion)
{
  Serial.println();
  Serial.println("----------- SENSOR DATA -----------");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity    : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Body Temp   : ");
  Serial.print(bodyTemp);
  Serial.println(" °C");

  Serial.print("Pressure    : ");
  Serial.println(pressureValue);

  Serial.print("Sound       : ");
  Serial.println(soundLevel);

  Serial.print("Motion      : ");
  Serial.println(motionDetected);

  Serial.print("Distance    : ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Emotion     : ");
  Serial.println(emotion);

  Serial.println("-----------------------------------");
}
