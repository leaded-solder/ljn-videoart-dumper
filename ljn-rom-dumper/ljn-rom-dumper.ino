/**
  LJN Video Art cartridge dumper
  Driver firmware to run the cartridge dumping process for a Video Art cartridge

  GNU GPL licensed by leadedsolder.com, 2023
*/

const uint16_t ROM_LENGTH = 16 * 1024; // 16K ROM assumed (LH2326xx), change this if yours is bigger/smaller

const uint8_t PIN_ROM_OE = 10;

// 74HCT165 to read data bus from cartridge
const uint8_t PIN_DATA_CLOCK = 4;
const uint8_t PIN_DATA_LATCH = 3;
const uint8_t PIN_DATA_DATA = 2; // Q7

// 74HCT595s to set address bus on cartridge
const uint8_t PIN_ADDRESS_STORAGE_CLK = 8; // RCLK
const uint8_t PIN_ADDRESS_SHIFT_CLK = 12; // SRCLK
const uint8_t PIN_ADDRESS_DATA = 11; // SERial data

void setup() {
  Serial.begin(38400);
  Serial.println("LJN VideoArt ROM Dumper");

  pinMode(PIN_ROM_OE, OUTPUT);
  digitalWrite(PIN_ROM_OE, HIGH);

  pinMode(PIN_DATA_DATA, INPUT);
  pinMode(PIN_DATA_LATCH, OUTPUT);
  digitalWrite(PIN_DATA_LATCH, HIGH);
  pinMode(PIN_DATA_CLOCK, OUTPUT);
  digitalWrite(PIN_DATA_CLOCK, LOW);

  pinMode(PIN_ADDRESS_SHIFT_CLK, OUTPUT);
  digitalWrite(PIN_ADDRESS_SHIFT_CLK, LOW);
  pinMode(PIN_ADDRESS_STORAGE_CLK, OUTPUT);
  digitalWrite(PIN_ADDRESS_STORAGE_CLK, LOW);
  pinMode(PIN_ADDRESS_DATA, OUTPUT);
  digitalWrite(PIN_ADDRESS_DATA, LOW);
}

void setOutputEnable(bool isEnabled) {
  // Confirmed active low
  digitalWrite(PIN_ROM_OE, isEnabled ? LOW : HIGH);
}

void assertAddressBus(uint16_t address) {
  digitalWrite(PIN_ADDRESS_STORAGE_CLK, LOW); // prepare to receive
  // high byte
  shiftOut(PIN_ADDRESS_DATA, PIN_ADDRESS_SHIFT_CLK, MSBFIRST, (address >> 8));
  // low byte
  shiftOut(PIN_ADDRESS_DATA, PIN_ADDRESS_SHIFT_CLK, MSBFIRST, address);
  digitalWrite(PIN_ADDRESS_STORAGE_CLK, HIGH); // latch it
}

uint8_t shiftIn165(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
  uint8_t value = 0;
  uint8_t i;

  for (i = 0; i < 8; ++i) {
    digitalWrite(clockPin, LOW);
    if (bitOrder == LSBFIRST) {
      value |= digitalRead(dataPin) << i;
    }
    else {
      value |= digitalRead(dataPin) << (7 - i);
    }
    digitalWrite(clockPin, HIGH);
    delayMicroseconds(20);
  }
  return value;
}

uint8_t clockOutData() {
  // Read the data on the data bus, assuming that
  // the output enable is already prepared
  digitalWrite(PIN_DATA_LATCH, LOW); // load into shift register
  delayMicroseconds(20);
  digitalWrite(PIN_DATA_LATCH, HIGH); // stop loading into shift register

  // Read out the data from the shift register
  // Not sure if MSBFIRST or LSBFIRST should be used here, d7 is tied to d7...
  // it does seem to fetch data...
  uint8_t result = shiftIn165(PIN_DATA_DATA, PIN_DATA_CLOCK, MSBFIRST);

  return result;
}

void dumpCartridgeToSerial() {
  uint16_t address = 0;
  while(address < 32 /*ROM_LENGTH*/ ) { // just for now
    assertAddressBus(address);
    setOutputEnable(true); // enabled

    uint8_t byte = clockOutData();
    Serial.print(byte, HEX);
    Serial.print(' ');

    setOutputEnable(false); // disabled

    ++address;
  }
}

void loop() {
  // This interface will eventually become deprecated when I do a client program?

  String commandLine = "";
  if(Serial.available()) {
    commandLine = Serial.readStringUntil('\n');
    if(commandLine.equalsIgnoreCase("dump")) {
      dumpCartridgeToSerial();
      Serial.println("");
      Serial.println("done.");
    }
    else if(commandLine.equalsIgnoreCase("info")) {
      Serial.println("Version 1");
    }
    else {
      String error = "Unknown command '";
      error.concat(commandLine);
      error.concat("'");
      Serial.println(error);
    }
  }
}