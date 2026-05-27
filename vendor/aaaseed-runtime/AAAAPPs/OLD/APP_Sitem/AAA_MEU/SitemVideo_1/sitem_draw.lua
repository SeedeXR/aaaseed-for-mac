function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()
	self:add_monitor()
	self:add_bu_texture_target_unit()
end

function meu:init()
	local ref = self.ref
--	ref.bdd		=	self:get_layer_bdd( 1 )
end

function meu:draw()
	local SX = 4
	local SY = SX/16*9
	if not sitem then return end
	local vid = sitem.vid_cur
	if not vid then return end

	local function draw_rect( xl, yb, xr, yt )
		local ox = 0
		local oy = -.05
		aaa.draw_rect_uv( xl+ox, yb+oy, xr+ox, yt+oy )
	end

	local function draw()
		if vid.b_tb then
			draw_rect( -SX, -SY, SX, SY )
		else
			draw_rect( -SX, 0, SX, SY )
			draw_rect( -SX, -SY, SX, 0 )
		end
	end

	local function draw_resize( bind )
		if not bind then
			self:print( "no bind")
			table.print( vid, "vid", 2 )
			return
		end
		local SX = 4
		local SY = SX/16*9
		gol.bind_texture( bind )
		local sx,sy = aaa.img.get_size( bind )
		local rx = sx / sy
		--self:print( rx )
		local oy
		if rx <= 1.777777 then
			self:print( "high")
			sy = SY
			sx = sy * rx
			oy = 0
		else
			self:print( "low "..sx.." "..sy )
			sx = SX
			sy = sx / rx
			self:print( "    "..sx.."     "..sy )
			oy = (SY - sy ) * .5
		end
		if vid.b_tb then
			draw_rect( -sx, -sy, sx, sy )
		else
			draw_rect( -sx, oy, sx, sy+oy )
			draw_rect( -sx, -sy-oy, sx+oy, -oy )
		end
	end

	--table.print( vid, "vid", 2 )
	local t = aaa.time.t - ( vid.__time_start or 0 )
	local alpha = clamp_01( t * 2 )
	if vid.seq_img then
		MEU.draw( self )
		--self:print( "slide")
		local seq	= vid.seq_img
		local nb	= seq:get_nb()

		--self:print( vid.__time_start.." t : "..t )
		t	= math.fmod( t * .3, nb )
		local id	= math.floor( t ) + 1
		local bind	= seq:get_bind( id )
		local r = t - id + 1
		gol.color_white( alpha )
		draw_resize( bind )
		local delta = .2
		local th = 1. - delta
		if r>th then
			r = (r-th)/delta
			--self:print( r )
			id  = id + 1
			if id > nb then id =1 end
			bind	= seq:get_bind( id )
			gol.color_white( alpha*r )
			draw_resize( bind )			
		end
	else
		self:set_ui_tex_src_sel( 1, 8 + 1-sitem.video_index )
		MEU.draw( self )
		gol.color_white( alpha )
		draw()
	end


	if sitem and sitem.meu_sitem then
		if sitem.meu_sitem.b_see_table then
			gol.bind_texture( 112 )
			local sx = SX * .8
			local sy = sx * .5
			local oy = (SY - sy ) * .5
			draw_rect( -sx, oy, sx, oy+sy )
			draw_rect( -sx, -oy-sy, sx, -oy )
		end
	end

end

