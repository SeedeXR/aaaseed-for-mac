
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	local ix = 1
	local iy = 1
	local SX = .8
	local SY = 1
	local DY = .2

	local DX = 3.8

	self:add_text_info(	{ix,iy,	8,SY * .65},		"Dramas"				)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop All Dramas", 		self, "stop_dramas"				)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Turtle", 		self, "start_turtle"			)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Turtle", 			self, "stop_turtle"				)

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start Mray", 			self, "start_mray"				)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop Mray", 			self, "stop_mray"				)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Parrot", 		self, "start_parrot"			)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Parrot", 			self, "stop_parrot"				)

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start Manta Clean", 	self, "start_manta_clean"		)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop Manta Clean", 	self, "stop_manta_clean"		)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Manta Eat", 		self, "start_manta_eat"			)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Manta Eat", 		self, "stop_manta_eat"			)

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start Manta Parade", 	self, "start_manta_parade"		)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop Manta Parade", 	self, "stop_manta_parade"		)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Potato Clean", 	self, "start_potato_clean"		)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Potato Clean", 	self, "stop_potato_clean"		)

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start Dolph DN3", 		self, "start_dolph_dn3"			)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop Dolph DN3", 		self, "stop_dolph_dn3"			)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Whitetip", 		self, "start_whitetip"			)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Whitetip", 		self, "stop_whitetip"			)

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start Tiger Chasse", 	self, "start_tiger_chasse"		)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop Tiger Chasse", 	self, "stop_tiger_chasse"		)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Whale DJ", 		self, "start_whale_dj"			)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Whale DJ", 		self, "stop_whale_dj"			)

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start Whale BB", 		self, "start_whale_bb"			)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop Whale BB", 		self, "stop_whale_bb"			)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Tiger", 			self, "start_tiger"				)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Tiger", 			self, "stop_tiger"				)

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start SharkWhale", 	self, "start_shark_whale"		)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop SharkWhale", 		self, "stop_shark_whale"		)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start SharkWhale SO", 	self, "start_shark_whale_so"	)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop SharkWhale SO", 	self, "stop_shark_whale_so"		)
	iy = iy + SY * 1.

	self:add_text_info(	{ix,iy,	8,SY * .65},		"Animation Condition"		)
	bu = self:add_trig_method(	{	ix + DX * 2.0, iy,		SX, SY},	"Stop All Anims", 		self, "stop_anim_condition"		)
	iy = iy + SY * 1.
	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Whale", 			self, "start_whale"				)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Whale", 			self, "stop_whale"				)

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start Titan", 			self, "start_titan"				)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop Titan", 			self, "stop_titan"				)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Dolph", 			self, "start_dolph"				)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Dolph", 			self, "stop_dolph"				)

	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start Murene", 		self, "start_mrayc"				)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop Murene", 			self, "stop_mrayc"				)
	iy = iy + SY * 1.

	bu = self:add_trig_method(	{	ix, iy,					SX, SY},	"Start Train Turtle", 	self, "start_turtle_train"		)
	bu = self:add_trig_method(	{	ix + DX, iy,			SX, SY},	"Stop Train Turtle", 	self, "stop_turtle_train"		)
	bu = self:add_trig_method(	{	ix + 2 * DX, iy,		SX, SY},	"Start Eel", 			self, "start_eel"				)
	bu = self:add_trig_method(	{	ix + 3 * DX, iy,		SX, SY},	"Stop Eel", 			self, "stop_eel"				)

end

function meu:init()
	self.drama_queue = {}
	self.drama_queue_ex = {}
	self.drama_running = {}
end

function meu:start_drama( fbx_name, name, time )
	if name and time and time ~= 0.0 then
		local item = {}
		item.fbx_name = fbx_name
	--	self:print( "aaa.time.t "..aaa.time.t)
		item.start_time = aaa.time.t + time
		self:print( "Queue item "..name.." starting at "..item.start_time )
		self.drama_queue[ name ] = item
		self.drama_running[ name ] = item
	else
		local meu_fbx = self:get_meu_by_name_no_error( fbx_name )
		if meu_fbx then
			meu_fbx:set_mu_value( 1 )
		--	meu_fbx:set_max_time()
			meu_fbx:restart()
			meu_fbx:time_set_play( true )
		else
			self:print_error( "Can't find meu "..fbx_name )
		end
	end
end

function meu:queue_drama( drama )
	if drama.start_offset ~= 0.0 then
	--	local item = {}
		--item.fbx_name = fbx_name
	--	self:print( "aaa.time.t "..aaa.time.t)
		drama.start_time = aaa.time.t + drama.start_offset
		self:print( "Queue item "..drama.name.." starting at "..drama.start_time )
		self.drama_queue_ex[ drama.name ] = drama
	else
		self:start_drama_ex( drama )
		-- local meu_fbx = self:get_meu_by_name_no_error( fbx_name )
		-- if meu_fbx then
		-- 	meu_fbx:set_mu_value( 1 )
		-- --	meu_fbx:set_max_time()
		-- 	meu_fbx:restart()
		-- 	meu_fbx:time_set_play( true )
		-- else
		-- 	self:print_error( "Can't find meu "..fbx_name )
		-- end
	end
end

function meu:start_drama_ex( drama )
	local meu_fbx = self:get_meu_by_name_no_error( drama.fbx_name )
	if meu_fbx then
		meu_fbx:set_mu_value( 1 )
		meu_fbx:set_max_time()
		meu_fbx:set_time_looping( drama.b_looping )
		meu_fbx:set_time_speed( drama.speed or 1.0 )
		if drama.blend_shape ~= nil then
			for i, bs in IPAIRS( drama.blend_shape )do
				if bs ~= nil then
					meu_fbx:set_blend_shape( bs.id, bs.manual, bs.factor )
				end
			end
		end
		meu_fbx:restart()
		meu_fbx:time_set_play( true )
		self.drama_running[ drama.name ] = drama
	else
		self:print_error( "Can't find meu "..drama.fbx_name )
	end

end


function meu:stop_drama( fbx_name, name )
	local meu_fbx = self:get_meu_by_name_no_error( fbx_name )
	if meu_fbx then
		meu_fbx:time_set_play( false )
		meu_fbx:set_mu_value( 0 )
		if self.drama_queue[ name ] then
			self.drama_queue[ name ] = nil
		end
		if self.drama_queue_ex[ name ] then
			self.drama_queue_ex[ name ] = nil
		end
	end
end

local time_start_offset = 10.0

function meu:start_turtle()
	if app:is_location( "KL4" ) or app:is_pc_dev() then
		self:print( "Starting Turtle Clean" )
		local drama = {}
		drama.name = "TURTLECLEAN"
		drama.fbx_name = "Fbx_TURTLE_D"
		drama.start_offset = time_start_offset
		drama.b_looping = false
		drama.speed = 1.0
		self:queue_drama( drama )
		--self:start_drama( "Fbx_TURTLE_D", "TURTLECLEAN", time_start_offset )
	end
end

function meu:stop_turtle()
	self:print( "Stop Turtle Clean" )
	self:stop_drama( "Fbx_TURTLE_D", "TURTLECLEAN" )
end

function meu:start_mray()
	if app:is_location( "KL1" ) or app:is_pc_dev() then
		self:print( "Starting MRAY Clean" )
		local drama = {}
		drama.name = "MRAYCLEAN"
		drama.fbx_name = "Fbx_MRAY_D_PIJ"
		drama.start_offset = time_start_offset
		drama.b_looping = true
		drama.speed = 1.0
		self:queue_drama( drama )
--		self:start_drama( "Fbx_MRAY_D_PIJ", "MRAYCLEAN", time_start_offset )
	end
end

function meu:stop_mray()
	self:print( "Stopping MRAY Clean" )
	self:stop_drama( "Fbx_MRAY_D_PIJ", "MRAYCLEAN" )
end

function meu:start_parrot()
	if app:is_kr() or app:is_pc_dev() then
		self:print( "Starting Parrot KR" )
		local drama = {}
		drama.name = "PARROTA"
		drama.fbx_name = "Fbx_PARROT_D_PIJ"
		drama.start_offset = time_start_offset + 70.0
		drama.b_looping = false
		drama.speed = 1.0
		self:queue_drama( drama )
	--	self:start_drama( "Fbx_PARROT_D_PIJ", "PARROTA", time_start_offset )
		local dramab = {}
		dramab.name = "PARROTB"
		dramab.fbx_name = "Fbx_PARROT_D_PIJ_B"
		dramab.start_offset = time_start_offset + 10.0 +70.
		dramab.b_looping = false
		dramab.speed = 1.0
		self:queue_drama( dramab )
		--self:start_drama( "Fbx_PARROT_D_PIJ_B", "PARROTB", time_start_offset + 10.0 )
		local dramac = {}
		dramac.name = "PARROTC"
		dramac.fbx_name = "Fbx_PARROT_D_PIJ_C"
		dramac.start_offset = time_start_offset + 20.0 +70.
		dramac.b_looping = false
		dramac.speed = 1.0
		self:queue_drama( dramac )
--		self:start_drama( "Fbx_PARROT_D_PIJ_C", "PARROTC", time_start_offset + 20.0 )
	end
	if app:is_kc() or app:is_pc_dev() then
		local drama = {}
		drama.name = "PARROTKC"
		drama.fbx_name = "Fbx_PARROT_D_PIJ_D"
		drama.start_offset = time_start_offset
		drama.b_looping = false
		drama.speed = 1.0
		self:queue_drama( drama )
	--	self:start_drama( "Fbx_PARROT_D_PIJ_D", "PARROTKC", time_start_offset + 15.0 )
	end
end

function meu:stop_parrot()
	self:print( "Stopping Parrot" )
	self:stop_drama( "Fbx_PARROT_D_PIJ", "PARROTA" )
	self:stop_drama( "Fbx_PARROT_D_PIJ_B", "PARROTB" )
	self:stop_drama( "Fbx_PARROT_D_PIJ_C", "PARROTC" )
	self:stop_drama( "Fbx_PARROT_D_PIJ_D", "PARROTD" )
end

function meu:start_manta_clean()
	if app:is_kr() or app:is_pc_dev() then
		self:print( "Starting Manta Clean" )
		local drama = {}
		drama.name = "MANTA_CLEAN"
		drama.fbx_name = "Fbx_MANTA_CLEAN"
		drama.start_offset = time_start_offset
		drama.b_looping = false
		drama.speed = 0.7
		drama.blend_shape = {}
		local blend_shape = {}
		blend_shape.manual = false
		blend_shape.id = 1
		blend_shape.factor = 0.0
		drama.blend_shape[ 1 ] = blend_shape
		local blend_shape2 = {}
		blend_shape2.manual = true
		blend_shape2.id = 2
		blend_shape2.factor = 0.0
		drama.blend_shape[ 2 ] = blend_shape2
		local blend_shape3 = {}
		blend_shape3.manual = false
		blend_shape3.id = 3
		blend_shape3.factor = 0.0
		drama.blend_shape[ 3 ] = blend_shape3
		self:queue_drama( drama )
	--	self:start_drama( "Fbx_MANTA_CLEAN", "MANTA_CLEAN", time_start_offset )
	end
end

function meu:stop_manta_clean()
	self:print( "Stopping Manta Clean" )
	self:stop_drama( "Fbx_MANTA_CLEAN", "MANTA_CLEAN" )
end

function meu:start_manta_eat()
	if app:is_location( "KC" ) or app:is_pc_dev() then
		self:print( "Starting Manta Eat" )
		local drama = {}
		drama.name = "MANTA_EAT"
		drama.fbx_name = "Fbx_MANTA_EAT"
		drama.start_offset = time_start_offset
		drama.b_looping = true
		drama.speed = 0.25
		drama.blend_shape = {}
		local blend_shape = {}
		blend_shape.manual = true
		blend_shape.id = 1
		blend_shape.factor = 1.0
		drama.blend_shape[ 1 ] = blend_shape
		local blend_shape2 = {}
		blend_shape2.manual = true
		blend_shape2.id = 2
		blend_shape2.factor = 1.0
		drama.blend_shape[ 2 ] = blend_shape2
		self:queue_drama( drama )
		--self:start_drama( "Fbx_MANTA_EAT", "MANTA_EAT", time_start_offset )
	end
end

function meu:stop_manta_eat()
	self:print( "Stopping Manta Eat" )
	self:stop_drama( "Fbx_MANTA_EAT", "MANTA_EAT" )
end

function meu:start_manta_parade()
	if app:is_location( "KC" ) or app:is_pc_dev() then
		self:print( "Starting Manta Parade" )
		local drama = {}
		drama.name = "MANTA_PARADE"
		drama.fbx_name = "Fbx_MANTA_PARADE"
		drama.start_offset = time_start_offset + 2.0
		drama.b_looping = false
		drama.speed = 0.25
		drama.blend_shape = {}
		local blend_shape = {}
		blend_shape.manual = true
		blend_shape.id = 1
		blend_shape.factor = 0.0
		drama.blend_shape[ 1 ] = blend_shape
		self:queue_drama( drama )
--		self:start_drama( "Fbx_MANTA_PARADE", "MANTA_PARADE", time_start_offset )
	end
end

function meu:stop_manta_parade()
	self:print( "Stopping Manta Parade" )
	self:stop_drama( "Fbx_MANTA_PARADE", "MANTA_PARADE" )
end

function meu:start_potato_clean()
	if app:is_location( "KL2" ) or app:is_pc_dev() then
		self:print( "Starting Potato Clean" )
		self:start_drama( "Fbx_POTATO_CLEAN", "POTATO_CLEAN", time_start_offset )
	end
end

function meu:stop_potato_clean()
	self:print( "Stopping Potato Clean" )
	self:stop_drama( "Fbx_POTATO_CLEAN", "POTATO_CLEAN" )
end

--todo regroup
function meu:start_dolph_dn3()
	self:print( "Starting Dolphins" )
--	self:start_drama( "Fbx_DOLPH_DN2", "DOLPH_DN2", time_start_offset )
	if app:is_pc_mas() == false then
		local m = self:get_meu_by_name_no_error( "ADolph_Tapestry" )
		if m then
			m:set_mu_value( 1 )
			m:start_anims()
			self:print( "Starting Dolphins " )
		else
			self:print_error( "Can't find meu ADolph_Tapestry" )
		end
	end
end

function meu:stop_dolph_dn3()
	self:print( "Stopping Dolphins" )
--	self:stop_drama( "Fbx_DOLPH_DN2", "DOLPH_DN2" )
	local m = self:get_meu_by_name_no_error( "ADolph_Tapestry" )
	if m then
		m:stop_anim()
		m:set_mu_value( 0 )
		self:print( "Stopping Dolphins " )
	else
		self:print_error( "Can't find meu ADolph_Tapestry" )
	end
end

function meu:start_whitetip()
	if app:is_kr() or app:is_pc_dev() then
		self:print( "Starting Whitetip" )
		local drama = {}
		drama.name = "WHITETIPKR1"
		drama.fbx_name = "Fbx_WHITETIP_DJ5"
		drama.start_offset = time_start_offset + 20.0
		drama.b_looping = false
		drama.speed = 0.8
		self:queue_drama( drama )
		local drama2 = {}
		drama2.name = "WHITETIPKR2"
		drama2.fbx_name = "Fbx_WHITETIP_DJ5_B"
		drama2.start_offset = time_start_offset + 2.5 + 20.0
		drama2.b_looping = false
		drama2.speed = 0.8
		self:queue_drama( drama2 )
	end
	if app:is_kc() or app:is_pc_dev() then
		self:print( "Starting Whitetip" )
		local drama = {}
		drama.name = "WHITETIPKC"
		drama.fbx_name = "Fbx_WHITETIP_DJ5_C"
		drama.start_offset = time_start_offset + 5.0
		drama.b_looping = false
		drama.speed = 0.8
		self:queue_drama( drama )
	end

end

function meu:stop_whitetip()
	self:print( "Stopping Whitetip" )
	self:stop_drama( "Fbx_WHITETIP_DJ5", "WHITETIPKR1" )
	self:stop_drama( "Fbx_WHITETIP_DJ5_B", "WHITETIPKR2" )
	self:stop_drama( "Fbx_WHITETIP_DJ5_C", "WHITETIPKC" )
end

function meu:start_tiger_chasse()
	if app:is_kr() or app:is_pc_dev() then
		self:print( "Starting Tiger Chasse" )
		local drama = {}
		drama.name = "TIGER_CHASSE_KR"
		drama.fbx_name = "Fbx_TIGER_CHASSE_KR"
		drama.start_offset = time_start_offset + 25
		drama.b_looping = false
		drama.speed = 0.3
		self:queue_drama( drama )
	end
	if app:is_kc() or app:is_pc_dev() then
	--	self:start_drama( "Fbx_TIGER_DJ5", "TIGERKR", time_start_offset + 30 )
		local drama = {}
		drama.name = "TIGER_CHASSE_KC"
		drama.fbx_name = "Fbx_TIGER_CHASSE_KC"
		drama.start_offset = time_start_offset + 45.0
		drama.b_looping = false
		drama.speed = 0.25
		self:queue_drama( drama )
	--	self:start_drama( "Fbx_TIGER_DJ5_B", "TIGERKC", time_start_offset + 61.0 )
	end
end

function meu:stop_tiger_chasse()
	self:print( "Stopping Tiger Chasse" )
	self:stop_drama( "Fbx_TIGER_CHASSE_KR", "TIGER_CHASSE_KR" )
	self:stop_drama( "Fbx_TIGER_CHASSE_KC", "TIGER_CHASSE_KC" )
end

function meu:start_tiger()
	if app:is_kr() or app:is_pc_dev() then
		self:print( "Starting Tiger" )
		local drama = {}
		drama.name = "TIGER_KR"
		drama.fbx_name = "Fbx_TIGER_KR"
		drama.start_offset = time_start_offset + 25
		drama.b_looping = false
		drama.speed = 0.3
		self:queue_drama( drama )
--		self:start_drama( "Fbx_TIGER_A", "TIGERA", time_start_offset )
	end
end

function meu:stop_tiger()
	self:print( "Stopping Tiger" )
	self:stop_drama( "Fbx_TIGER_KR", "TIGER_KR" )
end

function meu:start_whale_dj()
	if app:is_location( "KC" ) or app:is_pc_dev() then
		local drama = {}
		drama.name = "WHALE_DJ"
		drama.fbx_name = "Fbx_WHALE_DJ"
		drama.start_offset = 0.0
		drama.b_looping = false
		drama.speed = 0.22
		drama.blend_shape = {}
		self:queue_drama( drama )
		self:print( "Starting Whale DJ" )
	--	self:start_drama( "Fbx_WHALE_DJ", "WHALE_DJ", time_start_offset )
	end
end

function meu:stop_whale_dj()
	self:print( "Stopping Whale DJ" )
	self:stop_drama( "Fbx_WHALE_DJ", "WHALE_DJ" )
end

function meu:start_whale_bb()
	if app:is_location( "KC" ) or app:is_pc_dev() then
		self:print( "Starting Whale BB" )
		local drama = {}
		drama.name = "WHALE_BB"
		drama.fbx_name = "Fbx_WHALE_BB"
		drama.start_offset = .0
		drama.b_looping = true
		drama.speed = 0.1
		self:queue_drama( drama )
	--	self:start_drama( "Fbx_WHALE_BB", "WHALE_BB", time_start_offset )
	end
end

function meu:stop_whale_bb()
	self:print( "Stopping Whale BB" )
	self:stop_drama( "Fbx_WHALE_BB", "WHALE_BB" )
end

function meu:start_shark_whale()
	if app:is_kr() or app:is_pc_dev() then
		self:print( "Starting Shark Whale" )
		local drama = {}
		drama.name = "SHARK_WHALE_KR"
		drama.fbx_name = "Fbx_SHRK_WHAL_KR"
		drama.start_offset = 23
		drama.b_looping = false
		drama.speed = 0.2
		self:queue_drama( drama )
	end
	if app:is_kc() or app:is_pc_dev() then
		self:print( "Starting Shark Whale" )
		local drama = {}
		drama.name = "SHARK_WHALE_KC"
		drama.fbx_name = "Fbx_SHRK_WHAL_KC"
		drama.start_offset = 10
		drama.b_looping = false
		drama.speed = 0.2
		self:queue_drama( drama )
	end
end

function meu:stop_shark_whale()
	self:print( "Stopping Shark Whale" )
	self:stop_drama( "Fbx_SHRK_WHAL_KR", "SHARK_WHALE_KR" )
	self:stop_drama( "Fbx_SHRK_WHAL_KC", "SHARK_WHALE_KC" )
end


function meu:start_shark_whale_so()
	if app:is_so() or app:is_pc_dev() then
		local drama = {}
		drama.name = "SHARK_WHALE_SO"
		drama.fbx_name = "Fbx_SHRK_WHAL_SO"
		drama.start_offset = .0
		drama.b_looping = false
		drama.speed = 0.2
		self:queue_drama( drama )
		self:print( "Starting Shark Whale SO" )
	--	self:start_drama( "Fbx_SHRK_WHAL_SO", "SHARK_WHALE_SO", .0 )
	end
end

function meu:stop_shark_whale_so()
	self:print( "Stopping Shark Whale SO" )
	self:stop_drama( "Fbx_SHRK_WHAL_SO", "SHARK_WHALE_SO" )
end


function meu:start_anim( fbx_name )
	local meu_fbx = self:get_meu_by_name_no_error( fbx_name )
	if meu_fbx then
		meu_fbx:set_mu_value( 1 )
		meu_fbx:start_anim( .0 )
		self:print( "Starting animation condition "..fbx_name )
	else
		self:print_error( "Can't find meu "..fbx_name )
	end
end

function meu:stop_anim( fbx_name )
	local meu_fbx = self:get_meu_by_name_no_error( fbx_name )
	if meu_fbx then
		meu_fbx:stop_anim()
		--meu_fbx:restart()
		meu_fbx:set_mu_value( 0 )
	end
end

function meu:start_whale()
	if app:is_kr() or app:is_pc_dev() then
		self:print( "Starting Whale Condition" )
		self:start_anim( "WHALE_CA" )
	end
end

function meu:stop_whale()
	self:print( "Stopping Whale Condition" )
	self:stop_anim( "WHALE_CA" )
end

function meu:start_titan()
	if app:is_location( "KR3" ) or app:is_pc_dev() then
		self:print( "Starting Titan Condition" )
		self:start_anim( "TITAN_CA" )
	end
end

function meu:stop_titan()
	self:print( "Stopping Titan Condition" )
	self:stop_anim( "TITAN_CA" )
end

function meu:start_dolph()
	if app:is_kr() or app:is_pc_dev() then
		self:print( "Starting Dolph Condition" )
		self:start_anim( "DOLPH_CA" )
	end
end

function meu:stop_dolph()
	self:print( "Stopping Dolph Condition" )
	self:stop_anim( "DOLPH_CA" )
end

function meu:start_mrayc()
	if app:is_location( "KL1" ) or app:is_pc_dev() then
		self:print( "Starting MRAY Condition" )
		self:start_anim( "MRAYC_CA" )
	end
end

function meu:stop_mrayc()
	self:print( "Stopping MRAY Condition" )
	self:stop_anim( "MRAYC_CA" )
end

function meu:start_eel()
	self:print( "Starting EEL Condition" )
	local m = self:get_meu_by_name_no_error( "Eel_Tapestry" )
	if m then
		if app:is_location( "KR4" ) or app:is_pc_dev() then
			m:set_mu_value( 1 )
			m:start_anim()
		else
			m:set_mu_value( 0 )
		end
	else
		self:print_error( "Can't find meu Eel_Tapestry" )
	end
end

function meu:stop_eel()
	self:print( "Stopping EEL Condition" )
	local m = self:get_meu_by_name_no_error( "Eel_Tapestry" )
	if m then
		m:set_mu_value( 0 )
	end
end


function meu:start_xmas()
	self:print( "Starting XMAS" )
	local m = self:get_meu_by_name_no_error( "Xmas_Tapestry" )
	if m then
		if app:is_location( "KL1" ) or app:is_location( "KL3" ) or app:is_location( "KR1" ) or app:is_pc_dev() then
			m:set_mu_value( 1 )
		else
			m:set_mu_value( 0 )
		end
	else
		self:print_error( "Can't find meu Xmas_Tapestry" )
	end
end
function meu:stop_xmas()
	self:print( "Stopping XMAS" )
	local m = self:get_meu_by_name_no_error( "Xmas_Tapestry" )
	if m then
		m:set_mu_value( 0 )
	end
end


function meu:start_turtle_train()
	local m = self:get_meu_by_name_no_error( "TrainAnimation_Turtle" )
	if m then
		if app:is_kl() or app:is_pc_dev() then
			m:set_mu_value( 1 )
			m:start_anim()
		else
			m:set_mu_value( 0 )
		end
	else
		self:print_error( "Can't find meu TrainAnimation_Turtle" )
	end
end

function meu:stop_turtle_train()
	--if app:is_kl() or app:is_pc_dev() then
		local m = self:get_meu_by_name_no_error( "TrainAnimation_Turtle" )
		if m then
			m:stop_anim()
			--meu_fbx:restart()
			m:set_mu_value( 0 )
		end
	--end
end
-- bu = self:add_trig_method(	{	ix, iy,				SX, SY},	"Start Whale KC", 	self, "start_whale_kc"		)
-- bu = self:add_trig_method(	{	ix + 5, iy,			SX, SY},	"Stop Whale KC", 	self, "stop_whale_kc"		)
function meu:start_whale_kc()
	if app:is_kc() or app:is_pc_dev() then
		self:print( "must start whale KC")
		self:start_anim( "WHALEKC_CA" )
	end
end

function meu:stop_whale_kc()
	if app:is_kc() or app:is_pc_dev() then
		self:stop_anim( "WHALEKC_CA" )
	end
end

function meu:stop_dramas()
	self:stop_manta_clean()
	self:stop_manta_eat()
	self:stop_manta_parade()
	self:stop_mray()
	self:stop_parrot()
	self:stop_potato_clean()
	self:stop_shark_whale()
	self:stop_shark_whale_so()
	self:stop_tiger_chasse()
	self:stop_tiger()
	self:stop_turtle()
	self:stop_whitetip()
	self:stop_whale_bb()
	self:stop_whale_dj()
	self:stop_whale_kc()
	self:stop_dolph_dn3()
end

function meu:stop_anim_condition()
	self:stop_dolph()
	self:stop_mrayc()
	self:stop_titan()
	self:stop_turtle_train()
	self:stop_whale()
	self:stop_eel()
end

function meu:stop_all()
	self:stop_anim_condition()
	self:stop_dramas()
	self:stop_xmas()
	self:init()
end

function meu:reset_turbin( b_on )
	-- restart TURBIN with fish
	local name = "Turbin_Tapestry"
	local m = self:get_meu_by_name_no_error( name )
	if m then
		if app:is_location( "KL4") or app:is_location( "KR3" ) then
			m:set_mu_value( 1 )
			if b_on then
				m:start_anim()
			end
			m:set_show_static( not b_on )
		elseif app:is_pc_dev() == false then
			m:set_mu_value( 0 )
		end
	else
		self:print_error( "Can't find meu "..name )
	end
end

function meu:reset_pavona( b_on )
	-- restart PAVONA with fish
	local name = "Pavona_Tapestry"
	local m = self:get_meu_by_name_no_error( name )
	if m then
		if true then	--pavona slow all
			m:set_mu_value( 0 )
		else
			if app:is_location("KL3") or app:is_location("KL4") or app:is_location("KR3") or app:is_location("KR4") then
				m:set_mu_value( 1 )
				if b_on then
					m:start_anim()
				end
				m:set_show_static( not b_on )
			elseif app:is_pc_dev() == false then
				m:set_mu_value( 0 )
			end
		end
	else
		self:print_error( "Can't find meu "..name )
	end
end

function meu:reset_nemo( b_on )
	local name = "Nemo_Tapestry"
	local m = self:get_meu_by_name_no_error( name )
	if m then
		if app:is_location("KL2") or app:is_location("KR2") then
			if b_on then
				m:set_mu_value( 1 )
				m:start_anim()
			else
				m:set_mu_value( 0 )
			end
		else	--if app:is_pc_dev() == false then
			m:set_mu_value( 0 )
		end
	else
		self:print_error( "Can't find meu "..name )
	end
end

function meu:change_scene( name )
	self:stop_all()

	if app:is_tablet() then return end

	local b_night = false
	local b_nemo = false
	if name == "DJ1" then
		self:start_manta_clean()
		self:start_mray()
		self:start_parrot()
		self:start_potato_clean()
		self:start_xmas()
		self:start_eel()
		b_nemo = true
	elseif name == "PIJ1" then
		self:start_shark_whale()
		self:start_shark_whale_so()
		self:start_mrayc()
		self:start_titan()
		self:start_turtle()
		self:start_turtle_train()
		self:start_xmas()
		self:start_eel()
		b_nemo = true
	elseif name == "DJ2" then
		self:start_shark_whale_so()
		self:start_manta_parade()
		self:start_xmas()
		self:start_eel()
		b_nemo = true
	elseif name == "PIJ2" then
		self:start_dolph()
		self:start_mrayc()
		self:start_whale()
		self:start_xmas()
		self:start_eel()
		b_nemo = true
	elseif name == "DJ3" then
		self:start_whale_dj()
		self:start_xmas()
		self:start_eel()
		b_nemo = true
	elseif name == "PIJ3" then
		--	self:start_titan()
		self:start_shark_whale_so()
		self:start_shark_whale()
		self:start_mrayc()
		self:start_tiger()
		self:start_turtle_train()
		self:start_xmas()
		self:start_eel()
	elseif name == "DJ4" then
		b_night = true
		self:start_tiger()
		self:start_tiger_chasse()
		self:start_whitetip()
	elseif name == "DN1" then
		b_night = true
	elseif name == "PIN1" then
		b_night = true
	elseif name == "DN2" then
		b_night = true
		self:start_manta_eat()
	elseif name == "PIN2" then
		b_night = true
	elseif name == "DN3" then
		b_night = true
		self:start_dolph_dn3()
	elseif name == "PIN3" then
		b_night = true
		self:start_whale_bb()
	end
	self:reset_turbin( b_nemo )
	self:reset_pavona( b_nemo )
	self:reset_nemo( b_nemo )
end

function meu:update()
--	table.print( self.queue_drama, "self.queue_drama", 3)
	for key, item in PAIRS( self.drama_queue ) do
		if item ~= nil then
			if item.start_time then
				local start_time = item.start_time
				local time = aaa.time.t
				if start_time <= time then
					self:print( "Starting "..key.." with start time "..item.start_time)
					self:print( "At time "..aaa.time.t )
					self:start_drama( item.fbx_name )
					self.drama_queue[ key ] = {}
				end
			else
			--	self:print( "no item" )
			end
		end
	end
	for key, drama in PAIRS( self.drama_queue_ex ) do
		if drama ~= nil then
			if drama.start_time then
				local start_time = drama.start_time
				local time = aaa.time.t
				if start_time <= time then
					self:print( "Starting "..key.." with start time "..drama.start_time)
					self:print( "At time "..aaa.time.t )
					self:start_drama_ex( drama )
					self.drama_queue_ex[ key ] = {}
				end
			else
			--	self:print( "no item" )
			end
		end
	end
	-- Dramas are animations, set them looping if current sequence is looping
	for key, drama in PAIRS( self.drama_running ) do
		if drama ~= nil then
			local meu_fbx = self:get_meu_by_name_no_error( drama.fbx_name )
			if meu_fbx then
				--self:print( "setting drama "..drama.name.." to "..app:is_seq_looping() )
				meu_fbx:set_time_looping( app:is_seq_looping() )
			else
			end
		end
	end


end

MONACO_AQUA.preroll_anims =  {
	{ fbx_name = "Fbx_TURTLE_D", 		location = "KR" },
	{ fbx_name = "Fbx_MRAY_D_PIJ",		location = "KL1" },
	{ fbx_name = "Fbx_PARROT_D_PIJ",	location = "KR" },
	{ fbx_name = "Fbx_PARROT_D_PIJ_B",	location = "KR" },
	{ fbx_name = "Fbx_PARROT_D_PIJ_C",	location = "KR" },
	{ fbx_name = "Fbx_PARROT_D_PIJ_D",	location = "KC" },

	{ fbx_name = "Fbx_MANTA_CLEAN",		location = "KC" },
	{ fbx_name = "Fbx_MANTA_EAT",		location = "KC" },
	{ fbx_name = "Fbx_MANTA_PARADE",	location = "KC" },

	{ fbx_name = "Fbx_POTATO_CLEAN",	location = "KL2" },


	{ fbx_name = "Fbx_WHITETIP_DJ5",	location = "KR" },
	{ fbx_name = "Fbx_WHITETIP_DJ5_B",	location = "KR" },
	{ fbx_name = "Fbx_TIGER_CHASSE_KR", location = "KR" },
	{ fbx_name = "Fbx_TIGER_CHASSE_KC", location = "KC" },
	{ fbx_name = "Fbx_TIGER_KR",		location = "KR" },

	{ fbx_name = "Fbx_WHALE_DJ",		location = "KC" },
	{ fbx_name = "Fbx_WHALE_BB",		location = "KC" },
	{ fbx_name = "Fbx_WHALE_DJ",		location = "KR" },

	{ fbx_name = "Fbx_SHRK_WHAL_KR",	location = "KR" },
	{ fbx_name = "Fbx_SHRK_WHAL_KC",	location = "KC" },
	{ fbx_name = "Fbx_SHRK_WHAL_SO",	location = "SO" },

	{ fbx_name = "Fbx_MRAY_C_PIJ",		location = "KL1" },
	{ fbx_name = "Fbx_DOLPH_C",			location = "KR" },
	{ fbx_name = "Fbx_C_TITAN",			location = "KR3" },
	{ fbx_name = "Fbx_WHALE",			location = "KR" },

	--{ fbx_name = "Fbx_DOLPH_DN2",		location = "KR" },

	--, "Fbx_MRAY_D_PIJ",
	--"Fbx_PARROT_D_PIJ", "Fbx_PARROT_D_PIJ_B", "Fbx_PARROT_D_PIJ_C", "Fbx_PARROT_D_PIJ_D",
	--"Fbx_MANTA_CLEAN", "Fbx_MANTA_EAT", "Fbx_MANTA_PARADE",
	--"Fbx_POTATO_CLEAN",
	--"Fbx_DOLPH_DN2",
--	"Fbx_WHITETIP_DJ5", "Fbx_WHITETIP_DJ5_B",
--	"Fbx_TIGER_CHASSE_KR", "Fbx_TIGER_CHASSE_KC",
	--"Fbx_TIGER_KR", "Fbx_TIGER_KR",
	--"Fbx_WHALE_DJ", "Fbx_WHALE_BB", "Fbx_SHRK_WHAL_KR", "Fbx_SHRK_WHAL_KC", "Fbx_SHRK_WHAL_SO",
--	"Fbx_MRAY_C_PIJ", "Fbx_DOLPH_C", "Fbx_C_TITAN", "Fbx_WHALE"
}

function meu:preroll_load_fbx( fbxname )
	local meu_fbx = self:get_meu_by_name_no_error( fbxname )
	if meu_fbx then
		meu_fbx:set_mu_value( 1 )
		meu_fbx:draw()
		meu_fbx:set_mu_value( 0 )
	end
end

function meu:preroll_animation()
	if app:is_pc_dev() == false then
		for i, preroll in IPAIRS( app.preroll_anims ) do
			if app:find_location( preroll.location ) then
				self:preroll_load_fbx( preroll.fbx_name )
			end
		end
		self:stop_all()
	else
		self:stop_all()
		-- to test preroll
	--	for i, preroll in IPAIRS( app.preroll_anims ) do
	--		self:preroll_load_fbx( preroll.fbx_name )
	--	end
	end
end