if APP.DECLARE( "APP_KINECT", APP_GP ) then
end

local L_APP = APP_KINECT

function L_APP:init_app( ... )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	local ref = aaa.net.ref
	param.set( ref.obj, "active", 1 )

	self.b_shutdown_allowed = true
	local machine = aaa.net.machine
	self:print( "aaa.net.machine : "..machine )

	if string.find( machine, "NUC" ) then
		local rev = string.reverse( machine )
		local id = tonumber( string.sub( rev, 0, 1 ) )
		local b_left	= string.find( machine, "L" )
		local b_right	= string.find( machine, "R" )
		local pre_name = "KinMoveAuto_"
		for i = 1, 4 do
			local name_left		= pre_name.."KL"..i
			local name_right	= pre_name.."KR"..i
			local b = b_left and id == i
			self:activate_meu( name_left,			b )
			self:activate_meu( name_left.."Vert",	false )	--b ) --remove second image (vertical img_send and fbo are manually disabled)
			b = b_right and id == i
			self:activate_meu( name_right,			b )
			self:activate_meu( name_right.."Vert",	false )	--b ) --remove second image (vertical img_send and fbo are manually disabled)
		end
	end

	ga:set_ui_group_active( false, "top" )
	ga:set_ui_group_active( false )

	return true
end

function L_APP:init_ref_link_ip( link_id )
	local ref = aaa.net.ref
	local link = param.get_ref( ref.obj, "link_"..link_id )
	local link_ref
	if link then
		link_ref = param.get_obj_attached( link )
		if link_ref then
			self.ref["link_"..link_id.."_ip"] = param.get_ref( link_ref, "dst_ip" )
			local link_ip = param.get( self.ref["link_"..link_id.."_ip"] )
			self:print( "Got link_"..link_id.." ip : "..link_ip )
		else
			self:print( "Could not get link_1 attached obj." )
		end
	else
		self:print( "Could not get link_1 param.")
	end
end

function L_APP:init_ref_links( link_list )
	for _, li in PAIRS(link_list) do
		self:init_ref_link_ip( li )
	end
end

function L_APP:link_ip_change( ip_network, ip, link_id )
	self:print( "Setting ip to : "..ip_network..ip )
	param.set( self.ref["link_"..link_id.."_ip"], ip_network..ip )
	aaa.net.restart( link_id )
end

--todo this the lv case it should be in an lv app
function L_APP:__force_from_ip_lv()
	local ref = self.ref

	local function set_from_host( id )
		self:print_inverse( "____ force img_send link channel and texture dst ____" )
		param.set( ref.channel_id, id )
		local id_link = math.floor( id/10 )
		param.set( ref.link_index, 10+id_link )
		local id_kin = id % 10
		self:set_texture_bind_2d( 3, 128+id_kin-1 )
	end
	local host_id = aaa.net.host_id
	if inside(host_id,211,218) or inside(host_id,221,228) or inside(host_id,231,238) then
		set_from_host( host_id - 200 )
	end
	--test
	--set_from_host( 238 - 200 )

end

function L_APP:__init_mona()
	if self.__b_init_mona then return end

	self:print("Do force from ip")
	self:init_ref_links( { 10, 11, 12 } )

	if app.user then
		local name = app.user.name
		self:print( "app.user.name : "..name )
	end

	local machine = aaa.net.machine
	self:print( "aaa.net.machine : "..machine )

	if string.find( machine, "NUC" ) then
		local rev = string.reverse( machine )
		local id = tonumber( string.sub( rev, 0, 1 ) )
		local b_left = string.find( machine, "L" )
		local b_right = string.find( machine, "R" )
		local ip_network = "192.168.1."
		self:link_ip_change( ip_network, "205", 10 ) -- every nuc sends to SO through link 10
		if b_left then
			self:print( "id nucl : "..id )
			if id < 5 then
				self:link_ip_change( ip_network, "21"..id, 11 )
				self:link_ip_change( ip_network, "21"..id, 12 )
			end
		elseif b_right then
			self:print( "id nucr : "..id )
			if id < 5 then
				self:link_ip_change( ip_network, "22"..id, 11 )
				self:link_ip_change( ip_network, "22"..id, 12 )
			end
		end

		local function adjust_img_send( name )
			self:print( "adjust_img_send() for "..name )
			local meu = self:get_meu_by_name_cached( name )
			if meu then
				if string.find( name, "KLR" ) then
					self:print( "\tKLR case" )
					meu:set_texture_bind_2d( 1, 65 ) -- Src in F2
				elseif string.find( name, "SO" ) then
					self:print( "\tSO case" )
					meu:set_texture_bind_2d( 1, 64 ) -- Src in F1
					meu:set_link_index( 10 )
					if b_left then
						meu:set_texture_bind_2d( 3, 128+id-1 ) -- Target img is I1->I5 depending on NUCL id
					elseif b_right then
						meu:set_texture_bind_2d( 3, 128+4+id-1 ) -- Target img is I6->I10 depending on NUCR id
					end
				end
				return true
			end
		end

		self.__b_init_mona = adjust_img_send( "ImgSend_SO" ) and adjust_img_send( "ImgSend_KLR" )
	else
		self.__b_init_mona = true
	end
end

--todo ? add a mecanism
function L_APP:update_before()
	self:__init_mona()
	self:get_gp():update_before()
	oo.getsuper(L_APP).update_before(self)
end

function L_APP:net_quit_pc()
	aaa.shutdown_no_save()
end

function L_APP:net_shutdown_allowed( b_on )
	self.b_shutdown_allowed = b_on
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

