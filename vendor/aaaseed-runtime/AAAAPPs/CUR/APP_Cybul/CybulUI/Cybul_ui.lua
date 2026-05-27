
function CYBUL.draw_back_multi( bu )
	local self = app
	local stage = self.stage
	local bind
	if stage=="photo" then
		bind = self.photo.bind_out
	else
		bind = self.bind_rendered
	end
	bu:set_back_bind(bind)
	self.tab3d.bind = bind
	oo.getsuper( SLIDER_MULTI ).draw_back( bu )
end

function CYBUL:get_debug_point( pt )
	if pt then 
		return {pt[1] - 0.5, pt[2] - 0.5}
	else
		return pt
	end
end

function CYBUL:draw_debug_point( pt, size, remap )
	if pt then
		if remap then
			pt = self:get_debug_point( pt )
		end
		aaa.draw_rect_uv( pt[1] - size, pt[2] - size, pt[1] + size, pt[2] + size )
	end
end
function CYBUL:draw_debug_seg( pt1, pt2, remap )
	if pt1 and pt2 then
		if remap then
			pt1 = self:get_debug_point( pt1 )
			pt2 = self:get_debug_point( pt2 )
		end
		aaa.draw_line(	pt1[1],pt1[2], pt2[1],pt2[2] )
	end
end
function CYBUL:draw_3d_debug()
	gol.color_blue( .5 )

	-- show debug
	local pt
	local s = 0.01
	local t = app.tab3d
	self:draw_debug_point( t.P, s, false )
	self:draw_debug_point( t.Puv, s, false )
	self:draw_debug_point( t.Fu, s, false )
	self:draw_debug_point( t.Fv, s, false )

	self:draw_debug_point( t.vanishing_point_z, s, true )


	self:draw_debug_seg( t.Fu, t.Fv, false )
	self:draw_debug_seg( t.P,  t.Puv, false )

	gol.color_red( 1 )

	self:draw_debug_point( t.ext_wall_lt,  s, true )
	self:draw_debug_point( t.ext_wall_rt,  s, true )
	self:draw_debug_point( t.ext_wall_lh,  s, true )
	self:draw_debug_point( t.ext_wall_rh,  s, true )
	self:draw_debug_point( t.ext_floor_bl, s, true )
	self:draw_debug_point( t.ext_floor_br, s, true )

	self:draw_debug_seg( t.ext_wall_lt,  t.ext_wall_rt, true )
	self:draw_debug_seg( t.ext_wall_lt,  t.ext_wall_lh, true )
	self:draw_debug_seg( t.ext_wall_lh,  t.ext_wall_rh, true )
	self:draw_debug_seg( t.ext_wall_rh,  t.ext_wall_rt, true )
	self:draw_debug_seg( t.ext_wall_rh,  t.ext_floor_br, true )
	self:draw_debug_seg( t.ext_floor_bl, t.ext_floor_br, true )
	self:draw_debug_seg( t.ext_floor_bl, t.ext_wall_lh, true )
end

function CYBUL:set_grid_color()
	gol.color_orange( 0.5 )
end

function CYBUL:show_grid()
	local pts = app.tab3d.points
	local A = pts[1]
	local B = pts[2]
	local C = pts[3]
	local D = pts[4]
	local E = pts[5]
	local F = pts[6]

	local AB = V2.get_sub( B, A )
	local CA = V2.get_sub( A, C )
	local DB = V2.get_sub( B, D )
	local CD = V2.get_sub( D, C )

	local CE = V2.get_sub( E, C )
	local EF = V2.get_sub( F, E )
	local DF = V2.get_sub( F, D )

	local pt1 = {0, 0}
	local pt2 = {0, 0}

	if app.tab3d.grid_steps == 0 then app.tab3d.grid_steps = 1 end

	self:set_grid_color()

	local width = app.scene.wall_width
	local t = app.tab3d
	local x = t.grid_steps
	while x < width do
		V2.scale_add( pt1, AB, x / width, A )
		V2.scale_add( pt2, CD, x / width, C )
		self:draw_debug_seg( pt1, pt2, true )
		V2.scale_add( pt1, EF, x / width, E	)
		self:draw_debug_seg( pt1, pt2, true )
		x = x + t.grid_steps
	end

	local height = app.scene.wall_height
	local y = t.grid_steps
	while y < height do
		V2.scale_add( pt1, CA, y / height, C )
		V2.scale_add( pt2, DB, y / height, D )
		self:draw_debug_seg( pt1, pt2, true )
		y = y + t.grid_steps
	end

	local depth = app.scene.floor_depth
	local z = t.grid_steps
	while z < depth do
		V2.scale_add( pt1, CE, z / depth, C )
		V2.scale_add( pt2, DF, z / depth, D )
		self:draw_debug_seg( pt1, pt2, true )
		z = z + t.grid_steps
	end
end

function CYBUL:draw_photo_dots( bu )

	for i = 1,6 do
		local elt = bu:get_elt( i )
		elt:set_active( true )
	end

	gol.set_line_width( 2 )
	

	local changed = false

	gol.color_red( .75 )
	local xy = {}
	local s = .025
 	for i = 1,6 do
		local elt = bu:get_elt( i )
		local x,y = elt:get_xy()
		x,y = x-.5,y-.5
		xy[i] = {x,y}

		local prev = app.tab3d.points[i]
		if prev[1] ~= x + 0.5 or prev[2] ~= y + 0.5 then
			changed = true
		end

		app.tab3d.points[i] = {x + 0.5, y + 0.5}
		aaa.draw_plus_line(	x, y, s, s*16/9 )
 	end

	 local segs = { {1,2},{1,3},{2,4}, {3,4},{3,5},{4,6}, {5,6} }
	 for _,v in ipairs( segs ) do
		local xya = xy[v[1]]
		local xyb = xy[v[2]]

		local pts = {}
		local pt
		pt = self:get_intersection( xya, xyb, {-.5, -.5}, {-.5, .5})
		if #pts < 2 and pt[2] >= -.5 and pt[2] <= .5 then
			table.insert( pts, pt )
		end
		pt = self:get_intersection( xya, xyb, {.5, -.5}, {.5, .5})
		if #pts < 2 and pt[2] >= -.5 and pt[2] <= .5 then
			table.insert( pts, pt )
		end
		pt = self:get_intersection( xya, xyb, {-.5, -.5}, {.5, -.5})
		if #pts < 2 and pt[1] >= -.5 and pt[1] <= .5 then
			-- print( pt[1],pt[2] )
			table.insert( pts, pt )
		end
		pt = self:get_intersection( xya, xyb, {-.5, .5}, {.5, .5})
		if #pts < 2 and pt[1] >= -.5 and pt[1] <= .5 then
			table.insert( pts, pt )
		end

		if v[2] - v[1] == 1 or v[1] >= 3 then
			gol.set_line_width( 4 )
			gol.color_red( .375 )
			aaa.draw_line(	pts[1][1],pts[1][2], pts[2][1],pts[2][2] )
		end

		gol.set_line_width( 2 )
		gol.color_green( .75 )
		aaa.draw_line(	xya[1],xya[2], xyb[1],xyb[2] )
	end

	if app.tab3d.show_grid then
		self:show_grid()
	end
	if changed then
		self:update_perspective()
	end

	if app.tab3d.debug then
		self:draw_3d_debug()
	end
end

function CYBUL:draw_recadrage_dots( bu )
	for i = 7,10 do
		local elt = bu:get_elt( i )
		elt:set_active( true )
	end

	gol.set_line_width( 2 )
		gol.color_red( .75 )
	local xy = {}
	local s = .025
 	for i = 7,10 do
		 local elt = bu:get_elt( i )
		local x,y = elt:get_xy()
		x,y = x-.5,y-.5
		xy[i] = {x,y}

		aaa.draw_plus_line(	x, y, s, s*16/9 )
 	end

	local changed = false
	for i=7,10 do
		local prev_x = app.tab3d.points_recadrage[i - 6][1]
		local prev_y = app.tab3d.points_recadrage[i - 6][2]
		local x = xy[i][1]
		local y = xy[i][2]
		if x ~= prev_x or y ~= prev_y then
			-- keep the shape squared
			changed = true
			if i ==  7 then
				xy[ 8][2] = y
				xy[ 9][1] = x
			end
			if i ==  8 then
				xy[ 7][2] = y
				xy[10][1] = x
			end
			if i ==  9 then
				xy[ 7][1] = x
				xy[10][2] = y
			end
			if i == 10 then
				xy[ 8][1] = x
				xy[ 9][2] = y
			end

			for j=7,10 do
				bu:get_elt( j ):set_xy( xy[j][1]+.5, xy[j][2]+.5 )
				app.tab3d.points_recadrage[j-6] = {xy[j][1], xy[j][2]}
			end
		end
	end

	for i = 7,10 do
		local elt = bu:get_elt( i )
		local x,y = elt:get_xy()
	   	x,y = x-.5,y-.5
		   app.tab3d.points_recadrage[i - 6] = {x, y}
	end


	 local segs = { {7,8},{7,9},{8,10}, {9,10}}
	 for _,v in ipairs( segs ) do
		local xya = xy[v[1]]
		local xyb = xy[v[2]]
		gol.color_green( .75 )
		aaa.draw_line(	xya[1],xya[2], xyb[1],xyb[2] )
	end

	if changed then
		self:update_output_fbo_mapping()
	end
end

function CYBUL.draw_multi_fore( bu )
	for i = 1,bu:get_elt_nb() do
		local elt = bu:get_elt( i )
		elt:set_active( false )
		elt:set_text_visible( false )
	end

	if aaa.active_tab == "Photo" then
		app:draw_photo_dots( bu )
	end
	if aaa.active_tab == "Recadrage" then
		app:draw_recadrage_dots( bu )
	end
end

function CYBUL:load_image_bu( bu )
	local name = aaa.file.do_dialog_open( "Photo" )
	-- self:load_image( "C:\\__dev\\aaa\\test cybul\\immersion_allongéééééééééée.jpg" )
	self:load_image( name )
end

function CYBUL:change_stage( bu )
	local v = bu:get_value()
	local key = bu:get_item_text( v )
	self:print( "change_stage -> "..v.." "..key )
	aaa.active_tab = key
	self.stage = string.lower( key )
	self.__bus_ui_customer_for_tab:activate_bu_by_tab_key( key )

	local is_not_photo_tab = key ~= "Photo"
	for key,val in pairs( self.ui.bus_cam ) do
		val:set_visible( is_not_photo_tab )
	end

	self.draw_axe_in_stage = self.stage ~= "lumiere" and self.stage ~= "recadrage"
	local meus_3d = { "CybStore_1", "NdcPost_1", "LightPassV1_1", "Lights_1", "Materials_1" }
	local meu
	for i=1,#meus_3d do
		-- APP_GP:activate_meu(meus_3d[i].."_1", is_not_photo_tab)
		meu = app:get_meu_by_name_no_error(meus_3d[i])
		if meu then meu:set_mu_value( is_not_photo_tab )
		else print(meus_3d[i].." not found")
		end
	end
end

function CYBUL:use_grid_shortcuts( key )
	if 		key ==  "10cm" then
		self.tab3d.grid_steps = 100
	elseif  key ==  "50cm" then
		self.tab3d.grid_steps = 500
	elseif  key == "1m" then
		self.tab3d.grid_steps = 1000
	elseif  key ==  "2m" then
		self.tab3d.grid_steps = 2000
	end
end

function CYBUL:update_photo_zoom()
	local photo = self.photo
	if photo then
		local bind = photo.bind

		local sx,sy = aaa.img.get_size( bind )

		if sx ~= nil then
			local rx = sx/sy
			local z = self.tab3d.zoom * 0.01
			if rx >= 16/9 then
				-- substracting 0.001 to have a 1-pixel grey contour
				photo.sx = 1 * z
				photo.sy = photo.sx/rx
			else
				photo.sy = 9/16 * z
				photo.sx = photo.sy*rx
			end
		end
	end
end


function CYBUL:define_ui_photo( bus, name, x,y, SX,SY, M )
	local bu

	x = x + SX*.5

	bu = bus:add_slider( "photo_zoom", {x,y, SX,SY} )
	bu:set_text( "Zoom" )
		bu:set_min_max( 40, 160 )
		bu:set_target_lua( self.tab3d, "zoom" )
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )
		bu:set_method_on_value_change( self, "update_photo_zoom", bu )
		bu:set_value( 100 )
		bu:add_values_def( 60, 80, 100, 120, 140 )

	y = y - SY - M
	bu = bus:add_slider( "photo_hauteur", {x,y, SX,SY} )
		bu:set_text( "Hauteur" )
		bu:set_min_max( 1 * 1000, 10 * 1000 )
		-- bu:set_method_on_value_change( self, "post_change_scene_dimensions", "Height", bu )
		bu:set_target_lua( self.scene, "wall_height" )
		bu:set_value(2000)
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )

	y = y - SY - M
	bu = bus:add_slider( "photo_largeur", {x,y, SX,SY} )
		bu:set_text( "Largeur" )
		bu:set_min_max( 1 * 1000, 20 * 1000 )
		bu:set_method_on_value_change( self, "post_change_scene_dimensions", "Width", bu )
		bu:set_target_lua( self.scene, "wall_width" )
		bu:set_value(6000)
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )

	y = y - SY - M
	bu = bus:add_slider( "photo_profondeur", {x,y, SX,SY} )
		bu:set_text( "Profondeur" )
		bu:set_min_max( 1 * 1000, 20 * 1000 )
		bu:set_method_on_value_change( self, "post_change_scene_dimensions", "Depth", bu )
		bu:set_target_lua( self.scene, "floor_depth" )
		bu:set_value(2000)
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )

	y = y - SY - M
	y = y - SY - M
	bu = bus:add_button( "photo_rille", {x - SX * .4,y, SY,SY} )
		bu:set_text( "Grille" )
		bu:set_value( false )
		bu:set_target_lua( self.tab3d, "show_grid" )

	y = y - SY - M
	bu = bus:add_slider( "photo_grid_steps", {x,y, SX,SY} )
		bu:set_text( "Pas de la grille" )
		bu:set_min_max( 100, 2 * 1000 )
		bu:set_value( 1000 )
		bu:set_target_lua( self.tab3d, "grid_steps" )
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )
	y = y - SY - M
	bu = bus:add_trig( "10cm",	{x-3*SX/8,y, SX/4,SY} )
		bu:set_value( false )
		bu:set_method_on_click( self, "use_grid_shortcuts", "10cm" )
	bu = bus:add_trig( "50cm",	{x-SX/8,y, SX/4,SY} )
		bu:set_value( false )
		bu:set_method_on_click( self, "use_grid_shortcuts", "50cm" )
	bu = bus:add_trig( "1m",	{x+SX/8,y, SX/4,SY} )
		bu:set_value( false )
		bu:set_method_on_click( self, "use_grid_shortcuts", "1m" )
	bu = bus:add_trig( "2m", 	{x+3*SX/8,y, SX/4,SY} )
		bu:set_value( false )
		bu:set_method_on_click( self, "use_grid_shortcuts", "2m" )

	y = y - 3 * SY - M
	bu = bus:add_slider( "photo_contrast", {x,y, SX,SY} )
		bu:set_text( "Contraste" )
		bu:set_min_max( -1, 1 )
		bu:set_target_lua( self.photo_correction, "contrast" )
		bu:set_value(0)
		bu:set_min_max_strict( true )
	y = y - SY - M
	bu = bus:add_slider( "photo_brightness", {x,y, SX,SY} )
		bu:set_text( "Luminosite" )
		bu:set_min_max( -1, 1 )
		bu:set_target_lua( self.photo_correction, "brightness" )
		bu:set_value(0)
		bu:set_min_max_strict( true )
	y = y - SY - M
	bu = bus:add_slider( "photo_saturation", {x,y, SX,SY} )
		bu:set_text( "Saturation" )
		bu:set_min_max( -1, 1 )
		bu:set_target_lua( self.photo_correction, "saturation" )
		bu:set_value(0)
		bu:set_min_max_strict( true )
							
end

function CYBUL:change_cube_value( cube_id, name, bu )
	local val = bu:get_value() / 1000 -- mm to m
	if name == "center_z" then
		self:change_store_center( "center_z", val )
	elseif name == "center_x" then
		if cube_id == 1 then
			self.store.debut = bu:get_value()
		elseif cube_id == 2 then
			self.store.fin   = bu:get_value()
		end
		if self.store.debut > self.store.fin then
			local t = self.store.debut
			self.store.debut = self.store.fin
			self.store.fin = t
		end

		local dist = ( self.store.fin - self.store.debut ) * .001
		self:change_store_center( "center_x", ( self.store.debut + self.store.fin ) * .0005 )
		self:change_store_dimensions( "Width", dist )

		local bu_text = self.ui.bu_largeur_store
		if bu_text then
			bu_text:set_text( math.floor( dist * 1000 ) )
			bu_text:update()
		end
	end

	self:update_shadow_box()
end

function CYBUL:change_anchor_mode( bu )
	local v = bu:get_value()
	local key = bu:get_item_text( v )
	self.tab3d.anchor_mode = key
end

function CYBUL:change_store_type( bu )
	
	local v = bu:get_value()
	local key = bu:get_item_text( v )
	self.tab3d.store_type = key
		
	for k,val in pairs( self.ui.bus_banne ) do
		val:set_active( key == "Banne" )
	end
	for k,val in pairs( self.ui.bus_pergola ) do
		val:set_active( key == "Pergola Sol" )
	end
	for k,val in pairs( self.ui.bus_pergola_mur ) do
		val:set_active( key == "Pergola Mur" )
	end

	if self.tab3d.store_type == "Pergola Sol" then
		self:change_pergola_mode( self.ui.bu_pergola_type )
		self:change_pillar_count( self.ui.bu_pieds )
	end

end

function CYBUL:change_pillar_count( bu )
	local v = bu:get_value()
	local key = bu:get_item_text( v )
	if key:sub( 1, 1 ) == "4" then
		self.store.pillar_count = 4
	end
	if key:sub( 1, 1 ) == "6" then
		self.store.pillar_count = 6
	end

	local visible = self.tab3d.store_type == "Pergola Sol"

	for k, bu in pairs( self.ui.bus_zips_4 ) do
		bu:set_active( visible and self.store.pillar_count == 4 )
	end
	for k, bu in pairs( self.ui.bus_zips_6 ) do
		bu:set_active( visible and self.store.pillar_count == 6 )
	end
end


function CYBUL:change_pergola_mode( bu )
	local v = bu:get_value()
	local key = bu:get_item_text( v )
	self.tab3d.pergola_mode = key

	local visible = self.tab3d.store_type == "Pergola Sol"
	local lames = key == "Lames"
	local toile = key == "Toile"
	self.ui.bu_hessian_tex_text:set_active( visible and toile )
	self.ui.bu_hessian_tex:set_active( visible and toile )
	self.ui.bu_blades_tex_text:set_active( visible and lames )
	self.ui.bu_blades_tex:set_active( visible and lames )
	self.ui.bu_blades_angle:set_active( visible and lames )
	self.ui.bu_blades_retractation:set_active( visible and lames )
	self.ui.bu_blades_retractation_gauche:set_active( visible and lames )
	self.ui.bu_blades_retractation_droite:set_active( visible and lames )

end

function CYBUL:change_store_dimensions_bu( dim, bu )
	self:change_store_dimensions( dim, bu:get_value() / 1000 )
end

function CYBUL:change_store_center( name, val )
	local cybdraw = app:get_cybstores_meu()
	local store = cybdraw:get_layer_bdd( 6 )
	param.set( store, name, val )
end
function CYBUL:change_store_dimensions( dim, new_size )
	local cybdraw = app:get_cybstores_meu()
	local store = cybdraw:get_layer_bdd( 6 )
	local model = aaa.layer.get_model( cybdraw:get_layer( 6 ) )

	
	-- size_u = x
	-- size_v = y
	-- size_axe = z
	if model and store then
		param.set( model, "size_factor", 1 )
		if dim == "Height" then
			local size = param.get( model, "size_v" )
			param.set( model, "size_v", new_size )
			param.set( store, "center_y", param.get( store, "center_y" ) + ( new_size - size ) / 2 )
		elseif dim == "Width" then
			local size = param.get( model, "size_u" )
			-- stay centered
			param.set( model, "size_u", new_size )

		elseif dim == "Depth" then
			local size = param.get( model, "size_axe" )
			param.set( model, "size_axe", new_size )
			-- param.set( store, "center_z", param.get( store, "center_z" ) + ( new_size - size ) / 2 )
		end
	end

	self:update_hauteur_min()
	self:update_shadow_box()
end

function CYBUL:update_hauteur_min()
	local cybdraw = app:get_cybstores_meu()
	local store = cybdraw:get_layer_bdd( 6 )
	local model = aaa.layer.get_model( cybdraw:get_layer( 6 ) )

	local height = param.get( model, "size_v" )
	local depth  = param.get( model, "size_axe" )
	local angle  = self.store.hessian_angle / 180 * math.pi
	local hauteur_min  = height - depth * math.sin( angle )

	self.store.hauteur_min = hauteur_min * 1000

	if self.ui.bu_hauteur_min_banne and self.ui.bu_hauteur_min_pergola then 
		self.ui.bu_hauteur_min_banne:set_text( math.floor( hauteur_min * 1000 ) )
		self.ui.bu_hauteur_min_banne:update()
		self.ui.bu_hauteur_min_pergola:set_text( math.floor( hauteur_min * 1000 ) )
		self.ui.bu_hauteur_min_pergola:update()
	end
end

function CYBUL:change_hessian_pattern( bu )
	local i = 1
	local i_asked = bu:get_value()
	for key,val in pairs_sorted( self.tex_toiles ) do
		if i == i_asked then
			self.tab3d.tex_toile = val
			return
		end
		i = i + 1
	end
end
function CYBUL:change_hessian_pattern_dialog( bu, tex )
	if bu.__contact_nb == 1 then -- hack etienne to avoid double dialog.
		local name = aaa.file.do_dialog_open( "Motif" )
		if name then
			local img = IMGS.get_img( name )
			if tex == "Toile" then
				self.tab3d.tex_toile = img
				self:read_param_file( "hessian", name, img )
			end
			if tex == "Zips"  then
				self.tab3d.tex_zips  = img
				self:read_param_file( "zip_tex", name, img )
			end
			if tex == "lambrequin_lumineux"  then
				self.tab3d.tex_lambrequin_lumineux = img
			end
		end
	end
end

function CYBUL:draw_hessian_pattern_on_bu( name )
	local tex
	if name == "Toile" 				  then tex = self.tab3d.tex_toile 				end
	if name == "Zips"  				  then tex = self.tab3d.tex_zips  				end
	if name == "lambrequin_lumineux"  then tex = self.tab3d.tex_lambrequin_lumineux	end
	if tex then
		gol.set_wrap_2d_border()
		gol.set_texture_dim( 2 )
		gol.bind_texture( tex.bind )
		aaa.draw_rect_uv( -.5, -.5, .5, .5 )
		gol.set_texture_dim( 0 )
	end
end

function CYBUL:change_beams_tex( bu, store )
	local table
	local res_key
	local bu_text
	if store == "Banne" then
		table = self.tex_armatures.banne
		res_key = "banne_selected"
		bu_text = self.ui.bu_armature_banne
	end
	if store == "PergolaMur" then
		table = self.tex_armatures.pergola_mur
		res_key = "pergola_mur_selected"
		bu_text = self.ui.bu_armature_pergola_mur
	end
	if store == "Pergola" then
		table = self.tex_armatures.pergola_sol
		res_key = "pergola_selected"
		bu_text = self.ui.bu_armature_pergola
	end

	local i = 1
	local i_asked = bu:get_value()
	local res
	for key,val in pairs_sorted( table ) do
		if i == i_asked then
			res = val
		end
		i = i + 1
	end
	if res then
		local name = res:get_name()
		name = string.sub( name, 3, #name -4 )
		self.tex_armatures[res_key] = res
		bu_text:set_text( name )
		bu_text:update()
	end	
end
function CYBUL:draw_beams_tex_on_bu_selector( store )
	local table
	local count
	if store == "Banne"      then table = self.tex_armatures.banne      ; count = self.tex_armatures.banne_count       end
	if store == "PergolaMur" then table = self.tex_armatures.pergola_mur; count = self.tex_armatures.pergola_mur_count end
	if store == "Pergola"    then table = self.tex_armatures.pergola_sol; count = self.tex_armatures.pergola_sol_count end
	local i = 1
	for key, value in pairs_sorted( table ) do
		if value then
			gol.set_wrap_2d_border()
			gol.set_texture_dim( 2 )
			gol.bind_texture( value.bind )
			aaa.draw_rect_uv( -.5 + ( i - 1 ) / count, -.5, -.5 + i / count, .5 )
			gol.set_texture_dim( 0 )
		end
		i = i + 1
	end
end

function CYBUL:change_blades_tex( bu )
	local i = 1
	local i_asked = bu:get_value()
	for key,val in pairs_sorted( self.tex_lames ) do
		if i == i_asked then
			self.tab3d.tex_lames = val
		end
		i = i + 1
	end

	if self.tab3d.tex_lames then
		local name = self.tab3d.tex_lames:get_name()
		name = string.sub( name, 3, #name -4 )

		local bu_text = self.ui.bu_blades_tex_name
		bu_text:set_text( name )
		bu_text:update()
	end
end
function CYBUL.draw_blades_tex_on_bu()
	local tex = app.tab3d.tex_lames
		if tex then
		gol.set_wrap_2d_border()
		gol.set_texture_dim( 2 )
		gol.bind_texture( tex.bind )
		aaa.draw_rect_uv( -.5, -.5, .5, .5 )
		gol.set_texture_dim( 0 )
	end
end

function CYBUL.draw_blades_tex_on_bu_selector()
	local i = 1
	for key, value in pairs( app.tex_lames ) do
		if value then
			gol.set_wrap_2d_border()
			gol.set_texture_dim( 2 )
			gol.bind_texture( value.bind )
			aaa.draw_rect_uv( -.5 + ( i - 1 ) / app.tex_lames_count, -.5, -.5 + i / app.tex_lames_count, .5 )
			gol.set_texture_dim( 0 )
		end
		i = i + 1
	end
end


function CYBUL:change_blades_angle( bu )
	local val = bu:get_value()
	self.store.blade_angle = val * .25
end

function CYBUL:post_lambroquin_slider( bu )
	local max = bu:get_value()
	local b = self.ui.bu_lambrequin_lumineux_height
	-- allow logo to be taller than zip and "roll into" the bar.
	-- if b then
	-- 	b:set_min_max(0, max)
	-- 	if b:get_value() > max then
	-- 		app.lambrequin_lumineux.height = max
	-- 	end
	-- end
end

function CYBUL:change_lambrequin_lumineux_nombre( bu )
	--self.lambrequin_lumineux.count = bu:get_value()
	local v = bu:get_value()
	local key = bu:get_item_text( v )

	self.lambrequin_lumineux.count = v - 1

	local visible = (key ~= "Non")
	for k, bu in pairs( self.ui.bus_logo ) do
		bu:set_visible(visible)
	end
end

function CYBUL:define_ui_banne( bus, name, x,y, SX,SY, M )
	local bu

	bu = bus:add_text_info( "banne_armature_text", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Armature" )
		bu:set_text_color( "white" )
		table.insert( self.ui.bus_banne, bu )

	bu = bus:add_selector( "banne_armature_selector", {x + SX * .3, y, SX * .4, SY} )
		bu:set_nb( self.tex_armatures.banne_count, 1 )
		bu:set_text_draw( false )
		bu:set_method_on_value_change( self, "change_beams_tex", bu, "Banne" )
		bu:set_color_back( "white" )
		bu:set_value( 1 )
		bu.draw = function() self:draw_beams_tex_on_bu_selector( "Banne" ) end
		table.insert( self.ui.bus_banne, bu )
	local armature_bu = bu
		
		y = y - SY * .7 - M
		bu = bus:add_text_info( "banne_armature_text_info", { 		x + SX * .3, y, SX * .4, SY * .7 } )
		bu:set_text( "" )
		bu:set_text_color( "grey" )
		table.insert( self.ui.bus_banne, bu )
		self.ui.bu_armature_banne = bu
	armature_bu:set_value( 2 )
	armature_bu:set_value( 1 )
			
	y = y - SY - M
	bu = bus:add_slider( "banne_ouverture", {x,y, SX,SY} )
		bu:set_text( "Ouverture" )
		bu:set_min_max( 0, 1 )
		bu:set_target_lua( self.store, "hessian_opening" )
		bu:set_value( 1 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_banne, bu )

	y = y - SY - M
	bu = bus:add_slider( "banne_angle", {x,y, SX,SY} )
		bu:set_text( "Inclinaison" )
		bu:set_min_max( 0, 60 )
		bu:set_target_lua( self.store, "hessian_angle" )
		-- bu:set_value( 10 ) 
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_banne, bu )
		bu:set_method_on_value_change( self, "update_hauteur_min" )
		bu:set_value_type_integer( true )
	local angle_bu = bu

	y = y - SY - M
	bu = bus:add_text_info( "banne_hauteur_min_text", { 		x - SX * .25,		y,		SX * .5, SY	 } )
		bu:set_text( "Hauteur Min" )
		bu:set_text_color( "grey" )
		table.insert( self.ui.bus_banne, bu )
	bu = bus:add_text_info( "banne_hauteur_min_text_info", { 		x + SX * .3, y, SX * .4, SY } )
		bu:set_text( "0" )
		bu:set_text_color( "grey" )
		table.insert( self.ui.bus_banne, bu )
		self.ui.bu_hauteur_min_banne = bu
	angle_bu:set_value( 10 )

	y = y - SY - M
	bu = bus:add_text_info( "banne_toile_text", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Toile" )
		bu:set_text_color( "white" )
		table.insert( self.ui.bus_banne, bu )
	bu = bus:add_trig(  "banne_toile_trig", {x + SX * .3,y, SX * .4,SY} )
		bu:set_method_on_value_change( self, "change_hessian_pattern_dialog", bu, "Toile" )
		bu.draw_fore = function( bu ) self:draw_hessian_pattern_on_bu( "Toile" ) end
		bu:set_color_back( {1, 1, 1, 1} )
		table.insert( self.ui.bus_banne, bu )
		bu:set_text( "" )


	y = y - SY - M
	bu = bus:add_text_info( "banne_lambrequins", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Lambrequins" )
		bu:set_text_color( "white" )
		table.insert( self.ui.bus_banne, bu )
	bu = bus:add_trig(  "banne_lambroquins_trig", {x + SX*.3,y, SX*.4,SY} )
		bu:set_method_on_value_change( self, "change_hessian_pattern_dialog", bu, "Zips" )
		bu.draw_fore = function( bu ) self:draw_hessian_pattern_on_bu( "Zips" ) end
		bu:set_color_back( {1, 1, 1, 1} )
		table.insert( self.ui.bus_banne, bu )
		bu:set_text( "" )

	y = y - SY - M
	bu = bus:add_slider( "banne_lambroquins_slider", {x,y, SX,SY} )
		bu:set_text( "Lambrequins" )
		bu:set_min_max( 0, 5 * 1000 )
		bu:set_target_lua( self.zips, "length" )
		bu:set_value( 250 )
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )
		table.insert( self.ui.bus_banne, bu )
		bu:set_method_on_value_change(self, "post_lambroquin_slider", bu)
	y = y - SY - M
		bu = bus:add_slider( "banne_lambroquins_slider_opening", {x,y, SX,SY} )
		bu:set_text( "Ouverture" )
		bu:set_min_max( 0, 1 )
		bu:set_target_lua( self.zips, "opening" )
		bu:set_value( 1 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_banne, bu )
		bu:set_method_on_value_change(self, "post_lambroquin_slider", bu)
			
	-- y = y - SY - M
	-- bu = bus:add_text_info( "banne_lambroquins_lumineux", { 		x - SX * .25,		y,		SX * .4, SY	 } )
	-- 	bu:set_text( "Lambrequins lumineux" )
	-- 	bu:set_text_color( "white" )
	-- 	table.insert( self.ui.bus_banne, bu )
			
	y = y - SY - M
	bu = bus:add_button( "banne_lambrequin_transparent", {x - SX * .4,y, SY,SY} )
		bu:set_text( "Translucide" )
		bu:set_value( false )
		bu:set_target_lua( self.zips, "transparent" )
		table.insert( self.ui.bus_banne, bu )


	y = y - 1.5 * SY - M
	self.ui.bus_logo = {}
	bu = bus:add_text( "banne_lambrequin_lumineux_logo_nombre", { x - SX * .25, y, SX * .5, SY } )
		bu:set_text( "Logo" )
		table.insert( self.ui.bus_banne, bu )
	bu = bus:add_selector( "banne_lambrequin_lumineux_nombre_text_selector", {x + SX * .25, y, SX * .5, SY} )
		bu:set_nb( 3, 1 )
		bu:set_text_draw( false )
		bu:set_item_text( 1, "Non", "1", "2" )
		bu:set_value( 1 )
		bu:set_method_on_value_change( self, "change_lambrequin_lumineux_nombre", bu )
		bu:set_value( 1 )
		table.insert( self.ui.bus_banne, bu )

	-- y = y - SY - M
	-- bu = bus:add_button( "banne_lambrequin_lumineux", {x - SX * .4,y, SY,SY} )
	-- 	bu:set_text( "Logo" )
	-- 	bu:set_value( false )
	-- 	bu:set_target_lua( self.lambrequin_lumineux, "active" )
	-- 	table.insert( self.ui.bus_banne, bu )

	y = y - SY - M
	bu = bus:add_button( "banne_lambrequin_lumineux_lumineux", {x - SX * .4,y, SY,SY} )
		bu:set_text( "Lumineux" )
		bu:set_value( false )
		bu:set_target_lua( self.lambrequin_lumineux, "lumineux" )
		bu:set_visible( false )
		table.insert( self.ui.bus_banne, bu )
		table.insert( self.ui.bus_logo, bu )
	bu = bus:add_trig(  "banne_lambrequin_lumineux_trig", {x + SX * .3,y, SX * .4,SY} )
		bu:set_method_on_value_change( self, "change_hessian_pattern_dialog", bu, "lambrequin_lumineux" )
		bu.draw_fore = function( bu ) self:draw_hessian_pattern_on_bu( "lambrequin_lumineux" ) end
		bu:set_color_back( {1, 1, 1, 1} )
		table.insert( self.ui.bus_banne, bu )
		bu:set_text( "" )
		table.insert( self.ui.bus_logo, bu )
		bu:set_visible( false )

	y = y - SY - M
	bu = bus:add_slider( "banne_lambroquins_lumineux_height_slider", {x,y, SX,SY} )
		bu:set_text( "Hauteur" )
		bu:set_min_max( 0, 5 * 1000 )
		bu:set_target_lua( self.lambrequin_lumineux, "height" )
		bu:set_value( 250 )
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )
		table.insert( self.ui.bus_banne, bu )
		table.insert( self.ui.bus_logo, bu )
		bu:set_visible( false )
		self.ui.bu_lambrequin_lumineux_height = bu
	
	y = y - SY - M
	bu = bus:add_text( "banne_lambrequin_lumineux_logo_position_info", { x - SX * .25, y, SX * .5, SY } )
		bu:set_text( "Position" )
		table.insert( self.ui.bus_banne, bu )
		table.insert( self.ui.bus_logo, bu )
		bu:set_visible( false )

	bu = bus:add_slider( "banne_lambroquins_lumineux_position_slider", {x+SX*.125,y, SX*.25,SY} )
		bu:set_text( "Horizontal" )
		bu:set_min_max( -1, 1 )
		bu:set_target_lua( self.lambrequin_lumineux, "position_x" )
		bu:set_value( 0 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_banne, bu )
		table.insert( self.ui.bus_logo, bu )
		bu:set_visible( false )
	bu = bus:add_slider( "banne_lambroquins_lumineux_position_slider_y", {x+SX*.375,y, SX*.25,SY} )
		bu:set_text( "Vertical" )
		bu:set_min_max( 0, 1 )
		bu:set_target_lua( self.lambrequin_lumineux, "position_y" )
		bu:set_value( 0.5 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_banne, bu )
		table.insert( self.ui.bus_logo, bu )
		bu:set_visible( false )

	-- y = y - SY - M
	-- bu = bus:add_text( "banne_lambrequin_lumineux_nombre_text", { x - SX * .25, y, SX * .5, SY } )
	-- 	bu:set_text( "Nombre" )
	-- 	table.insert( self.ui.bus_banne, bu )
	-- bu = bus:add_selector( "banne_lambrequin_lumineux_nombre_text_selector", {x + SX * .25, y, SX * .5, SY} )
	-- 	bu:set_nb( 2, 1 )
	-- 	bu:set_text_draw( false )
	-- 	bu:set_item_text(1, "1", "2")
	-- 	bu:set_value( 1 )
	-- 	bu:set_method_on_value_change( self, "change_lambrequin_lumineux_nombre", bu )
	-- 	bu:set_value( 1 )
	-- 	table.insert( self.ui.bus_banne, bu )


	-- y = y - SY - M
	-- bu = bus:add_slider( "banne_lambroquins_lumineux_ratio_slider", {x,y, SX,SY} )
	-- 	bu:set_text( "Ratio" )
	-- 	bu:set_min_max( 0, 3 )
	-- 	bu:set_target_lua( self.lambrequin_lumineux, "ratio" )
	-- 	bu:set_value( 1 )
	-- 	bu:set_min_max_strict( true )
	-- 	table.insert( self.ui.bus_banne, bu )
end

function CYBUL:define_ui_pergola_mur( bus, name, x,y, SX,SY, M )
	local bu

	bu = bus:add_text_info( "pergola_mur_armature", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Armature" )
		bu:set_text_color ("white" )
		table.insert( self.ui.bus_pergola_mur, bu )
	bu = bus:add_selector( "pergola_mur_armature_selector", {x+SX*.3,y, SX* 4,SY} )
		bu:set_nb( self.tex_armatures.pergola_mur_count, 1 )
		bu:set_value( 1 )
		bu:set_text_draw( false )
		bu:set_method_on_value_change( self, "change_beams_tex", bu, "PergolaMur" )
		bu:set_color_back( {1, 1, 1, 1} )
		bu:set_value( 1 )
		bu.draw = function() self:draw_beams_tex_on_bu_selector( "PergolaMur" ) end
		table.insert( self.ui.bus_pergola_mur, bu )
	local armature_bu = bu
		
	y = y - SY * .7 - M
	bu = bus:add_text_info( "pergola_mur_armature_text_info", { 		x + SX * .3, y, SX * .4, SY * .7 } )
		bu:set_text( "" )
		bu:set_text_color( "grey" )
		table.insert( self.ui.bus_pergola_mur, bu )
		self.ui.bu_armature_pergola_mur = bu
	armature_bu:set_value( 2 )
	armature_bu:set_value( 1 )
			
	y = y - SY - M
	bu = bus:add_slider( "pergola_mur_ouverture", {x,y, SX,SY} )
		bu:set_text( "Ouverture" )
		bu:set_min_max( 0, 1 )
		bu:set_target_lua( self.store, "hessian_opening" )
		bu:set_value( 1 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_pergola_mur, bu )

	y = y - SY - M
	bu = bus:add_slider( "pergola_mur_angle", {x,y, SX,SY} )
		bu:set_text( "Inclinaison" )
		bu:set_min_max( 0, 60 )
		bu:set_target_lua( self.store, "hessian_angle" )
		-- bu:set_value( 10 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_pergola_mur, bu )
		bu:set_method_on_value_change( self, "update_hauteur_min" )
		bu:set_value_type_integer( true )
	local angle_bu = bu

	y = y - SY - M
	bu = bus:add_text_info( "pergola_mur_hauteur_min", { 		x - SX * .25,		y,		SX * .5, SY	 } )
		bu:set_text( "Hauteur Min" )
		bu:set_text_color( "grey" )
		table.insert( self.ui.bus_pergola_mur, bu )
	bu = bus:add_text_info( "pergola_mur_hauter_min_val", { 		x + SX * .3, y, SX * .4, SY } )
		bu:set_text( "0" )
		bu:set_text_color( "grey" )
		table.insert( self.ui.bus_pergola_mur, bu )
		self.ui.bu_hauteur_min_pergola = bu
	angle_bu:set_value( 10 )

	y = y - SY - M
	bu = bus:add_text_info( "pergola_mur_toile", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Toile" )
		bu:set_text_color( "white" )
		table.insert( self.ui.bus_pergola_mur, bu )
	bu = bus:add_trig(  "pergola_mur_toile_trig", {x + SX * .3,y, SX * .4,SY} )
		bu:set_method_on_value_change( self, "change_hessian_pattern_dialog", bu, "Toile" )
		bu.draw_fore = function( bu ) self:draw_hessian_pattern_on_bu( "Toile" ) end
		bu:set_color_back( {1, 1, 1, 1} )
		table.insert( self.ui.bus_pergola_mur, bu )
		bu:set_text( "" )

	y = y - SY - M
	bu = bus:add_text_info( "pergola_mur_lambrequins", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Lambrequins" )
		bu:set_text_color( "white" )
		table.insert( self.ui.bus_pergola_mur, bu )
	bu = bus:add_trig(  "pergola_mur_lambroquins_trig", {x + SX * .3,y, SX * .4,SY} )
		bu:set_method_on_value_change( self, "change_hessian_pattern_dialog", bu, "Zips" )
		bu.draw_fore = function( bu ) self:draw_hessian_pattern_on_bu( "Zips" ) end
		bu:set_color_back( {1, 1, 1, 1} )
		table.insert( self.ui.bus_pergola_mur, bu )
		bu:set_text( "" )

	
	y = y - SY - M
	bu = bus:add_slider( "pergola_mur_lambroquins_slider", {x,y, SX,SY} )
		bu:set_text( "Lambrequins" )
		bu:set_min_max( 0, 5 * 1000 )
		bu:set_target_lua( self.zips, "length" )
		bu:set_value( 250 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_pergola_mur, bu )
		bu:set_value_type_integer( true )

	y = y - SY - M
	bu = bus:add_button( "pergola_mur_lambrequin_transparent", {x-SX*.4,y, SY,SY} )
		bu:set_text( "Translucide" )
		bu:set_value( false )
		bu:set_target_lua( self.zips, "transparent" )
		table.insert( self.ui.bus_pergola_mur, bu )
	
	y = y - SY - M
	bu = bus:add_slider( "pergola_mur_deportage", {x,y, SX,SY} )
		bu:set_text( "Deportage" )
		bu:set_min_max( 0, 5 * 1000 )
		bu:set_target_lua( self.store, "deportage_mur" )
		bu:set_value( 0 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_pergola_mur, bu )
		bu:set_value_type_integer( true )

end

function CYBUL:set_pergola_opening_active( )
	local val = self.ui.bu_blades_retractation:get_value()
	local active = ( val > 0. )

	local bu_gauche = self.ui.bu_blades_retractation_gauche
	local bu_droite = self.ui.bu_blades_retractation_droite

	active = active and ( bu_gauche:get_value() or bu_droite:get_value() )

	if active then
		self.ui.bu_blades_angle:set_meter_color_named( "grey" )
		self.ui.bu_blades_angle:set_color_back( "grey" )
	else
		self.ui.bu_blades_angle:set_meter_color( .5, 1, 1, 1 )
		self.ui.bu_blades_angle:set_color_back( BU:get_color_back_named( "SLIDER" ) )
	end
end

function CYBUL:define_ui_pergola( bus, name, x,y, SX,SY, M )
	local bu

	bu = bus:add_text_info( "pergola_armature", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Armature" )
		bu:set_text_color( "white" )
		table.insert( self.ui.bus_pergola, bu )
	bu = bus:add_selector( "pergola_armature_selector", {x + SX * .3,y, SX * .4,SY} )
		bu:set_nb( self.tex_armatures.pergola_sol_count, 1 )
		bu:set_value( 1 )
		bu:set_text_draw( false )
		bu:set_method_on_value_change( self, "change_beams_tex", bu, "Pergola" )
		bu:set_color_back( {1, 1, 1, 1} )
		bu:set_value( 1 )
		bu.draw = function() self:draw_beams_tex_on_bu_selector( "Pergola" ) end
		table.insert( self.ui.bus_pergola, bu )
	local armature_bu = bu

	y = y - SY * .7 - M
	bu = bus:add_text_info( "pergola_armature_text_info", { 		x + SX * .3, y, SX * .4, SY * .7 } )
		bu:set_text( "" )
		bu:set_text_color( "grey" )
		table.insert( self.ui.bus_pergola, bu )
		self.ui.bu_armature_pergola = bu

	armature_bu:set_value( 2 )
	armature_bu:set_value( 1 )

	y = y - SY - M
	bu = bus:add_slider( "pergola_ecart_mur_pergola", {x,y, SX,SY} )
	bu:set_text( "Ecart mur-pergola" )
	bu:set_min_max( 0, 10 * 1000 )
	bu:set_method_on_value_change( self, "change_cube_value", 1, "center_z", bu )
	bu:set_value( 1 * 1000 )
	bu:set_min_max_strict( true )
	table.insert( self.ui.bus_pergola, bu )
	bu:set_value_type_integer( true )

	y = y - SY - M
	bu = bus:add_selector( "pergola_pieds", {x,y, SX,SY} )
		bu:set_nb( 2, 1 )
		bu:set_value( 1 )
		bu:set_item_text( 1, "4 pieds", "6 pieds" )
		bu:set_text_draw( false )
		bu:set_method_on_value_change( self, "change_pillar_count", bu )
		table.insert( self.ui.bus_pergola, bu )
		self.ui.bu_pieds = bu
	
	y = y - SY - M
	bu = bus:add_selector( "pergola_type", {x,y, SX,SY} )
		bu:set_nb( 2, 1 )
		bu:set_value( 1 )
		bu:set_item_text( 1, "Toile", "Lames" )
		bu:set_text_draw( false )
		bu:set_method_on_value_change( self, "change_pergola_mode", bu )
		table.insert( self.ui.bus_pergola, bu )
		self.ui.bu_pergola_type = bu

	y = y - SY - M
	bu = bus:add_text_info( "pergola_lames", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Lames" )
		bu:set_text_color( "white" )
		self.ui.bu_blades_tex_text = bu
		bu:set_active( false ) -- Toile by default
		table.insert( self.ui.bus_pergola, bu )

	bu = bus:add_selector( "pergola_lames_selector", {x + SX * .3, y, SX * .4, SY} )
		bu:set_nb( self.tex_lames_count, 1 )
		bu:set_value( 1 )
		bu:set_text_draw( false )
		bu:set_method_on_value_change( self, "change_blades_tex", bu )
		bu:set_active( false ) -- Toile by default
		bu:set_color_back( {1, 1, 1, 1} )
		bu:set_value( 1 )
		self.ui.bu_blades_tex = bu
		table.insert( self.ui.bus_pergola, bu )
		bu.draw = self.draw_blades_tex_on_bu_selector
		
	bu = bus:add_text_info( "pergola_toile", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Toile" )
		bu:set_text_color( "white" )
		table.insert( self.ui.bus_pergola, bu )
		self.ui.bu_hessian_tex_text = bu
	bu = bus:add_trig(  "pergola_toile_trig", {x + SX * .3,y, SX * .4,SY} )
		bu:set_method_on_value_change( self, "change_hessian_pattern_dialog", bu, "Toile" )
		bu.draw_fore = function( bu ) self:draw_hessian_pattern_on_bu( "Toile" ) end
		bu:set_color_back( {1, 1, 1, 1} )
		table.insert( self.ui.bus_pergola, bu )
		self.ui.bu_hessian_tex = bu
		bu:set_text( "" )

	y = y - SY * .7 - M
	bu = bus:add_text_info( "pergola_toile_text_info", { 		x + SX * .3, y, SX * .4, SY * .7 } )
		bu:set_text( "" )
		bu:set_text_color( "grey" )
		table.insert( self.ui.bus_pergola, bu )
		self.ui.bu_blades_tex_name = bu
	
	self.ui.bu_blades_tex:set_value( 2 )
	self.ui.bu_blades_tex:set_value( 1 )

	y = y - SY - M
	bu = bus:add_slider( "pergola_retractation", {x-SX*.25,y, SX*.5,SY } )
		bu:set_text( "Retractation" )
		bu:set_min_max( 0, 1 )
		bu:set_target_lua( self.store, "retractation" )
		bu:set_value( 0 )
		bu:set_min_max_strict( true )
		bu:set_method_on_value_change( self, "set_pergola_opening_active", bu)
		table.insert( self.ui.bus_pergola, bu )
		self.ui.bu_blades_retractation = bu

	bu = bus:add_button( "pergola_gauche", {x+SX*.125,y, SX*.25,SY} )
		bu:set_text( "Gauche" )
		bu:set_value( true )
		bu:set_target_lua( self.store, "lames_retract_gauche" )
		bu:set_method_on_value_change( self, "set_pergola_opening_active", bu)
		table.insert( self.ui.bus_pergola, bu )
		self.ui.bu_blades_retractation_gauche = bu
	bu = bus:add_button( "pergola_droite", {x+SX*.375,y, SX*.25,SY} )
		bu:set_text( "Droite" )
		bu:set_value( true )
		bu:set_target_lua( self.store, "lames_retract_droite" )
		bu:set_method_on_value_change( self, "set_pergola_opening_active", bu)
		table.insert( self.ui.bus_pergola, bu )
		self.ui.bu_blades_retractation_droite = bu

	y = y - SY - M
	bu = bus:add_slider( "pergola_ouverture", {x,y, SX,SY} )
		bu:set_text( "Ouverture" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( self, "change_blades_angle", bu )
		bu:set_min_max_strict( true )
		bu:set_value( 1 )
		bu:set_active( false ) -- Toile by default
		self.ui.bu_blades_angle = bu
		table.insert( self.ui.bus_pergola, bu )

	y = y - SY - M
	bu = bus:add_text_info( "pergola_zips_text_info", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Zips" )
		bu:set_text_color( "white" )
		table.insert( self.ui.bus_pergola, bu )
	bu = bus:add_trig(  "pergola_zips_trig", {x+SX*.3,y, SX*.4,SY} )
		bu:set_method_on_value_change( self, "change_hessian_pattern_dialog", bu, "Zips" )
		bu.draw_fore = function( bu ) self:draw_hessian_pattern_on_bu( "Zips" ) end
		bu:set_color_back( {1, 1, 1, 1} )
		table.insert( self.ui.bus_pergola, bu )
		bu:set_text( "" )

	y = y - SY - M
	bu = bus:add_button( "pergola_lambrequin_transparent", {x-SX*.3,y, SX*.4,SY} )
		bu:set_text( "Translucide" )
		bu:set_value( false )
		bu:set_target_lua( self.zips, "transparent" )
		table.insert( self.ui.bus_pergola, bu )
	
	-- y = y - SY - M
	bu = bus:add_slider( "pergola_zips_slider", {x+.2*SX,y, SX*.6,SY} )
		bu:set_text( "Zips" )
		bu:set_min_max( 0, 5 * 1000 )
		bu:set_target_lua( self.zips, "length" )
		bu:set_value( 0 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_pergola, bu )
		bu:set_value_type_integer( true )

	y = y - SY - M
	local start_x = x - SX*.5 + SX*.125
	local size_x = SX * .25
	self.ui.bus_zips_4 = {}
	bu = bus:add_button( "pergola_zips_gauche", {start_x,y, size_x,SY} )
		bu:set_text( "Gauche" )
		bu:set_value( true )
		bu:set_target_lua( self.zips, "left" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_4, bu )
		self.ui.bu_gauche = bu
	bu = bus:add_button( "pergola_zips_devant", {start_x+size_x*2,y, size_x,SY} )
		bu:set_text( "Devant" )
		bu:set_value( true )
		bu:set_target_lua( self.zips, "forward" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_4, bu )
		self.ui.bu_devant = bu
	bu = bus:add_button( "pergola_zips_derriere", {start_x+size_x*3,y, size_x,SY} )
		bu:set_text( "Derriere" )
		bu:set_value( false )
		bu:set_target_lua( self.zips, "wall" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_4, bu )
		self.ui.bu_derriere = bu
	bu = bus:add_button( "pergola_zips_droite", {start_x+size_x,y, size_x,SY} )
		bu:set_text( "Droite" )
		bu:set_value( true )
		bu:set_target_lua( self.zips, "right" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_4, bu )
		self.ui.bu_droite = bu

	start_x = x - SX*.5 + SX*.0833
	size_x = SX * .1666
	self.ui.bus_zips_6 = {}
	bu = bus:add_button( "pergola_zips_gauche_6", {start_x,y, size_x,SY} )
		bu:set_text( "Gauche" )
		bu:set_value( true )
		bu:set_target_lua( self.zips, "left" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_6, bu )
	bu = bus:add_button( "pergola_zips_dev1_6", {start_x+size_x*2,y, size_x,SY} )
		bu:set_text( "Dev 1" )
		bu:set_value( true )
		bu:set_target_lua( self.zips, "forward" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_6, bu )
	bu = bus:add_button( "pergola_zips_dev2_6", {start_x+size_x*3,y, size_x,SY} )
		bu:set_text( "Dev 2" )
		bu:set_value( true )
		bu:set_target_lua( self.zips, "forward_2" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_6, bu )
	bu = bus:add_button( "pergola_zips_der1_6", {start_x+size_x*4,y, size_x,SY} )
		bu:set_text( "Der 1" )
		bu:set_value( false )
		bu:set_target_lua( self.zips, "wall" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_6, bu )
		bu:set_active( false )
	bu = bus:add_button( "pergola_zips_der2_6", {start_x+size_x*5,y, size_x,SY} )
		bu:set_text( "Der 2" )
		bu:set_value( false )
		bu:set_target_lua( self.zips, "wall_2" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_6, bu )
		bu:set_active( false )
	bu = bus:add_button( "pergola_zips_droite_6", {start_x+size_x,y, size_x,SY} )
		bu:set_value( true )
		bu:set_target_lua( self.zips, "right" )
		table.insert( self.ui.bus_pergola, bu )
		table.insert( self.ui.bus_zips_6, bu )
		bu:set_text( "Droite" )

	y = y - SY - M
	bu = bus:add_slider( "pergola_deportage", {x,y, SX,SY} )
		bu:set_text( "Deportage" )
		bu:set_min_max( 0, 5 * 1000 )
		bu:set_target_lua( self.store, "deportage" )
		bu:set_value( 0 )
		bu:set_min_max_strict( true )
		table.insert( self.ui.bus_pergola, bu )
		bu:set_value_type_integer( true )
end

function CYBUL:define_ui_cybstores( bus, name, x,y, SX,SY, M )
	local bu

	x = x + SX*.5

	bu = bus:add_selector( "cybstores_store", {x,y, SX,SY} )
		bu:set_nb( 3, 1 )
		bu:set_value( 1 )
		bu:set_item_text( 1, "Banne", "Pergola Mur", "Pergola Sol" )
		bu:set_text_draw( false )
		bu:set_method_on_value_change( self, "change_store_type", bu )
		self.ui.bu_store_selector = bu
	
	y = y - SY - M
	bu = bus:add_slider( "cybstores_hauteur", {x,y, SX,SY} )
		bu:set_text( "Hauteur" )
		bu:set_min_max( 0, 5 * 1000 )
		bu:set_method_on_value_change( self, "change_store_dimensions_bu", "Height", bu )
		bu:set_value(3 * 1000)
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )


	y = y - SY - M
	bu = bus:add_slider( "cybstores_debut", {x,y, SX,SY} )
		bu:set_text( "Gauche" )
		bu:set_target_lua( self.store, "debut" )
		bu:set_min_max( -20 * 1000, 20 * 1000 )
		bu:set_method_on_value_change( self, "change_cube_value", 1, "center_x", bu )
		-- bu:set_value(-2 * 1000)
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )
	local bu_gauche = bu

	y = y - SY - M
	bu = bus:add_slider( "cybstores_fin", {x,y, SX,SY} )
		bu:set_text( "Droite" )
		bu:set_target_lua( self.store, "fin" )
		bu:set_min_max( -20 * 1000, 20 * 1000 )
		bu:set_method_on_value_change( self, "change_cube_value", 2, "center_x", bu )
		-- bu:set_value(2 * 1000)
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )
	local bu_droite = bu

	y = y - SY - M
	bu = bus:add_text_info( "cybstores_largeur", { 		x - SX * .25,		y,		SX * .5, SY	 } )
		bu:set_text( "Largeur" )
		bu:set_text_color( "grey" )
	bu = bus:add_text_info( "cybstores_largeur_text_info", { 		x + SX * .3, y, SX * .4, SY } )
		bu:set_text( "0" )
		bu:set_text_color( "grey" )
		self.ui.bu_largeur_store = bu
	
	bu_gauche:set_value( -2 * 1000 )
	bu_droite:set_value(  2 * 1000 )

	y = y - SY - M
	bu = bus:add_slider( "cybstores_profondeur", {x,y, SX,SY} )
		bu:set_text( "Profondeur" )
		bu:set_min_max( 0, 10 * 1000 )
		bu:set_method_on_value_change( self, "change_store_dimensions_bu", "Depth", bu )
		bu:set_value(4 * 1000)
		bu:set_min_max_strict( true )
		bu:set_value_type_integer( true )
	
	y = y - SY - M
	self:define_ui_pergola( bus, name, x,y, SX,SY, M )
	self:define_ui_banne( bus, name, x,y, SX,SY, M )
	self:define_ui_pergola_mur( bus, name, x,y, SX,SY, M )
	
end

function CYBUL:change_light_parameter( name, bu )
	local lights = self:get_meu_by_name( "NdcPost_1" )
	if lights then
		if name == "chaleur" then
			lights:set_bu_value( "Kelvin", 10000 * ( 1 - bu:get_value() ) )
		end
		if name == "exposition" then
			lights:set_bu_value( "Exposure", 4 * bu:get_value() )
		end
		-- for key, value in pairs( lights.ui.bu_cam_sel ) do
		-- 	print( key, " -- ", value )
		-- end
	end
end

function CYBUL:change_light_position( name, bu )
	if name == "longitude" then
		self.tab3d.sun_long = 180 * bu:get_value()
	end
	if name == "latitude" then
		self.tab3d.sun_lat = 85 * (.2 + .8 * bu:get_value()) -- not 90 to avoid gimbal lock
	end

	local light = aaa.lights.get_light( 1 )
	if light then
		local lat  = ( 90-self.tab3d.sun_lat )  / 180 * math.pi
		local long = self.tab3d.sun_long / 180 * math.pi

		param.set( light, "Position_x", math.sin( lat ) * math.sin( long ) )
		param.set( light, "Position_z", math.sin( lat ) * math.cos( long ) )
		param.set( light, "Position_y", math.cos( lat ) )

		-- param.set( light, "Position_x",  self.light.x * 1000 )
		-- param.set( light, "Position_y",  self.light.y * 1000 )
		-- param.set( light, "Position_z", -self.light.z * 1000 )

		self:update_shadow_box()
	end
end

function CYBUL:update_shadow_box()
	local center_x = 1000 * app.scene.scene_center_x
	local width    = 1000 * app.scene.ext_wall_width
	local wall_width    = app.scene.wall_width

	local light = aaa.lights.get_light( 1 )
	if light then
		local x_min = math.min( center_x - wall_width * .5, self.store.debut ) * .001 - 1
		local x_max = math.max( center_x + wall_width * .5, self.store.fin   ) * .001 + 1
		param.set(light, 'shadow_bbox_min_x', x_min)
		param.set(light, 'shadow_bbox_max_x', x_max)

		param.set(light, 'shadow_bbox_min_y', -.001)
		param.set(light, 'shadow_bbox_max_y', self.scene.wall_height * .001 + .001)

		local cybdraw = app:get_cybstores_meu()
		local store = cybdraw:get_layer_bdd( 6 )
		local model = aaa.layer.get_model( cybdraw:get_layer( 6 ) )

		if model and store then
			local z_max = param.get( model, "size_axe" )			
			param.set(light, 'shadow_bbox_min_z', -.001)
			param.set(light, 'shadow_bbox_max_z', z_max)
		end
	end
end

function CYBUL:change_light_filter( bu )
	local light = aaa.lights.get_light( 1 )
	if light then
		param.set( light, "shadow_filter_width", ( 1 - bu:get_value() ) * 0.1 )
	end
end


function CYBUL:change_light_pass_parameter( name, bu )
	local bu_value = bu:get_value()
	local lights = self:get_meu_by_name( "LightPassV1_1" )
	if lights then
		if name == "intensity" then
			lights:set_bu_value( "Light_Factor", bu_value * 0.5 )
		elseif name == "shadow_strength" then
			local bu_shadow = lights.ui.bu_shadow
			local r,g,b,a = bu_shadow:get_rgba()
			local f = bu_shadow:get_grey()
			local n = bu_value
			if n == 0 then n = 0.001 end
			if f == 0 then f = 0.001 end
			r = r / f
			g = g / f
			b = b / f
			bu_shadow:set_rgbf( r, g, b, n )
		elseif name == "tint" then
			local bu_shadow = lights.ui.bu_shadow
			local r,g,b,a
			local f = bu_shadow:get_grey()
			local n = bu_value

			-- red - yellow - white gradient
			r = 1
			g = math.min( 1.5 * n, 1 )
			b = 2 * math.max( 0, n - 0.5 )

			bu_shadow:set_rgbf( r, g, b, f )

			local light = aaa.lights.get_light( 1 )
			param.set( light, "Diffuse_red", r )
			param.set( light, "Diffuse_green", g )
			param.set( light, "Diffuse_blue", b )
		elseif name == "night_mode" then
			local sha = lights:get_shading()
			sha:set_frag_int(3, app.light.night_mode and 1 or 0)
		end
	end
end

function CYBUL:draw_light_intensity( bu )
	bu:__draw_slider()

	local g = 0
	local lights = self:get_meu_by_name( "LightPassV1_1" )
	if lights then
		g = lights:get_bu_value( "Light_Factor" )
	end

	-- -- print( r, g, b )
	gol.color3( g,g,g )
	aaa.draw_rect( -.5, -.5, .5, -.38 )	
end

function CYBUL:draw_light_tint( bu )
	bu:__draw_slider()

	local light = aaa.lights.get_light( 1 )
	local r,g,b
	r = param.get( light, "Diffuse_red" )
	g = param.get( light, "Diffuse_green" )
	b = param.get( light, "Diffuse_blue" )
	
	-- print( r, g, b )
	gol.color3( r, g, b )
	aaa.draw_rect( -.5, -.5, .5, -.38 )	
end

function CYBUL:draw_shadow_strength( bu )
	bu:__draw_slider()
	local lights = self:get_meu_by_name( "LightPassV1_1" )
	local bu_shadow = lights.ui.bu_shadow
	local light = aaa.lights.get_light( 1 )
	local r,g,b,a = bu_shadow:get_rgba()
	local f = bu_shadow:get_grey()
	r = r
	g = g
	b = b
	-- print( r, g, b )
	gol.color3( r, g, b )
	aaa.draw_rect( -.5, -.5, .5, -.38 )
end

function CYBUL:change_compass( bu )
	local fac = bu:get_elt( 1 )
	local sun = bu:get_elt( 2 )

	local bus = {}
	bus["angle_facade"] = fac
	bus["angle_sun"]    = sun

	for name,pos in pairs(bus) do
		local x,y = pos:get_xy()
		x = x - .5
		y = y - .5
		local rad = math.atan2( y, x )

		if name == "angle_facade" then
			local n = math.max(V2.norm({x, y}), 0.01)
			local r = 0.35 / n
			x = x * r
			y = y * r
			fac:set_xy(x + .5, y + .5)
		end

		if self.light[name] ~= rad then
			self.light[name] = rad
			local angle = self.light.angle_sun - self.light.angle_facade
			if angle > math.pi then
				angle = angle - 2 * math.pi
			end
			if angle < - math.pi then
				angle = angle + 2 * math.pi
			end
			self.tab3d.sun_long = angle / math.pi * 180

			if name == "angle_sun" then
				local dst = 1.5 * V2.norm({x, y})
				self.tab3d.sun_lat = (1 - dst) * 90
			end

			self:change_light_position()
		end
	end
end

function CYBUL:draw_compass( bu )
	self:change_compass( bu )

	local fac = bu:get_elt( 1 )
	local sun = bu:get_elt( 2 )

	local function draw_bu() end
	fac.draw = draw_bu
	sun.draw = draw_bu

	fac:set_visible( true )
	sun:set_visible( true )
	fac:set_active( true )
	sun:set_active( true )

	local function extend_in_rect(xya, xyb)
		local pts = {}
		local pt
		pt = self:get_intersection( xya, xyb, {-.5, -.5}, {-.5, .5})
		if #pts < 2 and pt[2] >= -.5 and pt[2] <= .5 then
			table.insert( pts, pt )
		end
		pt = self:get_intersection( xya, xyb, {.5, -.5}, {.5, .5})
		if #pts < 2 and pt[2] >= -.5 and pt[2] <= .5 then
			table.insert( pts, pt )
		end
		pt = self:get_intersection( xya, xyb, {-.5, -.5}, {.5, -.5})
		if #pts < 2 and pt[1] >= -.5 and pt[1] <= .5 then
			-- print( pt[1],pt[2] )
			table.insert( pts, pt )
		end
		pt = self:get_intersection( xya, xyb, {-.5, .5}, {.5, .5})
		if #pts < 2 and pt[1] >= -.5 and pt[1] <= .5 then
			table.insert( pts, pt )
		end

		-- keep order
		if V2.dot(V2.get_sub(xya, xyb), V2.get_sub(pts[1], pts[2])) > 0 then
			return pts[1], pts[2]
		else
			return pts[2], pts[1]
		end
	end

	-- draw fac
	if fac then
		local x, y = fac:get_xy()
		x = x - .5
		y = y - .5
		gol.color( .5 )
		aaa.draw_disk_axe_z( x,y,0, 0.1 )

	end
	-- draw sun
	if sun then
		local x, y = sun:get_xy()
		x = x - .5
		y = y - .5
		gol.color_yellow()
		aaa.draw_disk_axe_z( x,y,0, 0.1 )

		local l = .08
		for i=1,12 do
			local angle = math.pi / 6 * i
			aaa.draw_line(x, y, x + l * math.cos(angle), y + l * math.sin(angle))
		end
	end

	-- draw axes
	for i=1,16 do
		local angle = math.pi / 8 * i
		local l = .45
		
		gol.color_white( 0.15 )
		aaa.draw_line(0, 0, l * math.cos(angle), l * math.sin(angle))
		-- local a, b = extend_in_rect( {0, 0}, {math.cos(angle), math.sin(angle)} )
		-- aaa.draw_line(a[1], a[2], b[1], b[2])
	end

	-- draw line for facade
	if true then
		local facade = self.light.angle_facade
		local xya = {0, 0}
		local xyb = {math.cos(facade), math.sin(facade)}

		local x, y = fac:get_xy()
		x = x - .5
		y = y - .5
		local n = V2.norm({x, y})
		if n ~= 0 then 
			x = x * .35 / n
			y = y * .35 / n
			gol.color_white()
			aaa.draw_line(0, 0, x, y)
		end
	end

	-- draw facade
	if true then
		local facade = self.light.angle_facade + math.pi * .5
		local xya = { math.cos(facade), 		  math.sin(facade)			 }
		local xyb = { math.cos(facade + math.pi), math.sin(facade + math.pi) }
		
		xya, xyb = extend_in_rect(xya, xyb)
		local vec = V2.get_sub(xyb, xya)

		local function angle_sign(pt)
			local dot = V2.dot(vec, V2.get_sub(pt, xya))
			local cro = vec[1] * pt[2] - vec[2] * pt[1]
			return math.atan2(cro, dot)
		end
		
		local function angle_sign_order(pt1, pt2)
			local a1, a2 = angle_sign(pt1), angle_sign(pt2)
			return a1 < a2
		end
		local rect = { {-.5, -.5}, {-.5, .5}, {.5, -.5}, {.5, .5} }
		table.sort(rect, angle_sign_order)

		gol.color_white( 0.4 )
		local re1, re2 = rect[1], rect[2]
		gol.draw_triangles_2d(xya[1], xya[2],    xyb[1], xyb[2],    re1[1], re1[2])
		gol.draw_triangles_2d(xyb[1], xyb[2],    re1[1], re1[2],    re2[1], re2[2])

		gol.color_white()
		aaa.draw_line(xya[1], xya[2], xyb[1], xyb[2])
	end
end

function CYBUL:define_ui_lumiere( bus, name, x,y, SX,SY, M )
	local bu

	x = x + SX*.5

	local function add( text, ... )
		local bu = bus:add_text_info( "lumiere_boussole_facade_text_info_"..text, 		...	 )
			bu:set_text( text )
			bu:set_text_color( "white" )
		return bu
	end

	bu = bus:add_text_info( "lumiere_boussole_facade_text_info", { 		x,		y,		SX, SY	 } )
		bu:set_text( "Orientation Facade" )
		bu:set_text_color( "white" )
	y = y - 4 * SY - M
	add( "N", 	{x,				y + SY * 2.6,		SY * .7, SY * .7}	 )
	add( "S", 	{x,				y - SY * 2.6,		SY * .7, SY * .7}	 )
	add( "O", 	{x - SY * 2.6,	y,					SY * .7, SY * .7}	 )
	add( "E", 	{x + SY * 2.6,	y,					SY * .7, SY * .7}	 )
	add( "NO", 	{x - SY * 2.6,	y + SY * 2.6,		SY * .7, SY * .7}	 )
	add( "NE", 	{x + SY * 2.6,	y + SY * 2.6,		SY * .7, SY * .7}	 )
	add( "SO", 	{x - SY * 2.6,	y - SY * 2.6,		SY * .7, SY * .7}	 )
	add( "SE", 	{x + SY * 2.6,	y - SY * 2.6,		SY * .7, SY * .7}	 )
	
	bu = bus:add_slider_multi( "lumiere_boussole_facade", {x,y, 6*SY,6*SY}, 2 )
		bu:set_mobile( false )
		bu:set_text( "" )
		bu:set_min_max( -1, 1 )
		bu:set_elt_sxy( .15, .15 )
		bu.draw_back = function( bu ) self:draw_compass( bu ) end	
		bu:set_min_max_strict( true )
		bu:set_target_lua( self.light, "x" )
	self.ui_bu_compass = bu
	bu:get_elt( 1 ):set_xy( .25, .5  )
	bu:get_elt( 2 ):set_xy( .5 , .25 )
	
		
	y = y - 4 * SY - M	
	bu = bus:add_text_info( "lumiere_lumiere_text_info", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Lumiere" )
		bu:set_text_color( "white" )
	

	y = y - SY - M
	bu = bus:add_slider( "lumiere_intensite", {x,y, SX,SY} )
		bu:set_text( "Intensite" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( self, "change_light_pass_parameter", "intensity", bu )
		bu:set_value( 0.5 )
		bu:set_min_max_strict( true )
		bu.draw = function( bu ) self:draw_light_intensity( bu ) end


	y = y - SY - M
	bu = bus:add_slider( "lumiere_teinte", {x,y, SX,SY} )
		bu:set_text( "Couleur" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( self, "change_light_pass_parameter", "tint", bu )
		bu:set_value( 1 )
		bu:set_min_max_strict( true )
		bu.draw = function( bu ) self:draw_light_tint( bu ) end
		-- bu:set_color_back( {1, 1, 1, 1} )

	y = y - SY - M
	y = y - SY - M
	bu = bus:add_text_info( "lumiere_ombre_text_info", { 		x - SX * .25,		y,		SX * .4, SY	 } )
		bu:set_text( "Ombre" )
		bu:set_text_color( "white" )

	y = y - SY - M
	bu = bus:add_slider( "lumiere_ombre_force", {x,y, SX,SY} )
		bu:set_text( "Couleur" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( self, "change_light_pass_parameter", "shadow_strength", bu )
		bu:set_value( 1 )
		bu:set_min_max_strict( true )
		bu.draw = function( bu ) self:draw_shadow_strength( bu ) end 
	
	y = y - SY - M
	bu = bus:add_slider( "lumiere_ombre_durete", {x,y, SX,SY} )
		bu:set_text( "Durete" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( self, "change_light_filter", bu )
		bu:set_value( 0 )
		bu:set_min_max_strict( true )

	y = y - SY - M
	y = y - SY - M
	bu = bus:add_button( "lumiere_night_mode", {x - SX * .4,y, SY,SY} )
		bu:set_text( "Mode Nuit" )
		bu:set_value( false )
		bu:set_target_lua( self.light, "night_mode" )
		bu:set_method_on_value_change( self, "change_light_pass_parameter", "night_mode", bu )

end

function CYBUL:remove_camera_axes()
	local gbuf = self:get_meu_by_name( "Fbo_GBuffer" )	
	if gbuf then
		local bu_cam = gbuf.ui.bu_cam_sel
		bu_cam:set_value( 16 )
		local cam = gbuf:get_camera( 15 )
		if cam then
			param.set( cam, "draw_axe", 0 )
		end
    end
end

function CYBUL:change_camera_value( name, val )
	local gbuf = self:get_meu_by_name( "Fbo_GBuffer" )	
	if gbuf then
		local bu_cam = gbuf.ui.bu_cam_sel
		bu_cam:set_value( 16 )

		local cam = gbuf:get_camera( 15 )
		if cam then
			if name == "tra_z" then val = -val end
			
			param.set( cam, name, val )

			param.set( cam, "tra_x", 0 )
			param.set( cam, "tra_y", 0 )
		end
    end
end
function CYBUL:change_camera_value_bu( name, bu )
	self:change_camera_value( name, bu:get_value() )
end

function CYBUL:move_camera( name )
	local gbuf = self:get_meu_by_name( "Fbo_GBuffer" )	
	if gbuf then
		local bu_cam = gbuf.ui.bu_cam_sel
		bu_cam:set_value( 16 )

		local cam = gbuf:get_camera( 15 )
		if cam then
			if name == "reset" then
				param.set( cam, "rot_center_x", self.scene.scene_center_x )
				param.set( cam, "rot_center_y", self.scene.wall_height * .0005 )
				param.set( cam, "rot_center_z", 0 )

				param.set( cam, "rot_x", .1 )
				param.set( cam, "rot_y", 0 )
				param.set( cam, "rot_z", 0 )

				param.set( cam, "rot_x", .1 )
				param.set( cam, "rot_y", 0 )
				param.set( cam, "rot_z", 0 )

				param.set( cam, "tra_x", 0 )
				param.set( cam, "tra_y", 0 )
				param.set( cam, "tra_x", 0 )

				param.set( cam, "tra_z", -self.scene.wall_width * .001 * 2 )
				param.set( cam, "focal", 60 )

				self.ui.bus_cam["distance"]:set_value(self.scene.wall_width * .001 * 2)
		
			else
				local val
				local param_name
				local delta
				if name == "left" or name == "right" then
					param_name = "rot_y"
					delta = .01
				end
				if name == "up" or name == "down" then
					param_name = "rot_x"
					delta = .01
				end
				if name == "near" or name == "far" then
					param_name = "tra_z"
					delta = .3
				end
				if name == "tra_left" or name == "tra_right" then
					param_name = "tra_x"
					delta = 1
				end
				if name == "tra_up" or name == "tra_down" then
					param_name = "tra_y"
					delta = 1
				end

				val = param.get( cam, param_name )

				if name == "right" or name == "down" or name == "near" or name == "tra_right" or name == "tra_down" then
					val = val - delta
				end
				if name == "left" or name == "up" or name == "far" or name == "tra_left" or name == "tra_up" then
					val = val + delta
				end

				param.set( cam, param_name, val )
			end
		end
	end
end

function CYBUL:define_ui_recadrage( bus, name, x,y, SX,SY, M )
	local bu

	x = x + SX*.5

	bu = bus:add_slider( "recadrage_chaleur", {x,y, SX,SY} )
		bu:set_text( "Chaleur" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( self, "change_light_parameter", "chaleur", bu )
		bu:set_value( 0 )
		bu:set_min_max_strict( true )

	y = y - SY - M
	bu = bus:add_slider( "recadrage_exposition", {x,y, SX,SY} )
		bu:set_text( "Exposition" )
		bu:set_min_max( 0, 1 )
		bu:set_method_on_value_change( self, "change_light_parameter", "exposition", bu )
		bu:set_value( 0 )
		bu:set_min_max_strict( true )
end

function CYBUL:__define_ui_tab( bus_root, name, ox,oy, sx,sy )
--	self:box_debug( name )
	local bu
	local bus = BUS:create( "CYBUL_UI_"..name )
	bus:set_active( true )
	-- bus:set_text_visible( false )
	local M = .05	-- marge/ecart entre button
	local SX,SY = 1.5-M, .165	-- size buuron by define_ui
	local M = SY/8		
	bus:init_begin()

		-- quick code to see the coor system
		if false then
			for i=1,3 do
				local v = i-2
				bu = bus:add_button( "test "..i, {v,v, .1,.1} )
			end
		end
		local str = "define_ui_"..string.lower( name )
		local fn = self[ str ]
		if fn then
			fn( self, bus, name, 0, -SY*.5, SX,SY, M )
		else
			self:box_debug( "No method ".."define_ui_"..name  ) 
		end

	bus:init_end()

	local s = 3
	bu = bus_root:add_window( name, bus, {ox,oy, sx,sy} )
		bu:set_mobile( false )
		bu:set_text_visible( false )
	bus:set_transfo( sx,sy, .75,-sy*.5 )

	return bus
end


function CYBUL:add_bu_after( bu )
	-- if not self.tab3d then
	-- 	self:init_global_table()
	-- end
	if self.__b_ui_customer then 
		--bu:set_pos_load_save( false )
		bu:set_text_nice()
		if bu.set_show_value then
			bu:set_show_value( true )
		end
		bu:disable_uif()

		table.insert( self.ui.bus, bu )
	end

	if bu.set_dialog_simple then
		bu:set_dialog_simple( true )
	end

	return bu
end

function CYBUL:add_app_buttons( bus )
	local ui = self.ui

	local bu		
	local MY = .1
	local yt =  2.25 - MY
	
	local MX = .1
	local SX = .5
	local SY = .3

	-- top row
	local x = -4 - .02
	local y = yt - SY*.5
	bu = bus:add_trig( "Charger\n  image", {x + SX,y, SX * 1.5,SY} ):set_color_back( "save" )
		--bus:add_bu( bu )
		bu:set_method_on_click( self, "load_image_bu", bu )
		bu:set_text_xy_fxy( 0.05, .46, 1.1, .5 )

	ui.bus_cam = {}

	-- Camera center
	x = x + 2.25 * SX
	bu = bus:add_trig( "<", {x,y, SY+.01,SY} ):set_color_back( "focus" )
		bu:set_method_on_click( self, "move_camera", "tra_right" )
	table.insert( ui.bus_cam, bu )

	x = x + SY + .065
	bu = bus:add_trig( "^", {x,y+SY*.25, SY+.1,SY*.5} ):set_color_back( "focus" )
		bu:set_method_on_click( self, "move_camera", "tra_up" )
	table.insert( ui.bus_cam, bu )

	bu = bus:add_trig( "v", {x,y-SY*.25, SY+.1,SY*.5} ):set_color_back( "focus" )
		bu:set_method_on_click( self, "move_camera", "tra_down" )
	table.insert( ui.bus_cam, bu )

	x = x + SY + .065
	bu = bus:add_trig( ">", {x,y, SY+.01,SY} ):set_color_back( "focus" )
		bu:set_method_on_click( self, "move_camera", "tra_left" )
	table.insert( ui.bus_cam, bu )

	-- camera dist
	x = x + 2.5 * SY
	bu = bus:add_slider( "Distance", {x,y, 2*SX,SY} ):set_color_back( "focus" )
		bu:set_min_max( 1, 50 )
		bu:set_method_on_value_change( self, "change_camera_value_bu", "tra_z", bu )
		bu:set_value( 10 )
		bu:set_min_max_strict( true )
	table.insert( ui.bus_cam, bu )
	ui.bus_cam["distance"] = bu

	x = x + 2.2 * SX
	bu = bus:add_slider( "Focale", {x,y, 2*SX,SY} ):set_color_back( "focus" )
		bu:set_min_max( 5, 150 )
		bu:set_method_on_value_change( self, "change_camera_value_bu", "focal", bu )
		bu:set_value(60)
		bu:set_min_max_strict( true )
	table.insert( ui.bus_cam, bu )
	ui.bus_cam["focale"] = bu

	-- Camera pos
	x = x + 1.5 * SX
	bu = bus:add_trig( "<", {x,y, SY+.01,SY} ):set_color_back( "focus" )
		bu:set_method_on_click( self, "move_camera", "left" )
	table.insert( ui.bus_cam, bu )

	x = x + SY + .065
	bu = bus:add_trig( "^", {x,y+SY*.25, SY+.1,SY*.5} ):set_color_back( "focus" )
		bu:set_method_on_click( self, "move_camera", "up" )
	table.insert( ui.bus_cam, bu )

	bu = bus:add_trig( "v", {x,y-SY*.25, SY+.1,SY*.5} ):set_color_back( "focus" )
		bu:set_method_on_click( self, "move_camera", "down" )
	table.insert( ui.bus_cam, bu )

	x = x + SY + .065
	bu = bus:add_trig( ">", {x,y, SY+.01,SY} ):set_color_back( "focus" )
		bu:set_method_on_click( self, "move_camera", "right" )
	table.insert( ui.bus_cam, bu )
	
	x = x + SY + .17
	bu = bus:add_trig( "Reset", {x,y, 1.5*SY,SY} ):set_color_back( "focus" )
		bu:set_method_on_click( self, "move_camera", "reset" )
		bu:set_text_xy_fxy( 1/32, .25, .8*1.2, .5*1.2 )

	table.insert( ui.bus_cam, bu )

	-- x = x + SY + .11
	-- bu = bus:add_trig( "+", {x,y, SY+.1,SY} )
	-- 	bu:set_method_on_click( self, "move_camera", "near" )
	-- x = x + SY + .1
	-- bu = bus:add_trig( "-", {x,y, SY+.1,SY} )
	-- 	bu:set_method_on_click( self, "move_camera", "far" )
		
		
	-- bottom row
	x =  -4 + MX + SX/2
	bu = bus:add_trig( "Sauver", {x,-y, SX,SY} ):set_color_back( "save" )
		--bus:add_bu( bu )
		bu:set_method_on_click( self, "save_image" )
		bu:set_text_xy_fxy( 1/32, .25, .8*1.2, .5*1.2 )
	local sy = .4
	local xl = 4 - SX*3 - MY + SX*3*.5

	x = x + SX * .8
	bu = bus:add_selector( "Format",	{x,-y, SX/2,SY} )
		bu:set_nb( 1,2 )
		bu:set_value( 1 )
		bu:set_item_text( 1, "png", "jpg" )
		bu:set_text_draw( false )
		self.ui.bu_image_format = bu

	x = x + SX + MX
	bu = bus:add_trig( "     Sauver\nconfiguration", {x,-y, SX * 1.5,SY} ):set_color_back( "save" )
		--bus:add_bu( bu )
		bu:set_method_on_click( self, "save_process" )
		bu:set_text_align_x( "center" )
		bu:set_text_xy_fxy( 0.05, .46, 1.1, .5 )

	x = x + SX/2 + MX + SX
	bu = bus:add_trig( "    Charger\nconfiguration", {x,-y, SX * 1.5,SY} ):set_color_back( "save" )
		--bus:add_bu( bu )
		bu:set_method_on_click( self, "load_process", bu )
		bu:set_text_xy_fxy( 0.05, .46, 1.1, .5 )

	x = x + SX/2 + MX + SX
	bu = bus:add_trig( "    Copier\ninformations", {x,-y, SX * 1.5,SY} ):set_color_back( "save" )
		--bus:add_bu( bu )
		bu:set_method_on_click( self, "export_pergola_text", bu )
		bu:set_text_xy_fxy( 0.04, .46, 1.1, .5 )
	
	x = x + SX/2 + MX + SX
	bu = bus:add_trig( "Reinitialiser\n     projet    ", {x,-y, SX * 1.5,SY} ):set_color_back( "save" )
		--bus:add_bu( bu )
		bu:set_method_on_click( self, "reset_project", bu )
		bu:set_text_xy_fxy( 0.12, .46, 1.2, .5 )
		
	x = x + SX/2 + MX + SX * .8
	bu = bus:add_trig( "Sauver\n projet", {x,-y, SX * 1.1,SY} ):set_color_back( "save" )
		--bus:add_bu( bu )
		bu:set_method_on_click( self, "save_project", bu )
		bu:set_text_xy_fxy( 0.04, .46, 1.1, .5 )
	
	x = x + SX/2 + MX + SX * .8
	bu = bus:add_button( "Grille", {x - SX*.2,-y, SX,SY} )
		bu:set_value( false )
		bu:set_target_lua( self.tab3d, "show_grid" )
		bu:set_text_xy_fxy( 0, .25, .8*1.2, .5*1.2 )

	-- right bus
	local tabs = { "Photo", 	"CybStores",		"Lumiere",	"Recadrage" }
	bu = bus:add_selector( "Stage", {xl,yt-sy*.5, SX*3,sy} )
		bu:set_nb( 2,2 )
		bu:set_value( 2 )
		bu:set_item_text( 1, unpack( tabs ) )
		bu:set_text_draw( false )
		bu:set_method_on_value_change( self, "change_stage", bu )
		self.ui.bu_tab = bu
		--todo this is ugly
		self.__bus_ui_customer_for_tab = bus
	y = yt - sy - MY

	local sy_used = sy + MY
	local syw = 2 * yt - sy_used
	for i=1,#tabs do
		local name = tabs[i]
		bus:set_tab_key( name )
		self:__define_ui_tab( bus, name, xl, -sy_used*.5, SX * 3, syw )
	end

	bus:set_tab_key()
end

function CYBUL:add_dots( bus )
	local ISX = 6.15
	local ISY = ISX/16*9
		
	local MX = .1

	local bu
	bu = bus:add_slider_multi( "Dots", {-4+ISX*.5+MX,0, ISX,ISY},	6 )
		bu:set_mobile( false )
		bu:set_text_visible( false )
		-- 	bu:set_elt_text_xy_f_ratio( -.5, .0, .4 )
		-- 	bu:set_select_on_click_double()
		bu.draw_back = self.draw_back_multi
		bu.draw_fore = self.draw_multi_fore
	self.ui_bu_dots = bu

	local pts = {}
	for i=1,6 do
		table.insert( pts, self.tab3d.points[i] )
	end
	for i=1,4 do
		table.insert( pts, self.tab3d.points_recadrage[i] )
	end
	bu:set_values( pts )

	bu:set_elt_sxy(.02, .02 * 16/9)
end

function CYBUL:define_ui_customer()
	-- tries to create a ui cam, doesn't work.
	-- local gbuf = self:get_meu_by_name( "Fbo_GBuffer" )	
	-- if gbuf then
	-- 	local bu_cam = gbuf.ui.bu_cam_sel
	-- 	gbuf.ui.bu_cam_sel:set_value( 16 )
	-- end

	-- self:load_media()
	--	local s_text_type_last = BU:__get_text_type()
--	BU:set_text_nice()

	--should done in ga with also a protected call
	self.__b_ui_customer = true

	local bus = BUS:create( "cybul_ui_root" )

	bus:set_bu_pos_load_save( false )

	bus:set_active( true )
	bus:init_begin_add_to_ga()
		self:add_app_buttons( bus )
		self:add_dots( bus )
	bus:init_end()

--	BU:__set_text_type( s_text_type_last )

	self.ui.bu_tab:set_value( 1 )

	self.__b_ui_customer = nil

	self:print( "UI customer defined" )

	-- self:load_preset()

	-- demo
	self.ui.bu_tab:set_value( 1 )

	self:change_store_type( self.ui.bu_store_selector )

	local t = self.tab3d
	local pts = t.points
	t.vanishing_point_x		= self:get_intersection( pts[1], pts[2], pts[3], pts[4] )
	t.vanishing_point_y		= self:get_intersection( pts[1], pts[3], pts[2], pts[4] )
	t.vanishing_point_z		= self:get_intersection( pts[3], pts[5], pts[4], pts[6] )
	t.vanishing_point_x_bot = self:get_intersection( pts[3], pts[4], pts[5], pts[6] )
	self:extend_scene()
	self:update_ext_scene_dimensions()

	self:remove_camera_axes()
	return bus
end
