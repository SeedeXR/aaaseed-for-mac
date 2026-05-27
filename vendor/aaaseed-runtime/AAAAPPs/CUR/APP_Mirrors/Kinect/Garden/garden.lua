if APP.DECLARE( "GARDEN_MIN", APP_GP ) then
end

local L_APP = GARDEN_MIN

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

