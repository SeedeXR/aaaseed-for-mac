
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	self:add_camera()

	local ix,iy = 1,2
	local sy = 1
	--ui.bu_nb = self:add_text_info(	{ix,iy,	4,sy},			"None" )

	iy = iy + 2
	self:add_button( {ix, 	iy }, 	"Draw_center",	self, "b_draw_center",	false )
	--self:add_button( {ix+1,	iy+1 },	"phy",			self, "b_draw_phy",		false )
	--self:add_button( {ix+1,	iy+2 },	"Id",			self, "b_draw_id",		false )
end

function meu:init()
	--local ref = self.ref
	--ref.bdd_track	=	self:get_layer_bdd(3)

	--ref.blob_nb	=	param.get_ref( ref.bdd_track,	"blob_nb"	)
	--self.blobs_cur = { nb=0 }
	--self.blobs_prev = { nb=0 }
end
-- 						calamar,	manta,	shark,	tuna,	turtle
local size = 		{	1.4,		2,		2,		2.,		1.5		}
local anti_factor = {	1,			1.2,	1,		1,		1		}
local tr_y =		{	-.2,		-.5,	-.5,	0,		0		}
local anim_factor = {	2,			1,		1,		1,		2		}

function meu:draw_blobs( blobs )

	local part_under = self.bdd_part_under
	if not part_under then
		local meu_part_under = self:get_meu_by_name( "DPHack_under"	)
		if not meu_part_under then return end
		part_under = meu_part_under:get_bdd_part()
		if not part_under then return end
		self.bdd_part_under = part_under
	end

	local part_over = self.bdd_part_over
	if not part_over then
		local meu_part_over = self:get_meu_by_name( "DPHack_over"	)
		if not meu_part_over then return end
		part_over = meu_part_over:get_bdd_part()
		if not part_over then return end
		self.bdd_part_over = part_over
	end

	if not blobs then return end

	local dt = aaa.time.dt
	gol.reset()

	gol.set_line_width( 4 )
	local f = 5
	local r = .5
	local d2 = 1*1
	for id=1,blobs.nb do
		local b = blobs[id]
		local anim_id = b.anim_id
		local x,y = b.x_filt, b.y_filt
		local f = anti_factor[anim_id] * .5
		x,y = x+b.dx*f, y+b.dy*f
		local a = math.atan2( b.dx, b.dy )
		local anim = app.anim[ anim_id ]
		local bind
		local ph = b.ph * anim_factor[ anim_id ]
		if anim.b_triangle then		bind = anim:get_bind_in_cycle( ph )
		else						bind = anim:get_bind_in_cycle( ph )
		end
		local sx,sy = aaa.img.get_size( bind )
		sx = sx or 100
		sy = sy or 100
		local s = size[anim_id]
		if anim_id == 5 then --turtle
			for i =1,1 do
				aaa.bdd.add_particle_at( part_under, x, y, 0 )
			end
		elseif anim_id == 1 then --calamar
			local  calamar_level = b.calamar_level
			if calamar_level > .2 then
				for j=1,blobs.nb do
					local bj = blobs[j]
					if bj.anim_id ~= 1 then
						local tx,ty =  bj.x_filt - x, bj.y_filt - y
						local b_found = false
						if (tx*tx+ty*ty) < d2 then
							b_found = true
						else
							f = anti_factor[bj.anim_id] * .5
							tx,ty = bj.x_filt + bj.dx*f - x, bj.y_filt + bj.dy*f - y
							if (tx*tx+ty*ty) < d2 then
								b_found = true
							end
						end
						if b_found then
							for i =1,16	 do
								aaa.bdd.add_particle_at( part_over, x*4, y*4, 0 )
							end
							calamar_level = calamar_level - dt * 1
							b.calamar_level = calamar_level
							break
						end
					end
				end
			end
		end
		gol.push_matrix()
			gol.translate( x, y )
			gol.rotate_z( -a/math.pi2 )
			gol.translate( 0, tr_y[anim_id] )
			gol.scale( s*sx/sy, s )
			aaa.draw_bind_rect( bind, -r, -r, r, r )
			if self.b_draw_center then
				gol.translate( 0, -tr_y[anim_id] )
				aaa.draw_null( 0, 0, 0, 1. )
			end
		gol.pop_matrix()

		if self.b_draw_center then
			gol.push_matrix()
				gol.translate( b.x_filt, b.y_filt )
				gol.color_green()
				aaa.draw_null( 0, 0, 0, 1. )
				gol.color_white()
			gol.pop_matrix()
		end
	end
end


function meu:update_ui()
--	local ref = self.ref
--	local ui = self.ui
--	ui.bu_nb:set_text( "blobs : "..param.get( ref.blob_nb ) )
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )
		self:draw_blobs( app.animal )
	self:draw_layers_end()
end
