function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local iy

	self:add_camera()
	self:add_rgbf(	{9,11,	nil,nil}, "Text_", nil )

	bu = self:add_slider(	{1,3,	8,1},	"text_speed", self, "text_speed", 1 )
end

function meu:init()
	local ref = self.ref

	ref.transfo 	= aaa.obj.get_down_by_class( ref.obj, "transfo_trs" )
		ref.trx		= param.get_ref( ref.transfo, "translate_x" )
	ref.bdd_text	= aaa.obj.get_down_by_class( self:get_layer(1), "bdd_text" )
		ref.text	= param.get_ref( ref.bdd_text, "text" )

	meu:set_text_by_id( 2 )
end

local mess =
{
	{	"QWARTZ",		1,  -7.2 },
	{	"QWARTZ  MOVE", 1.4, -11.4 },
	{	"On peut écrire des trucs vachement plus longs", 1.4, -50 },
	{	"Grant de 7m	", 1.4, -50 },
	{	"None",			 2, -6		},

}
function meu:set_text( str, u_begin, u_end )
	u_begin	= u_begin or 0
	u_end	= u_end or 0
	self.u			=	u_begin
	self.u_begin	=	u_begin
	self.u_end		=	u_end
	param.set( self.ref.text, str )
end
function meu:set_text_by_id( id )
	local m = mess[id]
	self:set_text( m[1], m[2], m[3] )
end
function meu:is_done()
	return self.u < self.u_end
end
function meu:update()
	local ref = self.ref

	local u = self.u
	if self.u_begin ~= self.u_end then
		local speed = -4 * self.text_speed
		u = u + speed * aaa.time.dt
		--todo define and use a b_loop here
		--if u < self.u_end then
		--	u = self.u_begin
		--	self:set_text_by_id( math.random(3) + 1 )
		--end
		self.u = u
	end
	param.set( ref.trx, u*2 )
end
