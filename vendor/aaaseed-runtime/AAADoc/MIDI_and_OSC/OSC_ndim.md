# __ndim__ : OSC protocol to exchange n dimension data
The main motivation is to exchange “__tracking__” data
- Blobs
- Skeletons
- Output of camera analysis (color and/or depth) camera
- Calibration data
- ...

But are trying to keep this generic and extendable, hence the ndim name.

# Some references on OSC
- [Wikipedia](https://en.wikipedia.org/wiki/Open_Sound_Control)
- [Stanford](https://opensoundcontrol.stanford.edu/spec-1_0.html#introduction)
- [Best Practices](https://parlab.eecs.berkeley.edu/sites/all/parlab/files/osc-best-practices.pdf)
- [Chataigne software](https://benjamin.kuperberg.fr/chataigne/fr)
is a great tool to work with OSC and really good for test and debug.

## Address Patterns:
- they will all start with /AAAAeed/
- this way “AAASeed” can be used as a first selector to know if the AAASeed software should process the message

## Type Tag String:
We only use a reduce set.
```
i int32	        a 32 bits integer if you send to AAASeed a float it will be round up to the integer below
                  0.5688 will be be intepreted as 0.
f float32 	    a 32 bits floating point number.
s OSC-string    a text string.
```
The only standard type not implement in AAASeed yet (2026 March) is OSC_blob(b).\
Non standard type are not implemented either.\
AAASeed deal on reception through lua using ```aaa.net.osc_take_by_start( str_address )```\
there is also an ```OSC_MES``` class helper
```
local tab = aaa.net.osc_take_by_start( "AAASeed/example/what" )
```
Lua have a generic number type for integer and float. On reception we will get a number for int and float.\
```
OSC tag     C                       lua
T           TRUE_TYPE_TAG           true
F           FALSE_TYPE_TAG          false
N           NIL_TYPE_TAG            nil
I           INFINITUM_TYPE_TAG      number build from the max C positive DOUBLE
i           INT32_TYPE_TAG          number
f           FLOAT_TYPE_TAG          number
c           CHAR_TYPE_TAG           string
r           RGBA_COLOR_TYPE_TAG     number made from 4 x 8 bits as a C UINT32 
m           MIDI_MESSAGE_TYPE_TAG 	number made from 4 x 8 bits as a C UINT32 
h           INT64_TYPE_TAG          number made from a C INT64 
t           TIME_TAG_TYPE_TAG       number made from a C UINT64 probably problematic
d           DOUBLE_TYPE_TAG         number made from a C DOUBLE 
s           STRING_TYPE_TAG         string
S           SYMBOL_TYPE_TAG         string
b           BLOB_TYPE_TAG           nil
```
Sending is a problem we will deal with later.
# ip Address and Port
For OSC AAASeed is currently (2026 March) set to listen to udp packets on port 8000.\
Sender should use the ip address of the AAASeed listener they what to reach or they can use Broadcast address.\
You can also use localhost (127.0.0.1) if the sender run on the same machine.

# sending a value
## `/AAASeed/value`
* arguments `sf`
  * name
  * float32_value
* arguments `si`
  * name
  * int32_value
* arguments `ss`
  * name  
  * string_value
---
# sending dimensionnal data
Data is sent inside a "frame"
```
begin Frame
	send data
	...
	send data
end Frame
```
with corresponding commands.
## `/AAASeed/ndim/begin`
This is to be sent to begin new frame
* arguments `siii`
  * channel_name
    * sender can use this to name different flow of data (e.g. Cam_A, Yolo, Blobs). They can also include a part to identify the sender.
	* we recommend using different names for data flow that refresh asynchronously.
  * channel_frame_id
    * an integer > 1
    * we recommend the sender to increase this id with every frame sent
  * dimension_max
    * currently unused (2024 April) but could be used for allocation and check
  * total_point_nb
    * currently unused (2024 April) but could be used for allocation and check
---

## `/AAASeed/ndim/end`
When a listener receive this command, it can process the frame and use it.
* argument `ssi`
  * channel_name
    * see ndim/begin command
  * channel_frame_id
    * see ndim/begin command
---

## `/AAASeed/ndim/group_data`
This where the data is send
* arguments `sisiii` then n times `if...f` where the number of `f` correspond to the dimension
  * channel_name
    * see ndim/begin command
  * channel_frame_id
    * see ndim/begin command
  * group_name
    * sender can use this to name different data inside the same flow (e.g. Skeleton, Human, Blob).
  * group_id
    * an integer > 1
  * dimension		< dimension_max
    * ussualy 2 for 2d points (x y), 3 for 3d points (x y z), but other can be used (e.g. 4 for 2d rect as x,y,size_x,size_y; 6 for 3d rect).
  * nb
    * nb * (1 + dimension) must be smaller or equal to 1620. This is related to a maximum OSC out packet size choosen in AAASeed ( in net_lua.cpp: `CONSTEXPR	INT32	OSC_BLK_SIZE_CUSTOM = 8192;` 2024 April).
    * when nb are bigger the data should be split into several commands.  
    	* nb <= 810 for 1d data
	    * nb <= 540 for 2d data
	    * nb <= 405 for 3d data
	    * nb <= 324 for 4d data
      * nb <= 270 for 5d data
      * nb <= 231 for 6d data
	* followed here by nb * (1 + dimension) arguments
  * nb * points as
    * point_id
      * an integer > 1
    * fp32
    * fp32
	* ...
      * the number of fp32 by point is given by the dimension argument

---


## Random Notes to consider later (now is 2024 April)
## `/AAASeed/ndim/definition/segments`	isi ii...ii
  * channel_name
  * channel_frame_id
  * name
  * segments_nb	number of pairs begin/end
  * id_begin
  * id_end
  * ...

## `/AAASeed/2dim`
As group_data but for simplification
## `/AAASeed/3dim`
As group_data but for simplification
