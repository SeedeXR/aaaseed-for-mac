--todo create a COLOR too
if CLASS.DECLARE( "COLOR_REF" ) then
	COLOR_REF:set_class_status_doc(	CLASS.STATUS.CORE,
									"encapsulate a C color object" )
end

function COLOR_REF.__build_rgb_ref( dst, dst_prefix, obj, param_prefix )
	dst[ dst_prefix.."red" ]	= param.get_ref_no_error( obj, param_prefix.."red"	)
	dst[ dst_prefix.."green" ]	= param.get_ref_no_error( obj, param_prefix.."green"	)
	dst[ dst_prefix.."blue" ]	= param.get_ref_no_error( obj, param_prefix.."blue"	)
end
function COLOR_REF.__build_rgba_ref( dst, dst_prefix, obj, param_prefix )
	COLOR_REF.__build_rgb_ref( dst, dst_prefix, obj, param_prefix )
	dst[ dst_prefix.."alpha" ]	= param.get_ref_no_error( obj, param_prefix.."alpha"	)
end
function COLOR_REF.__build_rgbag_ref( dst, dst_prefix, obj, param_prefix )
	COLOR_REF.__build_rgba_ref( dst, dst_prefix, obj, param_prefix )
	dst[ dst_prefix.."grey" ]	= param.get_ref_no_error( obj, param_prefix.."grey"	)
end

function COLOR_REF:init_ref( ref_obj, param_prefix )
	local ref = self.ref
	COLOR_REF.__build_rgbag_ref( ref, "", ref_obj, param_prefix )

	ref.use_hsv		= param.get_ref_no_error( ref_obj, "use_hsv"		)
	ref.convert		= param.get_ref_no_error( ref_obj, "convert"		)
	ref.hue			= param.get_ref_no_error( ref_obj, "hue"			)
	ref.saturation	= param.get_ref_no_error( ref_obj, "saturation"		)
	ref.value		= param.get_ref_no_error( ref_obj, "value"			)
end

--todo generalize
function COLOR_REF:get_param_ref( name )
	return self.ref[ string.lower(name) ]
end
function COLOR_REF:create( name, obj_ref, param_prefix )
	local self
	if param_prefix then
		self = COLOR_REF:create_instance( name )
		self.ref = {}
	else
		self = COLOR_REF:create_instance_with_obj( name, obj_ref )
		param_prefix = "global_"
	end
	self:init_ref( obj_ref, param_prefix )
	return self
end

--[[
function COLOR_REF:set( r, g, b, a )
	self.r = r or 1
	self.g = g or 1
	self.b = b or 1
	self.a = a or 1
end
--]]
--todo deal with "incomplete COLOR_REF"
function COLOR_REF:set_hsv_mode( b )
	local pr = self.ref.use_hsv
	if pr then
		param.set( pr, b )
	end
end
function COLOR_REF:has_hsv()
	return self.ref.use_hsv and true or false
end
function COLOR_REF:is_hsv()
	local ref_use_hsv = self.ref.use_hsv
	return ref_use_hsv and param.get_bool( ref_use_hsv ) or false
end
--toodo convert should handle in the COLOR_REF object when we don't have it in the C object
function COLOR_REF:set_convert( b )		param.set( self.ref.convert, b )	end

if true then
--[[
	function COLOR_REF:set_rgb(		r, g, b )
		if type(r)=="table"	then	param.set_real_3(	self.ref.red,	r[1],	r[2],	r[3]			)
		else						param.set_real_3(	self.ref.red,	r,		g,		b				)
	end
	function COLOR_REF:set_rgba(	r, g, b, a )
		if type(r)=="table"	then	param.set_real_3(	self.ref.red,	r[1],	r[2],	r[3],	r[4] or 1	)
		else						param.set_real_4(	self.ref.red,	r,		g,		b,		a or 1		)
	end
--]]
	function COLOR_REF:set_rgb(		r, g, b )		param.set_real_3(	self.ref.red,	r,		g,		b				)	end
	function COLOR_REF:set_rgba(	r, g, b, a )	param.set_real_4(	self.ref.red,	r,		g,		b,		a or 1	)	end
	function COLOR_REF:set_rgb_t(	tab )			param.set_real_3(	self.ref.red,	tab[1],	tab[2],	tab[3]			)	end
	function COLOR_REF:set_rgba_t(	tab )			param.set_real_4(	self.ref.red,	tab[1],	tab[2],	tab[3],	tab[4]	)	end

	function COLOR_REF:set_hsv(		h, s, v )		param.set_real_3(	self.ref.hue,	h,		s,		v				)	end
else
	function COLOR_REF:set_rgb( r, g, b )
		local ref = self.ref
		local set = param.set
		set( ref.red,	r )
		set( ref.green,	g )
		set( ref.blue,	b )
	end
	function COLOR_REF:set_rgba( r, g, b, a )
		local ref = self.ref
		local set = param.set
		set( ref.red,	r )
		set( ref.green,	g )
		set( ref.blue,	b )
		set( ref.alpha, a or 1)
	end
	function COLOR_REF:set_rgb_t( tab )
		local ref = self.ref
		local set = param.set
		set( ref.red,	tab[1] )
		set( ref.green,	tab[2] )
		set( ref.blue,	tab[3] )
	end
	function COLOR_REF:set_rgba_t( tab )
		local ref = self.ref
		local set = param.set
		set( ref.red,	tab[1] )
		set( ref.green,	tab[2] )
		set( ref.blue,	tab[3] )
		set( ref.alpha, tab[4] or 1 )
	end
	function COLOR_REF:set_hsv( h, s, v )
		local ref = self.ref
		local set = param.set
		set( ref.hue,			h )
		set( ref.saturation,	s )
		set( ref.value,			v )
	end
end

function COLOR_REF:get_rgba()
	local color = {}
	local ref = self.ref
	local get = param.get
	local g = ref.grey and get( ref.grey ) or 1
	color[1] = get( ref.red )	* g
	color[2] = get( ref.green )	* g
	color[3] = get( ref.blue )	* g
	color[4] = get( ref.alpha )
	return color
end
function COLOR_REF:get_rgb()
	local color = {}
	local ref = self.ref
	--table.print( ref, "ref" )
	local get = param.get
	local g = ref.grey and get( ref.grey ) or 1
	color[1] = get( ref.red )	* g
	color[2] = get( ref.green )	* g
	color[3] = get( ref.blue )	* g
	return color
end

function COLOR_REF:set_red(		v )		param.set( self.ref.red,	v )		end
function COLOR_REF:set_green(	v )		param.set( self.ref.green,	v )		end
function COLOR_REF:set_blue(	v )		param.set( self.ref.blue,	v )		end
function COLOR_REF:set_grey(	v )		param.set( self.ref.grey,	v )		end
function COLOR_REF:set_alpha(	v )		param.set( self.ref.alpha,	v )		end

function COLOR_REF:get_alpha()			return param.get( self.ref.alpha )	end


function COLOR_REF:draw_rgba()
	local obj = self.ref.obj
	aaa.obj.update( obj )
	aaa.draw_rgba( obj )
--	local color = self:get_rgba()
--	gol.color( color[1], color[2], color[3], color[4] )
end
function COLOR_REF:draw_rgb()
	local obj = self.ref.obj
	aaa.obj.update( obj )
	aaa.draw_rgb( obj )
--	local color = self:get_rgb()
--	gol.color( color[1], color[2], color[3] )
end

