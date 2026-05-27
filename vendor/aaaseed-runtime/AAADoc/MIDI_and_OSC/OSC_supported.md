# OSC protocol in AAASeed
**Open Sound Control (OSC)** is used by a majority of real time software to **transmit** nearly any kind of **data**.\
It is **not suitable from high bandwidth information** (like video or images) because the data coding is pretty inefficient.\
**It encapsulate data in network packets** that can be passed locally or over networks.\
in ```AAAdoc/PureData``` there is PureData OSC patches sending data to AAASeed. We recommend [PlugData](https://plugdata.org/) to open those.


# Some references on OSC
- [Wikipedia](https://en.wikipedia.org/wiki/Open_Sound_Control)
- [Stanford](https://opensoundcontrol.stanford.edu/spec-1_0.html#introduction)
- [Best Practices](https://parlab.eecs.berkeley.edu/sites/all/parlab/files/osc-best-practices.pdf)
- [Chataigne software](https://benjamin.kuperberg.fr/chataigne/fr)
When you master it is a good tool to work with OSC and perfect for test and debug.

## Type Tag String:
We only use a reduce set.
```
i int32	        a 32 bits integer if you send to AAASeed a float it will be round up to the integer below
                  0.5688 will be be intepreted as 0.
f float32 	    a 32 bits floating point number.
s OSC-string    a text string.
```
All standard type are implement in AAASeed except OSC_blob(b) (2026 March).\
Non standard type are not implemented.\
Some messages (see below, e.g. MIDI) are processed directly by the **AAASeed** executable.\
**AAASeed** deal with the reception of other messages using lua ```aaa.net.osc_take_by_start( str_address )```\
There is also a lua ```OSC_MESS``` class helper
```
local tab = aaa.net.osc_take_by_start( "AAASeed/example/what" )
```
Lua have a generic number type for integer and float. On reception we will get a number for integer and float.\

## Address Patterns:
The default address pattern are recognized directly and proceesed by **AAASeed.exe** and **start with ```/aaa/```**.\
There are specific params in the c_net object (Flatland, then triple n) to choose to accept the messages described below. There is verbose option for each.   
For the moment (Febuary 2026)
```
address             data    result                                    example
/aaa/lua            s       execute a string as a lua script     /aaa/lua/ "aaa.print("Hello")"

/aaa/midi/ctrl      iif     set midi control                     /aaa/midi/ctrl    1 1 .78
/aaa/midi/control   iif     idem
/aaa/midi/note_on   iif     set midi velocity
/aaa/midi/note/on   iif     idem
/aaa/midi/note_off  ii      set midi velocity to 0
/aaa/midi/note/off  ii      idem

/aaa/variable/set   sf      will set a variable to a value      /aaa/variable/set "toto" 421.5
```
Any address can be used with **AAASeed** but they have to be retrieved and processed by the lua layer and are not handled directly by the executable.\
See document [OSC_ndim.md](OSC_ndim.md) describing some the OSC packet handled by some lua script. 


# ip Address and Port
For OSC **AAASeed** is currently (2026 March) set to listen to udp paclets on port 8000.\
Sender should use the ip address of the **AAASeed** listener they want to reach or they can use Broadcast addresses.\
You can also use localhost (127.0.0.1) if the sender run on the same machine.

# `/aaa/lua` execute a script
Execute the script passed in the string.
* arguments `s` :
  * Script text

[OSC_send_lua.pd](PureData/OSC_send_lua.pd) is a PureData patch showing an example of sending a lua script.

# `/aaa/midi` deal with midi
## `/aaa/midi/ctrl` or  `/aaa/midi/control`
* set midi control
* arguments `iif` : integer integer float
  * channel [1.16]
  * controller [0,511], midi per se is [0,127] but **AAASeed** maintain a bigger data matrix
  * float controller value.The range [0.,1.] correspond to midi [0,127]

## `/aaa/midi/note_on` or  `/aaa/midi/note/on`
* set midi velocity
* arguments `iif` : integer integer float
  * channel [1.16]
  * note_index [0,127]
  * float velocity value, the range [0.,1.] correspond to midi [0,127]

## `/aaa/midi/note_off` or  `/aaa/midi/note/off`
* set midi velocity to 0
* arguments `ii` : integer integer
  * channel [1.16]
  * note_index [0,127]

[Midi_to_Osc.pd](PureData/Midi_to_Osc.pd) is a PureData patch showing an example of sending a controller value.\
[Audio_Split_to_OSC.pd](PureData/Audio_Split_to_OSC.pd) is a PureData patch analysing the sound input and sending the result as midi controller values.

# `/aaa/variable/set` set an AAASeed variable
will set an AAASeed variable to a value.\
See **lua** `aaa.set()` and `aaa.get()` for access to these variables.\
These variables are also accessible using the NEU_TRAX and also c_trax.\
* arguments `sf` :
  * variable name, be careful AAASeed variable name are case sensitive.
  * float32 value

[OSC_send_variable.pd](PureData/OSC_send_variable.pd) is a PureData patch showing an example of sending a variable.
