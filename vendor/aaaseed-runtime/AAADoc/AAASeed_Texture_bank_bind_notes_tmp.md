bank 6 is special
is it remaped by the C
basicly when we leave an fbo
all the active texture attachment bind are stored there in the active order
depth is stroed at position 16
and eventually stencil at 17 (unused probably)
if an fbo use attachment 1,4 and 5
the corresponding bind will be stored in 0,1 and 2
So referencing this bank (6 or Fbo) will give different result in function of the last fbi used in the rendering chain
e.g. a common practise is to use bind 0 to get the first color attachment of the last fbo
this way the information can keep on passing from MEU to MEU without changing the source of MEU. 
These bind are so "fake bind" and should not be used execpt as "source texture", never as the choice for a color attachment or an image.

