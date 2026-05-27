function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
--	local par

	ui.cam = self:add_camera()

	local ix,iy = 1,2

--	local layer = self:get_layer(1)
--	bu = self:add_button(	{ix, iy }, "Top", aaa.layer.get_rendering( layer ), "top_line", true )
	bu = self:add_slider(	{ix,iy+1,	8,2},	"Phase",	nil, nil, 0, 0, 1 )
end

function meu:init()
	local ref = self.ref

	local layer = self:get_layer(1)
	ref.bdd = aaa.layer.get_bdd( layer )
	ref.pos			= param.get_ref( ref.bdd, "emission_origin_x" )
	ref.restart 	= param.get_ref( ref.bdd, "restart_trig" )
	ref.nb_by_sec	= param.get_ref( ref.bdd, "nb_by_sec" )
--	ref.max = param.get_ref_xyz_packed( ref.bdd, "bounding_box_max" )
end

function meu:update()
	local ref = self.ref
	--local id = (self:get_inst_key() == "1") and 1 or 2
	--self:set_bu_value( "inter", math.sin( aaa.time.t * .2 + id * math.pi ) )
	local ph = self:get_phase()
	if ph <= 0 then
		param.set( ref.restart, true )
	end
	if inside( ph, 0.01, .45 ) then
		param.set( ref.nb_by_sec, 1024*4	 )
	else
		param.set( ref.nb_by_sec, 0 )
	end
	local d = .5
	param.set( ref.pos, (.5-ph) * (8+d*2) - d )
--	self:init()
end


function meu:set_phase( phase )		self:set_bu_value( "phase", phase )	end
function meu:get_phase()			return self:get_bu_value( "phase" )	end