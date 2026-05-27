
if CLASS.DECLARE( "BU_SHADING", BUTTON, {
	__b_uif_outside			= false
	} ) then
	
	BU_SHADING:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
									"specialized button for shading" )
end

function BU_SHADING:get_shading()
	return self.__sha
end
function BU_SHADING:attach_shading( sha_or_sha_older )
	local sha
	if sha_or_sha_older then
		local class_name = sha_or_sha_older:get_class_name()
	--	self:print( "sha_or_sha_older is class "..sha_or_sha_older:get_class_name() )
		if class_name == "SHADING" then
			sha = sha_or_sha_older
		else
			sha = sha_or_sha_older:get_shading()
		end
	end
	self.__sha = sha
	if sha then
		self:set_target_param( sha.ref.active )
	end
end

--todo clean like the others BU
function BU_SHADING:create( name, rect, sha_or_sha_older )	
	local self = BU_SHADING:create_instance( name, rect )

	self:set_text( "Shader" )
	self:attach_shading( sha_or_sha_older )

	return self
end

function BU_SHADING:draw()
	local sha = self:get_shading()
	if sha then
		self:set_text_color_problem_white( sha:is_valid() )
	end
	oo.getsuper(BU_SHADING).draw( self )
end

function BU_SHADING:set_focus()
	local sha = self.__sha
	if sha then sha:set_focus() end
end
function BU_SHADING:edit_shader( what )
	local sha = self.__sha
	if sha then sha:edit_shader( what ) end
end
function BU_SHADING:reload()
	local sha = self.__sha
	if sha then sha:trig_reload() end
end

--	bu_click:set_method_on_click_double(	self, "__trig_shading_edit" )
function BU_SHADING:do_click_down( x,y )
	oo.getsuper(BUTTON).do_click_down( self, x,y )	--we need this to have UIF 
end

function BU_SHADING:get_uif_zones()
	local zones = {} --oo.getsuper(BU_MEU).get_uif_zones(self)
	local da = 1/32
	zones.Focus			= {	angle=3/4,			dangle=4/32,	dist=1,	}
	--local edit_angle = 28.5/32
	--zones.Edit		= {	angle=edit_angle,		dangle=5/32,	dist=1.3,	dy=.05,	}
	zones.Reload		= {	angle=1/2+1/8,		dangle=2/16,	dist=1.3,	dy=.05,	}

	local SX,SY = 2,1
	local X,Y = 3,0
	local DIST = 0
	local DD = .1
	zones.Vert	= { type="button",		angle=3/4,		dist=DIST,	rect={X-SX*.5-DD,	Y,	SX,SY}	}
	zones.Frag	= { type="button",		angle=3/4,		dist=DIST,	rect={X+SX*.5+DD,	Y,	SX,SY}	}
	SX = SX + .5
	DIST = DIST + SY + DD
	zones.Geom	= { type="button",		angle=3/4,		dist=DIST,	rect={X,			Y,	SX,SY}	}
	DIST = DIST + SY + DD
	zones.Comp	= { type="button",		angle=3/4,		dist=DIST,	rect={X+1,			Y,	SX,SY}	}
	--	zones.Dialog			= {	angle=3/4,			dangle=1/8,		dist=.7				-- type unused for now (2023 July)
	--	zones.But1				= { type="button",		angle=3/4,		dist=4,	rect={5,0},				},
	--	zones.TresGrosButtton	= {	type="button",		angle=3/4,		dist=4,	rect={-5,0,8},			},
	DIST = DIST + DD
	self:add_uif_zones_base( zones, DIST, -3.8 )
	return zones
end

function BU_SHADING:do_uif_command( uif )
	aaa.print_fn()
	local command = BU:get_uif_name_sel( uif )
	local b_used = true
	if		command == "edit"	then	self:edit_shader()
	elseif	command == "focus"	then	self:set_focus()
	elseif	command == "reload"	then	self:reload()
	elseif	array.find_index_by_val( {"vert", "geom", "frag", "comp" }, command ) then
		self:edit_shader( command )
	else
		b_used = false
	end

	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, BU_SHADING )
end

