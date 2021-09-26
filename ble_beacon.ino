#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEBeacon.h>
#include <esp_sleep.h>
#include <sys/time.h>


#define SERVICE_UUID "3feb1e8a-3981-4045-ad39-b225135013a0"
#define CONTROL_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define LED 23
char ledStatus = 48; 

#define GPIO_DEEP_SLEEP_DURATION     10  // sleep x seconds and then wake up
RTC_DATA_ATTR static time_t last;        // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

#ifdef __cplusplus
extern "C" {
#endif

uint8_t temprature_sens_read();
//uint8_t g_phyFuns;

#ifdef __cplusplus
}
#endif


BLEAdvertising *pAdvertising;
struct timeval now;

#define BEACON_UUID           "8ec76ea3-6668-48da-9866-75be8bc86f4d" 

BLECharacteristic controlCharacteristic(
CONTROL_CHARACTERISTIC_UUID,
BLECharacteristic::PROPERTY_READ |
BLECharacteristic::PROPERTY_WRITE
);

void setBeacon() {

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  oBeacon.setMajor((bootcount & 0xFFFF0000) >> 16);
  oBeacon.setMinor(bootcount&0xFFFF);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  
  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
  
  std::string strServiceData = "";
  
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData(); 
  oAdvertisementData.addData(strServiceData);
  
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
  pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);

}

void setup() {
Serial.begin(115200);
Serial.println("Starting BLE!");
Serial.println("Initializing device");
BLEDevice::init("Led control electrosoftcloud"); // Initializing the device with its name
Serial.println("Creating server");
BLEServer *pServer = BLEDevice::createServer(); // Create the server
Serial.println("Adding service UUID");
BLEService *pService = pServer->createService(SERVICE_UUID); // Creating a new service into server
// Adding a characteristic with the object name (official UUID), without object (this characteristic will not change)
Serial.println("Adding name characteristic");
BLECharacteristic *nameCharacteristic = pService->createCharacteristic(
BLEUUID((uint16_t)0x2A00),
BLECharacteristic::PROPERTY_READ
);
nameCharacteristic->setValue("Led");
// Adding a characteristic to control the led with 0 and 1
Serial.println("Adding control characteristic");
pService->addCharacteristic(&controlCharacteristic);
controlCharacteristic.setValue(&ledStatus); // Value uint8_t with length 1

Serial.println("Starting...");
pService->start();
Serial.println("Creating advertising");
// BLEAdvertising *pAdvertising = pServer->getAdvertising(); // this still is working for backward compatibility
BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
pAdvertising->addServiceUUID(SERVICE_UUID);
pAdvertising->setScanResponse(true);
pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
pAdvertising->setMinPreferred(0x12);
BLEDevice::startAdvertising();
Serial.println("Characteristic defined! Now you can read it in your phone!");
pinMode (LED, OUTPUT); // Set the LED pin as OUTPUT
}
void loop() {
std::string controlValue = controlCharacteristic.getValue();
if (controlValue[0] != ledStatus) {
Serial.print("Value changed... new value: ");
Serial.println(controlValue[0]);
ledStatus = controlValue[0];
if (ledStatus == 48) {
digitalWrite(LED, LOW); // LED Off
}
else if (ledStatus == 49) {
Serial.begin(115200);
  gettimeofday(&now, NULL);

  Serial.printf("start ESP32 %d\n",bootcount++);

  Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n",now.tv_sec,now.tv_sec-last);

  last = now.tv_sec;
  
  // Create the BLE Device
  BLEDevice::init("");

  // Create the BLE Server
  // BLEServer *pServer = BLEDevice::createServer(); // <-- no longer required to instantiate BLEServer, less flash and ram usage

  pAdvertising = BLEDevice::getAdvertising();
  
  setBeacon();
   // Start advertising
  pAdvertising->start();
  Serial.println("Advertizing started...");
  delay(100);
  pAdvertising->stop();
  Serial.printf("enter deep sleep\n");
  esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
  Serial.printf("in deep sleep\n");
}
}
}
