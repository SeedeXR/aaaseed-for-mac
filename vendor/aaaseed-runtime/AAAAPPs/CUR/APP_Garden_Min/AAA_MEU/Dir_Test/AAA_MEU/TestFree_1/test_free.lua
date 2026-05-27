function meu:define_ui()
	local ref = self.ref
	local bu
	local ix,iy = 1,1

	bu = self:add_trig_method(	{ix+6, iy,		4, 1},		"Test",	self, "test"	)
		:set_ui_top_size(true)
		iy = iy + 1
	local sy = 4
	local sx = 16/2
	for i=1,2 do
		--local sx = i==1 and 4 or 8
		self:add_bu_texture( {ix,   iy,	sx,sy,	b_compact=true},	"Tex L"..i,	i*2-1,	i==1	)
		self:add_bu_texture( {ix+sx,iy,	sx,sy,	b_compact=false},	"Tex R"..i,	i*2,	i==1	)
		iy = iy + sy
	end
	MEU.__b_bu_texture_use = true
	--todo check we don't habe already the same id
	self:add_bu_texture(		{ix,   iy,	sx,sy,	b_compact=true},	"New L",	5,		true )
	self:add_bu_texture(		{ix+sx,iy,	sx,sy,	b_compact=false},	"New R",	6,		true )	
	MEU.__b_bu_texture_use = false
	--MEU.__b_tex_sel_v2 = false
end

function meu:test()
	self:print( "Test" )
	aaa.img.save( self:get_bind_by_name( "Dif" ),	"./AAATest/AAA/BBB/CCC/DDD/Bresil/bordel", "png" )
end
