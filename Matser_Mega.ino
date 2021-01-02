// Programme de Master : Arduino Mega
/* Pour ne pas oublier deux chose :
  1- stop condition of Wire.Transmission is writed 2 times sinon la reception ne se fait pas
  2- il faut un delay imperatif lors de l'initalisation de STATE dans le code de slave
*/

#include <Wire.h>
#define SLAVE_ADDR  9

byte data[50] = {0};
int sizeofdata = 0 ;
char Resp = 'V';
char code = 'V';  // V = Vide ; initial value
                  // if code = 'H' => One check => response from Slave is H
                  // if code = 'M' => Start Measure => No response from Slave
                  // if code = 'S' => Get state => Response is 'N' if Not Ready
                  //                                        or 'R' if Ready
                  // if code = 'G' => Get Measure => Response is a flux of data

void setup() {
  // start I2C Bus as Master:
  Wire.begin();

  Serial.begin(9600);
  Serial.println("I2C Master Demonstration");
}

void loop() {
  delay(50);
  code = Serial.read();

  if (code == 'H'){   // One check Code
    Serial.println("Begin Transmission of 'H' code");
    // Start transfer of the code
    Wire.beginTransmission(SLAVE_ADDR); // start communication with slave
    Wire.write(code);                   // send the code byte
    Wire.endTransmission();             // end of Transmission
    Wire.endTransmission();

    Serial.println("End of Transmission and Begin Receive Response");
    // Receive the response from the slave
    
    Wire.requestFrom(SLAVE_ADDR,1);     // demand for a request
    while (Wire.available()){          
      Resp = Wire.read();               // Read the response from Slave
      Serial.print("End of Receive and the Response is : ");
      Serial.println(Resp);
    }
    }

    if (code == 'M'){   // Start Measure Code
      Serial.println("Begin Transmission of 'M' code");

      // Start transfer of the code
      Wire.beginTransmission(SLAVE_ADDR); // start communication with slave
      Wire.write(code);                   // send the code byte
      Wire.endTransmission();             // end of Transmission
      Wire.endTransmission(); 

      Serial.println("End of Transmission, No response will be arrived");
    }

    if (code == 'S'){   // Get state of Measure Code
      Serial.println("Begin Transmission of 'S' code");
      // Start transfer of the code
      Wire.beginTransmission(SLAVE_ADDR); // start communication with slave
      Wire.write(code);                   // send the code byte
      Wire.endTransmission();             // end of Transmission
      Wire.endTransmission(); 
      
      Serial.println("Receive Response");
      Wire.requestFrom(SLAVE_ADDR,1);     // demand for a request
      while (Wire.available()){          
        Resp = Wire.read();               // Read the response from Slave
       }
       
      if (Resp == 'N'){
        Serial.println("Measure is not Ready");
      }
      else if (Resp == 'R'){
        Serial.println("Measure is Ready");
      }
      else {
        Serial.print("Error! Code Receive is : ");
        Serial.println(Resp);
      }
    }

    if (code == 'G'){
      if (Resp == 'R')  // if the data is ready then we can demand data
      {
        Serial.println("Please enter the size of data (in 1 to 40) : ");
        sizeofdata =(uint8_t) Serial.read();
        // start of transfer the code byte
        Serial.println("Start transfer of the code");
        Wire.beginTransmission(SLAVE_ADDR); // start communication with slave
        Wire.write(code);                   // send the code byte
        Wire.endTransmission();             // end of Transmission
        Wire.endTransmission(); 
        
        // Receive the data
        Serial.println("Receiving data");
        Wire.requestFrom(SLAVE_ADDR,sizeofdata);
        byte i = 0;
        while(Wire.available()){
          data[i] = Wire.read();
          Serial.println(data[i]);
          i++;
        }
       Serial.println("All Measure request has been received");
      }

      else if (Resp == 'N'){  // Measure is not Ready, Ask for state again
        Serial.println("Measure is not Ready, Please Ask for the state again");
      }

      else{   // Resp = autre que (N,R) 
        Serial.println("Error! Please Ask for the state of Measure!");
      }
    }
}
