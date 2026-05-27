--lua in Info
--old and unused for a big while (Maa june 2015)
-- todo : mode edition : move info box
local b_info_init_done
local info
local info_orig
local info_ref_tra_x
local info_ref_tra_y
local info_ref_sca_x
local info_ref_sca_y
local info_ref_la
local info_show
local b_info_init_done
local b_interact

local function info_save()
	if info ~= nil then
		aaa.print( "Saving Info File tree_info.txt" )
		local f = io.open( "tree_info.txt", "w" )
		local 	charE = "\n"
		info.x = param.get( info_ref_tra_x )
		info.y = param.get( info_ref_tra_y )
		info.sx = param.get( info_ref_sca_x )
		info.sy = param.get( info_ref_sca_y )
		local	str = info.img_nb..";"..info.x..";"..info.y..";"..info.sx..";"..info.sy..";"..charE
		aaa.print( str )
		f:write( str )
		f:close()
	else
		aaa.print_error( "Can't Save Info File" )
	end
end

local function info_load()
	local f = io.open( "tree_info.txt", "r" )
	aaa.print( "Load Info file tree_info.txt" )
	if f then
		info = {}
		local line = f:read()
		local t = table.from_csv( line, ";" )

		info.img_nb = tonumber( t[1] )
		info.x = tonumber( t[2] )
		info.y = tonumber( t[3] )
		info.sx = tonumber( t[4] )
		info.sy = tonumber( t[5] )

		param.set( "GaBu/Multitouch/Info/fx_a.layer", "bind_2d", info.img_nb )

		f:close()
		aaa.print( table.show( info, "info" ) )
	end
end

local function info_reload()
	aaa.print( "Reload Info" )
	info_load()
	info_orig = table.copy_deep( info )
end

local function info_init()
	aaa.print( "Info init script" )

	info_load()
	info_orig = table.copy_deep( info )
	info_ref_tra_x = param.get_ref( "GaBu/Multitouch/Info/fx.transfo", "translate_x" )
	info_ref_tra_y = param.get_ref( "GaBu/Multitouch/Info/fx.transfo", "translate_y" )
	info_ref_sca_x = param.get_ref( "GaBu/Multitouch/Info/fx.transfo", "scale_x" )
	info_ref_sca_y = param.get_ref( "GaBu/Multitouch/Info/fx.transfo", "scale_y" )
	info_ref_la = param.get_ref( "GaBu/Multitouch/Info/fx_a.layer", "active" )
	aaa.print( tostring(info_ref_tra_x ) )
	aaa.print( tostring(info_ref_tra_y ) )
	aaa.print( tostring(info_ref_sca_x ) )
	info_show = 1
	b_info_init_done = 1
	b_interact = false
end

local function info_is_interacting()
	return b_interact
end

local function info_reset()
	if info_orig ~= nil then
		aaa.print( "Reset Info" )
		info = table.copy_deep( info_orig )
	end
end

local function info_is_inside( x, y )
	local	cx = x - info.x
	local	cy = y - info.y

	local 	dx = info.sx * 0.5
	local 	dy = info.sy * 0.5

	if math.abs( cx ) < dx and math.abs( cy ) < dy then
		return true
	end
	return false
end

local function info_update()
	if not info then return end

	info.img_nb = param.get( "GaBu/Multitouch/Info/fx_a.layer", "bind_2d" )

--	if app_mode == 3 then
--		param.set( info_ref_tra_x, info.x )
--		param.set( info_ref_tra_y, info.y )
--		param.set( info_ref_sca_x, info.sx )
--		param.set( info_ref_sca_y, info.sy )
--	end
	if show_info then
		 param.set( info_ref_la, 1 )
	else
		 param.set( info_ref_la, 0 )
	end
end

local function info_touch( i )
	b_interact = true
	aaa.print( "Ask Info for images "..i )
	aaa.net.osc_send( 1, "/erasme/sensor/matable/0/enter", i )
	aaa.net.osc_flush( 1 )
end

local function info_untouch( i )
	b_interact = false
	aaa.print( "Remove Info for images "..i )
	aaa.net.osc_send( 1, "/erasme/sensor/matable/0/leave" )
	aaa.net.osc_flush( 1 )
end
--f = loadfile("util.lua")
--f()
--b_info_init_done = nil
if b_info_init_done ~= 1 then
	info_init()
end
--rects_init()
--aaa.print( "pass in lua" )
--	aaa.print( table.show( rects, "rects" ) )
--b_interact = false
info_update()
--info_save()
--info_reload()