

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	self.time = 0.0
	self.b_play = false
	self.b_started = false
	self.b_verbose = false
	self.animation_name = ""
	local ix = 1
	local iy = 1
	local SY = 1
	local SY2 = 1 * .5
	local DY = SY*.2

	local SX = (8)/3

	iy = self:define_time( 	{ix,iy,	1,SY}, 500, false )

	ui.bu_position = self:add_sliders_xyz(	{ix,iy,		8,SY},	"Position",	self.pos_cur,	false,	35.0	)
	iy = iy + SY
	ui.bu_rotation = self:add_sliders_xyz(	{ix,iy,		8,SY},	"Rotation",	self.rot_cur, 	false,	0.5	)
	iy = iy + SY
	bu = self:add_trig_method(				{ix+5,iy,	SX,SY},	"Reset", 	self, "reset" )
end

function meu:init()
	local ref = self.ref
	self.pos_cur = {}
	self.rot_cur = {}
	self.anim_cond = {}
--	self:get_screen()
--	self.fbx = nil
end

function meu:update_ui()
	local ui = self.ui
	self:update_time_ui()
end

function meu:update()
	local t = self:update_time()
end

function meu:load_textures()
	--C2D_NEMO_PIJ_NEMO_GR_Cam01_00000
	self:print( "app:get_anim_path() "..app:get_anim_path() )
	local name_dir ="NEMO/".."ANIM_01".."/"
--	local name = aaa.pc.is_dev() and "Maa_256" or "Maa_1024"
	name_dir = name_dir.."C2D_NEMO_PIJ_NEMO_GR_Cam01_V02_"
	MEDIA.set_dir_media( app:get_anim_path() )
	if app:use_compressed_texture() then
		self:print( "Trying to load nemo DDS textures" )
		self.textures = IMG_SEQ:create( "nemo_01", name_dir, "dds", 0, 1200 )
		if not self.textures then
			self:print( "Failed to load nemo DSS textures" )
		end
	end
	if not self.textures or self.textures.nb == 0 then
		self:print( "Trying to load TGA nemo textures" )
		self.textures = IMG_SEQ:create( "nemo_01", name_dir, "tga", 0, 1200 )
	end
	self.b_textures_loaded = true
end

function meu:define_nemo()
	if self.data==nil and app.get_location then --init order troubles
		local all =
		{
			KL2 = { { id=1, position = { x = 0.102, y = -0.859 },	dy = 0.15, ar = 1300 / 2560 } 	},
			KR2 = { { id=2, position = { x = -0.895, y = -0.79 },	dy = 0.15, ar = 1300 / 2560 },
					{ id=3, position = { x =  0.55, y = -.86 },		dy = 0.15, ar = 1300 / 2560 }	},
		}
		self.data = all[app:get_location()]
		if self.data then
			self:define_anim()
		else
			self.data = {}
		end
	end
end

function meu:update_animation( anim )
	for key, val in PAIRS( anim ) do
		val.start_time_s = val.start_time / 60.
		val.end_time_s = val.end_time / 60.
		val.duration = val.end_time_s - val.start_time_s
	end
end

function meu:reset()
	self:print( "reset" )
	self:restart_time()
	for i, obj in IPAIRS( self.data ) do
	--	local obj = val[ i ]
		obj.start_anim_t = 0.0
		obj.animation_name = ""
		obj.time = 0.0
	end
end

function meu:start_anim()
	self:time_play( true )
	self:reset()
end

function meu:stop_anim()
	self:restart_anim()
	self:time_play( false )
end

function meu:define_anim()
	local nemo = {
			GR20 = { comment = "loop defensive",	start_time = 0,		end_time = 300 },
			GR30 = { comment = "rentrent",			start_time = 300,	end_time = 600 },
			GR40 = { comment = "peur",				start_time = 600,	end_time = 900 },
			GR50 = { comment = "sortir",			start_time = 900,	end_time = 1200 },
		}
	self:reset()
	self:update_animation( nemo )
	self.name = "NEMO"
	self.anim_cond = nemo
end


function meu:set_condition()
	for i, nemo in IPAIRS( self.data ) do
		nemo.start_anim_t = 0
		nemo.animation_name = ""
		nemo.time = 0
		nemo.start_anim_t = t
	end
end

function meu:update_fbx_start_time( obj, t )
	local start_time = self.anim_cond[ obj.animation_name ].start_time_s
	obj.time = start_time
	obj.start_anim_t = t
--	self:print( "Start Animation : "..eel.animation_name.." at "..start_time )
end

function meu:fbx_update_time( obj, t, next )
	local duration = self.anim_cond[ obj.animation_name ].duration
	if t - obj.start_anim_t >= duration then
		--self:print( "Animation "..obj.animation_name.." goto "..next )
		obj.animation_name = next
		self:update_fbx_start_time( obj, t )
	else
		obj.time = self.anim_cond[ obj.animation_name ].start_time_s + t - obj.start_anim_t
		--meu_fbx:set_time( meu_time )
	end
end

function meu:update_anim( obj, t )
--	local t = self:update_time()
	if t then
	--	meu_fbx:set_play(false)
		--self:print( "obj.animation_name "..obj.animation_name )
		if not obj.animation_name then
			obj.animation_name = ""
		end
		if obj.animation_name == "" then
			-- must start animation
			obj.animation_name = "GR20"
			local start_time = self.anim_cond[ obj.animation_name ].start_time_s
			--self:print( "self.anim_cond[ obj.animation_name ].start_time_s "..self.anim_cond[ obj.animation_name ].start_time_s )
			obj.start_anim_t = 0
			obj.time = start_time
		--	self:print( "Start Animation : "..self.anim_cond[ obj.animation_name ].animation_name.." at "..start_time )
		elseif obj.animation_name == "GR40" then
			local duration = self.anim_cond[ obj.animation_name ].duration
			if t - obj.start_anim_t >= duration then
				--self:print( "dgdgf")
				if obj.b_audience_present then
					obj.animation_name = "GR40"
					--self:print( "audience present stay in GR40")
					self:update_fbx_start_time( obj, t )
				else
					--self:print( "audience none goto GR50")
					obj.animation_name = "GR50"
					self:update_fbx_start_time( obj, t )
				end
			else
				obj.time = self.anim_cond[ obj.animation_name ].start_time_s + t - obj.start_anim_t
			end
		elseif obj.animation_name == "GR20" then
			local duration = self.anim_cond[ obj.animation_name ].duration
			--self:print( "GR20 "..duration.. " "..t - obj.start_anim_t)
			if t - obj.start_anim_t >= duration then
				if obj.b_audience_present then
					--self:print( "audience presents go to GR30" )
					obj.animation_name = "GR30"
					self:update_fbx_start_time( obj, t )
				else
					--self:print( "audience none stay in GR20")
					self:update_fbx_start_time( obj, t )
				end
			else
				obj.time = self.anim_cond[ obj.animation_name ].start_time_s + t - obj.start_anim_t
			--	self:print( "For anim index 2, setting meu time : "..meu_time )
				--meu_fbx:set_time( meu_time )
			end
		elseif obj.animation_name == "GR30" then
		--	self:print( "go to GR40")
			self:fbx_update_time( obj, t, "GR40" )
		elseif obj.animation_name == "GR50" then
			--self:print( "GR30" )
	--		self:print( "go to GR50")
			self:fbx_update_time( obj, t, "GR20" )
		end
	else
		--self:print_error( "gere")
	end
end

function meu:update_interactivity( obj, index )
	if app.const then
		local interactivity = app:get_presence_nemo_SO( index )
		local delta_x = 0.5
		local screen = 1
		if interactivity.coverage > 0.002 then
			obj.b_audience_present = true
		else
			obj.b_audience_present = false
		end
	end
end

function meu:draw()
	self:define_nemo()
	local nemos = self.data
	if not nemos or #nemos == 0 then return end

	local t = self:update_time()
	if not t then return end
	--aaa.show( t, "nemo_time" )

	if not self.b_do_anim then
		self.b_do_anim = true
	end

--	self.b_textures_loaded = false
--	aaa.show( self.b_textures_loaded, "self.b_textures_loaded" )
	if self.b_textures_loaded==nil then
		self:load_textures()
	end

	self:draw_layers_begin()
		for _, nemo in IPAIRS( nemos ) do
			--table.print( nemo, "nemo", 3 )
			self:update_interactivity( nemo, nemo.id )
			self:update_anim( nemo, t )
			local index = math.floor( nemo.time * 60.0 ) + 1
			local bind = self.textures:get_bind( index )
			--self:print( "Bind for nemo "..bind.." for time "..time )
			gol.set_blend_add()
			local ar = nemo.ar
			local x = nemo.position.x
			local y = nemo.position.y
			local dy = nemo.dy
			local dx = dy / ar
			aaa.draw_bind_rect( 	bind, x, y, x + dx, y + dy )
		end
	self:draw_layers_end()
end
