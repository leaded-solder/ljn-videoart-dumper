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
const uint8_t PIN_ADDRESS_STORAGE_CLK = 12; // SRCLK
const uint8_t PIN_ADDRESS_SHIFT_CLK = 8; // RCLK
const uint8_t PIN_ADDRESS_DATA = 11; // SERial data


void setup() {
  Serial.begin(38400);
  Serial.println("LJN VideoArt ROM Dumper");

  // TODO: Set pins
  pinMode(PIN_ROM_OE, OUTPUT);
  digitalWrite(PIN_ROM_OE, HIGH);

  pinMode(PIN_DATA_DATA, INPUT);
  pinMode(PIN_DATA_LATCH, OUTPUT);
  pinMode(PIN_DATA_CLOCK, OUTPUT);

  pinMode(PIN_ADDRESS_SHIFT_CLK, OUTPUT);
  pinMode(PIN_ADDRESS_STORAGE_CLK, OUTPUT);
  pinMode(PIN_ADDRESS_DATA, OUTPUT);
}

void setOutputEnable(bool isEnabled) {
  // Assuming active low
  digitalWrite(PIN_ROM_OE, isEnabled ? LOW : HIGH);
}

void assertAddressBus(uint16_t address) {
  digitalWrite(PIN_ADDRESS_STORAGE_CLK, LOW);
  shiftOut(PIN_ADDRESS_DATA, PIN_ADDRESS_SHIFT_CLK, MSBFIRST, address);
  digitalWrite(PIN_ADDRESS_STORAGE_CLK, HIGH);
}

uint8_t clockOutData() {
  // Read the data on the data bus, assuming that
  // the output enable is already prepared
  digitalWrite(PIN_DATA_LATCH, LOW); // load into shift register

  // Read out the data from the shift register
  // Not sure if MSBFIRST or LSBFIRST should be used here, d7 is tied to d7...
  uint8_t result = shiftIn(PIN_DATA_DATA, PIN_DATA_CLOCK, MSBFIRST);

  digitalWrite(PIN_DATA_LATCH, HIGH); // stop loading into shift register

  return result;
}

void dumpCartridgeToSerial() {
  uint16_t address = 0;
  while(address < ROM_LENGTH ) {
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
