  

# nextion_ez 

## Description

A simple object for interfacing with Nextion HMI displays that is compatible with the protocol and HMI code used by the Arduino Easy Nextion Library. 

The original library was developled by Thanasis Seitanis.  It uses a custom protocol that is simple to use and can be easily modified to meet one’s needs.
It was designed to provide a simple method to use Nextion displays for beginners and at the same time satisfying the needs of the more advanced programmers.

Full documentation on the Arduino Easy Nextion Library and protocol, as well as examples, can be found at https://seithan.com/Easy-Nextion-Library/

If you find this library useful, please consider supporting the author of the original Easy Nextion Library, Thanasis Seitanis at: [seithagta@gmail.com](https://paypal.me/seithan)

**NOTE**: Functions are similar to, but not identical to, those in the Arduino Easy Nextion Library.  It has been modified to make it completely compatible with my Spin/Spin2 object for the Parallax Propeller microcontrollers.  The protocol is completely compatible with the Easy Nextion Library and will allow reuse of HMI code between Propeller boards and Arduino boards using this or the original Easy Nextion Library.

Full documentation on the Arduino Easy Nextion Library and protocol can be found at https://seithan.com/Easy-Nextion-Library/ If you find this library useful, please consider donating to the author of the original Easy Nextion Library, Thanasis Seitanis.

Differences between the Arduino library and this library

1. The Arduino implementation automatically calls trigger functions, stored in a separate file,
    in response to Nextion commands.
        This object provides the methods cmdAvail(), getCmd(). getSubCmd() and readByte()
        to retreave the command packets sent from the Nextion.

2. The Arduino C++ library uses a single overloaded function writeStr() to send commands and
    update string values on the Nextion.
        This object uses separate methods sendCmd() and writeStr().

3. A argument fifo has been added to allow a new method pushCmdArg() that can be used to
    provide a variable number of arguments to sendCmd().

4. The the equivilent of the Arduino NextionListen() function has been named listen()
    in this implementation..

5. This object adds a method called addWave() to create a quick and easy interface to the
    Nextion waveform add command.

6. In this object the currentPageId and lastCurrentPageId variables can be accessed with the
    methods getCurrentPage() and getLastPage()

**NOTE**: `.HMI` files for Nextion Editor are also included in the demo folder.

## The public functions
- `start()`
- `writeNum()`
- `writeStr()`
- `sendCmd()`
- `pushCmdArg()`
- `addWave()`
- `readNum()`
- `readStr()` 
- `cmdAvail()`
- `getCmd()`
- `getSubCmd()`
- `readByte()`
- `writeByte()`
- `getCurrentPage()`
- `getLastPage()`

In order for the object to update the Id of the current page, you must write the Preinitialize Event of every page: `printh 23 02 50 XX` , where `XX` the id of the page in HEX.
Your code can then read the current page and previous page using the `getCurrentPage()` and `getLastPage()` functions.

Standard Easy Nextion Library commands are sent from the Nextion display with `printh 23 02 54 XX` , where `XX` is the id for the command in HEX.  
Your code should call the `listen()` function frequently to check for new commands from the display.  You can then use the `getAvail()`, `getCmd()` and `getSubCmd()` functions to parse any commands.

example:
```

```

Example:
```

```


##  Usefull Tips

**Manage Variables**
You can read/write the variables as any other component.

Use `readNum()` to read the value of a numeric variable.  
Example: `nextion.readNum("va0.val")`  
Example: `nextion.readNum("sys0")`

Use `writeNum()` to change the value of a numeric variable.  
Example: `nextion.writeNum("va0.val", 255)`  
Example: `nextion.writeNum("sys0", 375)`

Use `readStr()` to read the text of a String variable.  
Example: `nextion.readStr("va0.txt")`

Use `writeStr()` to change the text of a String variable.  
Example: `nextion.writeStr("va0.txt", "Hello World")`

For this to happen, the variables you want to read/write must be at the page you are currently on.  
Otherwise, if the variables are of **global** scope, you will need to use a prefix with the page name that the variables are at.  
Example: `nextion.readNumber("page0.va0.val")`   'If the variable is at page0  
The same goes for the other functions as well.

**NOTE**: (from the Nextion Editor Guide)
> In an HMI project a page is a localized unit. When changing pages, the existing page is removed from memory and the > > requested page is then loaded into memory. As such components with a variable scope of _**local**_ are only accessible while the page they are in is currently loaded. Components within a page that have a variable scope of _**global**_ are accessible by prefixing the page name to the global component .objname.
As an Example:
 A global Number component n0 on page1 is accessed by **page1.n0** . 
A local Number component n0 on page1 can be accessed by page1.n0 or n0, but there is little sense to try access a local component if the page is not loaded. Only the component attributes of a global component are kept in memory. Event code is never global in nature.


## Licence 

  Terms of Use: MIT License

  Permission is hereby granted, free of charge, to any person obtaining a copy of this
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify,
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be included in all copies
  or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
