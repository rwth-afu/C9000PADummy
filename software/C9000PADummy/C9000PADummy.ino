#include <SoftIIC.h>

#include <EEPROM.h>

/* 
Watt Value from DG5MM measurement
25  'B' 0x42  66  0b01000010
38  'T' 0x54  84  0b01010100
40            86
63      0xbd  189 0b10111001
165           190 0b10111010
166           191 0b10111011
168           192 0b10111100
163 'o' 0x6f  111 0b01101111
*/

/*I2C Analysis:
Setup Write to ['148'] + ACK
'5' + ACK
Setup Read to ['149'] + ACK
'5' + ACK
'0' + ACK
'0' + ACK
B + NAK
Setup Write to ['150'] + ACK
'5' + ACK
Setup Read to ['151'] + ACK
L + ACK
'255' + ACK
0 + ACK
M + NAK
Setup Write to ['152'] + ACK
'5' + ACK
Setup Read to ['153'] + ACK
D + ACK
D + ACK
D + ACK
D + NAK
*/

// Note: these can be *any* pins, not just a4/a5.
#define SDA_PIN  A4
#define SCL_PIN  A5

#define SERIAL_PORT_SPEED 38400

// note: speed is in KHz, ~10-100 is reasonable.                        
#define IIC_SPEED 50

#define CHIPADDR1 74    // In analysis Write: 148, Read 149, so address is 148/2
#define CHIPADDR2 75    // In analysis Write: 150, Read 151, so address is 150/2
#define CHIPADDR3 76    // In analysis Write: 152, Read 153, so address is 152/2

#define CHIPREG1  5
#define CHIPREG2  5
#define CHIPREG3  5

#define EEPROMFIRSTRUN   0x02
#define EEPROMTXPWRADDR  0x01

static uint8_t current_register_address_for_1 = 0x00;
static uint8_t current_register_address_for_2 = 0x00;
static uint8_t current_register_address_for_3 = 0x00;

static uint8_t txpower = 192;
static uint16_t runs = 0x00;

const uint8_t ANSWER1[]             = {0x05, 0x00, 0x00, 192};
const uint8_t ANSWER2[]             = {'L', 0xff, '0', 'M',};
const uint8_t ANSWER3[]             = {'D', 'D', 'D', 'D'};

// SoftIIC(uint8_t pin_scl, uint8_t pin_sda, uint16_t speed, bool pullups, bool multimastersupport, bool timeout);
SoftIIC  my_SoftIIC = SoftIIC(SCL_PIN, SDA_PIN, IIC_SPEED, false, false, false);
  
void setup() {
  Serial.begin(SERIAL_PORT_SPEED);
  if ((EEPROM.read(EEPROMFIRSTRUN) == 0x00) || (EEPROM.read(EEPROMFIRSTRUN) == 0xff))
  {
    // First run, so set default value of txpower
    setNewTXPower(192);
    EEPROM.update(EEPROMFIRSTRUN, 0xAB);
  }
  else
  {
    txpower = EEPROM.read(EEPROMTXPWRADDR);
  }
  noInterrupts();
}


void loop() {
  my_SoftIIC.SlaveHandleTransaction(
    respond_to_address,
    respond_to_command,
    respond_to_data,
    get_current_register_address,
    set_current_register_address,
    read_iic_slave,
    write_iic_slave);
//    if (runs < 100) { runs ++; } else { runs = 0; txpower++; }

  // NEW: Check if UART has received a byte
  if (UCSR0A & _BV(RXC0)) {
    // Read and clear pending RX Flag automatically
    uint8_t receivedByte = UDR0;
    setNewTXPower(receivedByte);
 
  }

//  delay(10000);
}

void setNewTXPower(uint8_t newtxpower)
{
  txpower = newtxpower;
  EEPROM.update(EEPROMTXPWRADDR, newtxpower);
}



//////////////////////////////////////////////////////////// These functions should be edited to give the iic slave a 'personality'. ////////////////////////////////////////////////////////////////



uint8_t generateanswer(uint8_t chipaddress, uint8_t registeraddress) {
  uint8_t retval = 0xFF;
  if (chipaddress == CHIPADDR1 && (registeraddress - CHIPREG1) == 3) { return txpower; }
  if (chipaddress == CHIPADDR1 && (registeraddress - CHIPREG1 - 1) < (sizeof(ANSWER1) / sizeof(uint8_t))) {    retval = ANSWER1[registeraddress - CHIPREG1];  }
  if (chipaddress == CHIPADDR2 && (registeraddress - CHIPREG2) < (sizeof(ANSWER2) / sizeof(uint8_t))) {    retval = ANSWER2[registeraddress - CHIPREG2];  }
  if (chipaddress == CHIPADDR3 && (registeraddress - CHIPREG3) < (sizeof(ANSWER3) / sizeof(uint8_t))) {    retval = ANSWER3[registeraddress - CHIPREG3];  }
  return retval;
}


uint8_t respond_to_address(uint8_t chipaddr){  
  if((chipaddr>>1)==CHIPADDR1) {return 0x01;}
  if((chipaddr>>1)==CHIPADDR2) {return 0x01;}
  if((chipaddr>>1)==CHIPADDR3) {return 0x01;}
 return 0x00;  
}


uint8_t respond_to_command(uint8_t commandaddr){  
 return 0x01;  
}


uint8_t respond_to_data(uint8_t commandaddr){  
 return 0x01;  
}


uint8_t get_current_register_address(uint8_t chipaddr) {
  if (chipaddr == CHIPADDR1) {    return current_register_address_for_1;  }
  if (chipaddr == CHIPADDR2) {    return current_register_address_for_2;  }
  if (chipaddr == CHIPADDR3) {    return current_register_address_for_3;  }
  return 0x00;
}


uint8_t set_current_register_address(uint8_t chipaddr, uint8_t regaddr) {
  if (chipaddr == CHIPADDR1) {    current_register_address_for_1 = regaddr;  }
  if (chipaddr == CHIPADDR2) {    current_register_address_for_2 = regaddr;  }
  if (chipaddr == CHIPADDR3) {    current_register_address_for_3 = regaddr;  }
  return 0x00;
}

uint8_t read_iic_slave(uint8_t chipaddress, uint8_t* value) {
  uint8_t registeraddress = get_current_register_address(chipaddress);
  *value = generateanswer( chipaddress, registeraddress);
  return 0x00;
}

uint8_t write_iic_slave(uint8_t chipaddr, uint8_t value) {
  // Don't do anything with writes.
  return 0x00;
}
