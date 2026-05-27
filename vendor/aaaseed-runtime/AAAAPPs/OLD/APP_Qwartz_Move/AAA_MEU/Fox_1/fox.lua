CLASS.DECLARE( "FOX", nil, {
	defs = {},
	def_ui = {},
	b_done = false,
--			spe_scale_use = 0
	} )

	local dur_base = { 15.5, 30.0 }
	local dur_long = { 20.0, 40.0 }
	local dur_text = { 10.0, 30.0 }

	FOX.defs[1]		= {	"spiral",			10,		scale_z=.1											}
	FOX.defs[2] 	= {	"skel",				100,	b_repeat=true,			scale_z=.1,		fade=1		}
	FOX.defs[3] 	= {	"rgb_pursuit_1",	10,		duration = dur_long,	b_repeat=true,	scale_z=.1	}
	FOX.defs[4] 	= {	"countdown",		10,		duration = 10,  		next = 2					}
	FOX.defs[5] 	= {	"spiral_math",		10,		scale_z=.1											}
	FOX.defs[6] 	= {	"adn",				10,		scale_z=.3											}
	FOX.defs[7] 	= {	"suns",				80,		duration = dur_long									}
	FOX.defs[8]		= {	"suns_70",			80,															}
	FOX.defs[9]		= {	"suns_71",			80,		scale_z=.2											}
	FOX.defs[10]	= {	"torus",			80,		scale_z=1.											}
	FOX.defs[11]	= {	"noise_2d",			0,															}
	FOX.defs[12]	= {	"noise_3d",			80,															}
	FOX.defs[13]	= {	"noise_3d_z",		80,		duration = dur_long 								}
	FOX.defs[14]	= {	"point_bdd",		80,		duration = { 10, 30 } 								}
	FOX.defs[15]	= {	"text",				80,		duration = dur_text 								}
	FOX.defs[16] 	= {	"kinect",			80,		b_repeat=true										}

function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local def_ui = FOX.def_ui

	local nb = 20
	local dx = 12
	local s = dx/(nb+0)
	local ix, iy

	ix = 9
	for i=1,nb do
		local ui = {}
		local iy = 2 + (i-1)*s
		bu = self:add_button( {ix+1, iy, s*.8, s*.8 }, "check_"..tostring(i) )
			bu:set_method_on_click( self, "flip_fn", i )
			bu:set_text_draw( false )
			--bu:set_text_factor( .5 )
			ui.enable = bu
		local text =  FOX.defs[i] and FOX.defs[i][1] or ""
		if text and text~="" then
			self:add_text(	{ix+s*2.5,iy,	3,s*.8},	text )
		end
		--todo something missing : a slider with a name but not showned text
		--todo specify number of digits
		bu = self:add_slider(	{ix+4.5,iy,	3.5,s}, "R"..i, nil, nil, nil, 0, 1 )
			ui.rank = bu

		def_ui[i] = ui
	end

	bu = self:add_selector(	{ix,2-s,	1,dx+s}, "Fox" )
		bu:set_nb_min_0( 1, nb+1 )
		bu:set_item_text_from_nb_minus_1( 2 )
		--bu:disable_mobile()
		def_ui.vox_selector = bu

	ix, iy = 1, 1
	bu = self:add_button( {ix,iy, 1,1}, "Manual" )
		def_ui.manual = bu
	bu = self:add_trig(	{ix+4, iy }, "Change" )
		def_ui.change = bu
	local sub = {}
	iy = iy + 1.4
	local dy = 1.2
	for i = 1,4 do
		bu = self:add_selector(	{ix,iy+(i-1)*dy,	8,dy}, "Rnd_"..i )
			bu:set_nb_min_0( 9, 2 )
			bu:set_item_text_from_nb_minus_1( 2 )
		--bu:disable_mobile()
		sub[i] = bu
	end
	def_ui.rnd_sel = sub

	local ui = self.ui

	local ix = 1
	local iy = 7.4
	ui.color = {}
	local sy

	sy = 1
	iy = iy + .2
	bu  = self:add_selector(	{ix,iy,	8,sy}, "col_in" )
		bu:set_nb_min_0( 8, 2 )
		bu:set_item_text_from_nb()
			--bu:set_min_max( 1, 8 )
		ui.color[1]  = bu
	iy = iy + 1.2
	bu  = self:add_selector(	{ix,iy,	8,sy}, "col_out" )
		bu:set_nb_min_0( 8, 2 )
		bu:set_item_text_from_nb()
		--bu:set_min_max( 1, 8 )
		ui.color[2]  = bu
	iy = iy + 1
	self:add_button( {ix,iy, 1,1}, "Inverse", self, "invert_color" )
	iy = iy + 1
	bu  = self:add_selector(	{ix,iy,	8,sy}, "Back" )
		bu:set_nb_min_0( 8, 2 )
		bu:set_item_text_from_nb()
		--bu:set_min_max( 1, 8 )
		ui.color[3]  = bu
		app.bu_color_sel_back = bu

--	bu = self:add_button( {ix, 12 }, "Scale Z", FOX, "spe_scale_use" ) --todo careful this 0 or 1 not true or false
	bu = self:add_button( {1,12, 4,1}, "Mood" )
		bu:set_multiple( { "zen", "+", "++" }, "file_menu" )
		bu:set_item_data( 1,  "Zen", "Energique", "A fond" )
		ui.mood = bu
end

function meu:set_color( id1, id2 )
	local color_sel = self.ui.color
	color_sel[1]:set_value( id1-1 )
	color_sel[2]:set_value( id2-1 )
end


function meu:flip_fn( i )
	aaa.print_method()
end
function meu:init()
	local ref = self.ref
end

function meu:update()
	local ref = self.ref
	local color = self.ui.color
	if true then
		local id1 = color[1]:get_value()+1
		local id2 = color[2]:get_value()+1
		if self.invert_color and self.invert_color > .5 then
			id1,id2 = id2,id1
		end
		app:set_color( id1, id2 )
	else
		--self:print( self.ui )
		local i = (self.invert_color and self.invert_color > .5 )and 2 or 1
		VOX.update_def_color_with_bu( color[i], color[3-i] )
	end

end

function FOX:pick_fox()
	local next = self.def.next
	local id
	if next then
		id = next
		aaa.print( "Switch to next -> "..next )
		self.def.next = nil
	else
		local defs = FOX.defs
		local def_ui = FOX.def_ui
		local nb = #defs
		local try_left = 100
		repeat
			local b_ok = false
			repeat
				id = math.random(nb)
				try_left = try_left - 1
				if try_left < 0 then
					b_ok = true
				else
					if def_ui[id].enable:get_value_as_bool() then
						b_ok = id ~= self.id_def or defs[id].b_repeat
					end
				end
			until b_ok
			--self:print( "try_left "..try_left.." : "..id )
		until math.random() < def_ui[id].rank:get_value() or try_left < 0
		self:print( "Done try_left "..try_left.." -> "..id )
	end
	return id
end

function FOX:create( name )
	local self = FOX:create_instance( name )
	self:init()
	self.ref = {}
	return self
end
function FOX:set_seq( id_def )
	if id_def > #(FOX.defs) then
		id_def = 1
	end
	local def = FOX.defs[id_def]
	self.id_def			=	id_def
	self.render_name	=	def[1]
		self:print( "Switch to "..def[1] )
	self.proba			=	def[2]
	self.def			=	def
	self.fade			=	def.fade or .2

	local dur = def.duration or dur_base
	if type(dur) == "table" then
		dur = interpolate( dur[1], dur[2], math.random() )
	end
	self.duration = dur
	self:init()
	return self
end
function FOX:init()
	self:start()
end
function FOX:start()
	self.t = -42
end
function FOX:stop()
	self.b_done = true
	self.vox:set_done( false )
end
function FOX:update_time()
	local def_ui = self.def_ui

	local b_manual = def_ui.manual:get_value_as_bool()
	local id
	local b_change = def_ui.change:get_value_as_bool()

	if b_manual then
		if b_change then
			id = def_ui.vox_selector:get_value()
			if id==0 then
				id = self:pick_fox()
			end
		end
	else
		if (self.b_done or b_change) and not app.meu_kinect:is_skeleton() then
			id = self:pick_fox()
		end
	end
	if id then
		self:set_seq( id )
		self.b_done = false
		self.vox:set_done( false )
	end

	local def = self.def
	local t = self.t
	--self:print( "time is "..t )
	if t and t >= 0 then
		local dur = (self.duration or 10) * 1
		--self:print( dura )
		t = t + aaa.time.dt --here
		self.t = t
		local fade = self.fade or 1
		local v
		if t < fade then
			v = t/fade
		elseif t < (dur-fade) then
			v = 1
		elseif self.t >= dur then
			if b_manual then
				v = 1
			else
				self:stop()
				v = 0
			end
		else
			v = (dur-t)/fade
		end

		--aaa.print( "FOX:update_time() : "..self.meu )
		self.meu:set_render_color_factor( nil, nil, nil, v )

		if self.t > dur or self.vox:is_done() then
			self:stop()
		end
	else
		--self:print( "Start set time to 0" )
		self.t = 0
	end
end
function FOX:update()
	FOX.cur = self
end

function FOX:set_meu_colors( meu )		self.meu_colors		= meu	end
function FOX:draw()
	FOX.cur = self
	local bdd = aaa.bdd
--	self:print( meu )
	local dst = 1
	bdd.render_op1(	"set_target",	dst		)

	local r,g,b,a = app:get_rgba_back()
	bdd.render_op1(	"fill", dst, r,g,b,a )
	--bdd.render_op1(	"scale",		dst,		1,1,1,1	)

	local vox = self.vox
	bdd.set_color_map( 1, -1 )
	vox:set_before_render()
	if app.meu_kinect:is_skeleton() then
		self.meu:set_render_color_factor( nil, nil, nil, 1 ) --avoid semi transparent character
		vox:render_kinect_low()
		local tk = self.t_kinect
		tk = tk or 0
		tk = tk + aaa.time.dt * .80
		self.t_kinect = tk
		if tk < 1 then
			vox:render_one_sun( tk )
		end
		self.b_done = true
	else
		self.t_kinect = 0
		self:update_time()

	--[[
		--self:print( self.b_scale_z )
		local def = self.def
		if FOX.spe_scale_use > .5 then
			local sz = def.scale_z or 1.
			self.meu:set_render_scale_z( sz )
		else
			self.meu:set_render_scale_z( 1 )
		end
	--]]
		aaa.bdd.render_op1(	"set_target", 1 )
		local name = self.render_name
		if name then
			local fn_name = "render_"..name
			vox[ fn_name ]( vox, self.t )
		end
	end
end

--[[
CLASS.DECLARE( "FOXS" )

function FOXS:create( name )
	local self = FOXS:create_instance( name )
	self:init()
	return self
end
function FOXS:init()
	self.t_fox = {}
end
function FOXS:add( fox )
	table.insert( self.t_fox, fox )
	return fox
end
function FOXS:remove( fox )
	array.remove_by_val( self.t_fox, fox )
	return fox
end
--]]
