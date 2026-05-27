if CLASS.DECLARE( "BU_KEY", BUTTON,  {
	__value_symbo 	= nil,
	__keyboard		= nil,
	__repeat_time	= .5,
	ref = {},
	} ) then
	BU_KEY:set_class_status_doc(	CLASS.STATUS.GABU,
									"specialization of BU used by KEYBOARD" )
end

local special_key = {}
special_key.RETURN = 13
special_key.BACKSPACE = 8
special_key.SPACE = 32

function BU_KEY:set_value_symbo( val )	self.__value_symbo = val 	end
function BU_KEY:get_value_symbo()		return self.__value_symbo	end

function BU_KEY:set_attach_keyboard( keyboard ) self.__keyboard = keyboard 	end
function BU_KEY:get_attach_keyboard()			return self.__keyboard		end

function BU_KEY:do_event( type )
	local str = self:get_text()
	local keyboard = self:get_attach_keyboard()
	local bu = keyboard:get_attach_bu()
	local key, alteration

	-- it's a special key
	if str:len() > 1 then
		-- get special key
		local val = self:get_value_symbo()
		-- CTRL or ALT
		if val == "LCTRL" or val == "LALT" then
			if type ~= "up" then
				keyboard:set_alteration(val)
			else
				keyboard:set_alteration(nil)
			end
		-- shift
		elseif val == "LSHIFT" or val == "RSHIFT" then
			if type == "down" then
				--if shift locked, then cancel shift locked
				if keyboard:is_shift_locked() then
					keyboard:set_shift( "false" )
				--elseif keyboard already shift then shift lock
				elseif keyboard:is_shift() then
					keyboard:set_shift( "locked" )
				--else shift
				else
					keyboard:set_shift( "true" )
				end
			end
		-- right and left arrow
		elseif val == "LEFT" or val == "RIGHT" then
			key = val
		-- second layout
		elseif val == "TYPE" then
			if type == "down" then
				keyboard:switch_type()
			end
		-- third layout
		elseif val == "&123" then
			if type == "down" then
				if keyboard:is_123() then
					keyboard:set_123(false)
				else
					keyboard:set_123(true)
				end
			end
		else
			key = special_key[val]
		end
	-- it's a normal key
	else
		key = string.byte(str, 1)
			-- disable shift if needed
		if keyboard:is_shift() and not keyboard:is_shift_locked() then
			keyboard:set_shift( "false" )
		end
	end
		if type == "up" then
		-- reinit delay for pressed touch
		self.delay_from_push = 0
	elseif type == "pressed" then
		local dt = aaa.time.dt_real
		self.delay_from_push = self.delay_from_push + dt
		-- test if delay is enough to do the action again
		if self.delay_from_push > self.__repeat_time then
			if bu then
				bu:push_letter( key, keyboard:get_alteration(), keyboard )
			end
		end
	else
		-- reinit delay for pressed touch
		self.delay_from_push = 0
		if bu then
			bu:push_letter( key, keyboard:get_alteration(), keyboard )
		end
	end
end

function BU_KEY:do_click_down( x, y )
	oo.getsuper(BU_KEY).do_click_down(self, x, y)
	self:do_event( "down" )
end

function BU_KEY:do_click_up( x, y )
	oo.getsuper(BU_KEY).do_click_up(self, x, y)
	self:do_event( "up" )
end

function BU_KEY:do_mouse_move( x, y )
	oo.getsuper(BU_KEY).do_mouse_move(self, x, y)
	self:do_event( "pressed" )
end

function BU_KEY:draw_shift()
	local keyboard = self:get_attach_keyboard()
	self:gol_color_back()
	if keyboard:is_shift_locked() then
		gol.color_white( .5 )
	elseif keyboard:is_shift() then
		gol.color( {0, .5, 1, .5} )
	end
	self:gol_draw_rect_plain()
	self:__draw_text()
end

function BU_KEY:draw_123()
	local keyboard = self:get_attach_keyboard()
	self:gol_color_back()
	if keyboard:is_123() then
		gol.color_white( .5 )
	end
	self:gol_draw_rect_plain()
	self:__draw_text()
end

function BU_KEY:draw()
	--self:print("draw")

	if self.b_draw_shift then
		self:draw_shift()
	elseif self.b_draw_123 then
		self:draw_123()
	else
		oo.getsuper(BU_KEY).draw(self)
	end
end

function BU_KEY:__draw_text()
	if self:is_text_draw() then
		local text = self:get_text()

		self:draw_text_color_sel()

		local rx, ry = self:get_ratio_xy()
		local sx, sy = self:get_sxy()

		local SY
		if sy > .5 then
			SY = rx / ry
		else
			SY = .4
		end
		--todo this should be commented at least
		--Draw text inside bu
		local SX = 1.82
		--[[
		local rect = {}
		rect.l = -SX
		rect.r = SX
		rect.t = .4
		rect.b = .4 - SY
		self:draw_text_in_rect( text, rect, "left" )
		--]]
		local OX = 0
		local OY = -.2
		self:draw_text_lrbt( text, -SX+OX, SX+OX, OY, OY-SY, "center" )
	end
end

-- function BU_KEY:update()
-- 	--self:print( "update() ")

-- 	oo.getsuper(BU_KEY).update(self)
-- end

function BU_KEY:create( name, rect, text_size, keyboard )
	local self = BU_KEY:create_instance( name, rect )

	self:set_attach_keyboard( keyboard )
	self:set_trig()
	self:set_check_mul( false )

	return self
end


