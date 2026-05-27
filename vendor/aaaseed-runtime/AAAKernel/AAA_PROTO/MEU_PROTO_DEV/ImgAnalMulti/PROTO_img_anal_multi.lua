
function meu:define_meu_infos( )
	return	{	author = "Mâa",
				tags = { "2d", "Core", "CoreGraphic", "Procedural", "texture", "unfinished" },
				help = {	"used to measure the coverage (number of pixel) in several rectangles",
							"  and detect the center of thses non black pixel.s",
			 				"  Encapsulate c_bdd_img_analyse."
						}
			}
end

local function draw_multi( bu )
	gol.color_white( 1 )
	local bind = bu.__multi_meu_caller.bind_src
	if bind then
		aaa.draw_bind_rect( bind, -.5,-.5, .5,.5 )
		local sx,sy = aaa.img.get_size( bind )
		if sx then
			local ratio = sx/sy
			if ratio > 16/9 then
				if ratio > 3 then
					ratio = 3
				end
				bu:set_sxy( 1, 1/ratio )
			else
				if ratio < .8 then
					ratio = .8
				end
				bu:set_sxy( (9/16)*ratio, 9/16 )
			end
		end
	else
	end
	
	gol.color_white( .25 )
	gol.set_line_width( 1 )
	local S = .5
	for f=-.5+S,.5-S,S do
		aaa.draw_line( f, -.5, f, .5 )
		aaa.draw_line( -.5, f, .5, f )
	end
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1
	local DY = .2

	self:add_slider_two(	{ix,iy,		8,SY},	"Coverage",	ref.bdd,	"coverage_min",	"coverage_max",			  0,1,	0,1	 )
	iy = iy + SY
	self:add_slider_two(	{ix,iy,		8,SY},	"Range in",	ref.bdd,	"threshold",	"threshold_max",		.25,1,	0,1  )
	local iy_push = iy + SY + DY

	
	self:set_tab_key_def()
	iy = 6
		--tdo chane this
		local SXM = 16
		local SYM = SXM/16*9
	--self:print( self.sx.." by "..self.sy )
	-- local SY = SX * self.sy/self.sx
	-- if SY > 10 then
	-- 	SY = 9
	-- 	SX = SY * self.sx/self.sy
	-- end
		--self:print( "SY is "..SY )
		--todo -1.2 on y is a pure hack, position of slider multi should be done with a fit to rect fns
		bu = self:add_slider_multi(	{9-SXM/2,iy,	SXM,SYM}, "zones", self.nb_pass )
			bu:set_elt_text_xy_f_ratio( -.5, .1, .4 )	
			bu:set_select_on_click_double()
			bu:set_ui_top( false )
	--		bu.draw = self.draw_multi
			bu.draw_back = draw_multi
			bu.__multi_meu_caller = self
		ui.bu_passes = bu

	self:set_tab_key( "More" )
	iy = iy_push
	self:add_bu_texture( {1,iy,	8,6},	"Tex",	1, 	false	)
	
	self:set_tab_key()
	ix,iy = 9,1
	local rect_data = {}
	for i = 1,self.nb_pass do
		if i==6 then
			self:set_tab_key( "More" )
		end
		local info = {}
		self:add_slider(	{ix,iy,		4,SY},	"Cov "..i,		info,"cov",	0,	0,1)
		self:add_slider(	{ix+4,iy,	2,SY},	"Center u "..i,	info,"u",	0,	0,1):set_color_back("u"):set_meter(false)
		self:add_slider(	{ix+6,iy,	2,SY},	"Center v "..i,	info,"v",	0,	0,1):set_color_back("v"):set_meter(false)
		rect_data[i] = info
	--	:set_draw_min(false)	--todo works but not the right way ? solve later
		
		iy = iy + SY
	end
	self.rect_data = rect_data

	--todo add ui to choose rect we use
	ix,iy = 9,1
	-- self:add_selector(		{ix,iy,			8,SY},	"Detection"		)
	-- 	:set_item_text( 1, "Detection", "Simulation", "Direct" )
	-- 	:set_target_lua( self, "s_detection" )
	-- 	--:set_nb( 3 )
	-- --	:set_value( 2 )
	-- iy = iy + SY

	iy = iy + SY*3 + DY

end


function meu:init()
	local ref = self.ref
	local bdd = self:get_layer_bdd(1)

	ref.bdd = bdd

	self.s_init = 0
	self.nb_pass = 12

	local pgr = param.get_ref
	
	ref.cov_min 		= pgr( bdd, "coverage_min" )
	ref.cov_max 		= pgr( bdd, "coverage_max" )
	ref.out_coverage    = pgr( bdd, "out_coverage" )

	ref.out_center_u 	= pgr( bdd, "out_center_u" )
	ref.out_center_v 	= pgr( bdd, "out_center_v" )

	ref.bind_src 		= pgr( bdd, "image_src" )

	ref.begin_u 		= pgr( bdd, "begin_u" )
	ref.end_u			= pgr( bdd, "end_u" )
	ref.begin_v 		= pgr( bdd, "begin_v" )
	ref.end_v			= pgr( bdd, "end_v" )

	ref.threshold_min	= pgr( bdd, "threshold" )
	ref.threshold_max	= pgr( bdd, "threshold_max" )

	param.set( bdd, "force_compute", true )	-- needed because we change rect but not texture (--todo should optimize)
end


-- function meu:get_circle_cov( i )
-- 	local pre = self.presence[i]
-- 	return pre.cov
-- end
-- function meu:get_circle_accum( i )
-- 	local pre = self.presence[i]
-- 	return pre.accum
-- end
-- function meu:set_circle_accum( i, accum )
-- 	local pre = self.presence[i]
-- 	pre.accum = accum
-- end

--meu.doc.get_rect_data = "( id ) if rectangle id is active return coverage, u,v ."
function meu:get_rect_data( id )
	if id then
		local data = self.rect_data[id]
		if data.b_on then
			return data.cov, data.u,data.v
		end
	end
end

--todo deal with coverage, center, rect this was done settle for nimes
--infact call it ImgAnalyse
function meu:update()
	local ref = self.ref

	local bind = self:get_texture_bind_2d()
	if self.bind_src ~= bind then
		param.set( ref.bind_src, bind )
		self.bind_src = bind
	end
	-- local dt = aaa.time.dt

	-- local pres = self.presence
	-- for i=1,self.nb_anal do
	--  	local pre = pres[i]
	--  	if self.s_detection~=3 then
	--  		if self.s_detection==1 then
	--  			param.set( pre.ref_cov_max, pre.coverage_max )
	--  			self:update_then_draw( pre.ref_bdd )
	--  			pre.cov = param.get( pre.ref_cov )
	--  		end
	--  		local cov = pre.cov
	-- 		--todo expose values used in the interface
	--  		pre.accum = clamp_01( pre.accum + ((cov>.05) and (cov*.5) or -1) * dt )
	--  	end
	-- end

--	aaa.show( aaa.time.get_time_now(), "time" )

--demo
--[[
	local cov, u,v = self:get_rect_data(1)
	if cov then
		self:print( cov.." "..u..","..v )
	end
--]]
end

function meu:draw()
	local ui = self.ui
	local ref = self.ref
	self:draw_layers_begin()

		local cov = self.out_cov
		local u = self.out_u
		local v = self.out_v
		math.randomseed( 42 )

		local b_draw = true
		local ps = param.set
		local pg = param.get
		local multi = ui.bu_passes
		local rect_data = self.rect_data	
		for i = 1,self.nb_pass do
			local elt = multi:get_elt( i )
			local info = rect_data[i]
			if elt and elt:get_value() then
				info.b_on = true
				if b_draw then
					gol.color( (.5 + math.random() * .5) )
				end
				--self:print_debug( "elt "..i )
				local x,y = elt:get_xy()
				local sx,sy = elt:get_sxy()
				local u_begin,u_end = x-sx*.5,x+sx*.5
				local v_begin,v_end = y-sy*.5,y+sy*.5
				ps( ref.begin_u, u_begin )
				ps( ref.end_u,   u_end )
				ps( ref.begin_v, v_begin )
				ps( ref.end_v,   v_end )
				--aaa.print_fn()
				self:draw_layer(1)
				info.cov = pg( ref.out_coverage )
				info.u   = pg( ref.out_center_u )
				--self:print( i.." "..info.u )
				info.v   = pg( ref.out_center_v )
			else
				info.b_on = false
				info.cov = 0
				info.u = .5
				info.v = .5
			end
		end
		--table.print( rect_data, "rect_data", 3 )
--[[
	
	for i=1,nb do
			local x,y = elt:get_xy()
			x = (x-.5) * SX
			y = (y-.5) * SY

			--aaa.show( x, "x" )
			local x,y = elt:get_xy()
			sx = sx * SX * .5
			sy = sy * SY * .5

			--self:print( i.."drawing elt "..x..","..y )

			if b_simul then
				local ph = elt.ph or 0
				if self.b_speed then
					ph = ph + aaa.time.dt * interpolate( self.speed_min, self.speed_max, (i-1)/(nb-1) )
				end
				elt.ph = ph
				if self.b_turbulence_active then
					ph = ph + aaa.math.get_turbulence( x, y, i+ aaa.time.t*self.turbulence_speed, 1, 1 ) * self.turbulence_factor
				end
				if self.s_move==2 then
					ph = ph * math.pi2
					x = x + self.radius * math.sin( ph )
					y = y + self.radius * math.cos( ph )
				elseif self.s_move==3 then
					x = -wrap_01(ph) * 12 + 6
				elseif self.s_move==4 then
					x = wrap_01(ph) * 12 - 6
				end
				if s_simul_draw == 4 then
					local bind = 224 + (i-1) % 5
					gol.bind_texture( bind )
					--self:print( "bind is "..bind )
				end
				draw( i, x,y, sx,sy )
			else
				aaa.draw_rect( x-sx, y-sy, x+sx, y+sy )
			end
		end
	end
--]]


	self:draw_layers_end()
end