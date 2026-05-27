		--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix,iy = 1,1
	self:add_button(	{	ix, iy },				"Analyse",	self, "b_anal", 			false)
	iy = iy + 1
	self:add_slider(	{ix+1,iy,	7,1},	"coverage",			nil, nil,			0,			0, 1 )
	self:add_slider(	{ix+1,iy+1,	7,1},	"coverage_min",		self, "cov_min",	0,			0, 1 )
	self:add_slider(	{ix+1,iy+2,	7,1},	"center_u",			nil, nil,			.5,			0, 1 ):set_meter( false )
	self:add_slider(	{ix+1,iy+3,	7,1},	"center_v",			nil, nil,			.5,			0, 1 ):set_meter( false )

	iy = iy + 5
	self:add_slider(	{ix+1,iy,	7,1},	"presence",			nil, nil,			0,			0, 1 )
	self:add_slider(	{ix+1,iy+1,	7,1},	"time_presence",	nil, nil,			0,			0, 100 )
	self:add_slider(	{ix+1,iy+2,	7,1},	"time_empty",		nil, nil,			0,			0, 100 )
end


function meu:init()
	local ref = self.ref
	ref.bdd_con	=	self:get_layer_bdd(1)
		ref.contour_nb	=	param.get_ref( ref.bdd_con,		"contour_nb"	)

	ref.bdd_anal =	self:get_layer_bdd(2)
		ref.coverage	= param.get_ref( ref.bdd_anal,		"out_coverage"				)
		ref.center_u 	= param.get_ref( ref.bdd_anal,		"out_center_u"				)
		ref.center_v 	= param.get_ref( ref.bdd_anal,		"out_center_v"				)
end

function meu:draw_blobs( blobs )
	gol.reset()
	gol.color_yellow()

	for id=1,#blobs do
		local b = blobs[id]
		aaa.draw_null( b.x*2,b.y*2, 0, .2 )
	end

	gol.set_line_width( 4 )
	gol.color_cyan( .4 	)
	aaa.draw_lines_vert_nb( 9, -1, .25, 	-1, 1 )
	aaa.draw_lines_hori_nb( 9, -1, .25,		-1, 1 )

end

function meu:draw()
	local ref	= self.ref

	MEU.draw( self )
	local blobs = BLOBS:read_bdd_in_array_xy( self.ref.bdd_con )
	self:draw_blobs( blobs )
	TANK.cur.__loyal_blobs = blobs
	--aaa.draw_rect_line( -.5,-.5, .5,.5 )
	--aaa.draw_rect_line( -1.,-1, 1,1	 )
	if self.b_anal then
		TANK.cur.__public = TANK.cur.__public or { presence=0, time_empty=0, time_presence=0  }
		local public = TANK.cur.__public
		public.cov = param.get(ref.coverage)
		public.center_u = param.get(ref.center_u)
		public.center_v = param.get(ref.center_v)
		self:set_bu_value( "coverage",	public.cov )
		self:set_bu_value( "center_u",	public.center_u	)
		self:set_bu_value( "center_v",	public.center_v	)

		local presence = public.presence
		if public.cov > self.cov_min then	presence = presence + aaa.time.dt * 4.
		else								presence = presence - aaa.time.dt * 4.
		end
		presence = clamp_01( presence )
		self:set_bu_value( "presence",	presence	)
		public.presence = presence

		if presence==1 then
			public.time_empty = 0
			public.time_presence = public.time_presence + aaa.time.dt
		elseif presence==0 then
			public.time_presence = 0
			public.time_empty = public.time_empty + aaa.time.dt
		end
		self:set_bu_value( "time_presence",	public.time_presence	)
		self:set_bu_value( "time_empty",	public.time_empty	)

	end
end