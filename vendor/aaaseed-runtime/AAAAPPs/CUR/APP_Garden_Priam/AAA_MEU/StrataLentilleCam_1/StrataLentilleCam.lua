function meu:define_cam( cam )
	self:print("name global is "..app.name )
	local bu
	local par
	local ix, iy
	local ref = self.ref
	local SX = 8
	local SY = 1
	local DY = .2
	ix = 1
	iy = 1
	
	self:add_camera( nil, 16 )
	self:add_text_info(	{ix + 8 ,iy -1,	8,1}, "camSymboName" )

	local	function add_size(	ox,	oy,		sx, sy,		name,	name_param, min, max, b_meter )
		local bu = self:add_param_obj_name(	{ix+ox,iy+oy,		sx,sy},	name,		cam, name_param, min, max )
			bu:set_meter( b_meter )
		return bu
	end
	local	function add( 		ox,	oy,					name,	name_param, min, max, b_meter )
		return add_size( 		ox,	oy,		SX,	SY,		name,	name_param, min, max, b_meter )
	end

	ix = 1
	iy = 1
	ref = self.ref
	ref.cam = cam
	
	bu = add( 	0,		0,		"Axe",		"draw_axe" )
	iy = iy + 1
	bu = add_size( 	0,	0,	SX/2, SY,		"Orbit",		"orbiting"				)
	bu = add_size( 	4,	0,	SX/2, SY,		"Play",			"orbiting_play"			)
	iy = iy + SY
	bu = self:add_trig( {	ix+4,	iy,		4,SY },	"Restart",		cam, "orbiting_restart_trig"	)

	iy = iy + 2

	bu = add_size( 	0,		0,	3, SY,	"Pers",		"perspective" )
	bu = add_size( 	0+3,	0,	5, SY,	"Ortho",	"ortho_size",		0, 16	)
	-- todo why iy skip the line by itself 
	bu = add_size( 	0+5,	1,	3, SY,	"Hori",		"focal_horizontal"	)
	bu = add_size(	0,		1,	5, SY,	"Focal",	"Focal", 			0, 180	)
		self.ui.bu_focal = bu
		ref.focal = param.get_ref( cam, "focal" )
	iy = iy + 2 + DY

	bu = add( 		0,		0,				"Tra Z",		"tra_z",		-16, 4, false ):set_color_back( "z" )
	iy = iy + SY

	bu = add_size( 	0, 0,		SX/2, SY,	"Tra X",		"tra_x",		-4, 4,	false ):set_color_back( "x" )
		ref.tra_x = param.get_ref( cam, "tra_x" )
	bu = add_size( 	SX/2, 0,	SX/2, SY,	"Tra Y",		"tra_y",		-4, 4,	false ):set_color_back( "y" )
		ref.tra_y = param.get_ref( cam, "tra_y" )
	iy = iy + SY + DY

	bu = add_size( 0,		0,	SX/3,SY,	"Rot X",		"rot_x",		-1, 1,	false ):set_color_back( "x" )
		bu:add_values_def( -.75, -.5, -.25, .25, .5, .75 )
	bu = add_size( SX/3,	0,	SX/3,SY,	"Rot Y",		"rot_y",		-1, 1,	false ):set_color_back( "y" )
		bu:add_values_def( -.75, -.5, -.25, .25, .5, .75 )
	bu = add_size( 2*SX/3,	0,	SX/3,SY,	"Rot Z",		"rot_z", 	-1, 1,	false ):set_color_back( "z" )
		bu:add_values_def( -.75, -.5, -.25, .25, .5, .75 )
	iy = iy + SY + DY

	bu = add_size( 0,		0,	SX/3,SY,	"Rot Cen X",	"rot_center_x",		-4, 4, false ):set_color_back( "x" )
	bu = add_size( SX/3,	0,	SX/3,SY,	"Rot Cen Y",	"rot_center_y",		-4, 4, false ):set_color_back( "y" )
	bu = add_size( 2*SX/3,	0,	SX/3,SY,	"Rot Cen Z",	"rot_center_z", 	-4, 4, false ):set_color_back( "z" )
	iy = iy + SY + DY

	bu = add_size( 0,		0,	SX/4,SY,	"World Scale",	"sca_factor",		-8, 8 ):set_color_back( "factor" )
	bu = add_size( SX/4,	0,	SX/4,SY,	"W Sca X",		"sca_x",			-8, 8 ):set_color_back( "x" )
	bu = add_size( 2*SX/4,	0,	SX/4,SY,	"W Sca Y",		"sca_y",			-8, 8 ):set_color_back( "y" )
	bu = add_size( 3*SX/4,	0,	SX/4,SY,	"W Sca Z",		"sca_z",			-8, 8 ):set_color_back( "z" )
	iy = iy + SY + DY

--	ref.render_edge = self:get_obj_down( "render_edge" )
--		self.bu_sv_edge	= self:add_param_obj_name(	{0+1,iy,			7,1}, "Edge Ep", ref.render_edge, "line_size", 0, 32 )
	

	ix = 9
	iy = 1
	
	iy = iy + 1 + SY + DY
	bu = add( 		0,		0,		"Time Factor",	"orbiting_time_factor",	0,	1	)
	iy = iy + SY + DY

	local function add_axe( letter )
		local pren = "R"..letter
		local prep = "rot_"..letter.."_"
		bu = add_size( 	0,		0,		8, SY,	pren.." Freq",		prep.."frequency",			-.5, .5,	false	):set_color_back( letter )
		bu = add_size( 	0,		0+SY,	4, SY,	pren.."SinF",		prep.."frequency_sin",		0, .5,		false	):set_color_back( letter )
		bu = add_size( 	0+4,	0+SY,	4, SY,	pren.."SinA",		prep.."amplitude_sin",		0,	1,		true	):set_color_back( letter )
		return bu
	end

	add_axe( 	"x" )
	iy = iy + 2*SY + DY
	add_axe( 	"y" )
	iy = iy + 2*SY + DY
	add_axe( 	"z" )
	iy = iy + 2*SY + DY

	bu = add( 	0,	0,		"F Sca X",		"final_scale_x", -8, 8 ):set_color_back( "x" )
	bu = add( 	0,	SY,		"F Sca Y",		"final_scale_y", -8, 8 ):set_color_back( "y" )
	bu = add( 	0,	2*SY,	"F Sca Z",		"final_scale_z", -8, 8 ):set_color_back( "z" )
	
	-- iy = iy + 3*SY + DY

	--	for APP_SERCEL 
	ref.frustum_x		=	param.get_ref( cam, "frustum_offset_x" )
	ref.frustum_y		=	param.get_ref( cam, "frustum_offset_y" )
end

function meu:define_ui()
	--self:add_camera()
	self:define_cam( self:get_camera( 0 ) )
	local bu
	local ix, iy
	local ref = self.ref
	local SX = 8
	local SY = 1
	local DY = .2
	ix = 1
	iy = 1
	
	iy = iy + 2*SY + DY
	local layerS = self:get_layers()
	bu = self:add_button({ix, iy+10, SY, SY}, "LockViewToUI", layerS, "camera_lock_view_to_ui", false):set_text_visible(true) --Checkmark button
end

function meu:draw()
	MEU.draw( self )
	self:__register_to_mus()
end

function meu:draw_icon()
	gol.set_line_width( 2 )
	local L = -.4
	aaa.draw_rect( L, -.25, L+.16, .25 )
	gol.draw_line_strip_2d(	L, -.1,
							L - .05, -.3,
							L - .05, .3,
							L, .1
						)
end

function meu:update()
    -- Ensure the meu names are correct, particularly if you're using an instantiated version of the meu
	local m = self:get_meu_by_name("StrataLentilleObject_1")
	-- for i = 1,200 do
	-- 	local ph = aaa.time.t + i
	-- 	local x = math.cos(ph)
	-- 	local y = math.sin(ph)
    --     -- Setting the value in StrataLentilleObject_1 - note the "m:" (as opposed to "self:") in front of "set_bu_value" 
	-- 	m:set_bu_value("translatex", x * 8 )
	-- 	m:set_bu_value("translatey", y * 8 )
	-- end
    -- Sets the value of this (StrataLentilleCam) object's "Tra Z" value based on the slider of StrataLentilleObject_1. Ensure you change the name above at line 151 if you're using an instantiated version ("StrataLentilleObject_1Inst" or similar)
    self:set_bu_value("Tra Z", m:get_bu_value("translatez"))
    -- GETTING COORDINATES FROM AVATAR
    local a = self:get_meu_by_name("Avatar_1")
    local a_ref = a.ref
    local points = aaa.bdd.get_points_with_id(a_ref.boid)
    -- Ensure points exists before we attempt to iterate through it, otherwise AAASeed becomes very unhappy
    if points then
        for i,t in ipairs(points) do
            local x,y,z = t[1],t[2],t[3]
            -- For now, I'm only setting a value based on the first point. However, this same code could apply to multiple sliders if you use the index (i) and conditionally assign sliders.
            if i == 1 then
                self:set_bu_value("Tra X", x)
                self:set_bu_value("Tra Y", y)
            -- EXAMPLE for if you were to want to set a second value based on the 2nd avatar x and y coordinates
            -- elseif i == 2 then
            --     self:set_bu_value("some other value", x)
            --     self:set_bu_value("yet another value", y)
            end
            
        end
    end
end

