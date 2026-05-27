aaa.show_file_begin( "OPENCL" )

--
--	SHADING
--
if CLASS.DECLARE( "OPENCL" ) then
	OPENCL:set_class_status_doc(	CLASS.STATUS.CORE,
									"Encapsulate the access to the C bdd_opencl_xxx objects" )
	OPENCL.TEXTURE_NB = 8
	OPENCL.PARAM_NB = 18
	OPENCL.FLOAT4_NB = 4
	OPENCL.DATASET_FLOAT_NB = 16
end

function OPENCL:get_param_ref(	index )			return 				self.ref.param[index]				end
function OPENCL:get_param(		index )			return	param.get(	self.ref.param[index] )				end
function OPENCL:set_param(		index, val )			param.set(	self.ref.param[index], val )		end

function OPENCL:get_vec_xyz(	index )
	local r = self.ref.param_vec
	index = (index-1)*4
	return param.get( r[index+1] ), param.get( r[index+2] ), param.get( r[index+3] )
end
function OPENCL:get_vec_xyzw(	index )
	local r = self.ref.param_vec
	index = (index-1)*4
	return param.get( r[index+1] ), param.get( r[index+2] ), param.get( r[index+3] ), param.get( r[index+4] )
end
--function OPENCL:set_vec(		index, val )			param.set(	self.ref.param_vec[ index ], val )	end
function OPENCL:set_vec_xyz(	index, x,y,z )
	local r = self.ref.param_vec
	index = (index-1)*4
	param.set( r[index+1], x )
	param.set( r[index+2], y )
	param.set( r[index+3], z )
end
function OPENCL:set_vec_xyzw(	index, x,y,z,w )
	local r = self.ref.param_vec
	index = (index-1)*4
	param.set( r[index+1], x )
	param.set( r[index+2], y )
	param.set( r[index+3], z )
	param.set( r[index+4], w )
end

function OPENCL:get_float_ref(	index )			return				self.ref.float[index]				end
function OPENCL:get_float(		index )			return	param.get(	self.ref.float[index] )				end
function OPENCL:set_float(		index, val )			param.set(	self.ref.float[index], val )		end

--todo add tex fns
function OPENCL:get_tex_src(	index )			return	param.get( self.ref.tex_bind[index] )			end
function OPENCL:set_tex_src(	index, bind )			param.set( self.ref.tex_bind[index], bind )		end

function OPENCL:set_nb_by_set(	val )					param.set(	self.ref.nb_by_set, val )			end

function OPENCL:build_all_ref()
	local ref = self.ref

	--todo regroup in table by texture
	ref.tex_use		= {}
	ref.tex_usage	= {}
	ref.tex_bind	= {}
	local function pgr( name )	return param.get_ref( ref.obj, name )	end
	local function bgr( name )	ref[name] = pgr( name )					end
	for i=1,OPENCL.TEXTURE_NB do
		local name = "texture_"..i.."_"
		ref.tex_use[i]		= pgr( name.."use" )
		ref.tex_usage[i]	= pgr( name.."usage" )
		ref.tex_bind[i]		= pgr( name.."bind" )
	end

	ref.param = {}
	for i=1,self.PARAM_NB do
		local pname = (i<10) and "param_0" or "param_"
		pname = pname..i
		ref.param[i] = pgr( pname )
	end

	ref.param_vec = {}
	for i=1,self.FLOAT4_NB do
		ref.param_vec[(i-1)*4+1] = pgr( "vec_0"..i.."_x" )
		ref.param_vec[(i-1)*4+2] = pgr( "vec_0"..i.."_y" )
		ref.param_vec[(i-1)*4+3] = pgr( "vec_0"..i.."_z" )
		ref.param_vec[(i-1)*4+4] = pgr( "vec_0"..i.."_w" )
	end

	ref.float_by_address = pgr( "float_pass_by_address" )
	ref.float = {}
	for i=1,self.DATASET_FLOAT_NB do
		ref.float[i] = pgr( "float_"..i )
	end

	local names	=	{
					"point_nb_used",	"grid_2d",			"nb_u",
					"filename",			"ready_to_run",
					"nb_by_set",
					"line",				"color",			"normal",			"attrib",
					"restart_trig",		"dataset_restart_trig"
					}
	for _,name in pairs(names) do bgr(name) end

end

function OPENCL:get_info()
	local ref =  self.ref
	local nb = param.get( ref.point_nb_used )
	local str = aaa.format.int_to_char_with_space(nb)
	if param.get_bool(ref.grid_2d) then
		local nb_u = param.get(ref.nb_u)
		str = " "..str.." ( "..aaa.format.int_to_char_with_space(nb_u).." x "..nb/nb_u.." )"
	end
	return str
end
function OPENCL:is_ready_to_run()	return param.get_bool( self.ref.ready_to_run )		end
function OPENCL:restart()			param.set( self.ref.restart_trig, true )			end
function OPENCL:trig_edit()			param.do_action_open( self:get_obj(), "filename" )	end

function OPENCL:create( name, obj )
	local self = OPENCL:create_instance_with_obj( name, obj )
	self:build_all_ref()
	return self
end

aaa.show_file_end( "OPENCL" )
