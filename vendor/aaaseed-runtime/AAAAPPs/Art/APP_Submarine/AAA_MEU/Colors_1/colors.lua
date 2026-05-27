function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu

	self.nb = 12
	local SY = 1
	local ix = 1
	local iy = 1

	local SX3 = 8/3
	self:add_trig_method(	{	ix,			iy,		SX3, SY},		"Set",		self, "set_free", true )
	self:add_trig_method(	{	ix+SX3,		iy,		SX3, SY},		"Clear",	self, "set_free", false )
	self:add_trig_method(	{	ix+SX3*2,	iy,		SX3, SY},		"Flip",		self, "flip_free" )
	iy = iy + SY
	local t = {}
	self.tb_free = {}
	for i = 1,self.nb do
		t[i] = self:add_rgbf(	{ix+SY,iy,	8-SY,SY},	"Colors_"..i,	false )
		self:add_button( {ix,iy,	SY, SY },		"free_"..i,		self.tb_free, i, true ):set_text_visible( false )
		iy = iy + SY
	end
	ui.bu_cols = t

	iy = iy + SY

	ix = 9
	iy = 1

	SY = 2/3
	bu = self:add_trig_method(	{ix,iy,			2,2},	"Gene",			self, "do_gene1" )
		bu:set_text_inside( true )
		bu:set_color_back( "action" )
	bu = self:add_slider_two(	{ix+2,iy,	6,SY},	"Hue",			self, "hue_min", "hue_max",	0,1,	0,1  )
	iy = iy + SY
	bu = self:add_slider_two(	{ix+2,iy,	6,SY},	"Saturation",	self, "sat_min", "sat_max",	0,1,	0,1  )
	iy = iy + SY
	bu = self:add_slider_two(	{ix+2,iy,	6,SY},	"Value",		self, "val_min", "val_max",	0,1,	0,1  )
	iy = iy + SY

	iy = iy + SY
	bu = self:add_trig_method(	{ix,iy,			2,2},	"Print",		self, "print_table" )
		bu:set_text_inside( true )
		bu:set_color_back( "action" )
end

function meu:get_preset_nb()	return 16	end

function meu:print_table( str )
	table.print( self.tb_free, str )
end

function meu:set_free( b )
	self:print( "meu:set_free "..b  )
	self:print_table( "before" )
	array.set( self.tb_free, b )
	self:print_table( "after" )
end
function meu:flip_free()
	for i=1,#self.tb_free do
		self.tb_free[i] = not self.tb_free[i]
	end
end

function meu:get_rgba( id )
	local bu = self.ui.bu_cols[id]
	if bu then
		return bu:get_rgba()
	end
end
function meu:set_rgba( id, r,g,b, a )
	local bu = self.ui.bu_cols[id]
	if bu then
		return bu:set_rgba( r,g,b, a )
	end
end

function meu:do_gene1()
	local function gene_float( min, max )	return min + math.random() * (max-min) 	end
	-- self:print( "do_gene1()" )
	for i = 1,self.nb do
		if self.tb_free[i] then
			local h = gene_float( self.hue_min, self.hue_max )
			local s = gene_float( self.sat_min, self.sat_max )
			local v = gene_float( self.val_min, self.val_max )
		-- 	self:print( string.format( "%d -- %2.3f,%2.3f,%2.3f", i, h,s,v ) )
	 		local r,g,b = aaa.color.hsv_to_rgb( h, s, v )
		-- 	self:print( string.format( "      %2.2f,%2.2f,%2.2f", r,g,b ) )
	 		self:set_rgba( i, r,g,b )
		end
	end
end

--function meu:update()
--end

