--	FBO_AND_OUT
--		encapsulate an fbo where we draw
--		and output than we draw
--
if CLASS.DECLARE( "FBO_AND_OUT" ) then
	FBO_AND_OUT:set_class_status_doc(	CLASS.STATUS.CORE,
										"used by old app encapsulate the c_fbo object: frame buffer object and drawing it",
										"use an FBO instance" )
end

function FBO_AND_OUT:create( pre, nb )
--	self:print( "in FBO_AND_OUT:create, args are pre : "..pre..", nb : "..nb )
	if self.verbose >= 1 then aaa.print_method() end
	local self = FBO_AND_OUT:create_instance( pre )
	self.ref = {}
	local ref = self.ref
	local layers			=	aaa.obj.get( pre.."_fbo_layers" )
	local fbo				=	aaa.obj.get_branch_by_class( layers, "fbo" )
	self.fbo = FBO:create( "for_render", fbo, layers )

	layers		=	aaa.obj.get( pre.."_fbo_out_layers" )
	ref.out_layers = layers
	ref.out		= {}
	self.out_nb	= nb
	for i=1,nb do
		ref.out[i] = {}
		local r = ref.out[i]
		r.layer				=	aaa.layers.get_layer( layers, i )
		if r.layer then
			r.shading			=	aaa.obj.get_branch_by_class_no_error( r.layer, "shading" )
			if r.shading then
				r.tex_u					= param.get_ref( r.shading, "fu_02_r_x" )
				r.tex_du				= param.get_ref( r.shading, "fu_02_b_z" )
				r.gamma					= param.get_ref( r.shading, "fu_01_a_w" )
			end
			r.grid				=	aaa.obj.get_branch_by_class_no_error( r.layer, "bdd_grid_adjustable" )
			if r.grid then
				r.x						= param.get_ref( r.grid, "center_x" )
				r.y						= param.get_ref( r.grid, "center_y" )
				r.nb_u					= param.get_ref( r.grid, "nb_u" )
				r.nb_v					= param.get_ref( r.grid, "nb_v" )
				r.curve_nb_u			= param.get_ref( r.grid, "curve_nb_u" )
				r.curve_nb_v			= param.get_ref( r.grid, "curve_nb_v" )
				r.reset					= param.get_ref( r.grid, "grid_reset_trig" )
			else
				r.grid				=	aaa.obj.get_branch_by_class( r.layer, "bdd_grid" )
			end
			if r.grid then
				r.x						= param.get_ref( r.grid, "center_x" )
				r.y						= param.get_ref( r.grid, "center_y" )
				r.nb_u					= param.get_ref( r.grid, "nb_u" )
				r.nb_v					= param.get_ref( r.grid, "nb_v" )
			end
			r.model				=	aaa.obj.get_branch_by_class_no_error( r.layer, "model" )
			if r.model then
				r.size_u				= param.get_ref( r.model, "size_u" )
				r.size_v				= param.get_ref( r.model, "size_v" )
			end
		else
			self:box_error( self.." no layer "..i )
		end
	end

	ref.layers_visu		=	aaa.obj.get_no_error( pre.."_fbo_visu_layers" )

	return self
end

function FBO_AND_OUT:get_out_nb()	return self.out_nb end

function FBO_AND_OUT:set_pixel_size( sx, sy )
	self.fbo:set_pixel_size( sx, sy )
end

function FBO_AND_OUT:set_u_one( id, l, r )
	local ref = self.ref.out[id]
	param.set( ref.tex_u,	l	)
	param.set( ref.tex_du,	r-l	)
end

function FBO_AND_OUT:set_u_with_table( t )
	for i=1,self.out_nb do
		self:set_u_one( i, t[i], t[i+1] )
	end
end

function FBO_AND_OUT:set_gamma_fuzzy_one( id, gamma )
	local ref = self.ref.out[id]
	param.set( ref.gamma,	gamma	)
end
function FBO_AND_OUT:set_gamma_fuzzy(gamma)	for id=1,self.out_nb do self:set_gamma_fuzzy_one( id, gamma ) end	end

function FBO_AND_OUT:set_size_uv( u,v )
	local fo = self.ref.out
	for id=1,self.out_nb do
		local ref = fo[id]
		if  ref.size_u then
			param.set( ref.size_u,	u	)
			param.set( ref.size_v,	v	)
		end
	end
end
function FBO_AND_OUT:set_xy( id, x,y )
	local ref = self.ref.out[id]
	param.set( ref.x,	x	)
	param.set( ref.y,	y	)
end

function FBO_AND_OUT:reset_grid_one( id )
	local ref = self.ref.out[id]
	param.set( ref.reset,	true	)
end
function FBO_AND_OUT:reset_grid()	for id=1,self.out_nb do self:reset_grid_one( id ) end	end

function FBO_AND_OUT:set_grid_nb_uv_one( id, curve_nb_u, curve_nb_v, nb_u, nb_v )
	local ref = self.ref.out[id]
	nb_u = nb_u or (curve_nb_u - 2)
	nb_v = nb_v or (curve_nb_v - 2)
	param.set( ref.curve_nb_u,	curve_nb_u	)
	param.set( ref.curve_nb_v,	curve_nb_v	)
	param.set( ref.nb_u,		nb_u	)
	param.set( ref.nb_v,		nb_v	)
end
function FBO_AND_OUT:set_grid_nb_uv( ... )	for id=1,self.out_nb do self:set_grid_nb_uv_one( id, ... ) end	end

--todo not sure we even clear the fbo here
function FBO_AND_OUT:draw_fbo()	self.fbo:update_then_draw()	end
function FBO_AND_OUT:draw_out()	aaa.obj.update_then_draw(	self.ref.out_layers	)	end
function FBO_AND_OUT:draw_visu()
	local r = self.ref.layers_visu
	if r then
		aaa.obj.update_then_draw( r )
	end
end

