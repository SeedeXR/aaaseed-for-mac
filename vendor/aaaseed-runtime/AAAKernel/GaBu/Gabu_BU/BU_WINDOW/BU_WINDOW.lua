if CLASS.DECLARE( "BU_WINDOW", BU ) then
end

--BU_WINDOW_LIST.create_instance = nil
-- function BU_WINDOW.create_instance( class, name, rect )
-- 	local self = oo.getsuper(BU_WINDOW).create_instance( class, name, rect )

-- 	--self.ui = {}
-- 	return self
-- end

function BU_WINDOW:do_key(key)
	self:print_do_key( "BU_WINDOW", key )

	local b_key_used = false
	if aaa.keyboard.is_alt() then
	else
	end

	return b_key_used or oo.getsuper(BU_WINDOW).do_key( self, key )
end

function BU_WINDOW:do_key_special(key)
	self:print_do_key_special( "BU_WINDOW", key )

	local b_key_used = false
	if not self:is_mini() and not self.__b_fix then
		if aaa.keyboard.is_alt() then
		end
	end

	return b_key_used or oo.getsuper(BU_WINDOW).do_key_special( self, key )
end
