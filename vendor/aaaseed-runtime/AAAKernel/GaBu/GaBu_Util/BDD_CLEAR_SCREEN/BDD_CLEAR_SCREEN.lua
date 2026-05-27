if CLASS.DECLARE( "BDD_CLEAR_SCREEN" ) then
	BDD_CLEAR_SCREEN:set_class_status_doc(	CLASS.STATUS.CORE,
											"encapsulate the C bdd_clear_screen object doing frame buffer erase" )
end

function BDD_CLEAR_SCREEN:__build_ref()
	local gr = param.get_ref
	local ref = self.ref
	local bdd = ref.obj

	ref.active		=	gr(	bdd,	"active"		)
	ref.color		=	gr(	bdd,	"color"			)
	ref.color_index	=	gr(	bdd,	"color_index"	)
	ref.depth		=	gr(	bdd,	"depth"			)
	ref.stencil		=	gr(	bdd,	"stencil"		)
	ref.trail_time	=	gr(	bdd,	"trail_time"	)
end
function BDD_CLEAR_SCREEN:create( name, obj )
	local self = BDD_CLEAR_SCREEN:create_instance_with_obj( name, obj )
	self:__build_ref()
	return self
end

function BDD_CLEAR_SCREEN:set_active( b )		       param.set( self.ref.active, b )		end
function BDD_CLEAR_SCREEN:get_active()			return param.get( self.ref.active )			end

function BDD_CLEAR_SCREEN:set_trail_time( v )	       param.set( self.ref.trail_time, v )	end
function BDD_CLEAR_SCREEN:get_trail_time()		return param.get( self.ref.trail_time )		end

function BDD_CLEAR_SCREEN:set_color_active( b )	       param.set( self.ref.color, b )		end
function BDD_CLEAR_SCREEN:get_color_active( b )	return param.get_bool( self.ref.color )		end
