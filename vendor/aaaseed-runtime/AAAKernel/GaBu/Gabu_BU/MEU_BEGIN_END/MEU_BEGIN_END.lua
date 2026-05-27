local help =
{
	"MuBegin/MuEnd are used to surround MUs in the rendering chain",
	"  then operations can be applied to these MUs: Loop, alter rendering order, draw selectively...",
	"  it is used mainly for looping and execute render operation requiring several passes",
	"The \"Z loop\" mode is well suited to move forward and/or backward in an infinite stack of MEU.",
	"The current MuBegin camera (the one used when rendering MuBegin) take over",
	"  the cameras of MUs inside which implicitly switch to current mode"
}
				
if CLASS.DECLARE( "MEU_BEGIN_END",	MEU ) then
	MEU_BEGIN_END:set_class_status_doc(	CLASS.STATUS.GABUZOMEU, "is the base class for MEU_BEGIN AND MEU_END" )
end

if CLASS.DECLARE( "MEU_MUBEGIN",	MEU_BEGIN_END	) then
	MEU_MUBEGIN:set_class_status_doc(	CLASS.STATUS.GABUZOMEU, help )
end

if CLASS.DECLARE( "MEU_MUEND",		MEU_BEGIN_END	) then
	MEU_MUEND:set_class_status_doc(		CLASS.STATUS.GABUZOMEU, help )
end

function MEU_BEGIN_END:define_meu_infos()
	return { author = "Mâa",
			tags = { "Art", "Core", "CoreGraphic", "procedural", "unfinished" },
		}
end


function MEU_MUEND:draw_icon()
	gol.set_line_width( 2 )
	gol.set_texture_dim( 0 )
	local S = .35
	gol.push_translate_scale_2d( -.35, 0, .3, 1. )
			gol.draw_line_strip_2d(	.5,		.5,
									-.2,	.5,
									-.2,	-.5
								)
			local s = .25
			gol.draw_triangles_2d(	-.2,	-.2+s*1.5,
									-.2-s,	-.2,
									-.2+s,	-.2
								)
	gol.pop_matrix()
end

function MEU_MUEND:modify_render_chain( level_stack )
	if level_stack:get_size() <= 0 then
		self:print_debug( "Empty stack for MuBegin/End ")
		return false
	end

--	aaa.print_fn()
	local level = level_stack:pop()
	--table.print( level.mu, self.." to_render")
	
	local meu_begin = level.meu_begin
	if not meu_begin then
		self:print_debug( "Unbalanced MuBegin/End ")
		return false
	end

	--self.__mu_to_render = level.mu
	meu_begin.__mu_to_render = level.mu
	return true
end

function MEU_MUBEGIN:define_ui()
	local ref = self.ref
	local bu
	local par
	local SY = 1
	local DY = .2
	local ix,iy = 1,1

	self:add_camera( {nil,nil,nil,1.6}, 16, true )
	self:add_transfo( {nil,iy+1.6+DY,	nil,SY*3} )
--	self:add_button(	{	1, 1 },				"Active",				self, "b_active", true		)
		--todo too raw: MU by Mu nearly
--		self:add_button(	{	2, 2 },			"Skip",					self, "b_skip", 	false		)
--		self:add_button(	{	2, 4 },			"Texture Increment",	self, "b_tex_inc",	false		)

	bu = self:add_slider(	{ix,iy,		8,SY},	"Nb",		self, "pass_nb", 1,	 0,32	)	--notext not save in preset)
		:set_value_type_integer( true )

	iy = iy + SY + DY
	local nb = 10
	bu = self:add_selector(	{ix,iy,		4,nb*SY},	"Mode" )	--notext not save in preset
		bu:set_nb_min_0( 1, nb )
		bu:set_item_text( 1, "Skip", "Regular",
							"Shadow First", "Shadow Last",
							"Repeat",
							"Left Right", "Right Left",
							"Bottom Top", "Top Bottom", "Z Loop" )
		bu:set_target_lua( self, "s_mode" )
		bu:set_text_draw( false )

	self:add_trig_method(	{9,8,		4,SY},	"Dump",			self, "dump_render_chain" )

	ix,iy = 5,iy+9*SY
	--self:add_sliders_xyz(	{ix,iy,		12,SY},	"Range",		self.range,	false,	10	)
	--self:add_sliders_xyz(	{ix,iy,		8,SY},	"Center",		self.center, false,	4	)
	bu = self:add_slider(	{5+8,iy,	4,SY},	"Size Z",		self, "size_z",		8,		0,8		):set_color_back('z')
	iy = iy + SY

	bu = self:add_slider(	{1+4,iy, 	4,SY},	"Speed",		self, "speed",		0,		-1,1	)
	bu = self:add_slider(	{1+8,iy, 	4,SY},	"Scale",		self, "scale",		1,		0,4		)
	bu = self:add_slider(	{5+8,iy,	4,SY},	"Fade Alpha",	self, "fade_alpha",	.25,	0,1		)

	iy = iy + SY
	--self:add_sliders_xyz(	{ix,iy,		12,SY},	"Range",		self.range,	false,	10	)
	bu = self:add_slider(	{5+8,iy,	4,SY},	"Fade Size",	self, "fade_size",	.25,	0,1		)
	self:add_sliders_xyz(	{ix,iy,		8,SY},	"Jitter",		self.rnd_range, false,		4		)

	ix = 9
	iy = 2
end

function MEU_MUBEGIN:init()
	local ref = self.ref
	local stereo = aaa.obj.get_from_top_by_class( "stereo" )
	ref.right = param.get_ref( stereo, "right_eye" )

	self.rnd_range = { x=0, y=0, z=0 }
	--self.center = { x=0, y=0, z=0 }
end

function MEU_MUBEGIN:set_mode_skip()	self.s_mode = 0	end
function MEU_MUBEGIN:set_mode_regular()	self.s_mode = 1	end

function MEU_MUBEGIN:set_pass_nb( nb )
	self.pass_nb = nb
end

function MEU_MUBEGIN:draw_icon()
	gol.set_line_width( 2 )
	gol.set_texture_dim( 0 )
	local S = .35
	gol.push_translate_scale_2d( -.35, 0, .3, 1. )
		gol.draw_line_strip_2d(	.5, -.5,
								-.2, -.5,
								-.2, .5
								)
		local s = .25
		gol.draw_triangles_2d(	-.2,	.2,
								-.2-s,	.2-s*1.5,
								-.2+s,	.2-s*1.5
								)
	gol.pop_matrix()
end



function MEU_MUBEGIN:modify_render_chain( level_stack ) --, mu, t_mu_to_render, id )
--	aaa.print_fn()
--	self:print( "modify_render_chain()" )
--	local ret = s_mode ~= 1
--	if ret then 
		self.__mu_to_render = nil
		level_stack:push( {mu={},meu_begin=self} )
		return true
--	end
--	return ret
end

function MEU_MUBEGIN:dump_render_chain()
	self:print( "dump_to_render()" )
	local mu_to_render = self.__mu_to_render
	table.print( mu_to_render, "mu_to_render" )
	if mu_to_render then
		for i = 1, #mu_to_render do
			local mu = mu_to_render[i]
			self:print( "\t"..i.." "..mu )
		end
	end
end

local param_list_lr =	{	"sub_viewport",	"stereo_use",
							"sub_viewport_left",	"sub_viewport_right", "final_scale_y"
						}
local param_list_bt =	{	"sub_viewport",	"stereo_use",
							"sub_viewport_bottom",	"sub_viewport_top", "final_scale_x"
						}

function MEU_MUBEGIN:__modify_cam_side_by_side_get( cam, list )
	--opt stire and use ref,
	local tab = {}
	for i=1, #list do
		tab[i] = param.get( cam, list[i] )
	end
	return tab
end
function MEU_MUBEGIN:__modify_cam_side_by_side_set( cam, list, tab )
	for i=1, #list do
		param.set( cam, list[i], tab[i] )
	end
end
function MEU_MUBEGIN:__modify_cam_side_by_side( cam, list, index, nb )
	self:__modify_cam_side_by_side_set( cam, list, { true, true, index / nb, (index+1) / nb, nb } )
end

--todo why not ca
function MEU_MUBEGIN:update()
	self.ph = (self.ph or 0) + aaa.time.dt * self.speed
--	self:print( self.ph )
end

function MEU_MUBEGIN:draw_loop( mu_to_render, alpha_factor )
	local cam_ui = aaa.camera.get_ui()

	local nb = #mu_to_render
	local nb_render = nb * self.pass_nb
	--self:print( nb_render )

	local delta = 1. / nb_render
	--self:print( delta )
	-- store and prepare by making sure camera are ignored
	local t_used = {}
	for i,mu in ipairs(mu_to_render) do
		local meu = mu:get_meu_used()
		local bu = meu:get_bu_by_key( "cam_use" )
		if bu then
			mu.__cam_used_stored = { bu=bu, val=bu:get_value() }
			--self:print( cam_use )
			bu:set_value( 0 )
		end
		table.insert( t_used, mu )
	end

	-- build a table 
	local t_draw = {}
	local ph = self.ph
	for ip=1,self.pass_nb do
		for i,mu in ipairs(t_used) do
			--self:print( i.." : "..ph )
			local floor = math.floor(ph)
			table.insert( t_draw, { mu, ph-floor, floor*1254+ip*i } )
			ph = ph + delta
		end
	end
	--table.print( t_draw, "MuBegin t_draw", 0 )

-- draw
	local rr = self.rnd_range
	local szh = self.size_z * .5
	local fade_z = self.fade_alpha * szh
	--local cen = self.center

	local f_scale = self.fade_size <= 0. and 100000000 or 1 / self.fade_size
	for i,elt in pairs_sorted( t_draw, function(t,a,b) return t[a][2] < t[b][2] end ) do
		local ph = elt[2]
		local z = ph * self.size_z
		
		math.randomseed( elt[3] )
	
		local z = z + (math.random()-.5) * rr.z

		local scale = clamp_01( z / self.size_z * f_scale )

		z = z - szh

		local alpha
		if		z < -szh		then	alpha = 0
		elseif	z < -szh+fade_z	then	alpha = (z+szh)/fade_z
		elseif	z <  szh-fade_z	then	alpha = 1
		elseif	z <  szh		then	alpha = (szh-z)/fade_z
		else							alpha = 0
		end

		gol.push_matrix()
			gol.translate( (math.random()-.5) * rr.x * scale,
						   (math.random()-.5) * rr.y * scale,
						   z	)
			gol.scale( scale * self.scale )
			elt[1]:render( alpha * alpha_factor )
			--mu:set_render_skip_next( true )
			--gol.translate_z( -z 
		gol.pop_matrix()
	end
	for i,mu in ipairs(t_used) do
		--self:print( i.." : "..ph )
		local store = mu.__cam_used_stored
		if store then
			store.bu:set_value( store.val )
			mu.__cam_used_stored = nil
		end
	end

	cam_ui = cam_ui or aaa.camera.get_cur()
	aaa.camera.set_ui( cam_ui )
end

function MEU_MUBEGIN:draw_low()
	local mu_to_render = self.__mu_to_render
	if not mu_to_render then return end

	local alpha_meu = self:get_alpha()
	local ref = self.ref

	local s_mode = self.s_mode
--	self:print( "s_mode is "..s_mode )
	local pass_info = { name="regular", index=1, pass_index=1 }

-- MU end is always at the end of the list
	local nb_mu = #mu_to_render
	local mu_end =  mu_to_render[nb_mu]
	mu_to_render[nb_mu] = nil

	if nb_mu > 1 then 

	-- SKIP we just skip all the mu
		if s_mode == 0 then
			-- but we need this to display in the UI that we skip
			for i,mu in ipairs(mu_to_render) do
				mu:set_render_skip_next( true )
				mu:render( alpha_meu )
			end
	-- REGULAR
		elseif s_mode == 1 then	-- mode Pass : do nothing
			for i,mu in ipairs(mu_to_render) do
				mu:render( alpha_meu )
			end
	-- LOOP : repeat mode created for Jaime and Tendayi
		elseif s_mode == 9 then -- mode Jaime repeat
			self:draw_loop( mu_to_render, alpha_meu )
		elseif s_mode <= 8 then
			--deal with pass_info
			ga:push_pass_info()
			local pass_nb = self.pass_nb
			--self:print( self.pass_nb )

			local list_stereo_cam
			local b_flip
			local b_right
			self.__mu_fbos_array = nil
			self.__mu_fbos_keys = nil
			if s_mode >= 5 then --modes stereo
				--self:print( "pass here" )
				list_stereo_cam = (s_mode >=7) and param_list_bt or param_list_lr
				--self:print_inverse( "render() "..chain.begin_id.."-"..chain.end_id )
				--aaa.show( ref.right, "right ref" )
				--table.print( chain.t_mu, "Toto", 1 )
				b_flip = (s_mode == 6) or (s_mode == 8)
				b_right = param.get( ref.right )
			elseif s_mode==2 or s_mode==3 then --modes shadow_first shadow_last
				--todo check reuse of table
				self.__mu_fbos_array = {}
				self.__mu_fbos_keys = {}
				local mu_gbuf = self:get_mu_by_name_no_error( "Fbo_GBuffer" )
				if mu_gbuf then
					self.__mu_fbos_keys[mu_gbuf] = true
					if s_mode==3 then
						table.insert( self.__mu_fbos_array, mu_gbuf )
					end
					for i=1,8 do
						local mu = self:get_mu_by_name_no_error( "Fbo_Shadow_"..i )
						if mu then
							if i < pass_nb then
								table.insert( self.__mu_fbos_array, mu )
							end
							self.__mu_fbos_keys[mu] = true
						end
					end
					if s_mode==2 then
						table.insert( self.__mu_fbos_array, mu_gbuf )
					end
				else
					self:print_error( "Can't find GBuffer MU" )
				end
			end

			local mus = MUS.get_cur()
			local mu = self:get_mu()
			local pass_info = { name="regular", index=1, pass_index=1 }
			for pass=1,pass_nb do
				pass_info.pass_index = pass
				if s_mode==2 or s_mode==3 then
					if s_mode==2 then --Shadow_First
						if pass==pass_nb then
							pass_info.name = "gbuffer"
							pass_info.index = 1
						else
							pass_info.name = "shadow"
							pass_info.index = pass
						end
					else			--Shadow_Last
						if pass==1 then
							pass_info.name = "gbuffer"
							pass_info.index = 1
						else
							pass_info.name = "shadow"
							pass_info.index = pass-1
						end
					end
				end

				ga:set_pass_info( pass_info )
				if GA.b_spy then aaa.spy.push_range( "Pass "..pass_info.name.." "..pass_info.index, 2+pass_info.pass_index ) end

					if list_stereo_cam then
						aaa.camera.set_cur()
						param.set( ref.right, pass==2 )
					end

					local cam_prev = nil
					local cam_store = nil

					for i,mu in ipairs(mu_to_render) do
						if list_stereo_cam then
							local meu = mu:get_meu()
							local cam = meu:get_camera_used()
							if cam and cam_prev ~= cam then
								--self:print( "\tpass "..pass.." "..mu.."with alpha "..alpha_meu )
								--opt avoid so much changes
								if cam_prev then
									self:__modify_cam_side_by_side_set( cam_prev, list_stereo_cam, cam_store )
								end
								cam_store = self:__modify_cam_side_by_side_get( cam, list_stereo_cam )
								self:__modify_cam_side_by_side( cam, list_stereo_cam, b_flip and pass-1 or (pass_nb-pass), pass_nb )
								cam_prev = cam
							end
						end
						if self.__mu_fbos_keys and self.__mu_fbos_keys[mu] then
							if self.__mu_fbos_array[pass] == mu then
								mu:render( alpha_meu )
							end
						else
							mu:render( alpha_meu )
						end
					end

					--make draw_render_chain nicer
					if pass < pass_nb then
						--todo make that cleaner
						mus:add_mu_rendered( mu, true, true )
					end

					if list_stereo_cam and cam_prev then
						self:__modify_cam_side_by_side_set( cam_prev, list_stereo_cam, cam_store )
					end

				if GA.b_spy then aaa.spy.pop_range() end
			end

			if list_stereo_cam then
				param.set( ref.right, b_right )
			end
			ga:pop_pass_info()
		else
			self:print_error( "we should not have s_mode at "..s_mode )
		end

	end

	--this is the MuEnd
	mu_end:render( alpha_meu )

	--self:print( "ask skip "..(chain.end_id - chain.begin_id - 1) )
--	MUS.get_cur().__skip_mu_nb = chain.end_id - chain.begin_id - 1
end

function MEU_MUBEGIN:draw()
	self:draw_layers_begin()
		self:draw_low()
	self:draw_layers_end()
end