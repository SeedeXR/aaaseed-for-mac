--todo declare PUPPET.PART instead ???
--todo memorize MEDIA dir
if CLASS.DECLARE( "PUPPET_PART_DEF" ) then
end

function PUPPET_PART_DEF:init()
	self.img_seqs = {}
	self.img_seqs_by_name = {}
	self.frame_nb_total = 0
	return self
end
function PUPPET_PART_DEF:create( name )
	local self = PUPPET_PART_DEF:create_instance( name )
	self:init()
	return self
end

--todo we need to reuse same IMG_SEQ not only same IMG
function PUPPET_PART_DEF:__add_img_seq ( name, filename, opts )
	self:print( "Add IMG_SEQ "..name.." from "..filename )
	local img_seq = IMG_SEQ:create_new( name )
	--todo memorize in img_seq to generalize
	local first,last,step
	local t_frame = opts.frame
	if t_frame then
		if t_frame.first then first = t_frame.first end
		if t_frame.last  then last  = t_frame.last end
		if t_frame.step  then step  = t_frame.step end
	end
	img_seq:explore(	filename,	"png", first, last, step	)	--	"DDS" )
	--self:box_debug( "stop" ) 
	local nb = img_seq:get_nb()
	if nb > 0 then
		self.frame_nb_total = self.frame_nb_total + nb
		table.insert( self.img_seqs, img_seq )
		self.img_seqs_by_name[name] = img_seq
		img_seq.b_flip_x	= opts.b_flip_x
		img_seq.x_offset	= opts.x_offset
		img_seq.b_pingpong	= opts.b_pingpong
		img_seq.frame		= t_frame	--todo check if we use it
		return img_seq
	else
		self:do_error( "Could not read\n"..filename )
	end
end
function PUPPET_PART_DEF:set_descriptor( desc )
	self:init()
	self.desc = desc
	local pre_name = desc.pre_name
	for i_img_seq, t in IPAIRS( self.desc ) do
		--table.print( t, "part descriptor", 2 )
		local fname = t[1]
		local dir_name = pre_name and pre_name..fname or fname
		if t.b_pre_name~= false and pre_name then
			fname = pre_name..fname
		end
		self:__add_img_seq( t[2], dir_name.."/"..fname.."_", t )
	end
end
function PUPPET_PART_DEF:load()
	for _, img_seq in PAIRS( self.img_seqs ) do
		local first,last,step
		local t_frame = img_seq.frame 
		if t_frame then
			if t_frame.first then first = t_frame.first end
			if t_frame.last  then last  = t_frame.last end
			if t_frame.step  then step  = t_frame.step end
		end
		img_seq:load( nil, nil, first,last,step )
	end
end
function PUPPET_PART_DEF:dump( pre )
	pre = pre or ""
	self:print( pre..self.frame_nb_total.." frames for amins" )
	for _, img_seq in IPAIRS( self.img_seqs ) do
		img_seq:dump( pre.."\t" )
	end
end
function PUPPET_PART_DEF:attach_to( part )
	self.part_def_attached = part
end

if CLASS.DECLARE( "PUPPET_PART" ) then
	function PUPPET_PART.set_sxy( sx,sy )
		PUPPET_PART.sx = sx
		PUPPET_PART.sy = sy
	end
	function PUPPET_PART.set_xy_sxy( x,y, sx,sy )
		PUPPET_PART.x = x
		PUPPET_PART.y = y
		PUPPET_PART.sx = sx
		PUPPET_PART.sy = sy
	end
	PUPPET_PART.set_xy_sxy( 0,0, 1,1 )
end

PUPPET_PART.b_draw_rect = false

function PUPPET_PART:reinit()
	self.offset_x = 0
	self.offset_y = 0
	self.scale = 1
	self.ph = 0
	self.fps = 12	--deal with fps by anim

	-- with procedural target we need to init value
	self.player_id_next = 1
	self.b_active = true	
	self.b_play = true	

	return self
end

function PUPPET_PART:init( part_def )
	self.part_def = part_def
	self.players = {}
	self.players_by_name = {}
	for _, img_seq in IPAIRS( self.part_def.img_seqs ) do
		local name = img_seq:get_name()
		--self:box_debug( "create player "..name ) 
		local player = IMG_SEQ_PLAYER:create( name, img_seq )
		--todo deal better with all this
		player.b_flip_x = img_seq.b_flip_x
		player.x_offset = img_seq.x_offset
		player:restart()
		table.insert( self.players, player )
		self.players_by_name[name] = player
	end
	self.player_id_cur = 1
	self.ui = {}
	return self:reinit()
end
function PUPPET_PART:create( name, puppet_part_def )
	if not puppet_part_def then
		self:box_debug( "No part_def" )
	end
	local self = PUPPET_PART:create_instance( name )
	self:init( puppet_part_def )
	return self
end

function PUPPET_PART:do_error( ... )
	self:print_error( ... )
--	self:box_error( ... )
end
function PUPPET_PART:set_offset_x( x )		self.offset_x = x	end
function PUPPET_PART:set_offset_y( y )		self.offset_y = y	end
function PUPPET_PART:set_offset_xy( x,y )	self.offset_x = x
											self.offset_y = y	end
function PUPPET_PART:set_scale( s )			self.scale = s		end

function PUPPET_PART:get_player( id )
	local player_id = id or self.player_id_cur
	local player = self.players[player_id]
	if not player then
		self:print( "no player for id "..player_id )
	end
	return player
end

function PUPPET_PART:define_ui( meu, pre, ix,iy, SY )
	local name = self:get_name()
	local pre_name = pre.."_"..name
	local f = .8
	SY = SY * f
	local bu

	local function make_target( field_name )
		return {how="method_v0", obj=meu, method="get_set_part", args={pre,name,field_name} }
	end

	meu:add_button( 		{ix,iy,			2.5,SY},	pre_name,			make_target( "b_active" ) )
		:set_text( name )
		:set_value_type_bool(true)
	meu:add_trig_method(	{ix+2.5,iy,		1,SY},		pre_name.."_Next", 	self,	"trig_player_next"	)
		:set_text( "Next" )
		:set_text_inside( true )
	meu:add_button( 		{ix+3.5,iy,		1,SY},		pre_name.."_Play",	make_target( "b_play" ), nil,	true )
		:set_text_visible( false )
	meu:add_slider(			{ix+4.5,iy,		2,SY},		pre_name.."_Phase",	make_target( "ph" ),	 nil,		0, 	0,1	)
		:set_text( "Phase" )
		:set_min_max_strict( true )
	bu = meu:add_text_info(	{ix+6.5,iy,		1.5,SY},	pre_name.."_frame_id"	)
		:set_text( "" )
	self.ui.bu_info = bu
	iy = iy + SY

	local players = self.players

	if #players > 1 then
		bu = meu:add_selector(	{ix+1,iy,	7,SY},	pre_name.."_parts" )
			:set_nb( 1 )
			:set_text_visible( false )
			:set_target_lua( make_target( "player_id_next" ) )
			--:set_target_lua( self, "player_id_next" )
		for i, player in IPAIRS( players ) do
			bu:set_item_text( i, player:get_name() )
		end
		bu:set_method_on_click_double( self, "set_player" )
		iy = iy + SY
	end

	return iy
end

function PUPPET_PART:update_ui()
	local player = self:get_player()
	if player then
		local bu = self.ui.bu_info
		if bu then
			bu:set_text( player:__get_id().."/"..player:get_nb() )
		end
	else
		self:print( "nil player" )
	end
end

--will draw relative to
function PUPPET_PART:attach_to( part )
	self.part_attached = part
end
function PUPPET_PART:set_player( player_id_next )
	player_id_next = player_id_next or self.player_id_next
	self.b_trig_next = false
	self.player_id_cur = player_id_next
end
function PUPPET_PART:trig_player_next()
	self.b_trig_next = true
end

function PUPPET_PART:set_method_next_anim( obj, method, opts )
	self.__method_next_anim = { obj, method, opts }
	local player_id = self.player_id_cur
end

function PUPPET_PART:update( dt )	
	if not self.b_active then return end

	local player = self:get_player()

	if dt > 0 and self.b_play then
		player:inc_time( dt )
	end
	self.ph = player:get_phase()
	player:set_phase( clamp( self.ph, 0,.99999 ) )	--todo deal with phase 1 at lower level

	if player.__loop_nb_done >= 1 then
		
		local m = self.__method_next_anim
		if m then
			--self:print( player.." done "..player.__loop_nb_done.." loop" )
		-- 	--self:print( self.." updated" )
			local obj = m[1]
			obj[m[2]]( obj, self, m[3] )
		end
		-- if false then -- todo detect end loop
		-- 	if self.b_trig_next then
		-- 		self:set_player()
		-- 	end
		-- end
	end
end

function PUPPET_PART:draw_as_texture( player, img, x,y,z, sx,sy, dt )
	--self:print( "draw" ))
	local attached = self.part_attached
	if attached then
		--self:print( "attached" )
		x = x + attached.offset_x
		y = y + attached.offset_y
	end
	x = x + self.offset_x
	y = y + self.offset_y
	--self:print( x..","..y )

	--todo refine
	if player.x_offset then
		x = x + sx*player.x_offset
	end

	sx = player.b_flip_x and -sx or sx

	img:draw_xyz_sxy( x,y,z, sx,sy )
end

function PUPPET_PART:draw( x,y,z )
	if not self.b_active then return end

	local sx = PUPPET_PART.sx
	local sy = PUPPET_PART.sy
	x = x + PUPPET_PART.x
	y = y + PUPPET_PART.y

	local player = self:get_player()
	local img = player:get_img()
	if not img then
		return
	end

	--todo where we control
	--if PUPPET_PART.b_draw_rect then

	if PUPPET_PART.b_draw_rect then
		gol.set_texture_dim( 0 )
		gol.color_white()
		local id = gol.get_program_used()
		gol.use_program( 0 )			
			img:draw_rects_xyz_sxy(  x,y,z, sx,sy, player.b_flip_x )
		gol.use_program( id )
		gol.set_texture_dim( 2 )
	end

	self:draw_as_texture( player, img, x,y,z, sx,sy )
end
