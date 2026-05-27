--
--	RACE
--		deal with one type of fish
--		and know how to draw it
--		it accumulate layer to draw in CELT (Creature Element: eg eye of a whale, body of the whale)
--		
--
if CLASS.DECLARE( "RACE" ) then
	RACE.catmull_seg_nb = 3
	RACE.size_factor = 1
end


function RACE.create( name, celt_nb )
	local self = RACE:create_instance( name )

	self:print( "CREATING" )
	self.s = {}
	self.ui = {}

--	self.ref = {}
--	local ref = self.ref
	self.__celts = {}
	for i=1,(celt_nb or 1) do
		self.__celts[i] = CELT:create( name, i )
	end

	return self
end

-- len needed to set the fish on the curve
--todo do it by fish

function RACE:set_time_factor( factor )
	--self:print( "set_time_factor()..factor )
	self.time_factor = factor
end

function RACE:get_def()
	local name  = self:get_name()
	local def = RACES.get_race_def()[ name ]
	if not def then
		self:box_error( "no def "..name )
	end
	return def
end
function RACE:is_def_bool( name )	-- we want to know before loading in this case
	local def = self:get_def()
	if def then
		return def[name]
	end
	return false
end
function RACE:get_meu_fbx()
	if self.b_fbx then
		return self.__celts[1].fbx.meu_fbx
	end
end

function RACE:process_def( def )
	def = def or self:get_def()
	if not def then
		self:print_error( "process_def() : No def" )
		return true
	end

	local b_retcode = false
--[[
	if def then		self.def = table.deepcopy( def )
	else			def = self.def
	end
--]]
	self.b_fbx = def.b_fbx
	self.meu_fish_name = def.meu_fish_name
	self:set_time_factor( def.time_factor )

-- LEN
--
	local elt_defs = def.elt_def
	if not elt_defs then
		elt_defs = {{}}
	end

	self:print( "process_def() Begin" )

	self.len_in_curve_segment 	= def.len_in_curve_segment or 3

	local tail = def.tail
	local tail_vert = (tail and tail.vert) and 1 or 0

	local size			=	table.copy_simple( def.size ) or {}
	size.min			=	(size.min or 1) --* RACE.size_factor
	size.max			=	(size.max or 1) --* RACE.size_factor
	size.range			=	size.max - size.min
	self.size			=	size

	local len = 0.
	local celts = self.__celts
	local celt_nb =  #celts
	self:print( "process_def() celt_nb is "..celt_nb )
	local meu_fish
	if self.meu_fish_name then
		meu_fish = app:get_meu_by_name_cached( self.meu_fish_name )
		if meu_fish then
			self.meu_fish = meu_fish
		else
			self:box_error( "CELT:process_def() :  No MEU fish named ".. self.meu_fish_name )
		end
	end
	if celt_nb > 0 then
		for i = 1, celt_nb do
			local elt_def = elt_defs[i]
			if elt_def then
				local celt = celts[i]
				celt:process_def( elt_def, self.b_fbx, meu_fish, tail_vert )
				if i==1 then
					self.b_v2021 = celt.b_v2021
				end
				local celt_len
				if celt.bbox_max then	-- we don't have always geometry associated
					celt_len = celt.bbox_max
				else
					self:print_debug( "process_def() celt "..i.." No bounding box" )
					celt_len = def.len or 1
				end
				len = math.max( len, celt_len )
			end
		end
	else
		self:print_error( "process_def() No celts" )
		len =  def.len or 1
	end

	self:print_inverse( "len is "..len )
	self.__len_max = len * size.max	--this info is for etienne field
	if TANK.cur.b_machine_mono then
		local seg_nb = math.floor( self.len_in_curve_segment +.99999999 )
		self.catmull_seg_nb = seg_nb
		self.seg_len = len / self.len_in_curve_segment	-- fish occupy 3 section out of 4
		self.__len_curve_needed = self.seg_len * (seg_nb + 1) --hack monaco maa take a 10 percent secu
	else
		self.s_offset_for_curve = len * 2.5
		self.seg_len = self.s_offset_for_curve * .2	-- fish occupy 2 section out of 5 ( 2 + one for offs ) + 2 for catmull
		self.catmull_seg_nb = 3
	end
	self.seg_len_over =  1. / self.seg_len	

	-- TAIL
	--
	--table.print( tail, "tail", 2 )
	local begin_seg 	=	tail and tail.begin_seg
	local finish_seg	=	tail and tail.end_seg
	local tail_out = {}
	tail_out.begin		=	begin_seg		and		(self.seg_len * begin_seg)		or	( (tail and tail.begin)		or	(len*.5)	)
	tail_out.finish		=	finish_seg 		and		(self.seg_len * finish_seg)		or	( (tail and tail.finish)	or	 len		)
	tail_out.amp		=	(tail and tail.amp) and (3.1416 * .25 * tail.amp / (  - tail_out.begin )) or nil
	tail_out.freq		=	(tail and tail.freq) or 1.
	tail_out.freq		=	tail_out.freq * 2 * 3.14159
	self.tail			=	tail_out

	self.clip_dist		=	def.clip_dist

	local morph 		= 	def.morph
	self.morph			=	morph
	self.morph_index_max = 0
	if morph then
		for i=1,8 do
			if morph[i] then
				self.morph_index_max = i
			end
		end
	end

	self.touch 			=	def.touch

	if self.b_fbx then
		local len_to_anim		=	table.copy_simple( def.len_to_anim ) or {}
		len_to_anim.min			=	len_to_anim.min or 1
		len_to_anim.max			=	len_to_anim.max or 1
		len_to_anim.range		=	len_to_anim.max - len_to_anim.min

		len_to_anim.duration 	=	len_to_anim.duration or 1
		self.len_to_anim		=	len_to_anim

		self.transparent_render	=	table.copy_simple( def.transparent_render )
	end

	if def.curvature then
		self.curvature = table.copy_simple( def.curvature )
	end
	--todo
	self.speed_vertical_ratio_max = self.speed_vertical_ratio_max

	b_retcode = true

	self:print( "process_def() End" )
	return b_retcode
end

--[[
--unused and look buggy
function RACE:set_color( r, g, b, a )
	if r then
		local ref= self.ref
		ref.color = COLOR_REF:create( "zobi", aaa.obj.get( "zobi" ) )
		self.color = { r, g, b, a }
	else
		self.color = nil
	end
end
--]]

function RACE:reload_shader()					table.apply_method( self.__celts, "reload_shader" )					end
function RACE:apply_to_shader( method, ... )	table.apply_method( self.__celts, "apply_to_shader", method, ... )	end
function RACE:set_fog_current()					table.apply_method( self.__celts, "set_fog_current" )				end

function RACE:set_focus_layer( celt_index, layer_index )	self.__celts[celt_index]:set_focus_layer( layer_index )	end
function RACE:focus_shading( celt_index )					self.__celts[ celt_index or 1 ]:focus_shading()			end
function RACE:get_shading()									return self.__celts[1]:get_shading()						end
function RACE:set_shader( sha_type, val_type, id, value )
--	aaa.print_fn()
	--todomona
	table.apply_fn( self.__celts,	function(celt) local sha = celt:get_shading() if sha then sha:set_val(  sha_type, val_type, id, value )	end end		)
end

function RACE:update()
--	aaa.print_method()
	--self:reload_shader()
	--this is aqua uin code
	local bu = self.ui.bu_win
	if bu then
		if self:is_active() then
			bu.__line_color_forced = nil
		else
			bu.__line_color_forced = BU.__border_line_color_red
		end
	end

	local celts = self.__celts
	for i = 1, #celts do
		celts[i]:update()
	end
end

--
--	LOAD / SAVE
--
function RACE:load( name )
	self.__b_draw_can = false

	local def = self:get_def()
	if def then
		if not def.b_fbx and not def.meu_fish_name then
			name = name or self:get_name()
			local celts = self.__celts
			for i = 1, #celts do
				local celt = celts[i]
				celt:load( name )
				if celt.__b_valid then
					self.__b_draw_can = true
				end
			end
		else
			self.__b_draw_can = true
		end
	end

	--table.print( def, 3 )
	--infact in process def in bfx case or  meu_fish_name data is loaded
	if not self:process_def() then
		self.__b_draw_can = false
	end

	if not self.__b_draw_can then
		self:print_error( "load() : problem so this race will not be drawn" )
	end
end
function RACE:save( name )
	self:print_inverse( "Saving RACE "..name )
	table.apply_method( self.__celts, "save", name )
end
function RACE:reload()
	--self:print( "will try to reload "..name )
	self:load()
end

function RACE:save_by_index() self:save( self:get_name() ) end		-- could be self.name_..(self.index_save+1)

--
--	UI
--
function RACE:add_selector( key, nb_x, nb_y )
	local bu = bus_cur:add_selector( "Race", {x,y, nb_x,nb_y} )	--, "bank_"..text )
	bu:set_text_draw( false )
	--bu.text_u = .22
	--bu.text_v = -1.15
	bu:set_nb_min_0( nb_x, nb_y )
	self[key] = 0
	bu:set_target_lua( self, key )
end

function RACE:define_ui_base()
	--self:print( "define_ui_base()" )

	local bus = BUS:begin_window( self:get_name() )

		bus:set_bu_pos_load_save( false )

		local bu

		bu = bus:add_but_target_lua( "Active", 			nil,	self, "__b_active",		true )

		bus:add_but_trig_method(	"Shader Reload",	nil,	self, "reload_shader"	)
		bus:add_but_trig_method(	"Reload",			nil,	self, "reload"			)
		bus:add_but_trig_method(	"RACE Save",		nil,	self, "save_by_index"	)

		bus:move_next_col()

		for i,celt in IPAIRS( self.__celts ) do
			celt:define_ui( -.5 + i, 1 )
		end

	bus:end_window()

	return bus
end

function RACE:define_ui( bus, x, y, size )
	local name = self:get_name()
	local bu = bus:add_window( "WIN_"..name, self:define_ui_base(),		{3,2, 2,2} )
		bu:set_text( name )
		bu:set_sxy( 2, 2 )
		bu:set_pos_mini( {x*size,(y*1.2)*size+.1, size,size} )
		bu:__set_window_state("mini")
		self.ui.bu_win = bu
	return bu
end

--
--	RACES
--		deal with one type of fish
--
if CLASS.DECLARE( "RACES" ) then
	RACES.__race_def = {}
	function RACES.get_race_def()
		return RACES.__race_def
	end
end

function RACES.create( name )
	local self = RACES:create_instance( name )
	self.__race	= {}
	return self
end

--function RACES:get_all()
--	return self.__race
--end

function RACES:update()							table.apply_method( self.__race, "update" )			end
function RACES:reload_shader()					table.apply_method( self.__race, "reload_shader" )	end
function RACES:apply_to_shader( method, ... )	table.apply_method( self.__race, "apply_to_shader", method, ... )	end
function RACES:process_race_defs()				table.apply_method( self.__race, "process_def" )	end

function RACES:set_fog( b )
	self:apply_to_shader( "set_frag_int_1", b and 1 or 0 )
end
function RACES:set_fog_current()
	table.apply_method( self.__race, "set_fog_current" )
end

function RACES:add( name, celt_nb )
	if self.__race[name] then
		self:box_error( "we already have a \""..name.."\" race" )
	end
	self:print_inverse( "RACES:load( "..aaa.get_method_arg_as_str().." ) Begin" )
		local race = RACE.create( name, celt_nb )
		race:load()
		self.__race[name] = race
	self:print_inverse( "RACES:load( "..aaa.get_method_arg_as_str().." ) End" )
	return race
end
function RACES:get( name )
	local race = self.__race[name]
	if not race then
		self:print_error( "Can't find race "..name )
	end
	return race
end

function RACES:get_nb()		return table.count_item( self.__race )	end

function RACES:define_ui( bus, b_in_ga )
	local nb = self:get_nb()
	--self:box_debug( "RACES:define_ui with "..nb.." race" )
	if nb > 0 then
		local S = .22 * math.min( 14/nb, 1. )
		local i = 1
		for _,race in pairs( self.__race ) do
			if b_in_ga then
				race:define_ui( bus, S, i, S*.25 )
			else
				race:define_ui( bus, 1, i, S )
			end
			i = i + 1
		end
	end
end

