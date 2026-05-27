-- MEU Rendering 
--
function MEU:set_alpha( alpha )
	--self:print( "alpha "..alpha )
	self.__alpha_meu = alpha
end
function MEU:get_alpha()
	return self.__alpha_meu
end

MEU.doc.is_render = "() can be redefined for each MEU.\nis_render(val)\nval is the MU value\nthis default fn return true for val > 0"
function MEU:is_render()
	if self.verbose >= 2 then aaa.print_method() end
	return self:get_mu():is_render()
end

--function MEU:update()				end
--todo extend and refine for other stuff
--todo deal with module case
function MEU:__do_before_draw_layers()
	--aaa.print_fn()
	local trs = self.__transfo
	if trs then
		--table.print( trs )
		self:__update_transfo( trs )
	end
end 
function MEU:draw()
	self:__do_before_draw_layers()
	aaa.obj.update_then_draw(	self.ref.__obj_main 		)
end
--todoaqua refine for module
function MEU:draw_layers_begin()
	self:__do_before_draw_layers()
	aaa.layers.draw_begin(		self.ref.__layers_main 		)
end
function MEU:draw_layers_end()		aaa.layers.draw_end(		self.ref.__layers_main		)	end
function MEU:draw_layers()			aaa.layers.draw_layer_all(	self.ref.__layers_main		)	end
function MEU:draw_layer( layer_id )
	local layer = self:get_layer( layer_id )
	if layer then
		aaa.obj.update_then_draw( layer	)
		return
	end
	--self:print_error( "No layer "..layer_id.." to draw" )
	self:show_error( "No layer "..layer_id.." to draw")
end
function MEU:draw_layer_two( id1, id2 )
	local layer = self.ref.layer
	local utd = aaa.obj.update_then_draw
	utd( layer[id1].obj )
	utd( layer[id2].obj )
end
function MEU:draw_layer_three( id1, id2, id3 )
	local layer = self.ref.layer
	local utd = aaa.obj.update_then_draw
	utd( layer[id1].obj )
	utd( layer[id2].obj )
	utd( layer[id3].obj )
end

function MEU:__register_to_mus()
	--self:print( "__register_to_mus() ".. MUS.get_cur() )
	local mus = MUS.get_cur()
	if mus then
		--self:print( "__register_to_mus() ".. MUS.get_cur() )
		mus:register_meu_last_rendered_by_type( self )
	else
		aaa.debug.print_traceback()
		self:print_error( "in MEU:__register_to_mus() we got a nil mus" ) 
	end
end

-- function MEU:draw_registered()
-- 	MEU.draw( self )	--call from draw so we avoid self calling
-- 						--todo clean by using a clean documented strategy
-- 	self:__register_to_mus()
-- end


function MEU:set_restoring( b )			self.__b_restoring = b		end
function MEU:is_restoring()				return self.__b_restoring	end

function MEU:get_render_index()			return self.__render_index	end
function MEU:set_render_index( index )
	--self:print( meu.." ".. index .. " vs " .. GP.cur:get_render_index() )
	index = index or GP.cur:get_render_index()
	self.__render_index	= index
end

function MEU:is_rendered_in_this_frame()
	--self:print( self:get_mu():get_render_index().." / "..GP.cur.get_render_index() )
	--return self:get_mu():get_render_index() == GP.cur:get_render_index()
	return self:get_render_index() == GP.cur:get_render_index()
end
function MEU:__begin_meu_render( alpha )
	-- ALPHA
	self.__alpha_meu = alpha
	local col = self.ref.color
	if col then
		col:set_alpha( alpha )
	end

	--build the rendering list
	local meu_prev = MEU.__meu_prev
	self.__meu_prev = meu_prev
	if meu_prev then
		meu_prev.__meu_next = self
	end

	self.__meu_render_count = self.__meu_render_count + 1
end
function MEU:__end_meu_render()
	self.__meu_fbo_used = MEU.__meu_fbo_cur	--we do it after in case the MEU use another fbo
	MEU.__meu_prev = self
	MEU_CTX.cur.__inst_last_by_type[self:get_meu_type_lower()] = self
	local render_index = GP.cur:get_render_index()
	self:set_render_index( render_index )
	local meu_used = self:get_meu_used()
	if meu_used ~= self then
		meu_used:set_render_index( render_index )
	end
end


function MEU:render( alpha )
	--self:print( "render" )
	if self:is_proto_and_isolated() then
		--	signal a problem
		self:print( "should never render() a pure proto" )
		--	we should never in particular update_then_draw( self.ref.__obj_main )
		aaa.debug.print_traceback()
		return
	end

	--perhaps we should still call begin/end like below
	if self.__b_define_ui_needed then
		if self.verbose > 0 then
			self:print_debug( "\tinit or define was not succesfull so we don't update or draw" )
		end	
		return
	end

	local timer
	if MEU.__b_timings then
		timer = self.__timer_render
		if not timer then
			timer = TIMER:create( self:get_name() )
			self.__timer_render = timer
		end
		timer:start()
	end

	-- set global meu variable so the MEU script can be executed correctly
	_G.meu = self

	--todo clean (destroy or revive/document)
	if self.lerp then self:do_lerp() end
	
	--self:print( "render( "..alpha.." )" )
	self:__begin_meu_render( alpha )

		-- update then draw
		--self:print( "__b_meu_update "..self.__b_meu_update )
		local update = self.__b_meu_update and self.update
		local draw   = self.__b_meu_draw   and self.draw
		if GA.b_spy then
			if update then
				aaa.spy.push_range( self.." Update()", 1 )
					update( self )
				aaa.spy.pop_range()
			end
			if draw then
				aaa.spy.push_range( self.." Draw()", 2 )
					draw(   self )
				aaa.spy.pop_range()
			end
		else
			if update then
				--self:print( "update" )
				update( self )
			end
			if draw then
				--self:print( "draw" )
				draw( self )
			end
		end

	self:__end_meu_render()

	if timer then
		timer:store()
	end
end

