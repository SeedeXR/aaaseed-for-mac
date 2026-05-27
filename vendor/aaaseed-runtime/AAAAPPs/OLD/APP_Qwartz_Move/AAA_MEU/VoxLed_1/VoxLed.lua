function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local iy

	local bdd_voxel = ref.voxel.obj
	--self:print( "zobi "..bdd_voxel )

	--[[
	iy = 7

	bu = self:add_slider(	{9,iy,	4,1},	"Tex_x",  self, "tex_x", .5 )
		bu:set_min_max( 0., 1. )
	bu = self:add_slider(	{13,iy,	4,1},	"Tex_y",  self, "tex_y", .5 )
		bu:set_min_max( 0., 1. )
	bu = self:add_slider(	{9,iy+1,	4,1},	"Tex_Sx", self, "tex_sx", .25 )
		bu:set_min_max( .5, 3. )
	bu = self:add_slider(	{13,iy+1,	4,1},	"Tex_Sy", self, "tex_sy", .25 )
		bu:set_min_max( .5, 3. )
	--]]

	self:add_camera()
	--self:add_rgbfa(	{9,11,	nil,nil}, "Test_", nil )

	self.ui.bu_info	= self:add_text_info(	{1,1,	8,.7},	"info"	)
	self.ui.bu_nbs	= self:add_text_info(	{1,2,	16,.7},	"nbs" )

	local iy = 2.7
	bu = self:add_button(	{1,iy, 1,1}, 	"simul", self, "b_simul", true )
		bu:set_text_rect_ratio( 2 )
	--bu = self:add_button(	{5,	3 }, 	"image", self, "b_image", false )

	local geo = {}
	self.geo = geo

	bu = self:add_button(	{3.5,	iy},	"quinc",	geo,		"b_hexa", false )
		bu:set_text_rect_ratio( 2 )
	bu = self:add_button(	{6,		iy }, 	"sphere",	bdd_voxel,	"led_as_sphere", true )
		bu:set_text_rect_ratio( 2 )

	bu = self:add_button(	{9,		iy, 4,1}, 	"Noir nuit", self, "b_black_at_night", false )
	bu = self:add_button(	{13,	iy},	"Save Image", self, "b_save", false ):set_color_back("save")


	iy = iy + 1
	local dy = .8
	bu = self:add_slider(	{5,iy,	6,dy},	"XPos",		bdd_voxel,	"box_center_x",	.5, .0, 1 		)
		bu:set_meter( false )
	bu = self:add_slider(	{11,iy,	6,dy},	"XRange",	bdd_voxel,	"box_range_x",	.5, .0, 1 		)
	iy = iy + dy
	bu = self:add_slider(	{5,iy,	6,dy},	"YPos",		bdd_voxel,	"box_center_y",	.5, .0, 1 		)
		bu:set_meter( false )
	bu = self:add_slider(	{11,iy,	6,dy},	"YRange",	bdd_voxel,	"box_range_y",	.5, .0, 1 		)
	iy = iy + dy
	bu = self:add_slider(	{5,iy,	6,dy},	"ZPos",		bdd_voxel,	"box_center_z",	.5, .0, 1 		)
		bu:set_meter( false )
	bu = self:add_slider(	{11,iy,	6,dy},	"ZRange",	bdd_voxel,	"box_range_z",	.5, .0, 1 		)

	iy = iy - 2 * dy
	bu = self:add_button(	{1,		iy,			dy,dy },	"Box Force",	bdd_voxel,	"box_force",	false ):set_text( "Force" )
	bu = self:add_button(	{1,		iy+dy,		dy,dy },	"Disable",		bdd_voxel,	"box_disable",	false )
	bu = self:add_button(	{1,		iy+dy*2,	dy,dy },	"Inverse",		bdd_voxel,	"box_inverse",	false )
	bu = self:add_button(	{1,		iy+dy*3, 	dy,dy },	"Black Back",	self,		"b_black", 		false )

	--	local vox = self.vox
	--bu = self:add_slider(	{1,6,	8,1},	"Radius",	vox,	"radius", 1 )
	--	bu:set_min_max( 0, 16. )
	--	bu:add_values_def( 1, 2.3 )

	--bu = self:add_button(	{1,	9 }, 	"play", self, "b_play", true )
	--	self.time = 0
	--bu = self:add_button( {1, 10 }, "Live", self, "b_live", false )
	--[[
	bu = self:add_selector(	{3.5,10,	5.5,2},	"mocap" )
		bu:set_nb( 8, 2 )
		bu:set_item_text_from_nb()
		bu:set_text_draw( false )
	--]]
	--iy = 6
	--bu = self:add_slider(	{1,iy,	8,1},	"h_step", geo, "h_step", .3 )
	--	bu:set_min_max( .2, .5 )
	--bu = self:add_slider(	{1,iy+1,	8,1},	"r_int",  geo, "r_int",  .2 )
	--	bu:set_min_max( .15, .5 )
	--bu = self:add_slider(	{1,iy+2,	8,1},	"r_dec",  geo, "r_dec",  .2 )
	--	bu:set_min_max( 0, 3 )

	--bu = self:add_button(	{1,	iy+5 }, 			"sym_4", geo, "b_sym_4", true )

	--bu = self:add_slider(	{1,iy+6,	8,1},	"d_y", geo, "d_y", .2 )
	--	bu:set_min_max( .2, 2. )
	local ix = 9
	local iy = 3
	--[[
	bu = self:add_slider(	{ix,iy,	8,1},	"dark",		self,	"dark",		.2 		)
	bu = self:add_slider(	{ix,iy+1,	8,1},	"Size",		vox,	"size_fac", 1 )
		bu:set_min_max( .1, 10 )
		bu:add_values_def( .25, .5, 1., 2. )
	bu = self:add_slider(	{ix,iy+2,	8,1},	"Size Int",	vox,	"size_int", 1 )
		bu:add_values_def( .25, .5, 1., 2. )
	--]]
	self:build()

	self.b_active_no = {}
	self.b_force = {}

	local boules = self.boules
	local ix = 1
	local f = 7.5/18
	local d = f*1.08
	local iy = 7- d*.5
	for i=1,#boules do
		local bou = boules[i]
		local x,y = bou[1]-1, bou[2]-1
		local ox = ix + f*x + d*.25
		local oy = iy + f*y
		bu = self:add_button( {ox+8, oy, d, d },	"force_"..i, self.b_force, i, false )		--, tab_or_ref, name, val )
			bu:set_text_draw( false )
		bu = self:add_button( {ox, oy, d, d },	"active_no_"..i, self.b_active_no, i, false )		--, tab_or_ref, name, val )
			bu:set_text_draw( false )
	end
	self:add_rgbf(	{1,15,	8,1},	"Force", bdd_voxel, "force_" )

--	bu = self:add_button(	{1,	14 }, 		"Inauguration",	geo,	"b_inaug",	false)
end

function meu:set_render_scale_z( v )		param.set( self.ref.voxel.scale_z, v )			end
function meu:set_out_use_alpha( b )		end --param.set( self.ref.voxel.out_use_alpha, b )	end
function meu:set_blend_mode( str )
	--aaa.print_method()
	param.set( self.ref.voxel.blend, str )
end
function meu:set_blend_mode_replace()			self:set_blend_mode( "REPLACE" )			end
function meu:set_blend_mode_over()				self:set_blend_mode( "OVER" )				end
function meu:set_blend_mode_add_using_alpha()	self:set_blend_mode( "ADD_USING_ALPHA" )	end
function meu:set_blend_mode_add()				self:set_blend_mode( "ADD" )				end
function meu:set_blend_mode_max()				self:set_blend_mode( "MAX" )				end

function meu:set_render_color_generic( prefix, r,g,b,a )
	local ref = self.ref
	if r then param.set( ref[prefix.."red"],	r ) end
	if g then param.set( ref[prefix.."green"],	g ) end
	if b then param.set( ref[prefix.."blue"],	b ) end
	--if  then param.set( ref[prefix.."grey"],	1 ) end
	if a then param.set( ref[prefix.."alpha"],	a ) end
end
function meu:set_render_color_offset( r,g,b,a )	self:set_render_color_generic( "render_color_offset_", r,g,b,a )	end
function meu:set_render_color_factor( r,g,b,a )	self:set_render_color_generic( "render_color_factor_", r,g,b,a )	end

function meu:init()
	local ref = self.ref

	local layers_ref = self:get_layers()

	local voxel = {}
	ref.voxel = voxel
	voxel.obj = aaa.obj.get_down_by_class( layers_ref, "bdd_voxel" )
		voxel.scale_z = param.get_ref( voxel.obj, "scale_z" )
		COLOR_REF.__build_rgbag_ref( ref, "render_color_offset_", voxel.obj, "render_color_offset_" )
		COLOR_REF.__build_rgbag_ref( ref, "render_color_factor_", voxel.obj, "render_color_factor_" )
		voxel.blend = param.get_ref( voxel.obj, "blend_mode" )
		voxel.out_use_alpha = param.get_ref( voxel.obj, "image_compute_use_alpha" )
		voxel.img_save_trig = param.get_ref( voxel.obj, "image_save_trig" )
		--voxel.b_sphere		= param.get_ref( voxel.obj, "led_as_sphere" )
	--aaa.box_warning( self.."\nJust got ref on bdd_voxel :\n" ..ref.voxel )
	ref.skel = aaa.obj.get( "skeleton_1" )

	ref.use_bdd = self:get_layer_ref_table(2).use_bdd
end

function meu:set_mood( mood )
	--aaa.print_method()
	local factor = 1
	local offset = 0
	if mood==1 then
		factor = 4
		offset = -1
	elseif mood==2 then
		factor = 8
		offset = -4
	end
	param.set( self.ref.render_color_offset_grey, offset )
	param.set( self.ref.render_color_factor_grey, factor )
end

function meu:update()
	local ref = self.ref
end

function meu:build_grid_version()
	local	geo		=	self.geo
	local	SY		=	geo.sy
	local	SYH		=	SY *.5
	local	re		=	geo.r_ext

	local	DY		=	geo.v_step
	local	DYH		=	DY * .5
	local	b_hexa	=	geo.b_hexa
	--local 	b_inaug	=	geo.b_inaug

	local	dy		=	DYH

	local	h_step	=	geo.h_step
	local	nb		=	geo.nb_forced
	nb = nb or math.floor( re*2. / h_step ) + 1
	self:print( h_step.." "..nb )
	local	sta		=	-((nb-1) * h_step) / 2

	local	base	=	self.base

	--local	dr		=	1/(r_nb)
	local	nbs = ""

	--self:print( "voxel ref is "..self.ref.voxel )
	aaa.bdd.set_lua_cur( self.ref.voxel.obj )
	aaa.bdd.erase_points()

	local	d2 = re*re
	local	nb_boule	=	0
	local	nb_ligne	=	0
	local	z = sta

	local boules = {}
	self.boules = boules
	--todo index in z first
	--self:print( "INAUG "..b_inaug )
	for iz = 1,nb do
		--if (not b_inaug) or inside( iz,8,11 ) then -- or
			local x = sta
			local tmp = nb_ligne
			for ix = 1,nb do
				--if (not b_inaug) or inside( iz,9,10 ) or inside( ix,4,15 ) then
					if ( x*x + z*z ) <= d2 then
						nb_ligne = nb_ligne + 1
						if b_hexa then
							dy = math.fmod( math.fmod( iz, 2 ) + math.fmod( ix, 2 ), 2) * DYH
						else
						end
						table.insert( base, { x=x, z=z, dy=dy } )
						--	line vertical
						local y = SYH+DY - dy
						local nby = 32
						local pixel_index = (iz-1)*nb+ix-1
						--self:print( nb_ligne.." : line "..pixel_index )
						aaa.bdd.add_point_line_y( x, y, z, -DY,	nby, pixel_index )
						nb_boule = nb_boule + nby
						table.insert( boules, {ix,iz} )
					end
				--end
				x = x + h_step
			end
			nbs = nbs.." "..(nb_ligne-tmp)
		--end
		z = z + h_step
	end
	--local layer = self.layer
	self.nb_ligne	=	nb_ligne
	self.nb_boule	=	nb_boule
	self.nbs		=	nbs
end

local function table_is_similar( tbl1, tbl2 )
	for k, v in pairs( tbl1 ) do
		if v ~= tbl2[k] then return false end
	end
	return true
end

function meu:build()
	local geo = self.geo
	geo.r_ext		=	2.38
	geo.sy			=	8
	geo.h_step		=	.27
	geo.nb_forced	=	18
	geo.v_step		=	.25
	geo.r_dec		=	0
	if not self.geo_last or not table_is_similar( self.geo, self.geo_last ) then
		self.base	=	{}
		self:print( "build()" )
		self:build_grid_version()
		self.geo_last = table.copy_shallow( self.geo )
	end
end

function meu:draw_lustre()
	--if true then return end
	local ref = self.ref
	self:draw_layers_begin()
		self:draw_layer(1)
		--	deal with simul
		param.set( ref.use_bdd, self.b_simul and "update_and_draw" or "update" )
		if self.b_simul then
			gol.color( .3 )
			aaa.draw_disk_axe_y( 0, 4.2, 0, 7, 48 )
		end

		self:draw_layer(2)
	--	if true then return end
		if self.b_image then
			self:draw_layer(3)
		end
	self:draw_layers_end()
end


function meu:render_in_lustre()
	local bdd	=	aaa.bdd
	if false then
		local s = .2
		local dst = 1
		s = .8
		bdd.render_op1(	"set_target",	dst						)
		bdd.render_op1(	"scale",		dst,		s, s, s		)
		vox:render_plane( 1,	-4, 4,	.5, .25,	5,	1, 0, 0 )
		vox:render_plane( 2,	-6, 6,	1, .5,		5,	0, 1, 0 )
		vox:render_plane( 3,	-4, 4,	.5, .25,	5,	0, 0, 1 )
	end
	for i=1,248 do
		bdd.set_line_y( i, not self.b_active_no[i], self.b_force[i] )
	end
end

function meu:draw()
	--self:print( "toto "..aaa.bdd.set_lua_cur( 4 ) )
	--self:print( "toto "..aaa.bdd.set_lua_cur( self.ref.voxel ) )

--	BUILD
	self:build()

	--	RENDER
	local bdd	=	aaa.bdd
	bdd.set_lua_cur( self.ref.voxel.obj )
	if self.b_black then
		bdd.render_op0(	"image_color_clear",		0, 0, 0	)
	else
		bdd.render_op0(	"image_color_clear",		0, .5, .5	)
	end
	bdd.begin_render()
		local s = .0
		local dst = 1
		bdd.render_op1(	"set_target",	dst	)
		--bdd.render_op1(	"fill", dst, 1,0, 1 )
		--bdd.render_op1(	"scale",		dst,		s, s, s		)
		bdd.render_op1(	"clear", dst )
		self:render_in_lustre()
		app:render_fox( self )
		bdd.render_op1(	"set_target",	dst	)
	bdd.end_render()

	if self.b_save then
		param.set( self.ref.voxel.img_save_trig, true )
	end
--	DRAW
	self:draw_lustre()
end
function meu:update_ui()
	local ui = self.ui
	ui.bu_info:set_text( self.nb_ligne.." lignes with "..self.nb_boule.." boules" )
	ui.bu_nbs:set_text( self.nbs )
end
