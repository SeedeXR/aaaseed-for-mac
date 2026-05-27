function meu:define_meu_infos( )
	return { author = "Abdalight", date = "2024",
			   tags = { "2d", "3D", "Art", "Draw", "Generator", "Geometry", "Procedural" },
			   help = "Using the curve from the bdd to draw a ribbon with attributes" }
end

if CLASS.DECLARE( "STROKE", nil ) then
	STROKE:set_class_status_doc(   CLASS.STATUS.GABU,  "STROKE group all compute and drawing function to do ribbon on Curve"  )
end

STROKE.doc.create = "( ref_bdd, curve_id ) create a new stroke using the curve from the bdd"
function STROKE:create( ref_bdd, curve_id )
	self = STROKE:create_instance( "Stroke_"..curve_id )
	self.ref_bdd  = ref_bdd
	self.curve_id = curve_id
	self:init()
	return self
end

STROKE.doc.init = "() init the stroke"
function STROKE:init()
	self.uv          = {}
	self.ribbon      = {}
	self.curve       = {}
	self.id_smooth   = 5
	self.ph_uv       = 0
	self.ph_begin    = 0
	self.ph_end      = 1
	self.thick       = 1
	self.uv_speed    = 0
	self.texture_id  = 1
	self.length      = 0
	self.point_nb    = 0
	self.color       = { 1, 1, 1, 1 }
	self.b_recompute = false
end

STROKE.doc.update = "( b_move ) update the stroke"
function STROKE:update( b_move )
	if b_move then
		self.ph_uv = self.ph_uv + aaa.time.dt * self.uv_speed
		self:get_and_set_length()
		self:compute_uv()
	end

	if self.b_recompute then
		self:compute_stroke()
	end
end

STROKE.doc.compute_uv = "() comppute the uv for the ribbon from the attributes of the stroke"
function STROKE:compute_uv()
	local uv = {}

	local point_begin = math.floor( self.length * self.ph_begin )
	local point_end   = math.floor( self.length * self.ph_end * 100 )
	local s_fac       = 0.05
	--local s_fac       = 0.15
	
	local id = 1
	local s  = s_fac * point_begin

	for i = point_begin, point_end do
		uv[id],   uv[id+1] = s + self.ph_uv, 1
		uv[id+2], uv[id+3] = s + self.ph_uv, 0
		s = s + s_fac
		id = id + 4
	end
	self.uv = uv
end

STROKE.doc.set_phase = "( ph_begin, ph_end ) set the phase of the stroke"
function STROKE:set_phase( ph_begin, ph_end )
	self.ph_begin = ph_begin
	self.ph_end   = ph_end
end

STROKE.doc.compute_ribbon = "() compute the ribbon using attributes of the stroke"
function STROKE:compute_ribbon()
	local ribbon      = {}
	local ph_begin    = self.ph_begin
	local ph_end      = self.ph_end
	local fn          = aaa.bdd.coor_to_world
	local curve_id    = self.curve_id
	local thick       = self.thick
	local bdd         = self.ref_bdd
	local f           = 0.2
	local s_fac       = f / self.length
	local ease_len    = 5 * s_fac
	local point_begin = math.floor( self.length / f * ph_begin )
	local point_end   = math.floor( self.length / f * ph_end   )
	local id, u_var   = 1, 0
	local s           = s_fac * point_begin

	for i = point_begin, point_end + 1 do
		if     s - ph_begin <=   ease_len then u_var = ( s - ph_begin ) / ease_len
		elseif s - ph_end   <= - ease_len then u_var = 1
		else                                   u_var = ( ph_end - s )   / ease_len
		end

		ribbon[id],   ribbon[id+1], ribbon[id+2] = fn( bdd , curve_id, s,   u_var * thick,     0 )
		ribbon[id+3], ribbon[id+4], ribbon[id+5] = fn( bdd , curve_id, s,  -u_var * thick,     0 )
		s = s + s_fac
		id = id + 6
	end
	self.ribbon = ribbon
	self:recompute_ribbon_with_gaussian_smoothing()
end

STROKE.doc.recompute_ribbon_with_gaussian_smoothing  = "recompute_ribbon_with_gaussian_smoothing ; gauss = exp( - d²/(2*sigma²)), sigma = id_smooth / 2"
function STROKE:recompute_ribbon_with_gaussian_smoothing()
	local ribbon          = self.ribbon
	local len             = #ribbon / 6
	local id_smooth       = self.id_smooth
	local smoothed_ribbon = {}
	local smooth_value

	local function gaussian_weight( distance, id_smooth )
		return math.exp( - ( 2 * distance * distance ) / ( id_smooth * id_smooth ) )
	end

	local function smooth_point( i, id_smooth )
		local weighted_sum_x,  weighted_sum_y,  weighted_sum_z  = 0, 0, 0
		local weighted_sum_x2, weighted_sum_y2, weighted_sum_z2 = 0, 0, 0
		local total_weight = 0
		local distance, weight, idx

		for j = i - id_smooth, i + id_smooth do
			distance = math.abs( i - j )
			weight   = gaussian_weight( distance, id_smooth )
			idx      = ( clamp( j, 1, len ) - 1 ) * 6

			weighted_sum_x  = weighted_sum_x  + ribbon[idx + 1] * weight
			weighted_sum_y  = weighted_sum_y  + ribbon[idx + 2] * weight
			weighted_sum_z  = weighted_sum_z  + ribbon[idx + 3] * weight
			weighted_sum_x2 = weighted_sum_x2 + ribbon[idx + 4] * weight
			weighted_sum_y2 = weighted_sum_y2 + ribbon[idx + 5] * weight
			weighted_sum_z2 = weighted_sum_z2 + ribbon[idx + 6] * weight

			total_weight = total_weight + weight
		end

		local id = #smoothed_ribbon
		smoothed_ribbon[id + 1] = weighted_sum_x  / total_weight
		smoothed_ribbon[id + 2] = weighted_sum_y  / total_weight
		smoothed_ribbon[id + 3] = weighted_sum_z  / total_weight
		smoothed_ribbon[id + 4] = weighted_sum_x2 / total_weight
		smoothed_ribbon[id + 5] = weighted_sum_y2 / total_weight
		smoothed_ribbon[id + 6] = weighted_sum_z2 / total_weight
	end
   
	for i = 1, len do
		smooth_value = math.min( i - 1, id_smooth, len - i )     --symetrie à ajouter
		smooth_point( i, smooth_value )
	end
	self.ribbon = smoothed_ribbon
end

STROKE.doc.draw_curve  = "() draw only the curve"
function STROKE:draw_curve()
	if self.curve      then gol.draw_line_strip_3d( self.curve )         end
end

STROKE.doc.draw_ribbon = "() draw the ribbon using attributes of the stroke"
function STROKE:draw_ribbon()
	if self.ribbon     then
		if self.color then
			gol.color( self.color )
		end
		if self.texture_id then
			gol.set_wrap_2d_repeat()
			gol.bind_texture( self.texture_id )
		end
		if self.uv then
			gol.set_attrib_uv( self.uv )
		end
		gol.draw_quad_strip_uv_3d( self.ribbon )
	end
end

STROKE.doc.compute_curve = "() from bdd compute the curve [x,y,z,...]"
function STROKE:compute_curve()
	local ref_bdd  = self.ref_bdd
	local curve_id = self.curve_id
	local point_nb = self.point_nb
	local step     = 1 / point_nb
	local curve    = {}
	local s        = 0
	local id       = 1

	for i = 0, point_nb do
		curve[id], curve[id+1], curve[id+2] = aaa.bdd.coor_to_world( ref_bdd, curve_id, s, 0, 0 )
		s  = s  + step
		id = id + 3
	end
	self.curve  = curve
end

STROKE.doc.compute_attributes = "( thickness, ref_texture, text_speed ) from bu compute the stroke attribute"
function STROKE:compute_attributes( thickness, ref_texture, text_speed )
	self.thick      = thickness
	self.texture_id = param.get( ref_texture )
	self.uv_speed   = text_speed
end

STROKE.doc.get_and_set_length = "() get length from bdd and set it"
function STROKE:get_and_set_length()
	self.length   = aaa.bdd.get_len( self.ref_bdd, self.curve_id )
end

STROKE.doc.get_and_set_point_nb = "() get point_nb from bdd and set it"
function STROKE:get_and_set_point_nb()
	self.point_nb = aaa.bdd.get_control_point_nb( self.ref_bdd, self.curve_id )
end

STROKE.doc.compute_stroke = "() from bdd compute all the functions to have a stroke"
function STROKE:compute_stroke()
	self:get_and_set_length()
	self:get_and_set_point_nb()
	self:compute_curve()
	self:compute_ribbon()
	self:compute_uv()
	self.b_recompute = false
end

STROKE.doc.get_and_set_color = "( ui, alpha ) get color from ui and set it"
function STROKE:get_and_set_color( ui, alpha )
	local r, g, b, a = ui.color:get_rgba()
	self.color       = { r, g, b, a * alpha }
end

STROKE.doc.replay = "( replay_speed ) replay the stroke with a sinusoidal phase"
function STROKE:replay( replay_speed )
	local ribbon_source = {}
	if self.color      then gol.color        ( self.color      ) end
	if self.texture_id then gol.bind_texture ( self.texture_id ) end
	if self.uv         then gol.set_attrib_uv( self.uv         ) end
	if self.ribbon     then ribbon_source = self.ribbon          end

	local phase_ribbon = math.floor( #ribbon_source * 0.5 * ( math.sin( replay_speed * aaa.time.t ) + 1 ) )
	gol.draw_quad_strip_uv_3d( ribbon_source,  phase_ribbon )
end


function meu:define_ui()
	local ix, iy  = 1  , 1
	local SX, SY  = 4/3, 1

	self:add_rendering()
	self:add_camera()
	self:bu_attribut( ix,iy,    SX,SY,  self.ref, self.ui )
end

function meu:bu_attribut( ix, iy, SX, SY, ref, ui )
	local bu
	local command = aaa.obj.do_command

	iy = iy + SY
	
	self:begin_bu_group( "Curve Selection" )
	bu = self:add_trig_method(  {ix,iy,               8,SY},       "Delete",        self,      "delete_curve"             )
	bu = self:add_trig_method(  {ix+8,iy,             8,SY},       "Delete All",    self,      "clear_all"                )
		bu:set_target_param( ref.delete_all )
	iy = iy + SY

	bu = self:add_trig_fn(      {ix,iy,               2*SX,SY},       "First",        command,   ref.bdd, "curve.set_begin" )
	bu = self:add_trig_method(  {ix+2*SX,iy,            2*SX,SY},       "Prev",         self,      "prev_curve"               )
	bu = self:add_slider(       {ix+4*SX,iy,        4*SX,SY},       "Curve",        self, 	   "s_curve",   1,      0,20  )
		bu:set_value_type_integer( true )
		bu:set_target_param( ref.curve_id )
	bu = self:add_trig_method(  {ix+8*SX,iy,          2*SX,SY},       "Next",         self,      "next_curve"               )
	bu = self:add_trig_method(  {ix+10*SX,iy,          2*SX,SY},       "Last",         self,      "last_curve"               )
	iy = iy + SY
	self:end_bu_group()
	

	self:begin_bu_group( "Stroke Attributes" )
	ui.color = self:add_rgbfa(  {ix,iy,               16,SY},       "Color",        false )
	iy = iy + SY
	
	bu = self:add_bu_texture_target_unit( {ix+4,iy,   8,4,b_compact=true},                     "Tex",       1,      true  )
		ui.bu_texture = bu
	iy = iy + 4*SY

	bu = self:add_slider(       {ix,iy,                8,SY},       "Smoothing",    self, 	   "id_smooth", 5,      0,20  )
		bu:set_value_type_integer( true )
		bu:set_color_back( "w" )
	bu = self:add_slider(       {ix+8,iy,                8,SY},       "Thickness",    self,      "ribbon_thick", 1,    0,4  )
		bu:add_values_def( 0.5,1,2,3,4 )
	iy = iy + SY

	bu = self:add_button(       {ix,iy,               8,SY},       "Move",         self,      "b_move",           false  )
	bu = self:add_slider(       {ix+8,iy,                8,SY},       "Speed",        self,      "text_speed",   0,	 -1,1  )
	iy = iy + SY

	bu = self:add_slider(       {ix,iy,                8,SY},       "Phase_begin",  self,      "ph_begin",     0,     0,1 )
		bu:add_values_def( 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 )
		bu:set_color_back( "u" )
	bu = self:add_slider(       {ix+8,iy,              8,SY},       "Phase_end",    self,      "ph_end",       1,     0,1 )
		bu:add_values_def( 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 )
		bu:set_color_back( "v" )
	iy = iy + SY + 0.1
	self:end_bu_group()
	
	self:begin_bu_group( "Stroke Replay" )
	bu = self:add_button(       {ix,iy,             4*SX,SY},       "Replay",       self,      "b_replay",          false )
	bu = self:add_button(       {ix+4*SX,iy,        4*SX,SY},       "M_Replay",     self,      "multiple_replay",   false )
	bu = self:add_slider(       {ix+8*SX,iy,        4*SX,SY},       "Speed",        self,      "r_speed",    0.1,    0,1  )
		bu:add_values_def( 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 )
	iy = iy + SY
	self:end_bu_group()

	self:begin_bu_group( "Stroke Save" )
	bu = self:add_trig_method(  {ix,iy,                8,SY},       "Save",         self,      "save_strokes"             )
	bu = self:add_trig_method(  {ix+8,iy,              8,SY},       "Load",         self,      "load_strokes"             )
	iy = iy + SY
	self:end_bu_group()
end

function meu:init()
	local ref      = self.ref
	local pgr      = param.get_ref
	local layer    = self:get_layer( 1 )

	ref.bdd        = self:get_layer_bdd( 1 )
	ref.number_pt  = pgr( ref.bdd, "ui_u"       )
	ref.curve_id   = pgr( ref.bdd, "dataset_id" )
	ref.delete_all = pgr( ref.bdd, "erase_all"  )
	ref.bank_text  = pgr( layer,   "bank_2d"    )
	ref.bind_text  = pgr( layer,   "bind_2d"    )

	self:self_init()
end

function meu:self_init()
	self.strokes      = {}
	self.text_speed   = 0
	self.ribbon_thick = 1
	self.last_stroke  = 0
	self.ph_begin     = 0
	self.ph_end       = 1
	self.r_speed      = 0.1
	self.len_total    = 0
	self.stroke_index = 0
end

function meu:clear_all()
	param.set( self.ref.curve_id, 9999 )
	self.ui.bu_texture:set_bind_2d( 1 )
	self.ui.color:set_rgba( 1, 1, 1, 1 )
	self:self_init()
end

function meu:new_curve()
	local ref      = self.ref
	local curve_id = param.get( ref.curve_id )
	
	self.strokes[ curve_id ] = STROKE:create( ref.bdd, curve_id )
	self.strokes[ curve_id ]:compute_attributes( self.ribbon_thick, ref.bind_text, self.text_speed )
	self.strokes[ curve_id ]:get_and_set_color(  self.ui, self:get_alpha() )
	
	self.last_stroke = self.last_stroke + 1
end


function meu:new_point_control( x, y, z )
	local ref      = self.ref
	local curve_id = param.get( ref.curve_id )
   
	aaa.obj.do_command(         ref.bdd,    "point.insert"                    )
	aaa.bdd.set_control_point(  ref.bdd,  param.get( ref.number_pt ), x, y, z )
	self.strokes[ curve_id ].b_recompute = true
end

function meu:next_curve()
	aaa.obj.do_command(    self.ref.bdd,    "curve.set_next"    )
	local stroke = self.strokes[ param.get( self.ref.curve_id ) ]
	if stroke then 
		self:stroke_to_self( stroke )
	end
end

function meu:prev_curve()
	aaa.obj.do_command(    self.ref.bdd,    "curve.set_prev"    )
	local stroke = self.strokes[ param.get( self.ref.curve_id ) ]
	if stroke then 
		self:stroke_to_self( stroke )
	end
end

function meu:last_curve()
	param.set( self.ref.curve_id, self.last_stroke )
	local stroke = self.strokes[ param.get( self.ref.curve_id ) ]
	if stroke then 
		self:stroke_to_self( stroke )
	end
end



function meu:delete_curve()
	aaa.bdd.clear_control_points( self.ref.bdd,  param.get( self.ref.curve_id ) )
end



function meu:draw()
	self:draw_layers_begin()
		self:draw_layer( 1 )
		for _, stroke in pairs( self.strokes ) do
			if self.multiple_replay then 
				stroke:replay( self.r_speed )
			else
				stroke:draw_ribbon()
			end
		end
		if self.b_replay then self:replay() end
	self:draw_layers_end()
end



function meu:update()
	local ref            = self.ref
	local len_total      = 0
	local current_stroke = param.get( ref.curve_id )

	for _, stroke in pairs( self.strokes ) do
		if stroke then
			stroke:update( self.b_move )
			if stroke.length then len_total = len_total + stroke.length end
		end
	end
	self.len_total = len_total

	if current_stroke <= self.last_stroke and not self.b_replay then
		local stroke = self.strokes[ current_stroke ]
		if stroke then
			stroke:draw_curve()
			self:self_to_stroke( stroke )
		end
	end
end

function meu:stroke_to_self( stroke )
	self.ui.color:set_rgba( stroke.color[1], stroke.color[2], stroke.color[3], stroke.color[4] )
	self.ui.bu_texture:set_bind_2d( stroke.texture_id )
	self.ribbon_thick = stroke.thick
	self.text_speed   = stroke.uv_speed
	self.id_smooth    = stroke.id_smooth
	self.ph_begin     = stroke.ph_begin
	self.ph_end       = stroke.ph_end
	
end

function meu:self_to_stroke( stroke )
	local r, g, b, a   = self.ui.color:get_rgba()
	stroke.color       = { r, g, b, a * self:get_alpha() }
	stroke.texture_id  = param.get( self.ref.bind_text )
	stroke.id_smooth   = self.id_smooth
	stroke.uv_speed    = self.text_speed
	stroke.ph_begin    = self.ph_begin
	stroke.ph_end      = self.ph_end
	stroke.thick       = self.ribbon_thick
	stroke.b_recompute = true
end

function meu:replay()
	local local_ph  = 0
	local len_total = self.len_total
	if not self.replay_initialized then
		for _, stroke in pairs( self.strokes ) do
			stroke.ph_end = 0
			stroke.ribbon = {}
			stroke.ph_global_begin = local_ph
			stroke.ph_global_end   = stroke.length / len_total + local_ph
			local_ph               = stroke.ph_global_end
		end
		self.stroke_index = 1
		self.replay_initialized = true
	end

	if self.stroke_index > #self.strokes then
		self.b_replay = false
		self.replay_initialized = false
		return
	end

	local stroke = self.strokes[ self.stroke_index ]
	local phase_start, phase_end = 0, 1

	if not stroke then
		self.stroke_index = self.stroke_index + 1
		return
	end
	
	if stroke.ph_global_end <= self.ph_end then
		phase_end = 1
	else
		phase_end = self.ph_end - stroke.ph_global_begin
	end
	
	if stroke.ph_global_begin >= self.ph_begin then
		phase_start = 0
	else
		phase_start = self.ph_begin - stroke.ph_global_begin
		phase_end = 0
	end

	if stroke.ph_end < phase_end then
		stroke.ph_end = math.min( stroke.ph_end + 0.1 * self.r_speed, phase_end )
		stroke:set_phase( phase_start, stroke.ph_end )
		stroke.b_recompute = true
	else
		self.stroke_index = self.stroke_index + 1
	end
end

function meu:save_strokes()
	-- Define file paths for saving curves and their attributes
	--todo why two dialogs
	--todo deal with cancel
	local fpath_curves     = aaa.file.do_dialog_save( "Save Strokes",   "*.csv" )
	local fpath_attributes = aaa.file.do_dialog_save( "Save Attributes", "*.csv" )
	local file = io.open( fpath_attributes, "w" )
	local stroke_data

	-- Save the current state of the BDD (curve definitions) to the curve file
	aaa.obj.save( self.ref.bdd, fpath_curves )
	-- Check if the attributes file was successfully opened
	if file then
		-- Iterate through all strokes in the `self.strokes` table
		for _, stroke in pairs(self.strokes) do
			-- Combine stroke attributes into a single formatted string
			stroke_data = table.concat({
				"curve_id:"   .. stroke.curve_id,                       -- The ID of the curve
				"ref_bdd:"    .. stroke.ref_bdd,                        -- Reference to the BDD (data structure)
				"thick:"      .. stroke.thick,                          -- Thickness of the ribbon
				"texture_id:" .. stroke.texture_id,                     -- ID of the texture applied to the ribbon
				"color:"      .. table.concat( stroke.color, "," ),     -- Color values (RGBA) concatenated as a comma-separated string
				"uv_speed:"   .. stroke.uv_speed,                       -- Speed of texture animation along the ribbon
				"ph_begin:"   .. stroke.ph_begin,                       -- Beginning phase of the ribbon
				"ph_end:"     .. stroke.ph_end,                         -- Ending phase of the ribbon
				"id_smooth:"  .. stroke.id_smooth,                      -- Smoothing ID for the ribbon
			}, "\n")
			-- Write the formatted string to the attributes file, followed by a blank line
			file:write( stroke_data, "\n\n" )
		end
		-- Close the file after all strokes are saved
		file:close()
	else
		-- Print an error message if the file could not be opened
		self:print( "Save Failed, try again" )
	end
end


function meu:load_strokes()
	--topdo check for cancel
	local fpath_curves     = aaa.file.do_dialog_open( "Load Strokes",    "*.csv" )
	local fpath_attributes = aaa.file.do_dialog_open( "Load Attributes", "*.csv" )
	
	-- Load curves from the CSV file
	aaa.obj.load( self.ref.bdd, fpath_curves )
	local attributes   = self:load_attributes( fpath_attributes )
	local total_curves = #attributes
	local new_id       = self.last_stroke + 1

	for curve_id = 1, total_curves do 
		local attr   = attributes[ curve_id ]
		local stroke = STROKE:create( self.ref.bdd, curve_id )

		if attr then
			stroke.texture_id = attr.texture_id
			stroke.id_smooth  = attr.id_smooth
			stroke.uv_speed   = attr.uv_speed
			stroke.ph_begin   = attr.ph_begin
			stroke.ph_end     = attr.ph_end
			stroke.thick      = attr.thick
			stroke.color      = attr.color
		end

		stroke:compute_stroke()
		self.strokes[ new_id ] = stroke
		new_id = new_id + 1
	end
	-- Update the ID of the last ribbon
	self.last_stroke = new_id - 1
	self:print( "Loading complete: " .. tostring( total_curves ) .. " ribbons added after the last one." )
end

function meu:load_attributes( fpath )
	local attributes = {}
	local file = io.open( fpath, "r" )
	if not file then
		self:print( "Failed to load stroke attributes from " .. fpath )
		return attributes
	end

	local attr = {}
	for line in file:lines() do
		if line == "" then
			if attr.curve_id then
				attributes[ tonumber( attr.curve_id ) ] = attr
			end
			attr = {}
		else
			local key, value = line:match( "([^:]+):(.+)" )
			if key and value then
				if key == "color" then
					attr.color = {}
					for c in value:gmatch( "[^,]+" ) do
						table.insert( attr.color, tonumber( c ) )
					end
				elseif key == "thick"      then
					attr.thick      = tonumber( value )
				elseif key == "texture_id" then
					attr.texture_id = tonumber( value )
				elseif key == "uv_speed"   then
					attr.uv_speed   = tonumber( value )
				elseif key == "curve_id"   then
					attr.curve_id   = tonumber( value )
				elseif key == "ref_bdd"    then
					attr.ref_bdd    = tonumber( value )
				elseif key == "id_smooth"  then
					attr.id_smooth  = tonumber( value )
				elseif key == "ph_begin"   then
					attr.ph_begin   = tonumber( value )
				elseif key == "ph_end"     then
					attr.ph_end     = tonumber( value )
				end
			end
		end
	end

	if attr.curve_id then
		attributes[ tonumber( attr.curve_id ) ] = attr
	end
	
	file:close()
	return attributes
end
