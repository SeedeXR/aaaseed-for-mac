if CLASS.DECLARE( "MEU_REF",	MEU ) then
	MEU_REF:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
									"MEU_REF reference another MEU the same way in a file folder a link reference another folder.",
									"using a reference MEU you can use the same MEU several times in a rendering chain" )
end

function MEU_REF:define_meu_infos()
	return { author = "Mâa",
			tags = { "Core"	},
		}
end

-- function MEU_REF:define_ui()	--we need it even if it does nothing so init will be done
-- end
--todo perhaps is too simple
-- update, update_ui, render ?
function MEU_REF:__get_meu_referred()
	if self:is_proto() then
		return
	end

	local m = self.__meu_target
	if m and m.__b_freed then
		self:print_debug( "Ref reference a MEU which was freed : "..m.." this MEU just forget it now" )
		self.__meu_target = nil
		m = nil
	end

	--todo this should not be called when we free and GP:unregister_mu
	if not m then
		--self:box_debug( "init self.__meu_target" )
		local meu_type, inst = MEU_CTX.cur:split_meu_type_inst( self:get_name() )
		--	self:box_debug( "__get_meu_referred "..meu_type.." "..inst )
		--todo refine search up doping level by level
		--self:print( "inst is "..inst )
		--m = app:get_meu_by_name_no_error( inst )

		--self:print_debug( "Ref search "..inst )
		m = self:get_meu_by_name_no_error( inst )
		if not m then	--if not found at the same level or below search from the top
			m = app:get_meu_by_name_no_error( inst )
		end
		if m then
			self.__meu_target = m
--			m:add_meu_using( self )
		else
			self:show_error( "Ref to a non existing Meu instance "..inst )
		end
	end
	return m
end

-- function MEU_REF:forget_meu( m )
-- 	if self.__meu_target == m then
-- 		self.__meu_target = nil
-- 	end
-- end

function MEU_REF:get_meu_used()
	local ref = self:__get_meu_referred()
	--self:print( " --> " ..ref )
	--self:print( " --> " ..(ref or self) )
	return ref or self
end

function MEU_REF:draw_icon()
	if not self:is_proto() then
		local ref = self:__get_meu_referred()
		if ref then
			ref:draw_icon()
		end
		gol.set_texture_dim(0)
		gol.color_yellow()
		gol.set_line_width( 1.5 )
		local x,y = -.44, -.25 -- -.435,-.2
		gol.draw_lines_2d(	-.5,y,	x,y,
							x,y, 	x,-.5,
							x,y,	-.53,-.625	)
	end
end

function MEU_REF:update()
	--self:print( "REF update" )
	local ref = self:__get_meu_referred()
	if ref then
		ref:update()
	end
end
function MEU_REF:draw()
	--self:print( "REF draw" )
	local ref = self:__get_meu_referred()
	if ref then
		ref:draw()
	end
end
