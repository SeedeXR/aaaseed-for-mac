--
--	FBO
--		encapsulate an fbo
--
if CLASS.DECLARE( "FBO" ) then
	FBO:set_class_status_doc(	CLASS.STATUS.CORE,
								"encapsulate the c_fbo object: frame buffer object" )
end

function FBO:__build_ref()
	local gr = param.get_ref
	local ref = self.ref
	local fbo = ref.obj

	ref.active			=	gr(	fbo,	"active"				)
	ref.valid			=	gr(	fbo,	"valid"					)
	ref.size_x			=	gr(	fbo,	"size_x"				)
	ref.size_y			=	gr(	fbo,	"size_y"				)
	ref.size_asked_x	=	gr(	fbo,	"size_asked_x"			)
	ref.size_asked_y	=	gr(	fbo,	"size_asked_y"			)
	ref.channel_nb		=	gr(	fbo,	"channel_nb"			)
	ref.channel_type	=	gr(	fbo,	"channel_type"			)

	ref.depth = {}
	ref.depth.active			=	gr(	fbo,	"depth_use"				)
	ref.depth.as_texture		=	gr(	fbo,	"depth_as_texture"		)
	ref.depth.bind				=	gr( fbo,	"depth_bind_2d_asked"	)
	ref.depth.on_cpu			=	gr(	fbo,	"depth_on_cpu"			)	
	ref.depth.on_cpu_pbo_use	=	gr(	fbo,	"depth_on_cpu_use_pbo"	)	

	ref.stencil_use		=	gr(	fbo,	"stencil_use"			)
	ref.attach			=	gr(	fbo,	"attach"				
)

	local texs = {}
	ref.texs = texs
	for i=1,8 do
		local tex = {}
		texs[i] = tex
		local pre = "tex_"..i.."_"
		tex.active				=	gr( fbo, 	pre.."active"			)
		tex.bind				=	gr(	fbo,	pre.."bind_2d_asked"	)
		tex.on_cpu				=	gr(	fbo,	pre.."on_cpu"			)
		tex.on_cpu_pbo_use		=	gr(	fbo,	pre.."on_cpu_use_pbo"	)

		tex.define_format		=	gr( fbo, 	pre.."define_format"	)
		tex.channel_nb			=	gr( fbo, 	pre.."channel_nb"		)
		tex.channel_type		=	gr( fbo, 	pre.."channel_type"		)
		tex.mipmap_generate		=	gr(	fbo,	pre.."mipmap_generate"	)
	end

	local bdd = aaa.obj.get_down_by_class( fbo, "bdd_clear_screen" )
	ref.bdd_clear_screen = BDD_CLEAR_SCREEN:create( self:get_name(), bdd )
	
end

function FBO:create( name, fbo, layers )
	local self = FBO:create_instance_with_obj( name, fbo )
	self.ref.layers = layers
	self:__build_ref()
	return self
end
function FBO:set_focus()
	local fbo = self.ref.obj
	aaa.obj.set_focus_ui( fbo )
end

function FBO:set_pixel_size( sx,sy )
	if type(sx) == "string" then
		sx,sy = PIXEL_SIZE:get_sxy( sx )
	end
	local ref = self.ref
	if sx then param.set( ref.size_asked_x,	sx	) end
	if sy then param.set( ref.size_asked_y,	sy	) end
end
function FBO:get_pixel_size()
	local ref = self.ref
	local get = param.get
	return get(	ref.size_x ), get( ref.size_y )
end

function FBO:is_valid()						return param.get_bool( self.ref.valid )				end
function FBO:is_attach()					return param.get_bool( self.ref.attach )			end

function FBO:get_channel_nb()				return param.get( self.ref.channel_nb )				end
function FBO:set_channel_nb( nb )			return param.set( self.ref.channel_nb, nb )			end

function FBO:get_channel_type()				return param.get( self.ref.channel_type )			end
function FBO:set_channel_type( ch_type )	return param.set( self.ref.channel_type, ch_type )	end

function FBO:get_channel_nb_type()
	return self:get_channel_nb(), self:get_channel_type()
end
function FBO:set_channel_nb_type( ch_nb, ch_type )
	self:set_channel_nb( ch_nb )
	self:set_channel_type( ch_type )
end

function FBO:get_pixel_size_channel_type()
	local ref = self.ref
	local get = param.get
	return get(	ref.size_x ),get( ref.size_y ),	get( ref.channel_nb ),	param.get_str( ref.channel_type )
end

--todo pass bind in array with integer
--,ake depth fns more symetric
function FBO:__get_tex_table( id )
	if type(id)=="string" then
		return id=="depth" and self.ref.depth or nil
	end
	return self.ref.texs[id]
end
function FBO:set_attachment_active( id, b )
	local tex_table = self:__get_tex_table( id )
	if tex_table then param.set( tex_table.active, b ) end
end
function FBO:get_attachment_active( id )
	local tex_table = self:__get_tex_table( id )
	if tex_table then return param.get_bool( tex_table.active ) end
end

function FBO:set_attachment_bind( id, bind )
	local tex_table = self:__get_tex_table( id )
	if tex_table then
		if type(bind)=="string" then
			bind = TEXS:get_bind_by_name( bind )
		end		
		param.set( tex_table.bind, bind )
	end
end
function FBO:get_attachment_bind( id )
	local tex_table = self:__get_tex_table( id )
	if tex_table then return param.get( tex_table.bind ) end
end

function FBO:set_attachment_tex_on_cpu( id, b, b_pbo_use )
	local tex_table = self:__get_tex_table( id )
	if tex_table then
		param.set( tex_table.on_cpu, b )
		if b_pbo_use~=nil then
			param.set( tex_table.on_cpu_pbo_use, b_pbo_use )
		end
	end
end
function FBO:is_attachment_tex_on_cpu( id )
	local tex_table = self:__get_tex_table( id )
	if tex_table then return param.get_bool( tex_table.on_cpu ) end
end

function FBO:set_attachment_mipmap_generate( id, b )
	if type(id)=="string" then return end
	local tex_table = self:__get_tex_table( id )
	if tex_table then param.set( tex_table.mipmap_generate, b ) end
end
function FBO:is_attachment_mipmap_generate( id )
	if type(id)=="string" then return false end
	local tex_table = self:__get_tex_table( id )
	if tex_table then return param.get_bool( tex_table.mipmap_generate ) end
end

function FBO:set_attachment_channel_type( id, ch_nb, ch_type, b_define_format ) --todofbo deal with define format
	if type(id)=="string" then return end
	local tex_table = self:__get_tex_table( id )
	if tex_table then
		if ch_nb then
			param.set( tex_table.channel_nb,   ch_nb )
		end
		if ch_type then
			param.set( tex_table.channel_type, ch_type )
		end
		--todofbo
		param.set( tex_table.define_format,	b_define_format )
	end
end

function FBO:set_attachment_depth_as_texture( b )
	local tex_table = self:__get_tex_table( "depth" )
	if tex_table then
		param.set( tex_table.as_texture, b )
	end
end
function FBO:get_attachment_depth_as_texture()
	local tex_table = self:__get_tex_table( "depth" )
	if tex_table then
		return param.get_bool( tex_table.as_texture )
	end
end
function FBO:get_bdd_clear_screen() return self.ref.bdd_clear_screen	end

function FBO:update()
--	MEU.__fbo_cur = self
	local obj = self:get_obj()
	aaa.obj.update( obj )
end
function FBO:draw()
--	MEU.__fbo_cur = self
	local obj = self:get_obj()
	aaa.obj.draw( obj )
end
