function meu:define_meu_infos( )
	return { author = "Mâa", date="2025",
			tags = { "Tutorial" },
			help = "example about the GABU_OBJ base structure"
			 }
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
    local ix,iy = 1,1
    local SY,DY = 1,.2  
 
    self:add_rendering()
	self:add_camera()

    iy = iy + SY + DY
    bu = self:add_button(   {ix,iy,	4,1},  "GABU_OBJ" )
        bu:set_method_on_click( self, "print_hierarchy" )
end

function GABU_OBJ:print_class_custom( level )
	local name = self:get_class_name()
	--self:print( "print_classes( "..level.." "..name.." )" )
--	table.print( class[CLASS.__str_class_field], name, 2 )
--	table.print( classes_by_name, CLASS..".classes_by_name" )

    local prefix = "  "

	local doc, status = self:get_class_doc(), self:get_class_status()
	status = string.len(status)>0 and status or nil
    local super = self:get_class_super()

    local str = status and name.." status is "..status.."," or name
    if super then str = "CLASS "..str.." SUPER is "..super:get_class_name() end
	aaa.print( str )

    local classes_by_name = self:get_classes_by_name()
    if next(classes_by_name)~=nil then
        str = prefix .. "derived class are "		
        for name, class_down in pairs_sorted(classes_by_name) do
           str = str..name.." "
        end
        aaa.print( str )
    end

    self:print( "  " ..self:get_created_nb().." instances created" )
    if doc then
        if type(doc) == "table" then
            local str
            for _,line in ipairs(doc) do
                aaa.print( line )  
            end
        else
            doc = string.len(doc)>0 and doc or nil
            if doc then
                aaa.print( doc )     
            end
        end
    end
end
-- function GABU_OBJ:print_class_custom( level )
-- 	local name = self:get_class_name()
-- 	--self:print( "print_classes( "..level.." "..name.." )" )
-- --	table.print( class[CLASS.__str_class_field], name, 2 )
-- --	table.print( classes_by_name, CLASS..".classes_by_name" )

-- 	local doc, status = self:get_class_doc(), self:get_class_status()
--     if doc==nil or string.len(doc)==0 then
--         self:print( "have mo doc" )
--     end   
-- end

function meu:print_hierarchy()
    --local t = self:print_classes( GABU_OBJ )
    self:print_inverse( "---------------------------------------" )
    --table.print( t, "sumup", 1)
    --for i,l in ipairs(t) do
     --   self:print( l )
    --end
    GABU_OBJ:parse_class( 0, 3, "print_class_custom" )
end

function meu:draw()
    --self:draw_layers_begin()
    --   self:draw_layer( 1 )
    --self:draw_layers_end()
end
