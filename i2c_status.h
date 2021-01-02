/*
 * i2c_status.h
 *
 * Created: 25/12/2020 02:55:42
 *  Author: HP
 */ 


#ifndef I2C_STATUS_H_
#define I2C_STATUS_H_

// SLAVE RECEIVER (SR) CODES
#define SR_SLA_ACK				0x60			  // slave address receive and ack send SLA+W
#define SR_ARB_LOST_SLA_ACK		0x68			  //Arbitration lost in SLA+R/W and ack send
#define SR_GEN_CALL_ACK			0x70			  // General call address received and ack send
#define SR_DATA_ACK				0x80		      // data received With SLA_ACK addressable and ack send
#define SR_DATA_NACK			0x88			  // data received and No ack send
#define SR_GEN_CALL_DATA_ACK	0x90			  // general call data receive and ack send
#define SR_GEN_CALL_DATA_NACK	0x98			  // general call data receive and NO ack send

// SLAVE TRANSMETTER (ST) CODES
#define ST_SLA_ACK			0xA8			  //slave address receive and ack send SLA+R
#define ST_ARB_LOST_SLA_ACK	0xB0			  //Arbitration lost in SLA+R/W and ack send
#define ST_DATA_ACK			0xB8			  // data TRANSMETTED  and ack RECEIVED
#define ST_DATA_NACK		0xC0			  // data TRANSMETTED and NOT ack RECEIVED
#define ST_DATA_ACK_TWEA0	0xC8			  // Data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
#define TW_STOP				0xA0

const char MSG0[] = "\n\rIN INTERRUPT VECTOR \n\r";
const char MSG1[] = "\n\raddress Receive with SLA_ACK IN Write Mode \n\r";
const char MSG2[] = "\n\raddress Receive with Arbitration lost in SLA+R/W \n\r";
const char MSG3[] = "\n\rGeneral call receive \n";
const char MSG4[] = "\n\rData received with SLA_ACK addressable and Code is : \n\r";
const char MSG5[] = "\n\rData received with SLA_NACK addressable and data is \n\r";
const char MSG6[] = "\n\rData received with GEN_CALL_ACK addressable and data is \n\r";
const char MSG7[] = "\n\rData received with GEN_CALL_NACK addressable and data is \n\r";
const char MSG8[] = "\n\rNone of previous code has been arrived \n\r";
const char MSG9[] = "\n\raddress Receive with SLA_ACK IN READ Mode and DATA has been charged in TWDR\n\r";
const char MSG10[] = "\n\rFirst Data has been TRANSMETED with SLA_ACK addressable \n\r";
const char MSG11[] = "\n\rData has been TRANSMETED with SLA_ACK addressable \n\r";
const char MSG12[] = "\n\rLast Data in memory has been TRANSMETED; TWDR = 'L' if the master send ACK \n\r"; // L : to tell Master that was the last data and stop demand
const char MSG13[] = "\n\rData has been TRANSMETED and NOT ACK has been received \n\r";
const char MSG14[] = "\n\rData byte in TWDR has been transmitted ; TWEA = 0 ; ACK has been received \n\r";
const char MSG15[] = "\n\rA Stop condition arrived \n\r";
const char MSG16[] = "\n\rDifferent Code has been arrived \n\r";

#endif /* I2C_STATUS_H_ */