/*!
 * nextion_ez.cpp - Easy library for Nextion Displays
 * Copyright (c) 2022 Charles Current
 * Copyright (c) 2020 Athanasios Seitanis < seithagta@gmail.com >. 
 * All rights reserved under the library's licence
 */

// include this library's description file
#ifndef nextion_ez_h
#include "nextion_ez.h"
#endif

//#ifndef trigger_h
//#include "trigger.h"
//#endif

//-------------------------------------------------------------------------
 // Constructor : Function that handles the creation and setup of instances
//---------------------------------------------------------------------------

nextion_ez::nextion_ez(HardwareSerial& serial){  // Constructor's parameter is the Serial we want to use
  _serial = &serial;
}

void nextion_ez::begin(unsigned long baud){
  _serial->begin(baud);  // We pass the initialization data to the objects (baud rate) default: 9600
  
  delay(100);            // Wait for the Serial to initialize

  _cmdFifoHead = 0;     // setup FIFO 
  _cmdFifoTail = 0;
  _cmdAvail = false;

  _tmr1 = millis();
  while(_serial->available() > 0){     // Read the Serial until it is empty. This is used to clear Serial buffer
    if((millis() - _tmr1) > 400UL){    // Reading... Waiting... But not forever...... 
      break;                            
    } 
      _serial->read();                // Read and delete bytes
  }
}

int nextion_ez::getCurrentPage(){   //returns the current page id
    return _currentPageId;
}

void nextion_ez::setCurrentPage(int page) {  // set the current page id
    _currentPageId = page;
}

int nextion_ez::getLastPage(){      //returns the previous page id 
    return _lastCurrentPageId;
}

void nextion_ez::setLastPage(int page) {  // set the last page id
    _lastCurrentPageId = page;
}

bool nextion_ez::cmdAvail(){        //returns true if commands in the buffer
    bool avail = _cmdAvail;
    _cmdAvail = false;
    return avail;
}

int nextion_ez::getCmd(){           //returns the 1st command byte 
    return _cmdGroup;
}

int nextion_ez::getSubCmd(){        //returns the 2nd command byte
    return _subCmd;
}

int nextion_ez::getCmdLen(){        //'returns the number of command bytes (for use in custom commands)
    return _cmdLength;
}

/*
 * -- writeNum(String, uint32_t): for writing in components' numeric attribute
 * String = objectname.numericAttribute (example: "n0.val"  or "n0.bco".....etc)
 * uint32_t = value (example: 84)
 * Syntax: | myObject.writeNum("n0.val", 765);  |  or  | myObject.writeNum("n0.bco", 17531);       |
 *         | set the value of numeric n0 to 765 |      | set background color of n0 to 17531 (blue)|
 */
void nextion_ez::writeNum(String compName, uint32_t val){
	_component = compName;
	_numVal = val;
  
	_serial->print(_component);
    _serial->print("=");
    _serial->print(_numVal);
	_serial->print("\xFF\xFF\xFF");
}

/*
 * -- writeByte(uint8_t): Main purpose and usage is for sending the raw data required by the addt command
 * Where we need to write raw bytes to serial 
 * uint8_t = raw byte value (0-255 or 0x00-0xFF)
 * Syntax: | myObject.writeByte(0);  |  or  | myObject.writeByte(0xA0);  |
 */

void  nextion_ez::writeByte(uint8_t val){
    _serial->write(val);
}

/*
 * -- pushCmdArg(uint32_t): Used to load the argument FIFO with numeric arguments that are to be sent with the command using sendCmd()
* uint32_t = argument (example: 1 or 255)
 * Syntax: | myObject.pushCmdArg(1);   |  or  | myObject.pushCmdArg(3); |
 *         | loads 1 into the FIFO     |      | loads 3 into the FIFO    |
 */

void nextion_ez::pushCmdArg(uint32_t argument){
    _cmdFifo[_cmdFifoHead] = argument;
    if (_cmdFifoHead++ > 15) _cmdFifoHead = 0;
}

/*
 * -- sendCmd(String): for sending a command string along with any numeric arguments previously pushed to the FIFO
 * String   = command (example: "page"  or "ref")
 *           myObject.pushCmdArg(1);              myObject.pushCmdArg(3);
 * Syntax: | myObject.sendCmd("page");   |  or  | myObject.sendCmd("ref"); |
 *         | change to page 1            |      |  Refresh component with the id of 3  |
 */
void nextion_ez::sendCmd(String command){ 
	_component = command;
	uint8_t _count;
    uint8_t x;
    uint32_t _argument = 0;

    if (_cmdFifoHead < _cmdFifoTail) {
        _count = _cmdFifoHead + 16 - _cmdFifoTail;
    } else {
        _count = _cmdFifoHead - _cmdFifoTail;
    }

    _serial->print(_component);

    if(_count > 0) {
        _serial->print(" ");
        for (x = 0; x < _count; x++) {
            if (x > 0) _serial->print(",");         // only need commas between arguments, not between command and 1st argument
            _argument = _cmdFifo[_cmdFifoTail];
            _serial->print(_argument);
            if(_cmdFifoTail++ > 15) _cmdFifoTail = 0;
        }
    }
    _serial->print("\xFF\xFF\xFF");
}

/*
 * -- addWave(uint8_t, uint8_t, uint8_t): for writing in components' text attributes
 * uint8_t No1 = id number of the waveform object (id number not name)
 * uint8_t No2 = channel number to update
 * uint8_t No3 = value to add to the channel
 * Syntax: | myObject.addWave(5, 1, 255);  | 
 *         | add a value of 255 to channel 1 of waveform with id 5 |     
 */
void nextion_ez::addWave(uint8_t id, uint8_t channel, uint8_t val){ 
    _serial->print("add ");
    _serial->print(id);
    _serial->print(",");
    _serial->print(channel);
    _serial->print(",");
    _serial->print(val);
    _serial->print("\xFF\xFF\xFF");
}

/*
 * -- writeStr(String, String): for writing in components' text attributes
 * String No1 = objectname.textAttribute (example: "t0.txt"  or "b0.txt")
 * String No2 = value (example: "Hello World")
 * Syntax: | myObject.writeStr("t0.txt", "Hello World");  |  or  | myObject.writeNum("b0.txt", "Button0"); |
 *         | set the value of textbox t0 to "Hello World" |      | set the text of button b0 to "Button0"  |
 */
void nextion_ez::writeStr(String command, String txt){ 
	_component = command;
	_strVal = txt;
  
    _serial->print(_component);
    _serial->print("=\"");
    _serial->print(_strVal);
    _serial->print("\"");
    _serial->print("\xFF\xFF\xFF");
}

String nextion_ez::readStr(String TextComponent){
  
  String _Textcomp = TextComponent;  
  bool _endOfCommandFound = false;
  char _tempChar;
  
    _tmr1 = millis();  
  while(_serial->available()){                  // Waiting for NO bytes on Serial, 
                                                    // as other commands could be sent in that time.
    if((millis() - _tmr1) > 1000UL){                // Waiting... But not forever...after the timeout 
      _readString = "ERROR";
      break;                                // Exit from the loop due to timeout. Return ERROR
    }else{
      listen();                      // We run listen(), in case that the bytes on Serial are a part of a command 
    }
  }
  
  // As there are NO bytes left in Serial, which means no further commands need to be executed,
  // send a "get" command to Nextion
  
  _serial->print("get ");
  _serial->print(_Textcomp);             // The String of a component you want to read on Nextion
	_serial->print("\xFF\xFF\xFF");
  
  // And now we are waiting for a reurn data in the following format:
  // 0x70 ... (each character of the String is represented in HEX) ... 0xFF 0xFF 0xFF
  
  // Example: For the String ab123, we will receive: 0x70 0x61 0x62 0x31 0x32 0x33 0xFF 0xFF 0xFF
  
  _tmr1 = millis();  
  while(_serial->available() < 4){                  // Waiting for bytes to come to Serial, an empty Textbox will send 4 bytes 
                                                    // and this the minimmum number that we are waiting for (70 FF FF FF)
    if((millis() - _tmr1) > 400UL){                // Waiting... But not forever...after the timeout 
       _readString = "ERROR";
       break;                        // Exit the loop due to timeout. Return ERROR
    }
  }
  
  if(_serial->available() > 3){         // Read if more then 3 bytes come (we always wait for more than 3 bytes)
    _start_char = _serial->read();      //  variable (start_char) read and store the first byte on it  
    _tmr1 = millis();
      
    while(_start_char != 0x70){      // If the return code 0x70 is not detected,
      if(_serial->available()){      // read the Serial until you find it
        _start_char = _serial->read();
      }
        
      if((millis() - _tmr1) > 100UL){     // Waiting... But not forever...... 
        _readString = "ERROR";          // If the 0x70 is not found within the given time, break the while()
                                        // to avoid being stuck inside the while() loop
        break;
      }   
    }
      
    if(_start_char == 0x70){  // If the return code 0x70 is detected,   
      _readString = "";   // We clear the _readString variable, to avoid any accidentally stored text
      int _endBytes = 0;  // This variable helps us count the end command bytes of Nextion. The 0xFF 0xFF 0xFF
      _tmr1 = millis(); 
      
      while(_endOfCommandFound == false){  // As long as the three 0xFF bytes have NOT been found, run the commands inside the loop
        if(_serial->available()){
          _tempChar = _serial->read();  // Read the first byte of the Serial
         
          if(_tempChar == 0xFF || _tempChar == 0xFFFFFFFF){  // If the read byte is the end command byte, 
            _endBytes++ ;      // Add one to the _endBytes counter
            if(_endBytes == 3){  
              _endOfCommandFound = true;  // If the counter is equal to 3, we have the end command
            }                         
          }else{ // If the read byte is NOT the end command byte,
            _readString += _tempChar;  // Add the char to the _readString String variable 
          }
        }
          
        if((millis() - _tmr1) > 1000UL){     // Waiting... But not forever...... 
          _readString = "ERROR";           // If the end of the command is NOT found in the time given,
                                           // break the while
          break;                           
        }
      }
    }
  } 

  return _readString;
}

/*
 * -- readNumber(String): We use it to read the value of a components' numeric attribute on Nextion
 * In every component's numeric attribute (value, bco color, pco color...etc)
 * String = objectname.numericAttribute (example: "n0.val", "n0.pco", "n0.bco"...etc)
 * Syntax: | myObject.readNumber("n0.val"); |  or  | myObject.readNumber("b0.bco");                       |
 *         | read the value of numeric n0   |      | read the color number of the background of butoon b0 |
 */

uint32_t nextion_ez::readNum(String component){
  
  _comp = component;
  bool _endOfCommandFound = false;
  char _tempChar;
  
    _numberValue = 777777;                      // The function will return this number in case it fails to read the new number
  
    _tmr1 = millis();  
  while(_serial->available()){                  // Waiting for NO bytes on Serial, 
                                                    // as other commands could be sent in that time.
    if((millis() - _tmr1) > 1000UL){                // Waiting... But not forever...after the timeout 
      _numberValue = 777777;
      break;                                // Exit from the loop due to timeout. Return 777777
    }else{
      listen();                      // We run listen(), in case that the bytes on Serial are a part of a command 
    }
  }
  
  
  // As there are NO bytes left in Serial, which means no further commands need to be executed,
  // send a "get" command to Nextion
  
  _serial->print("get ");
  _serial->print(_comp);             // The String of a component you want to read on Nextion
	_serial->print("\xFF\xFF\xFF");
  
  // And now we are waiting for a reurn data in the following format:
  // 0x71 0x01 0x02 0x03 0x04 0xFF 0xFF 0xFF
  // 0x01 0x02 0x03 0x04 is 4 byte 32-bit value in little endian order.

    _tmr1 = millis();  
  while(_serial->available() < 8){                  // Waiting for bytes to come to Serial, 
                                                    // we are waiting for 8 bytes
    if((millis() - _tmr1) > 400UL){                // Waiting... But not forever...after the timeout 
       _numberValue = 777777;
       break;                                  // Exit the loop due to timeout. Return 777777
    }
  }
  
  if(_serial->available() > 7){         
    _start_char = _serial->read();      //  variable (start_char) read and store the first byte on it  
    _tmr1 = millis();
      
    while(_start_char != 0x71){      // If the return code 0x71 is not detected,
      if(_serial->available()){      // read the Serial until you find it
        _start_char = _serial->read();
      }
        
      if((millis() - _tmr1) > 100UL){     // Waiting... But not forever...... 
        _numberValue = 777777;          // If the 0x71 is not found within the given time, break the while()
                                        // to avoid being stuck inside the while() loop
        break;
      }   
    }
      
    if(_start_char == 0x71){  // If the return code 0x71 is detected, 
  
			for(int i = 0; i < 4; i++){   // Read the 4 bytes represent the number and store them in the numeric buffer 
		   
        _numericBuffer[i] = _serial->read();
	    }
      
      
      int _endBytes = 0;  // This variable helps us count the end command bytes of Nextion. The 0xFF 0xFF 0xFF
      _tmr1 = millis();  
      
      while(_endOfCommandFound == false){  // As long as the three 0xFF bytes have NOT been found, run the commands inside the loop
        
        _tempChar = _serial->read();  // Read the next byte of the Serial
         
        if(_tempChar == 0xFF || _tempChar == 0xFFFFFFFF){  // If the read byte is the end command byte, 
          _endBytes++ ;      // Add one to the _endBytes counter
          if(_endBytes == 3){  
            _endOfCommandFound = true;  // If the counter is equal to 3, we have the end command
          }                         
        }else{ // If the read byte is NOT the end command byte,
          _numberValue = 777777;
          break;            
        }
          
        if((millis() - _tmr1) > 1000UL){     // Waiting... But not forever...... 
          _numberValue = 777777;           // If the end of the command is NOT found in the time given,
                                           // break the while
          break;                           
        }
      }
    }
  }
  
  if(_endOfCommandFound == true){
    // We can continue with the little endian conversion
    _numberValue = _numericBuffer[3];
    _numberValue <<= 8;
    _numberValue |= _numericBuffer[2];
    _numberValue <<= 8;
    _numberValue |= _numericBuffer[1];
    _numberValue <<= 8;
    _numberValue |= _numericBuffer[0];
  }else{
    _numberValue = 777777;
  }
  
  return _numberValue;
}

/*
 * -- readByte(): Main purpose and usage is for the custom commands read
 * Where we need to read bytes from Serial inside user code
 */

int  nextion_ez::readByte(){
  
 int _tempInt = _serial->read(); 

 return _tempInt;
  
}

/*
 * -- listen(): It uses a custom protocol to identify commands from Nextion Touch Events
 * For advanced users: You can modify the custom protocol to add new group commands.
 * More info on custom protocol: https://seithan.com/Easy-Nextion-Library/Custom-Protocol/ and on the documentation of the library
 */
/*! WARNING: This function must be called repeatedly to response touch events
 * from Nextion touch panel. 
 * Actually, you should place it in your loop function.
 */
void nextion_ez::listen(){
	if(_serial->available() > 2){         // Read if more then 2 bytes come (we always send more than 2 <#> <len> <cmd> <id>
    _start_char = _serial->read();      // Create a local variable (start_char) read and store the first byte on it  
    _tmr1 = millis();
    
    while(_start_char != '#'){            
      _start_char = _serial->read();        // whille the start_char is not the start command symbol 
                                           //  read the serial (when we read the serial the byte is deleted from the Serial buffer)
      if((millis() - _tmr1) > 100UL){     //   Waiting... But not forever...... 
        break;                            
      }   
    }
    if(_start_char == '#'){            // And when we find the character #
      _len = _serial->read();          //  read and store the value of the second byte
                                       // <len> is the lenght (number of bytes following) 
      _tmr1 = millis();
      _cmdFound = true;
      
      while(_serial->available() < _len){     // Waiting for all the bytes that we declare with <len> to arrive              
        if((millis() - _tmr1) > 100UL){         // Waiting... But not forever...... 
          _cmdFound = false;                  // tmr_1 a timer to avoid the stack in the while loop if there is not any bytes on _serial
          break;                            
        }                                     
      }                                   
  
      if(_cmdFound == true){                  // So..., A command is found (bytes in _serial buffer egual more than len)
        _cmd1 = _serial->read();              // Read and store the next byte. This is the command group
        readCommand();                        // We call the readCommand(), 
                                              // in which we read, seperate and execute the commands 
			}
		}
	}
}

void nextion_ez::readCommand(){

				
  switch(_cmd1){
    case 'P': /*or <case 0x50:>  If 'P' matches, we have the command group "Page". 
               *The next byte is the page <Id> according to our protocol.
               *
               * We have write in every page's "preinitialize page" the command
               *  printh 23 02 50 xx (where xx is the page id in HEX, 00 for 0, 01 for 1, etc).
               * <<<<Every event written on Nextion's pages preinitialize page event will run every time the page is Loaded>>>>
               *  it is importand to let the Arduino "Know" when and which Page change.
               */
      _lastCurrentPageId = _currentPageId;
      _currentPageId = _serial->read();                   
      break;
      
      
    case 'T':   /* Or <case 0x54:>  If 'T' matches, we have the command group "Trigger". 
                 * The next byte is the trigger <Id> according to our protocol.
                 *
                 * We have to write in a Touch Event on Nextion the following: < printh 23 02 54 xx >
                 * (where xx is the trigger id in HEX, 01 for 1, 02 for 2, ... 0A for 10 etc).
                 * With the switch(){case}, we call a predefined void with prefixed names
                 * as we have declare them on trigger.cpp file. Starting from trigger0()......up to trigger50()
                 * The maximum number of predefined void is 255
                 * We declare the trigger in your code as a function, 
                 * in order to write any kind of code and run it, by sending the < printh > command needed,
                 * from a Touch event on Nextion, such as pressing a button. Example:
                 */
                /**                   void trigger0(){
                                        digitalWrite(13, HIGH); // sets the digital pin 13 on
                                        delay(1000);            // waits for a second
                                        digitalWrite(13, LOW);  // sets the digital pin 13 off
                                      }
                 */
                /* In Touch Press Event of a button on Nextion write <printh 23 02 54 01>
                 * Every time we press the Button the command <printh 23 02 54 01> will be sent over Serial
                 * Then, the library will call the void trigger1()
                 * the code inside the trigger1() will run once ...
                 */
        _cmdAvail = true;
        _cmdGroup = _cmd1;  // stored in the public variable cmdGroup for later use in the main code
        _subCmd = _serial->read();
      break;
    
    default:            //custom commands can be variable length, we pull just the first and leave the rest for main code to deal with 
      _cmdGroup = _cmd1;  // stored in the public variable cmdGroup for later use in the main code
      _cmdLength = _len;  // stored in the public variable cmdLength for later use in the main code
      _cmdAvail = true;
                    
      break;
               
            /*   More for custom protocol and commands https://seithan.com/Easy-Nextion-Library/Custom-Protocol/
               
      easyNexReadCustomCommand() has a weak attribute and will be created only when user
      declare this function on the main code
      More for custom protocol and commands https://seithan.com/Easy-Nextion-Library/Custom-Protocol/
      our commands will have this format: <#> <len> <cmd> <id> <id2>
      and we must send them from Nextion as HEX with the printh command
      like: printh 23 03 4C 01 01

      <#> start marker, declares that a command is followed
      <len> declares the number of bytes that will follow
      <cmd> declares the task of the command or command group
      <id> declares the properties of the command
      <id2> a second property of the command
      
      When we send a custom command with the above format, the function NextionListen() will capture the start marker # and the len (first 2 bytes)
      and it will wait until all the bytes of the command, as we have declared with the len byte, arrive to the Serial buffer and inside the timeout limits.
      
      After that, the function will read the next byte, which is the command group and the function readCommand() takes over and through a switch command
      tries to match the _cmd variable that holds the command group value with the statements of the cases.
      
      If we do NOT have a match with the predefined, cmd of P for page and T for triggers, it will continue to the default where we store the _cmd and _len to the public variables
      cmdGroup and cmdLenght as we are going to need access to them from the main code in the next step.
      
      Next we call the the easyNexReadCustomCommand() with the precondition and ONLY if we have declared the function in the main code.
      
      From this point we can handle the assign of cmdGroup and IDs from the easyNexReadCustomCommand() in the user code, where we can go on with a switch case
      for the cmdGroup, the one that we have stored the _cmd for public use and we can call it with myObject.cmdGroup. This is why we made cmdGroup a public variable.
                     */
  }
}

