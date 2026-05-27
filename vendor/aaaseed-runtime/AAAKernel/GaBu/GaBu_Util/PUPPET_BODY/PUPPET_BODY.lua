
if CLASS.DECLARE( "PUPPET_BODY_DEF" ) then
end

function PUPPET_BODY_DEF:init()
	self.part_def = {}
	self.part_def_by_name = {}
	self.frame_nb_total = 0
	return self
end
function PUPPET_BODY_DEF:process_descriptor( desc )
	self.desc = desc
	MEDIA.set_dir_media( self.desc.dir )
	local pre_name = desc.pre_name
	self:init()
	for i_part, t_part in IPAIRS( desc.parts ) do
		--table.print( t_part, i_part.." part", 2 )
		local name = t_part[1] 
		local part_def = PUPPET_PART_DEF:create( name )

		t_part[2].pre_name = pre_name
		part_def:set_descriptor( t_part[2] )

		table.insert( self.part_def, part_def )
		self.part_def_by_name[name] = part_def
		self.frame_nb_total = self.frame_nb_total + part_def.frame_nb_total
		--todo should we attach the part or the part_def or both
		local attach = t_part.attached_to
		if attach then
--			self:box_debug( "on passe pas la on dirait "..attach.."\n".. self:get_part( attach ) )
			part_def:attach_to( self:get_part_def( attach ) )
		end
	end
end
function PUPPET_BODY_DEF:create( name, desc )
	local self = PUPPET_BODY_DEF:create_instance( name )
	self:init()
	self:process_descriptor( desc )
	return self
end
function PUPPET_BODY_DEF:load()
	MEDIA.set_dir_media( self.desc.dir )
	for _, part_def in IPAIRS( self.part_def ) do
		part_def:load()
	end
end
function PUPPET_BODY_DEF:dump( pre )
	pre = pre or ""
	for _, part in IPAIRS( self.part_def ) do
		part:dump( pre.."\t" )
	end
	self:print( "total frame : "..self.frame_nb_total )
	table.print( self, "obj "..self, 3 )
end


function PUPPET_BODY_DEF:get_part_def( name )
	return self.part_def_by_name[name]
end


if CLASS.DECLARE( "PUPPET_BODY" ) then
end

function PUPPET_BODY:init( body_def )
	--todo create parts here
	self.parts = {}
	self.parts_by_name = {}
	for _, part_def in IPAIRS( body_def.part_def ) do
		local name = part_def:get_name()
		--self:box_debug( "create part "..name ) 
		local part = PUPPET_PART:create( name, part_def )
		table.insert( self.parts, part )
		self.parts_by_name[name] = part
	end
	self.__body_def = body_def
	return self
end
function PUPPET_BODY:create( name, body_def )
	if not body_def then
		self:box_debug( "No body_def" )
	end
	local self = PUPPET_BODY:create_instance( name )
	self:init( body_def )
	return self
end

function PUPPET_BODY:get_part( name )
	local part = self.parts_by_name[name]
	if not part then
		self:print_error( "No part "..name )
	end
	return part
end
function PUPPET_BODY:set_part_field( name, field, val )
	local part = self:get_part(name)
	if part then
		part[field] = val
	end
end

function PUPPET_BODY:get_body_def()
	return self.__body_def
end

function PUPPET_BODY:define_ui( meu, pre, ix,iy, SY )
 	local parts = self.parts

 	for i=#parts,1, -1 do
		local part =  parts[i]
 		iy = part:define_ui( meu, pre, ix,iy, SY )
 	end
 	return iy
end

function PUPPET_BODY:update_ui()
	for _, part in IPAIRS( self.parts ) do
		part:update_ui()
	end
end

function PUPPET_BODY:update( dt )
	for _, part in IPAIRS( self.parts ) do
		part:update( dt )
	end
end

function PUPPET_BODY:draw( x,y )
	local z = 0
	local dz = .1

	--self:print( "PUPPET_BODY:draw()" )
	for _, part in IPAIRS( self.parts ) do
		part:draw( x,y,z )
		z = z + dz
	end
	

end