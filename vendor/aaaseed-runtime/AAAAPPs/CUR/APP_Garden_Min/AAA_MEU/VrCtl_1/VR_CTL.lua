CLASS.DECLARE( "VR_CTL", nil,  {
	my_var = 0,
	scale = 1,
	} )

function VR_CTL:init_ref( obj_ref )
	local ref = {}
	self.ref = ref
	ref.ctl = obj_ref
	--todo change to translate and rotate
	ref.tx = param.get_ref( ref.ctl, "translation_x" )
	ref.ty = param.get_ref( ref.ctl, "translation_y" )
	ref.tz = param.get_ref( ref.ctl, "translation_z" )
	ref.rx = param.get_ref( ref.ctl, "rotation_x" )
	ref.ry = param.get_ref( ref.ctl, "rotation_y" )
	ref.rz = param.get_ref( ref.ctl, "rotation_z" )
	ref.dx = param.get_ref( ref.ctl, "m20" )
	ref.dy = param.get_ref( ref.ctl, "m21" )
	ref.dz = param.get_ref( ref.ctl, "m22" )
	ref.buts = {}
	ref.buts[1] = param.get_ref( ref.ctl, "button_1" )
	ref.buts[2] = param.get_ref( ref.ctl, "button_2" )
	ref.buts[3] = param.get_ref( ref.ctl, "button_3" )
	ref.buts[4] = param.get_ref( ref.ctl, "button_4" )
	ref.padx = param.get_ref( ref.ctl, "value_1")
	ref.pady = param.get_ref( ref.ctl, "value_2")
end

function VR_CTL:create( name, obj_ref )
	local self = VR_CTL:create_instance( name )
	self:print( "create()" )
	self:init_ref( obj_ref )
	self.pos = V3.new()
	self.rot = V3.new()
	self.dir = V3.new()
	self.buts = {}
	for i=1,4 do
		local but = {}
		but.value= false
		but.b_has_changed = false
		but.time_pressed = 0
		but.start_timer = 0
		self.buts[i] = but
		--same as self["buts"][i] = but
	end
	self.padx = 0
	self.pady = 0
	return self
end

function VR_CTL:update_pos_and_rot()
	local ref = self.ref
	local pget = param.get
	local vec = self.pos
	vec[1] = pget(ref.tx)
	vec[2] = pget(ref.ty)
	vec[3] = pget(ref.tz)
--	self:print( ref.ty )
	vec = self.rot
	vec[1] = pget(ref.rx)
	vec[2] = pget(ref.ry)
	vec[3] = pget(ref.rz)
	--	self:print( "zobi "..self.pos[2] )
	return self.pos, self.rot
end

function VR_CTL:update_dir()
	local ref = self.ref
	local pget = param.get
	local vec = self.dir
	vec[1] = pget(ref.dx)
	vec[2] = pget(ref.dy)
	vec[3] = pget(ref.dz)
	return self.dir
end

function VR_CTL:update_buts()
	-- Update buttons states --
	-- if b_buts_has_changed
	local ref = self.ref
	for i=1, 4 do
		local but = self.buts[i]
		local but_last = but.value
		local but_val = param.get( ref.buts[i] )
		local b_changed = (but_last ~= but_val)
		if b_changed then
			but.value = but_val
		end
		but.b_has_changed = b_changed
		if but.b_has_changed and but.value == 1 then
			but.time_pressed = 0
			but.start_timer = aaa.time.t
		end
		if but.value == 1 then
			but.time_pressed = but.time_pressed + aaa.time.t - but.start_timer
		end
		if but.b_has_changed and but.value == 0 then
			but.time_pressed = 0
			but.start_timer = 0
		end
	end
	-- Then update touchpad x and y --
	self.padx = param.get( ref.padx )
	self.pady = param.get( ref.pady )
end

function VR_CTL:get_rot()	return self.rot				end
function VR_CTL:get_pos()	return self.pos				end
function VR_CTL:get_dir()	return self.dir				end
function VR_CTL:get_buts()	return self.buts 			end
function VR_CTL:get_pads()	return self.padx, self.pady	end

function VR_CTL:update()
	self:update_pos_and_rot()
	self:update_dir()
	self:update_buts()
end

CLASS.DECLARE( "VR_CTX", nil, {
	ctl_name_short	= { "hmd", "ctl_1", "ctl_2", "ref_1", "ref_2" },
	ctl_name		= { "hmd", "controller_1", "controller_2", "reference_1", "reference_2" },
--	my_var = 0,
--	scale = 1,
	} )

function VR_CTX:init_ref( obj_name )
	local ref = {}
	self.ref = ref
	ref.aaavr = aaa.obj.get_no_error( obj_name )
	self.b_device_use = { true, true, true, true, true }
	self.b_device_draw = { true, true, true, true, true }
end

function VR_CTX:create( name )
	local self = VR_CTX:create_instance( name )
	--self:print( "create()" )
	local obj_name = aaa.dir.get_dir_tracker().."/default.aaavr"
	self:init_ref( obj_name )
	local ref = self.ref
	if ref.aaavr then
		self.ctls = {}
		for i, str in ipairs(self.ctl_name) do
			local obj = param.get_obj_attached( ref.aaavr, str.."->" )
			local ns = self.ctl_name_short[i]
			self.ctls[ns] = VR_CTL:create( ns, obj )
		end
	end
	return self
end

function VR_CTX:get_ctl( i_or_name )
	if i_or_name ~= nil and type(i_or_name) == "number" then
		i_or_name = self.ctl_name_short[i_or_name]
	end
	return self.ctls and self.ctls[i_or_name]
end

function VR_CTX:update_all()
	for i=1,5 do
		if self.b_device_use[i] then
			local ctl = self:get_ctl( i )
			if ctl then
				ctl:update()
			end
			--ctl:print( ctl.pos[2] )
		end
	end
end

function VR_CTX:draw_ctl( i, layer )
	local ctl = self:get_ctl( i )
	if not ctl then return end
	local tra = ctl.pos
	local rot = ctl.rot

	gol.push_matrix()
		gol.translate( tra[1], tra[2], tra[3] )
		gol.rotate_x( rot[1] )
		gol.rotate_y( rot[2] )
		gol.rotate_z( rot[3] )
		if i < 4 then
			aaa.obj.update_then_draw(layer)
		else
			aaa.draw_box( .2, .1, .1	)
		end
	gol.pop_matrix()
end

function VR_CTX:draw( meu )
	--local layers = aaa.layers.get_cur()
	--if not layers then
	--	return
	--end
	--local layer_ctl = aaa.layers.get_layer(layers, 1)
	--local layer_hmd = aaa.layers.get_layer(layers, 2)
	--aaa.layers.draw_begin( layers )
	--aaa.layers.draw_layer_all( layers )

		for i=5,1,-1 do
			if self.b_device_draw[i] then
				if i == 1 then
					self:draw_ctl( i, meu:get_layer( 2 ))
				elseif i < 4 then
					self:draw_ctl( i, meu:get_layer( 1 ) )
				else
					self:draw_ctl( i, nil )
				end
			end
		end
	--aaa.layers.draw_end( layers )
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera( nil, 8 )

	local ix=1
	local iy=1
	local names 		= VR_CTX.ctl_name
	--local names_short	= VR_CTX.ctl_name
	for i=1,5 do
		bu = self:add_button( {ix, 	iy+i, 4 }, "use_"..names[i],	self.ctx.b_device_use,  i, true )
			bu:set_text( names[i] )
		bu = self:add_button( {ix+4, iy+i, 2 }, "draw_"..names[i],	self.ctx.b_device_draw, i, true )
			bu:set_text( "Draw" )
	end
end

function meu:init()
	if not self.ctx then
		self.ctx = VR_CTX:create( "ctx" )
	end
end

function meu:update()
	local ctx = self.ctx
	if ctx then
		ctx:update_all()
	end
end

function meu:draw()
	self:draw_layers_begin()
		local ctx = self.ctx
		if ctx then
			ctx:draw( self )
		end
	self:draw_layers_end()
end

function meu:draw_icon()
	meu:draw_icon_text( "VR" )
end




local tab = VR_CTL:get_doc()
	tab.init_ref = 				"Inits self.ref with param.get_ref on every Vive controller params ."
	tab.create =				"Constructor for VR_CTL, calls init_ref to link with controller params and  "..
								"then initialize vars in self : rot, pos, dir, buts, padx, pady ."
	tab.update_pos_and_rot =	"Uses param.get( self.ref.tx-y-z / self.ref.rx-y-z ) to update self.pos and "..
								"self.rot . It means that controller position and rotation are updated ."
	tab.update_dir =			"Uses param.get( self.ref.dx-y-z ) to update self.dir . It means that       "..
								"controller direction is updated ."
	tab.update_buts =			"Uses param.get( self.ref.buts[i] ) to update self.buts[i].value and other  "..
								"vars : ..buts[i].b_has_changed, ..buts[i].time_pressed and                 "..
								"..buts[i].start_timer. It means that the state of the 4 controller buttons "..
								"are updated ."
	tab.get_rot =				"Getter for self.rot ."
	tab.get_pos =				"Getter for self.pos ."
	tab.get_dir =				"Getter for self.dir ."
	tab.get_buts =				"Getter for self.buts ."
	tab.get_pads =				"Getter for self.padx, self.pady ."
	tab.update =				"Calls update_pos_and_rot, update_dir and update_buts to update general     "..
								"state of the controller ."


tab = VR_CTL:get_doc()
	tab.init_ref =				""
	tab.create =				"Constructor for VR_CTX, calls init_ref to link with aaavr object, then     "..
								"initialize a pair of controllers by instantiating a pair of VR_CTL ."
	tab.get_ctl =				"Getter for self.ctls and self.ctls[i_or_name] ."
	tab.update_all =			"Calls ctl:update() for every active ctl ."
	tab.draw_ctl =				"Draws a controller, gets it with self:get_ctl( i ) .                       "..
								"Uses corresponding layer for hmd ( headset ) and ctl ( hand controller ),  "..
								"draws with aaa.draw_box otherwise. Hmd and ctl layers contain corresponding"..
								"3d object ."
	tab.draw =					"Draws all active VR_CTL in VR_CTX, with self:draw_ctl( i, layer or nil ) . "


--[[
function VR_CTL:update_all()
	----	Consider metatable for Vive / ctls / headset / base station ?	----
	if not self.ctls then
		self.ctls =  {}
		for i=1,4 do
			local obj_name =  aaa.dir.get_dir_tracker().."/default.aaavr."..string.char(97+i)..".sensor"
			--self:box_good( obj_name )
			--if aaa.obj.get_no_error( obj_name ) then
			self.ctls[i] = VR_CTL:create( "ctl_"..i, obj_name ) -- flag ctl_type
			if aaa.obj.get( obj_name ) then
			end
		end
	end
	for i=1,2 do
		local ctl = self.ctls[i]
		ctl:update() -- flag ctl_type

		-- stocker les refs des layers puis get_obj_down
		-- draw puis do transfo
		-- update and draw
		-- pop
		local transfo = aaa.obj.get_by_name( "transfo_ctl"..i )
		local box = aaa.obj.get_by_name( "Vive_box_ctl"..i )
		--self:box_good( box )
		ctl:move_box( box, transfo )
		ctl:rotate_box( box, transfo )
		self:print( self.ctls[i] )
	end
	for i=1,2 do
		local ctl = self.ctls[i+2]
		local transfo = aaa.obj.get_by_name( "transfo_base"..i )
		local box = aaa.obj.get_by_name( "Vive_box_base"..i )
		ctl:update_pos_and_rot()
		ctl:move_box( box, transfo )
		--pr:print(self.tx)
		--self:print( self )
	end


	local layers = aaa.layers.get_cur()
	aaa.layers.draw_begin( layers )
	aaa.layers.draw_layer_all( layers )
	aaa.layers.draw_end( layers )
	aaa.layers.skip_rest()
end

VR_CTL:update_all()
--]]