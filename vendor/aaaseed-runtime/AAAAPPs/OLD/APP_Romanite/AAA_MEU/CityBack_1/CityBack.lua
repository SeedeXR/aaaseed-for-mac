
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()

	local ix,iy = 1,1

--	bu = self:add_button(	{ix, iy }, "Take Control" )


end


local names			=	{	"epidemik",		"nomensland",	"suez" 		}

function meu:init()
	local ref = self.ref
	local cams = {}
	for i=1,3 do
		cams[i] = self:get_camera( i )
	end
	ref.cams = cams

	MEDIA.set_dir_media( "AAARom/Demo" )

	local tab = { 	"3D/SOL.tga", "3D/BATIMENTS.tga", "3D/TRONCS.tga", "3D/FEUILLAGE.tga"	}
	for i=1,4 do
		local bind = IMGS.get_bind( tab[i] )
		--aaa.box_good( bind )
		aaa.layer.set_bind_2d( self:get_layer(i+2), bind )
	end
end

function meu:init_spe()
	local tmeu = {}

--	tmeu.kinect		= self:get_meu_by_name( "Kinect_2" )
	self.tmeu = tmeu
end

function meu:set_cam_pos_target( id, tab )
	local cam = self.ref.cams[id]
	local cx, cz = tab[4], tab[6]
	param.set( cam, "tra_x", tab[1] )
	param.set( cam, "tra_y", tab[2] )
	param.set( cam, "tra_z", tab[3] )
	param.set( cam, "center_x", tab[4] )
	param.set( cam, "center_y", tab[5] )
	param.set( cam, "center_z", tab[6] )
	param.set( cam, "focal", interpolate( 45, 15, clamp_01( math.sqrt( cx*cx+cz*cz) / 500	 ) )  )
end
function meu:generate_pos( x, z, ph, dph, yc, rc, yt, rt )
	local xc = x + math.cos( ph ) * rc
	local zc = z + math.sin( ph ) * rc
	ph = ph + dph
	local xt = x + math.cos( ph ) * rt
	local zt = z + math.sin( ph ) * rt
	return { xc, yc, zc, xt, yt, zt }
end
function meu:inter( a, b, t )
	t = math.fmod( t, 2 )
	t = math.sin( t * math.pi ) *.5 + .5
	if t > 1 then		t = 2-t	end
	for i=1,#a do
		a[i] = a[i]*(1-t) + b[i]*t
	end
end
function meu:update()
	if not roma or not roma.section_cur then return end

	local sections = roma.sections
	if not sections then return end

	local c = roma.cam_center
	local x, y, z = c.x, c.y, c.z
	local inter = roma.section_inter
	if inter then
		inter = step_smooth_01( inter )
		local ct = roma.sections[roma.section_target].center
		x = interpolate( x, ct.x, inter )
		y = interpolate( y, ct.y, inter )
		z = interpolate( z, ct.z, inter )
	end
	c.x, c.y, c.z = x, y, z

	local ph = roma.cam_ph or 0
	if roma.x_begin or roma.b_ph_release_trig then
		ph = ph - roma.ph_dx * 3.
		if roma.b_ph_release_trig then
			aaa.print_inverse( "spike" )
			roma.b_ph_release_trig = nil
			roma.ph_y_inter = 1
			roma.cam_ph = ph
		end
	else
		ph = ph -.1 * aaa.time.dt
		roma.cam_ph = ph
	end

	local yc = 3
	local rc = 4
	--roma.ph_y_inter = 0
	local inter = roma.ph_y_inter
	if roma.y_begin then
		inter = 1
	else
		if inter then
			inter = inter - aaa.time.dt
			if inter < 0 then
				inter = 0
				roma.ph_y_inter = nil
			end
			roma.ph_y_inter = inter
		end
	end
	if inter then
		yc = yc - inter * roma.ph_dy * 2.2
		rc = rc - inter * roma.ph_dy * 2.2
	end


	local pos = self:generate_pos( x, z,  ph, 3.14 * .5,
										yc, rc,
										y, 0.25 )

--[[	local pos2 = self:generate_pos( 0, 0, -.10, 3.14 * .5,
										2, 2,
										0, .5	 )
	self:inter( pos1, pos2, aaa.time.t*.05 )
--]]
	self:set_cam_pos_target( 1, pos )
end

function meu:add_pos( x,y,z )
	local tab = roma.pos_hack
	x,y,z = aaa.camera.world_to_screen( x,y,z )
	local i = #tab
	table.insert( tab, {x,y,z,id=i+1} )
end

function meu:draw()
	MEU.draw( self )

	if not aaa.lua.global.get( "roma" ) then return end

	roma.pos_hack = {}

	local subs = roma.subs_all
	for i=1,#subs do
		local sub = subs[i]
		local bat = sub.bat
		meu:add_pos( bat.x, bat.y, bat.z  )
	end
end
