

function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "Core", "CoreGraphic", "texture", "input", "unfinished" },
			help = 	{
					"Show the state of the Fbo at the MEU Monitor position in the rendering chain",
					"  double click on the big fix Monitor in the MEU, open a window with a mobile monitor",
					"It is a dynamic Fbo inspector, move the MU around to see the Fbo states along the rendering chain.",
					"  internally this MEU execute low level texture copies (cost some GPU power)",
					"This copies can be accessed by MEUs used later:",
					"  MEU Monitor is also way to define a \"texture flux\" on the fly"
					}
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local SY = 1
	local SYT = 1
	local ix,iy = 1,SYT
	local SYM = 9
	local DY = .5


	bu = self:add_text_info(		{ix,iy,		16,SYT},	"Fbo Src" )
		ui.bu_fbo_src_info = bu
	iy = iy + SYT

	ui.mon = self:add_monitor(		{ix,iy,		16,SYM},	"Dst", self.__bind_no )
		ui.mon:set_info_draw( true )
		ui.mon:set_size_draw( true )
	iy = iy + SYM + DY
	
	local names = { "1", "2", "3", "4", "Depth" }
	ui.mons = {}
	for i=1,5 do
		local mon = self:add_monitor({ix,iy,	4,SYM/4},	names[i], self.__bind_no )
		mon:set_text_info_size_fxy( 1.5 )
		mon:set_info_draw( true )
		mon:set_method_on_click( self, "select_attachment", i )
		ui.mons[i] = mon
		if i==4 then
			ix = 1
			iy = iy + SYM/4
		else
			ix = ix + 4
		end
	end
	--self.ui.mon_sx,self.ui.mon_sy = ui.mon:get_sxy()
end
function meu:init()
	self.__attachment_id = 1
	self.__bind_no = TEXS:get_bind_by_name( "nobind" )
	self.__bind_pb = TEXS:get_bind_by_name( "pb" )
end

function meu:change_name( name )
	MEU.change_name( self, name ) 
	self:__update_dst_bind( true )
end

function meu:__make_attachment_id( i )
	return i==5 and "depth" or i
end

function meu:select_attachment( i )
	local id  = self:__make_attachment_id( i )
	self.__attachment_id = id
	self:print( "select_attachment( "..i.." -> "..id )
--	aaa.show( id, "attachment_id set" )
end

function meu:get_monitor_name_and_bind()
	local name,bind
	local inst_key = self:get_inst_key()
	local id
	if inst_key:sub(1,1) == "M" then
		id = tonumber( inst_key:sub(2) )
	end
	if not id then
		id = tonumber( inst_key )
	end
	if id and math.floor(id) ~= id then
		id = nil
	end
	if id then
		name = "M"..id
	end
	--self:box_debug( "name is "..name ) 
	if not name then
		local gp = GP.cur
		local mon = gp:get_monitor( inst_key )
		if mon then
			name = mon:get_name()
			bind = mon:get_texture_bind_2d() 
		else
			self:show_error( "MEU Monitor only support certain names not "..inst_key )
			bind = TEXS.BIND_HACK_BEGIN
			name = inst_key
		end
	end

	if not bind then
		bind = TEXS:get_bind_by_name(name)		
	end

	return name, bind
end

function meu:__update_dst_bind( b_force )
	if not self.__bind_dst or b_force then
		local name,bind = self:get_monitor_name_and_bind()
	--	local bind = self:get_dst_bind()
		local mon = self.ui.mon
		mon:set_texture_bind_2d( bind or self.__bind_no )
		mon:set_text( name )
		self:set_icon_bind(bind)
		if not bind then
			self:show_error( "Monitor name don't refer to a texture" )
		end
		self.__bind_dst = bind
	end
end

--[[
function meu:get_dst_bind()
	local gp = GP.cur
	if not gp then return end

	local name,bind = self:get_monitor_name_and_bind()

--	local mon = gp:get_monitor( name )
--	self:print( "self:get_monitor_name() is "..name.." -> "..mon )
--	local bind
--	if mon then
--		bind = mon:get_texture_bind_2d() 
--	else	--todo deal with this case
--		local inst_key = self:get_inst_key()
		--self:print( "self:get_inst_key() is "..self:get_inst_key() )
		-- if		inst_key == "KinA1" then	bind = 256
		-- elseif	inst_key == "KinA2" then	bind = 256 + 32
		-- elseif	inst_key == "KinB1" then	bind = 256 + 32 * 2
		-- elseif	inst_key == "KinB2" then	bind = 256 + 32 * 3
		-- else
--		local id = tonumber( inst_key )
--		--hack
--		if id then
--			local name = "M"..id 
--			bind = TEXS.BIND_HACK_END - (id-1)
--				self:print( inst_key.." "..id )
--		else
--			self:show_error( "MEU Monitor only support certain names not "..inst_key )
--			return TEXS.BIND_HACK_BEGIN
--			--local m = self:get_meu_by_name_no_error( "fbo_"..inst_key )
--			--bind = 
--		end
--	end
	return bind
end
--]]

function meu:update_ui()
	self:__update_dst_bind( false )

	local mfbo = self:get_meu_fbo_used()
	if not mfbo then return end

	local ui = self.ui

	local bind_dst = self.__bind_dst
	local bind_src = self.__bind_src
	ui.bu_fbo_src_info:set_text( mfbo:get_name().." | Attach..."..self.__attachment_id..
													": "..bind_src.." -> "..bind_dst )
	ui.mon:set_info_src_bind( bind_src or self.__bind_no )


	local fbo = mfbo:get_fbo()
	local mons = ui.mons
	for i=1,5 do
		local id = self:__make_attachment_id(i)
		
		local b_active,bind = fbo:get_attachment_active(id),fbo:get_attachment_bind(id)
		--self:print( id.." active is "..b_active )
		--aaa.show( bind, "att "..id )
		local mon = mons[i]
		if mon then
			if b_active then
				if id == "depth" and b_active then
					if not fbo:get_attachment_depth_as_texture(id) then
						bind = self.__bind_pb
					end
				end
				--mon:set_active( true )
			--	local f = b_active and 1 or .75
	--			mon:set_sxy( self.ui.mon_sx*f,self.ui.mon_sy*f )
				--mon:set_texture_bind_2d( bind )
			else
				bind = self.__bind_no
				--mon:set_active( true )
			end
			mon:set_texture_bind_2d( bind )
			--mon:set_ui_active( b_active ) 
			--self:print( id.." active is "..b_active )
			--mon:interpolate_alpha_bu( true )
		end
	end
end

-- function meu:update()
-- end
-- function meu:draw_icon()
-- end
function meu:draw()
	--todo avoid every frame it goes with name
	self:__update_dst_bind( false )

	local bind_dst = self.__bind_dst
	--self:print( bind_dst ) 
	if not bind_dst then self:print( "No Bind dst" ) return end

	local mfbo = self:get_meu_fbo_used()
	if not mfbo then return end

	--aaa.show( " to bind "..bind_dst, self.." Attachment_id "..self.__attachment_id )
	--todo add mipmap ui and pass arg
	local bind_src = mfbo:__copy_attachment_to_tex( self.__attachment_id, bind_dst, true, false )
	--if bind_src then
		--aaa.img.move_from_gpu( bind_src )
	self.__bind_src = bind_src
	--end
	
	local ui = self.ui
	local mon_ass = ui.mon_associated
	if not mon_ass then
		mon_ass = ui.mon:get_gp_monitor_associated()
		ui.mon_associated = mon_ass
	end
	if mon_ass then
		--self:print( "set_info_src_bind "..self.bind_src )
		--self.ui.mon:set_info_src_bind( nil )
		mon_ass:set_info_src_bind( bind_src )
	end
end
