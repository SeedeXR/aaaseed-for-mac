CLASS.DECLARE( "VR_FX", nil,  {
	threshold = 0.005,
	s_def = 0.1,
	b_draw = true,
	b_draw_lua_strip = true,
	color = { r = 1, g = 1, b = 1, a = 1 },
	b_active = false,	--todo init is too long this should be refined
	}
)

function VR_FX:init_ref(  )
	local ref = {}
	self.ref = ref
end

function VR_FX:create( name, ctl, curve )
	local self = VR_FX:create_instance( name )
	self:init_ref( )
	self.curve = curve
	self.b_drawing = false
	self.last_pos = V3.new_from(0, 0, 0)
	self.ctl = ctl
	local dataset
	for i=1,9999 do
		if aaa.bdd.get_control_point_nb(curve, i) == 0 then
			dataset = i
			break
		end
	end
	self.dataset = dataset and dataset-2 or 9997
	self.strokes = {}
	self:fill_stroke_all()
	return self
end

function VR_FX:get_stroke( dataset )
	local index = (dataset+1)/2
	local stroke = self.strokes[index]
	if not stroke then
		stroke = { float_tab = {}, float_nb = 0, color = { r=1, g=1, b=1, a=1 } }
		self.strokes[index] = stroke
	end
	return stroke
end

function VR_FX:make_stroke_filename( dir, id_fx, id_dataset )
	return dir.."fx_"..id_fx.."_stroke_"..((id_dataset+1)/2)..".mlua"
end

function VR_FX:load_strokes( dir, fx_id )
	for i=1,self.dataset,2 do
		--self:print( "VR_FX : load stroke "..(i+1)/2 )
		local color = self:get_stroke( i ).color
		tab = {}
		--self:print( aaa.file.do_if_exist( self:make_stroke_filename( dir, fx_id, i ) ) )
		color.r = tab.r or 1
		color.g = tab.g or 1
		color.b = tab.b or 1
		color.a = tab.a or 1
	end
end

function VR_FX:save_stroke( fname, stroke )
	if stroke then
		local  col = stroke.color
		local text = "tab.r = "..col.r
		text = text.."\ntab.g = "..col.g
		text = text.."\ntab.b = "..col.b
		text = text.."\ntab.a = "..col.a
		self:print( aaa.file.save_text( fname, text ) )
	end
end

function VR_FX:save_strokes( dir, fx_id )
	if not self.dataset then
		self:print("no dataset")
		return
	end
	for i=1,self.dataset,2 do
		local stroke = self:get_stroke( i )
		if stroke then
			self:save_stroke( self:make_stroke_filename( dir, fx_id, i ), stroke )
		end
	end
end

function VR_FX:fill_stroke_all()
	for i=1,self.dataset,2 do
		self:fill_stroke( i )
	end
end

function VR_FX:dec_dataset()
	if self.dataset > -1 then
		self.dataset = self.dataset - 2
	end
end
function VR_FX:inc_dataset()
	if self.dataset < 9997 then
		self.dataset = self.dataset + 2
	end
end

function VR_FX:set_size()
	local ctl = self.ctl
	if self.bu_size then
		local x, y = ctl:get_pads()
		if x ~= 0.0 or y ~= 0.0 then
--			self.bu_size:set_value(  )
			self.bu_size:set_value_cano( (x + 1) / 2 )
		end
	end
end

function VR_FX:get_stroke_nb_pts( )
	local nb = 0
	for i=1,self.dataset,2 do
		local stroke = self:get_stroke( i )
		if stroke then
			nb = nb + stroke.float_nb
			--self:print( stroke.float_nb )
		end
	end
	return nb / 6
end

VR_FX.b_tab = true --todo crash with true
function VR_FX:fill_stroke( dataset )
	-- if true then
	-- 	self:print( "we should solve this" )
	-- 	return
	-- end

--	aaa.print_fn()
	--self:box_debug( "LA" )
	local ls = aaa.bdd.get_len( self.curve, dataset )
	if ls > 0 then
		local stroke = self:get_stroke( dataset )
		local index = 1
		local tab
		local str_dbg
		if VR_FX.b_tab then
			tab = stroke.float_tab
			str_dbg = "float_tab"
		else
			tab = stroke.new_tab or aaa.array.new_fp32( 300000 )
			stroke.new_tab = tab
			--self:print( tab )
			str_dbg = "new_tab"
		end
		if true then
			--self:print( "on attaque "..self.s_def.." "..ls )
			local t1 = aaa.bdd.get_points_s( self.curve, dataset,	self.s_def/ls, 0, 1 )
			local t2 = aaa.bdd.get_points_s( self.curve, dataset+1,	self.s_def/ls, 0, 1 )
			if t1 and t2 then
				local nb = math.min( #t1, #t2 )
				--self:box_good( str_dbg.."\n"..#t1.." "..#t2.."\nnb is "..nb.." "..nb/3 )
				local i1=1
				for i=1,nb/3 do
					tab[index]		= t1[i1]
					tab[index+1]	= t1[i1+1]
					tab[index+2]	= t1[i1+2]

					tab[index+3]	= t2[i1]
					tab[index+4]	= t2[i1+1]
					tab[index+5]	= t2[i1+2]
					index = index + 6
					i1 = i1 + 3
				end
			end
		else
			for j=0,1,self.s_def/ls do
				local px, py, pz = aaa.bdd.get_point_s( self.curve, dataset , j )
				tab[index] = px
				tab[index+1] = py
				tab[index+2] = pz
				px, py, pz = aaa.bdd.get_point_s( self.curve, dataset + 1, j )
				tab[index+3] = px
				tab[index+4] = py
				tab[index+5] = pz
				index = index + 6
			end
		end
		stroke.float_nb = index-1

--		stroke.color = { r = 1, g = 1, b = 1, a = 1 }
--		stroke.color.r = self.color.r
--		stroke.color.g = self.color.g
--		stroke.color.b = self.color.b
--		stroke.color.a = self.color.a
	end
	--self:print( "Done" )
end

function VR_FX:push_point( b_inc_dataset )
	local ctl = self.ctl
	local pos = ctl:get_pos()
	local d = V3.dist_squared( self.last_pos, pos )
	if d > self.threshold then
		local scale = ctl.scale
		local dir = ctl:get_dir()
		V3.cpy(self.last_pos, pos)
		if b_inc_dataset then
			self:inc_dataset()
		end
		local pos2 = V3.new()
		if self.size == nil then
			self.size = 0.01
		end
		V3.scale( dir, self.size )
		V3.sub( pos2, pos, dir )
		aaa.bdd.push_control_point_back( self.curve, self.dataset,		pos[1],		pos[2],		pos[3] )
		aaa.bdd.push_control_point_back( self.curve, self.dataset + 1,	pos2[1],	pos2[2],	pos2[3] )
		self:fill_stroke( self.dataset )
	end
end

function VR_FX:clear_points()
	local ctl = self.ctl
	if ctl then
		local b = ctl:get_buts()[3].value
		local trig = ctl:get_buts()[3].b_has_changed
		if b and trig then
			local dataset = self.dataset
			aaa.bdd.clear_control_points( self.curve, self.dataset )
			aaa.bdd.clear_control_points( self.curve, self.dataset + 1 )
			local stroke = self:get_stroke( dataset )
			if stroke then
				stroke.float_nb = 0
			end
			self:dec_dataset()
		end
	end
end

function VR_FX:clear_points_all()
	local ctl = self.ctl
	if ctl then
		local but = ctl:get_buts()[3]
		if but.value == 1 and but.time_pressed > 30 then
			aaa.bdd.clear_datasets( self.curve )
			for i=1,self.dataset,2 do
				local stroke = self:get_stroke( i )
				if stroke then
					stroke.float_nb = 0
				end
			end
			self.dataset = -1
		end
	end
end

function VR_FX:draw()
	for i=1,self.dataset,2 do
		local stroke = self:get_stroke( i )
		--self:print( "draw "..i )
		if stroke then
			local nb = stroke.float_nb
			if nb > 0 then
				--self:print( "draw "..i.." "..nb )
				gol.color( stroke.color.r, stroke.color.g, stroke.color.b, stroke.color.a )
				--gol.color_white()
				if VR_FX.b_tab then
					gol.draw_quad_strip_3d( stroke.float_tab, nb / 3 )
				elseif stroke.new_tab then
					gol.draw_quad_strip_3d( stroke.new_tab, nb / 3 )
				end
			end
		end
	end
end

function meu:load_fx()
	--self:print( "VrFx:load_fx" )
	if not self.fxs then
		aaa.mess.show( "error: cannot load fx, fxs is nil", 0.7 )
	else
		local dir = self:build_dirname()
		--self:box_good( dir )
		for i=1,2 do
			self.fxs[i]:load_strokes( dir, i )
		end
		aaa.mess.show( "Fxs load" )
	end
end

function meu:save_fx()
	self:print( "VrFx:save_fx" )
	if not self.fxs then
		aaa.mess.show( "error: cannot save fx, fxs is nil", 0.7 )
	else
		local dir = self:build_dirname()
		--self:box_good( dir )
		for i=1,2 do
			self.fxs[i]:save_strokes( dir, i )
		end
		aaa.mess.show( "Fxs saved" )
	end
end

function meu:change_def( bu )
	--local dir = self:build_dirname()
	--aaa.print( "VrFx:change_def dir is "..dir )
	aaa.print( "VrFx:change_def" )
	for i=1,2 do
		local fx = self.fxs[i]
		--self:box_good( dir )
		--fx:save_strokes( dir, i )
		fx:fill_stroke_all()
		--fx:load_strokes( dir, i )
	end
end

function meu:stroke_options( bu )
	local bu_val = bu:get_value()
	if bu_val == 0 then
		VR_FX.b_draw = true
	elseif bu_val == 1 then
		VR_FX.b_draw = false
	elseif bu_val == 4 then
		local class_list = self:print_classes( GABU_OBJ )
		print(class_list)
	end
end

function meu:draw_icon()
	meu:draw_icon_text( "VR" )
end

function meu:define_ui()
--	if not VR_FX.b_active then return end

	local ref = self.ref
	local bu
	local par

	if not aaa.lua.global.get('VR_FX') then
		self:box_error( "VR_FR needed and not defined" )
	end
	self:add_camera()

	local ix, iy = 1,1
	bu = self:add_button(	{	ix, iy }, 	"curves",		self,  "b_curves_draw", 	true )
	bu = self:add_button(	{	ix, iy+1 },	"strokes",		self,  "b_strokes_draw",	true )
	bu = self:add_button(	{	ix, iy+2 },	"particles",	self,  "b_particles_draw",	true )

	iy = 10
	bu = self:add_trig_method(	{	ix, iy},	"Load Fx",	self, "load_fx" ):set_color_back("load")
	bu = self:add_trig_method(	{	ix, iy+1},	"Save Fx",	self, "save_fx" ):set_color_back("save")

  	self.bu_rgb = self:add_rgbf(	{9,6,	nil,nil}, nil, false )
	self.bu_rgb.__bu_rgb[1]:set_target_lua( VR_FX.color, "r" )
	self.bu_rgb.__bu_rgb[2]:set_target_lua( VR_FX.color, "g" )
	self.bu_rgb.__bu_rgb[3]:set_target_lua( VR_FX.color, "b" )

	bu = self:add_slider(	{9,3,	4,1}, "Taille", ref, "size", 0.5, 0.01, 0.1 )
		bu:set_target_lua( VR_FX, "size" )
		VR_FX.bu_size = bu

	bu = self:add_slider(	{9,4,	4,1}, "S_Definition", ref, "s_def", 0.001, 0.0001, 0.1 )
		bu:set_target_lua( VR_FX, "s_def" )
		bu:set_method_on_value_change( self, "change_def", bu, self )

	bu = self:add_slider(	{9,5,	4,1}, "Threshold", ref, "thresh", 0.0001, 0.00001, 0.001 )
		bu:set_target_lua( VR_FX, "threshold" )

	bu = self:add_selector(	{13,4,	4,2}, "Selector" )
		bu:set_nb_min_0( 4, 2 )
		bu:set_item_text( 1, "Strip", "Select", "", "" )
		bu:set_item_text( 5, "Print", "", "", "" )
		bu:set_method_on_click( meu, "stroke_options", bu )
end

function meu:init()
	if not VR_FX.b_active then return end

	--self:print( "vrfx meu:init()" )
	local ctx = self:get_meu_by_name( "VrCtl_1" ).ctx
	if not ctx then
		aaa.box_good( "Cannot get VrCtl_1" )
		return
	end

	--local dir = self:build_dirname()
	--self:box_good( dir )

	local fxs = {}
	self.fxs = fxs
	for i=1,2 do
		--self:print( "vrfx meu:init() VR_FX:create "..i )
		fxs[i] = VR_FX:create( "fx_"..string.char(48+i), ctx:get_ctl(i+1), aaa.obj.get_by_name( "LuaVive_curve"..i ) )
	end

	--hack
	self:print_error( "load_fx is commented for now, Jean avoid this in init please" )
	--self:load_fx()
	--self:print( "vrfx meu:init() Done" )
end

--function meu:update()
	--self:print( MEU:get_classes_by_name() )
--end

function meu:draw()
	if not VR_FX.b_active then return end

	self:draw_layers_begin()

		self:draw_layer(1)
		--self:print( "MEU:get_classes_by_name()", MEU:get_classes_by_name() )
		if not self.fxs then
			self:print("no fx instance attached")
			return
		end
		for i=1,2 do
			local fx = self.fxs[i]
			gol.color( 0.25, 0.75, 1, 1 )

			if self.b_strokes_draw then
				if VR_FX.b_draw_lua_strip then
					--self:print( "draw" )
					fx:draw()
				end
			end
			--fx:set_size()

			if VR_FX.b_draw then
				local ctl = fx.ctl;
				if ctl then
					local but = ctl:get_buts()[2]
					local trig = but.b_has_changed
					if but.value == 1 then
						fx:push_point( trig )
					end
				end
				fx:clear_points()
				fx:clear_points_all()
			end
		end
		if self.b_curves_draw then
			self:draw_layer(2)
			self:draw_layer(3)
		end
		if self.b_particles_draw then
			self:draw_layer(4)
		end

	self:draw_layers_end()
end

function meu:save_to_dir_direct( dirname, b_complete )
	local b_retcode = MEU.save_to_dir_direct( self, dirname, b_complete )
	if b_retcode then
	end
end


local tab = VR_FX:get_doc()
	tab.init_ref = 					"Inits self.ref, contains no links to any param for now ."
	tab.create =					"Constructor of VR_FX, initialize following vars in self :                       "..
									"curve, b_drawing, last_pos, ctl, dataset, strokes ."
	tab.get_stroke =				"Getter for a stroke, takes dataset ( nb ) as parameter ."
	tab.make_stroke_filename =		"Makes stroke filename with dir, id_fx, id_dataset. Used to save a stroke ."
	tab.load_strokes =				"Loads every strokes in dir corresponding to fx_id ."
	tab.save_stroke =				"Saves a stroke with given filename and stroke ."
	tab.save_strokes =				"Saves strokes in dir for id_fx, for every available dataset ."
	tab.fill_stroke_all =			"Calls self:fill_stroke( i ) for every available dataset ."
	tab.dec_dataset =				"Decrements 2 to self.dataset ( between -1 and 9999 ) ; self.dataset is the      "..
									"current dataset ."
	tab.inc_dataset =				"Increments 2 to self.dataset ( between -1 and 9999 ) ; self.dataset is the      "..
									"current dataset ."
	tab.set_size =					"Sets stroke width with padx."
	tab.get_stroke_nb_pts =			"Getter for stroke length in number of points ."
	tab.fill_stroke =				"Fills stroke.float_tab or stroke.new_tab, with curves points ."
	tab.push_point =				"Pushes new points in curve if ctl has moved more than the threshold since last  "..
									"push_point() ."
	tab.clear_points =				"Clear points of current stroke ."
	tab.clear_points_all =			"Clear points of every strokes of this VR_FX ."
	tab.draw =						"Draws every strokes with aaa.draw_quad_strip_3d ."
