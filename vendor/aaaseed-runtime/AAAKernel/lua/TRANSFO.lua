aaa.show_file_begin( "TRANSFO" )

--
--	TRANSFO TRS
--
if CLASS.DECLARE( "TRANSFO_TRS" ) then
	TRANSFO_TRS:set_class_status_doc(	CLASS.STATUS.CORE,
										"encapsulate interface to a transfo_trs c_obj_ui" )
end

--todo make reading of all param auto for all obj
function TRANSFO_TRS:build_all_ref()
	local ref = self.ref
	local obj = ref.obj
	local pgr = param.get_ref

	local function get( name )
		local tab = {
					pgr(obj,name.."_x"),  pgr(obj,name.."_y"),  pgr(obj,name.."_z"), 
					pgr(obj,name)
					}
		return tab
	end

	ref.active = pgr( obj, "active" )

	ref.tra = get( "translate" )
	ref.rot = get( "rotate" )
	ref.sca = get( "scale" )

	return ref
end

function TRANSFO_TRS:set_active( b )
	param.set( self.ref.active, b )
end
function TRANSFO_TRS:set_translate( x,y,z, b )
	--aaa.print_fn()
	--self:print( x,y,z )
	local tref = self.ref.tra
	param.set( tref[1], x )
	param.set( tref[2], y )
	param.set( tref[3], z )
	if b~=nil then
		param.set( tref[4], b )
	end
end
function TRANSFO_TRS:set_rotate( x,y,z, b )
	local tref = self.ref.rot
	param.set( tref[1], x )
	param.set( tref[2], y )
	param.set( tref[3], z )
	if b~=nil then
		param.set( tref[4], b )
	end	
end
function TRANSFO_TRS:set_scale( x,y,z, b )
	local tref = self.ref.sca
	param.set( tref[1], x )
	param.set( tref[2], y )
	param.set( tref[3], z )
	if b~=nil then
		param.set( tref[4], b )
	end
end

function TRANSFO_TRS:create( name, obj )
	if not obj then
		obj = app:create_obj_by_cid( "transfo_trs" )
	end
	local self = TRANSFO_TRS:create_instance_with_obj( name, obj )
	self:build_all_ref()
	return self
end

--
--	TRANSFO THREE
--
if CLASS.DECLARE( "TRANSFO_THREE" ) then
	TRANSFO_THREE:set_class_status_doc(	CLASS.STATUS.CORE,
										"encapsulate a C 2d transformation object defined using 3 points" )
end

--todo make reading of all param auto for all obj
function TRANSFO_THREE:build_all_ref()
	local ref = self.ref
	local obj = ref.obj

	ref.type			=	param.get_ref( obj, "type"	)
	ref.axe				=	param.get_ref( obj, "axe"	)
	ref.scale			=	param.get_ref( obj, "scale"	)
	ref.inter			=	param.get_ref( obj, "inter"	)

	ref.start_channel	=	param.get_ref( obj, "start_channel"	)
	ref.start_node		=	param.get_ref( obj, "start_node"	)
	ref.stop_channel	=	param.get_ref( obj, "stop_channel"	)
	ref.stop_node		=	param.get_ref( obj, "stop_node"		)
	ref.third_channel	=	param.get_ref( obj, "third_channel"	)
	ref.third_node		=	param.get_ref( obj, "third_node"	)

	return ref
end

function TRANSFO_THREE:set_type( v )				param.set( self.ref.type, v )			end
function TRANSFO_THREE:set_axe( v )					param.set( self.ref.axe, v )			end
function TRANSFO_THREE:set_scale( v )				param.set( self.ref.scale, v )			end
function TRANSFO_THREE:set_inter( v )				param.set( self.ref.inter, v )			end

function TRANSFO_THREE:set_start_channel( channel )	param.set( self.ref.start_channel, channel )	end
function TRANSFO_THREE:set_stop_channel( channel )	param.set( self.ref.stop_channel, channel )		end
function TRANSFO_THREE:set_third_channel( channel )	param.set( self.ref.third_channel, channel )	end
function TRANSFO_THREE:set_channel( channel )
	local ref = self.ref
	param.set( ref.start_channel, channel )
	param.set( ref.stop_channel, channel )
	param.set( ref.third_channel, channel )
end

function TRANSFO_THREE:set_start_node( node )	param.set( self.ref.start_node, node )	end
function TRANSFO_THREE:set_stop_node(  node )	param.set( self.ref.stop_node,  node )	end
function TRANSFO_THREE:set_third_node( node )	param.set( self.ref.third_node, node )	end

function TRANSFO_THREE:create( name, obj )
	if not obj then
		obj = app:create_obj_by_cid( "transfo_three" )
	end
	local self = TRANSFO_THREE:create_instance_with_obj( name, obj )
	self:build_all_ref()
	return self
end

aaa.show_file_end( "TRANSFO" )
