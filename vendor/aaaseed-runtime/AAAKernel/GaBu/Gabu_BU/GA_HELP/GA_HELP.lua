	--
--	GA HELP
--
if CLASS.DECLARE( "GA_HELP", nil, {} ) then
 	GA_HELP:set_class_status_doc(	CLASS.STATUS.GABU,
 								"GA_HELP handle a basic global help page" )
end

function GA_HELP.init_instance(self)
	oo.getsuper(GA_HELP).init_instance( self )
	self.text		= BU.__text_obj
	--self.text		= TEXT:create("Help")
	self.b_active	= false
	self.line_begin	= 1
	self.line_nb	= 32
	return self
end
function GA_HELP:create()
	return GA_HELP:create_instance( "Global" )
end
function GA_HELP:flip_help()		self.b_active = not self.b_active	end
function GA_HELP:is_help()			return self.b_active				end
function GA_HELP:inc_line( inc )	self.line_begin = self.line_begin + inc	end
function GA_HELP:dec_resolution()	self.line_nb = math.max( self.line_nb - 1, 8 )		end
function GA_HELP:inc_resolution()	self.line_nb = math.min( self.line_nb + 1, 128 )	end

function GA_HELP:define()
	local lines = {}
	local keys = {}

	local function add_line( line )
		local key = line.key
		if key then
			local key_found = keys[key]
			if key_found then -- reuse
				if not line[1] then
					line = key_found
				end
			else -- add
				if not keys[key] then
					keys[key] = line
				end
			end
		end
		table.insert( lines, line )
	end

	local function add( str )					add_line{ str } end
	local function add_title( str )				add_line{ str, b_title=true, color={0,1,0} } end
	local function add_tab( nb, str )			add_line{ str, tab_nb=nb } end
	local function add_skip( )					add( "" ) 	end
	local function add_key( key, str, color )	add_line{ str, key=key, color=color } end

	add_title( "Help" )	
	add_line{	"the interface is still active except for the keys used by this help :",	tab_nb=3, color={0,1,0} }
	add_line{	"F1 or CTRL h: Toggle Help    +|- | CTRL Mouse Wheel : Size"  ,				tab_nb=1, color={0,1,0} }
	add_line{	"Arrows|Page up/Down|Home|End|Mouse Wheel : Navigate",						tab_nb=1, color={0,1,0} }

	add_title(	"AVOID AVOID AVOID Caps Lock : it will get you lost"	)
	add_title(	"Use a Mouse or TouchScreen"	)

	add_title(	"GaBuZoMeu"	)
		add_key(	"GA",	"Global Action"															)
		add_key(	"BU",	"Box User: the interface elements you can interact with"				)
		add_key(	"ZO",	"unused for now (don't worry we working very very hard on it)"			)
		add_key(	"MEU",	"Module Editable Unit: an encapsulated functional Block"				)
		add_key(	"MU",	"Module Unit: a compact MEU view used to control the execution order"	)

	add_title(	"MOUSE" )
		add_key(	"Left Button",	"Action"	)
		add_key(	"Wheel Button",	"Scroll"	)
		add_key(	"Right Button",	"Menu"		)

	add_title(	"Basic Keys" )
		add_key(	"w or W",			"(w)indow: flip from windows to fullscreen"				)
		add_key(	"CTRL w",			"navigate all the full screen configurations (with SHIFT in reverse)"	)
		add_key(	"F2",				"toggle GaBu Interface (with SHIFT partially)"				)
		add_key(	"CTRL F2",			"force visible the interface and loop it through screens configurations"	)
		add_key(	"",					"  in some cases it goes through unused screen so repeat"			)
		add_key(	"SHIFT CTRL F2",	"Same in reverse order"	)
		add_key(	"F3",				"toggle MEU rendering"										)
		add_key(	"CTRL Tab",			"Toggle size of GaBuZoMeU"									)
		add_key(	"m or M",			"Pop console window to read the (m)essages"					)
		add_key(	"s",				"toggle multitouch (s)imulator"								)

	add_title(	"Flatland" )
	add_line{ "When ON it consumes the keys it uses (e.g. Arrows|/|*|+|- ... )",	tab_nb=1, color={0,1,0} }
	add_line{ "in this case GaBuZoMeu don't get them",								tab_nb=2, color={0,1,0} }
		add_key(	"Tab",					"Toggle Flatland On/Off"				)	--
		add_key(	"CTRL e",				"Toggle (e)dit for Flatland"			)	--
		add_key(	"Mouse Wheel",			"Scroll"								)	--
		add_key(	"Arrow Up/Down",		"change current param Up/Down"			)	--
		add_key(	"Arrow Left/Right",		"Navigate focus history"				)	--
	add_line{ "Change param value, depend on the type",							tab_nb=1, color={0,1,0} }
	add_line{ 	"boolean: click and drag left or right, for trig just click",	tab_nb=2, color={0,1,0} }
	add_line{	"integer and float: Click and turn around click position",		tab_nb=2, color={0,1,0} }	
		add_key(	"Space or Click",		"Open/Close if possible"				)	--
		add_key(	"Double Click",			"Trigger dialog or action if possible"	)	--
		add_key(	"+|-|*|/| . |Enter",	"Change param value | Floor | Negate"	)	--
		add_key(	"Page up/Down",			"Set param value to Maximum/Minimum"	)	--
		add_key(	"Home/End",				"Set param value to Default/Inactive"	)	--
	add_line{ "These change the focus (\"the root of Flatland\")",					tab_nb=1, color={0,1,0} }	
		add_key(	"F4",			"Focus on current Camera"				)		
		add_key(	"F10",			"Focus on Preferences"					)
		add_key(	"Double F10",	"Focus on Starting Preferences"			)
		add_key(	"CTRL F10",		"Focus on Starting Preferences"			)
		add_key(	"Triple F10",	"Focus on App"							)
		add_key(	"1",			"Focus on current c_modules c_obj_ui"	)
		add_key(	"2",			"Focus on current c_module c_obj_ui"	)
		add_key(	"3",			"Focus on current c_layers c_obj_ui"	) 
		add_key(	"4",			"Focus on current c_layer c_obj_ui"		)
		add_key(	"Triple n",		"Focus on (n)etwork"					)
	add_line{ "Less used",								tab_nb=1, color={0,1,0} }
		add_key(	"SHIFT Tab",	"Toggle associated lua draw hook"		)		

	add_title(	"QUIT SAVE RELOAD" )
--		add_key(	"ALT F4",		"Exit No save"					)
		add_key(	"Double ESC",	"Exit but save globals first, except if SHIFT is pushed"			)
		add_key(	"ALT F4",		"Quit with confirmation" )
		add_key(	"CTRL S ",		"Save globals"				)
		add_key(	"SHIFT CTRL S",	"Save globals and all MEUs"	)
		add_key(	"F12",			"Reload the GaBuZoMeu APP"	)

	add_title(	"BU StarMenu" )
	add(		"Some BU open their StarMenu when you click, when this is not the case use Quick Drag-out" )
		add_key (	"Quick Drag-out",	"Click, and with the mouse button pressed, go quickly out of the BU rectangle" )

	add_title(	"MEDIA Loading:" )
	add(		"When displaying the Bind Selector for texture or video" )
		add_tab(1,	"double clicking on the selector item will open a file dialog" )
		add_tab(1,	"you can then choose several files which would be used on sucessive binds" )
	add(		"For image you can also use the BU_BANK, every Monitor inside have a StarMenu (Quick Drag-out)" )
	add(		"To read video you need to install codecs" )
		add_tab(1,	"We use https://codecguide.com/download_kl.htm, using the Mega one with default options" )
	add(		"We recommand also https://www.xnview.com/ to browse image " )


	add_title(	"BU, when possible and allowed, Flatland have precedence so close it" )
		add_key(	"ALT Pushed",			"Switch move/resize mode (display blue rects)"				)
		add_key(	"ALT Arrows",			"Move"														)
		add_key(	"ALT +|-|*|/",			"Resize"													)
		add_key(	"Arrows",				"Change selected in the arrow direction"					)
		add_key(	"+|-|*|/| . |Enter",	"Change value | Floor | Negate"								)
		add_key(	"CTRL z | y",			"Undo | Redo"  												)
		add_key(	"CTRL c | v",			"Copy | Paste"  											)
		add_key(	"Home",					"Set default value"  										)
		add_key(	"End",					"Set default inactive value"  								)
		add_key(	"PageDown | PageUp",	"Set Min | Max"  											)

		--todo explicit
		add_key(	"SHIFT CTRL c | v",			"Idem but with all the BU inside (BU names are used for the paste)"	)
		add_key(	"Click Double | Space",		"Toggle full screen"									)
		add_key(	"Click Double on top",		"in the 10% at the top of the BU toggle mini size"		)
		add_key(	"CTRL Space",			"Idem: toggle mini size"								)

	add_title(	"MONITOR which are special BU (BU_MONITOR)"	)
		add_tab(1,	"Next two combinations are described first in case but are confusing if you don't understand yet banks and binds" )
		add_key(	"  CTRL Click",				"Texture bind increment/decrement depending on left/right location of the click in monitor" )
		add_key(	"  SHIFT CTRL Click",		"Same but increment/decrement by 32" )
		add_key(	"Click", 					"Depend where you click in monitor, grey lines shows the areas"	)
		add_key(	"Click Middle Right",		"Toggle through Alpha mode"	)
		add_key(	"Click Bottom Right",		"Toggle RGBA Display"	)
		add_key(	"Click Middle Top",			"Toggle top left info display"	)
		add_key(	"Click Middle Left",		"Toggle top and bottom left info (d)isplay"	)
		add_key(	"+ | -",					"Increment | Decrement Alpha mode"	)
		add_key(	"Click Middle Bottom",		"Toggle bottom left info display"	)
		add_key(	"Click Top Right",			"Toggle Top draw dialog"	)
		add_key(	"Click Left of Top Right",	"Toggle Display mode dialog"	)

	add_title(	"BU_RECT, these are the green or red rectangles under the MUs"	)
		add_tab(1,	"they hold a value which is the alpha passed to the MUs inside" )
			add_tab(2,	"if alpha is <= 0 there is no render and the BU_RECT is red" )
			add_tab(2,	"the rendering order of the MUs inside is bottom to top then left to right" )
		add_tab(1,	"they are name Render_x where x determine their rendering order" )
		add_tab(1,	"if their glue state is on (symbolized by a bar under the name)" )
			add_tab(2,	"moving the BU_RECT move all the visible MUs inside also" )
		add_tab(1,	"when selected"	)
		add_key(	"CTRL Insert",		"Insert a new one after the existing one : it appear in the center in Red (inactive)"	)
		add_key(	"CTRL Delete",		"Delete it"	)
		add_key(	"CTRL Up Down",		"Move up or down in the BU_RECT order"	)
		
	add_title(	"CAMERA"	)
		add_key(	"Axe",			"by default :"					)
		add_key(	"X is red",		"left to right",	{1,0,0}		)
		add_key(	"Y is green",	"bottom to top",	{0,1,0}		)
		add_key(	"Z is blue",	"back to front"	,	{0,.2,1}	)
		add_tab(	1,	"Infact AAASeed use a right hand reference system with z in your face"	)
		add_key(	"F4"				)
		add_key(	"Double F4",		"Toggle Camera Edit mode, same as a click on BU_CAM"	)
		add_key(	"",					"\tor the corresponding param in Flatland: pref/ui/camera_edit"	)		
	add_title(	"CAMERA if editing is allowed"	)
		add_key(	"SHIFT CTRL F4",	"Reset Camera if in Camera edit mode"			)
		add_key(	"CTRL F4",			"Toggle Camera orbiting play"				)
		add_key(	"Mouse Middle",		"Start camera movement"		)
		add_key(	"Mouse Movement",	"Rotate or with middle button pushed Translate"	)
		add_key(	"Mouse Wheel",		"Move along camera axe (z)"		)
		add_key(	"Mouse Left",		"Validate new position"			)
		add_key(	"Mouse Right",		"Cancel camera move, go back to position before camera move started"	)
		add_key(	"X x Y y Z z",		"Align with axe or reverse"			)
		add_key(	"o | O",			"(o)rthogonal : Toggle perspective"	)
	add_title(	"CAMERA selector in MEUs"	)
		add_key(	"Double Click",		"Focus flatland on Camera"				)
		add_key(	"CTRL Click",		"Copy at position the current Camera"	)
		add_key(	"SHIFT Click",		"Toggle the locking of the camera, can be seen by a red rectangle"	)
	add_title(	"CAMERA in flying mode"	)
		add_key(	"Arrow Up/Down",			"\tTranslate Forward/Backward"	)
		add_key(	"Arrow left/Right",			"\tRotate Left/Right"			)
		add_key(	"SHIFT Arrow Up/Down",		"\tRotate Up/Down"				)
		add_key(	"SHIFT Arrow left/Right",	"\tTranslate Left/Right"		)

-- removed
--		add_key(	"SHIFT CTRL F1",	"Reset Camera Rotate"		)
--		add_key(	"SHIFT CTRL F2",	"Reset Camera Translate"	)

	add_title(	"MEU"	)
		add_key(	"CTRL f | F",		"(f)ind and display MUs containing the string (case insensitive)"	)

	add_title(	"PRESET in particular for MEU"	)
		add_key(	"Click",		"Recall"	)
		add_key(	"CTRL Click",	"Save"		)
		add_key(	"CTRL ALT",		"Remove"	)

	add_title(	"BU_COLOR (the left part of the area where colors are picked )" )
		add_key(	"Click",			"Flip value display"				)
		add_key(	"Click Double",		"Flip rgb to hsv mode"				)

	add_title(	"Mouse Pointer" )
		add_key(	"p",			"Flip (p)ointer visibility when static"	)
		add_key(	"ALT p",		"flip (p)ointer visibility over render area"	)

	add_title(	"Function keys, " )
	add(		"for Flatland ones Flaland need to be ON" )
	add(		"ALT Function key function only with the right ALt for now (2024 Feb)" )
		add_key(	"F1",				"Toggle Help or customer interface is one defined"	)
		add_key(	"CTRL F1",			"Print the lua global table"	)
		add_key(	"SHIFT F1",			"Print the aaa table on 8 levels of depth" 	)
		add_key(	"SHIFT CTRL F1",	"Trigger a redefine of he customer ui"	)	
		add_key(	"F2"				)
		add_key(	"SHIFT F2",			"Toggle on MU/BU_rect part of the interface"		)
		add_key(	"CTRL F2"			)
		add_key(	"SHIFT CTRL F2" 	)
		add_key(	"CTRL ALT F2",		"Reset current mapping scale to 1" )
		add_key(	"F3"				)
		add_key(	"CTRL ALT F3",		"Reset current mapping origin to 0" )
		add_key(	"F4"				)
		add_key(	"Double F4"			)
		add_key(	"SHIFT CTRL F4"		)
		add_key(	"CTRL F4"			)
		add_key(	"ALT F4",			"Quit with confirmation" )
		add_key(	"F5",				"Focus on Mapping"	)
		add_key(	"CTRL F5",			"Focus on Tex_anim"	)
		add_key(	"SHIFT F5",			"Toggle on Tex_anim active"	)
		add_key(	"SHIFT CTRL F5",	"Reset GABUIN background" 	)
		add_key(	"ALT F5",			"Focus on Tex_video"	)
		add_key(	"F6",				"Focus on Render"	)
		add_key(	"CTRL F6",			"Focus on Model"	)
		add_key(	"SHIFT CTRL F6",	"Flip ga calage" 	)
		add_key(	"F7",				"Color or Lights Focus depending on render state"	)
		add_key(	"CTRL ALT F7",		"  like F7 but using the inverse of render state"	)
		add_key(	"F8",				"Focus on bdd_mocap"	)
		add_key(	"F9",				"Focus on layers traxs"		)
		add_key(	"ALT F9 | CTRL F9",	"Focus on Module traxs"		)
		add_key(	"CTRL ALT F9",		"Focus on Modules traxs"	)
		add_key(	"F10"				)
		add_key(	"Double F10"		)
		add_key(	"Triple F10"		)
		add_key(	"CTRL F10"			)
		add_key(	"F12"				)

	add_title(	"BANK"	)
		add_key(	"CTRL Arrow",		"Move bind in direction"		)
	--	add_key(	"CTRL c",		"Move bind in direction"		)
	--	add_key(	"Delete",			"Toggle Camera orbiting play"				)

	add_title(	"Double (n or ALT then" )
		add_key(	"j | J",	"Focus on (j)oystick"	)
		add_key(	"l | L",	"Focus on (l)ua_master"	)
		add_key(	"m | M",	"Focus on (m)idi_a"		)
		add_key(	"n | N",	"Triple n: Focus on (n)etwork"	)
		add_key(	"s | S",	"Focus on (s)tereo"		)
		add_key(	"d | D",	"Focus on (D)MX"		)
		add_key(	"3",		"Focus on (3)D mouse (SpaceMouse)"	)

	add_title(	"Double H then"	)
--		add_key(	"a | A",	"(a)utoconnect to midi trax"	)
--		add_key(	"b | B",	"Disconnect trax (flatland)"	)
		add_key(	"e | E",	"Switch to (e)nglish keyboard"	)
		add_key(	"f | F",	"Switch to (f)rench keyboard"	)
		add_key(	"x | X",	"Switch to mac french keyboard"	)
		add_key(	"R",		"Enable (R)emote send"			)
		add_key(	"r",		"Disable (R)emote send"			)

	add_title(	"Miscellaneous" )
		add_key(	"u",		"Flip BU_SEND"				)
		add_key(	"e",		"Flip global (e)rase"		)
		add_key(	"E",		"Flip global (E)rase Depth"	)

	add_title(	"Specialize your environment"	)
		add(		"Open AAAKernel/GaBu/GaBu_BU/GA_user/GA_user.lua" )
		add_tab(1,	"add a field in GA.__user_machines with your name and the list of the machine you use"	)
		add_tab(1,	"then you can use GA:is_user_name( \"toto\" ) to test if you are toto"					)
		add_tab(1,	"The hello message at start display the user_name found"								)

	add_title(	""	)
	add(		"We warmly thanks here AAAproduction (www.aaaproduction.fr) who kindly allow us"	)
	add_tab(1,	"to use the Shadoks Ga Bu Zo Meu language for this Open Source Software"	)

	return lines
end

function GA_HELP:draw()
	if not self.b_active then return end

	--todo this a hack
	if aaa.keyboard.is_ctrl() then
		if	aaa.mouse.is_wheel_up()			then self:dec_resolution()
		elseif	aaa.mouse.is_wheel_down()	then self:inc_resolution()
		end
	else
		if	aaa.mouse.is_wheel_up()			then self:inc_line( -1)
		elseif	aaa.mouse.is_wheel_down()	then self:inc_line( 1)
		end
	end

	local tab = self:define()

	local t = 2.25
	local b = -t
	local l = -4
	local line_nb = self.line_nb
	local dy = (t-b) / (line_nb+3)
	local sy = dy * 1.1
	local sx = sy * 1.1
	local x,y = l+sx*.5, t
	local d = .05 * sy

	local text = self.text
	gol.color_black( .4 )
	aaa.draw_rect( l, b, x+5*32/line_nb,	t )

	--we do that because we use a single bdd_text
	local ft_pushed = text:get_font_type()
	text:set_font_type( "glut_line" )

	local line_width = 24 * BU.__draw_text_line_width / line_nb

	local function draw_str( str, x,y, f, color )
		text:set_next_shadow()
		text:draw( 0, str, x,y, sx*f,sy*f  )
	end
	local function draw_line( t, x,y )
		local str = t[1]
		local key = t.key
		local b_title = t.b_title
		local f = b_title and 1.3 or 1
		y = y - dy * (b_title and 1.6 or 1)
		if t.tab_nb then x = x + t.tab_nb * sx end

		gol.set_line_width( line_width * (b_title and 2 or 1.5) )
		local color = t.color or {1,1,1}
		if color then
			gol.color( color[1], color[2], color[3], 1 )
		else
			gol.color_white()
		end
		if key then
			draw_str( key, x+sx,	y,	f	)
			draw_str( str, x+sx*8,	y,	f	)
		else
			draw_str( str, x,		y,	f	)
		end
		return y
	end

	--aaa.show(  #tab-line_nb+8, "nb" )
	--aaa.show( help.line_begin, "help.line_begin" )
	--local line_begin = wrap_int( self.line_begin, 1, #tab )
	local line_begin = clamp( self.line_begin, 1, #tab-2-2 )
	self.line_begin = line_begin
	--aaa.show( line_begin,+ "line_begin" )
	y = draw_line( tab[1], x,y )
	y = y + dy
	y = draw_line( tab[2], x,y )
--	y = draw_line( tab[3], x,y )
--	y = draw_line( tab[4], x,y )
--	y = y - dy*.5

	for i=line_begin+2,100000000 do
		local t = tab[i]
		if t then
			y = draw_line( t, x,y )
			if y < b then break end 
		else
			break
		end
	end

	text:set_font_type( ft_pushed )
end


-- KEYBOARD
--
function GA_HELP:do_key( key )
	--aaa.print_fn()
	local b_key_used = true
	if		key == 8 and aaa.keyboard.is_ctrl() then	self:flip_help()	-- CTRL h		
	else		
		if self:is_help() then
			b_key_used = true
			if		key==43	then	self:dec_resolution()	-- +
			elseif	key==45	then	self:inc_resolution()	-- -
			else b_key_used = false
			end
		else
			b_key_used = false 
		end
	end
	return b_key_used
end

function GA_HELP:do_key_special( key )
	aaa.print_fn()
	local b_key_used = true
	if aaa.keyboard.is_key_fn( key, 1 ) and aaa.keyboard.is_no_modifier() then
		if not ga:flip_ui_group_active( "customer" ) then 
			self:flip_help()
		end
	else
		b_key_used = self:is_help()
		if b_key_used then
			b_key_used = true
			if		key == 279					then self:inc_line( -self.line_nb )	-- page up		
			elseif	key == 280					then self:inc_line( self.line_nb )	-- page down
			elseif	key == 272 or key == 274	then self:inc_line( -1 )	-- arrow up left
			elseif	key == 273 or key == 275	then self:inc_line( 1 )	-- arrow down right
			elseif	key == 277 					then self.line_begin = 1	-- home
			elseif	key == 278					then self.line_begin = 100000	-- end
			else b_key_used = false
			end
		end
	end
	return b_key_used
end


