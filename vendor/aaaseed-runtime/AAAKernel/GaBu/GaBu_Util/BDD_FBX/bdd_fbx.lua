--
--	BDD_FBX
--		encapsulate an a bdd_fbx
--
if CLASS.DECLARE( "BDD_FBX" ) then
	-- SHADING.master = { ref = {}}
	-- local ref = SHADING.master.ref
	-- ref.obj = aaa.obj.get_from_top_by_class( "shader_master" )
	-- ref.include_reload = param.get_ref( ref.obj, "include_reload_trig" )
	BDD_FBX:set_class_status_doc(	CLASS.STATUS.CORE,
									"encapsulate the C bdd_fbx object" )
end

function BDD_FBX:__build_ref()
	local gr = param.get_ref
	local ref = self.ref
	local obj = ref.obj
--	ref.active	=	gr(	obj,	"active"			)

	-- for i=1,8 do
	-- 	ref["bind_"..i]				=	gr(	fbo,	"tex_"..i.."_bind_2d_asked"	)
	-- 	ref["tex_"..i.."_active"]	=	gr( fbo, 	"tex_"..i.."_active"		)
	-- 	ref["tex_"..i.."_on_cpu"]	=	gr(	fbo,	"tex_"..i.."_on_cpu"		)
	-- end
	ref.fbx_loaded	= param.get_ref( obj, "fbx_loaded" )
end

function BDD_FBX:create( name, bdd ) --layer )
	local self = BDD_FBX:create_instance_with_obj( name, bdd )
	--self.ref.layers = layers
	self:__build_ref()
	return self
end

function BDD_FBX:is_fbx_loaded()			return param.get_bool(self.ref.fbx_loaded)		end

function BDD_FBX:get_filename()							return self:get_param( "filename" )					end
function BDD_FBX:set_filename( filename )				self:set_param( "filename", filename )				end
function BDD_FBX:set_filename_and_nosave( filename )	self:set_param_and_nosave( "filename", filename )	end
function BDD_FBX:load()									self:do_param_action_open( "filename" )				end
function BDD_FBX:reload()								self:set_param( "reload_trig", true )				end

function BDD_FBX:get_position()				return param.get_real_3( self:get_param_ref( "position_x" ) )  			end
function BDD_FBX:get_translation()			return param.get_real_3( self:get_param_ref( "translate_x" ) )  		end
function BDD_FBX:get_recenter_position()	return param.get_real_3( self:get_param_ref( "recenter_position_x" ) )  end
function BDD_FBX:get_rotation()				return param.get_real_3( self:get_param_ref( "rotate_x" ) )  			end

function BDD_FBX:set_time( time )			param.set( self:get_param_ref( "time" ), time  )  				end
function BDD_FBX:set_time_update( b )		param.set( self:get_param_ref( "time_update" ), b  )  			end
function BDD_FBX:is_time_update()			return param.get_bool( self:get_param_ref( "time_update" ) )  	end

function BDD_FBX:set_transformation_update( b )		param.set( self:get_param_ref( "transformation_update" ), b  )  			end
function BDD_FBX:is_transformation_update()			return param.get_bool( self:get_param_ref( "transformation_update" ) )  	end


function BDD_FBX:get_bbox_min()		return param.get_real_3( self:get_param_ref( "mesh_bbox_min_x" ) )  			end
function BDD_FBX:get_bbox_max()		return param.get_real_3( self:get_param_ref( "mesh_bbox_max_x" ) )  			end


function BDD_FBX:set_scale( x, y, z)
	param.set( self:get_param_ref( "scale_x" ), x )
	param.set( self:get_param_ref( "scale_y" ), y )
	param.set( self:get_param_ref( "scale_z" ), z )
end
function BDD_FBX:set_translation( x, y, z)
	param.set( self:get_param_ref( "translate_x" ), x )
	param.set( self:get_param_ref( "translate_y" ), y )
	param.set( self:get_param_ref( "translate_z" ), z )
end
function BDD_FBX:set_rotation( x, y, z)
	param.set( self:get_param_ref( "rotate_x" ), x )
	param.set( self:get_param_ref( "rotate_y" ), y )
	param.set( self:get_param_ref( "rotate_z" ), z )
end

local over_360 = 1. / 360.
function BDD_FBX:set_rotation_deg( x, y, z)
	param.set( self:get_param_ref( "rotate_x" ), x * over_360 )
	param.set( self:get_param_ref( "rotate_y" ), y * over_360 )
	param.set( self:get_param_ref( "rotate_z" ), z * over_360 )
end

BDD_FBX.tex_name_for_file	= {	"ALBEDO",	"NORMAL",	"ROUGHNESS",	"METALLIC",	"EMISSIVE"  }
BDD_FBX.tex_name			= {	"Albedo",	"Normal",	"Roughness",	"Metal",	"Emissive"	}

