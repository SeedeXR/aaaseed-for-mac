--EVENT
if (not EVENT) or (not EVENT.verbose) then
	EVENT = nil
	CLASS.DECLARE( "EVENT" )

	--mouse
	--todo do this on the C side
	local mouse = {}
	mouse.button_name = {
					"BUTTON_UNKNOWN",
					"BUTTON_LEFT",
					"BUTTON_MIDDLE",
					"BUTTON_RIGHT",
					"WHEEL_UP",
					"WHEEL_DOWN",
					"SCROLL_LEFT",
					"SCROLL_RIGHT",
					"BUTTON_X",
					"BUTTON_Y",
				}
	mouse.button_name[0] = "BUTTON_NONE"

	mouse.state_name = {
					"STATE_UNKNOWN",
					"DOWN",
					"UP",
					"DBLE_CLICK",
				}
	mouse.state_name[0] = "STATE_NONE"

	EVENT.mouse = mouse

	--print
	EVENT.header = "EVENT.hook : "
end

function EVENT.print( str )
	aaa.print( EVENT.header..str )
end


function EVENT.transform_move_coor( x,y )
	if EVENT.verbose > 1 then
		EVENT.print( "render coor : "..x..", "..y )
	end
	local blobs = ga:get_blobs()
	if blobs then
		x,y = blobs:convert_coor_to_blob( x,y, true )
		--EVENT.print( "blobs render coor : "..rx..", "..ry )
	end
	return x,y
end


-- x,y are in pixel coor relative to left/top
function EVENT.process_move_event( x,y )
	--aaa.print_fn()
	if not aaa.lua.global.get( "ga" ) then
		return false
	end

	if aaa.keyboard.is_alt_only() then
		ga:set_ui_intercept( nil )
		return false
	end
	
	--x = nil	--	to test bug in handling of error
	local x,y = aaa.mouse.convert_xy_pixel_to_render( x,y )
	x,y = EVENT.transform_move_coor( x,y )
	local bu = ga:find_bu_by_pos( x,y )
	if bu then
		if EVENT.verbose > 0 then
			EVENT.print( "found_bu "..bu )
		end
		if bu.obj and bu.obj.ref.focus then
			aaa.obj.set_focus_ui( bu.obj.ref.draw )
			ga:set_ui_intercept( bu.obj.ref.focus )
		else
			ga:set_ui_intercept( nil )
		end
		--hack need false to use multitouch simulator
		return not GABU.is_simulation()
		--return true
	else
		if EVENT.verbose > 1 then
			EVENT.print( "no bu at "..x..", "..y )
			--BUS.set_ui_intercept( nil )
		end
	end
	return false
end

function EVENT.hook( e_type, e_sub_type, a,b,c,d,e )
	if app and app.event_hook_before and app:event_hook_before( e_type, e_sub_type, a,b,c,d,e ) then return true end
	-- if true then return true end
	-- if true then return false end
	if GABU_OBJ.b_box_on then return false end

	if EVENT.verbose > 1 then
		EVENT.print( e_type.."\t"..e_sub_type.."\t"..a.." "..b.." "..c.." "..d.." "..e )
	end

	local ret = false
	local mouse = EVENT.mouse
	if e_type == "mouse" then
		if e_sub_type == "move" then
			if EVENT.verbose > 1 then
				EVENT.print( "Move "..a.." "..b )
			end
			--if c==1 then
--todo we commented it because it was slowing down AAAseed a Lot
				--EVENT.process_move_event( a, b )
				--ret = EVENT.b_mouse_consumed or false
			--end
		elseif e_sub_type == "button" then
			local but_name = mouse.button_name[c]
			local state_name = mouse.state_name[d]
			if EVENT.verbose > 0 then
				EVENT.print( but_name.." -> "..state_name )
			end
			--if b==0 then
			if state_name == "DOWN" then
				if aaa.flatland.is_in( a,b ) then
					aaa.mouse.set_grabbed( false )	
					if EVENT.verbose > 0 then
						EVENT.print( "In flatland" )
						EVENT.print( "Set mouse grabbed to false" )
					end		
				elseif but_name == "BUTTON_LEFT" then
					local b_used = EVENT.process_move_event( a, b )
					aaa.mouse.set_grabbed( b_used )
					if EVENT.verbose > 0 then
						EVENT.print( "Set mouse grabbed to "..b_used )
					end
					--EVENT.b_mouse_consumed = EVENT.process_move_event( a, b )
					--ret = EVENT.b_mouse_consumed
				end
			elseif state_name == "UP" then
				--aaa.print( "UP" )
				aaa.mouse.set_grabbed( false )
				if EVENT.verbose > 0 then
					EVENT.print( "Set mouse grabbed to false" )
				end
				--ret = EVENT.b_mouse_consumed
				--EVENT.b_mouse_consumed = false
			end

			--end
		end
	elseif e_type == "keyboard" then
	elseif e_type == "window"	then
	else
		EVENT.print( e_type..","..e_sub_type.." -> "..a.." "..b.." "..c.." "..d.." "..e )
	end

	--EVENT.print( "ret is "..ret )
	return ret
end


