--
--	MU
--		these are materialization of a MEU in the interface
--
if CLASS.DECLARE( "MU", BU, {
	__b_inertia = false,	--todo add ui to change this
	} ) then
	aaa.lua.global.declare( "mu" )
	MU:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
								"MU is a Module BU, it is associated with a MEU and represent it in the rendering process",
								"the associated MEU can be and instance or a reference",
								"when rendering you can get the current MU using global mu" )

end

local doc = MU.doc
--function MU.c_init()		end
doc.get_slider = "() return the slider used by the MU.\n"..
					" A MU display an icon (left) and a slider (right).\n"..
					" The MU is rendered if the slider value is more than zero\n"..
					" for a MU/MEU for which it have sense this also alpha value to be used when rendered\n"..
					"  in other case it could also be used as an intensity, a volume... whatever make sense for the MEU type."
function MU:get_slider()	return self.__mu_slider end
doc.set_slider_text = "( text ) set the text of the slider used by the MU"
function MU:set_slider_text( text )
	--if self.__mu_slider then	-- __mu_slider is not defined yet when called from MU:create_instance
		self.__mu_slider:set_text( text, 1 )
	--end
end

function MU:get_path_and_name()
	local dir_app = app:get_dir_absolute()
	self:print( "app dir is "..dir_app )
	local dir, name = self:get_dir_up(), self:get_name()
	self:print( "Mu dir is "..dir )
	dir = string.remove_trailing_slash( dir )
	local path = aaa.file.get_dir_name( dir )
	path = dir_app..path
	self:print( "   path "..path )	
	self:print( "   name is "..name )
	return path, name
end
--todo check all this
function MU:__get_pos_rect_new_close()
	local x,y, sx,sy = self:get_xy_sxy()
	return { x+sx*.2,y+sy, sx,sy }
end

function MU:arrange_new( mus, x,y )
	aaa.print_fn()
	if not x then
		if mus:is_mus_root() then
			--self:print( "mus is root")
			x,y = mus:get_pos_free()
			self:set_xy( x,y )
			self:begin_fx( 50, 1 )
		end
	else
		self:set_xy( x,y )
	end
	self.__b_new_visible_forced  = true
end

function MU:free()
	local meu = self:get_meu()

	--aaa.print_fn()
	local mus = self:__get_mus_up()
	mus:remove_mu( self, true )

	-- we do it here before in the mus:remove_mu we try to access the meu used sonetimes
	if not meu.__get_meu_referred then	--hack avoid for meu_ref
		meu:free()
	end

	oo.getsuper(MU).free( self )
end
function MU:destroy()
	local meu = self:get_meu()
	if meu.__b_freed then
		self:print_debug( "MU:destroy() the meu is already freed" )
	else
		local path, name = self:get_path_and_name()
		self:print( "try to remove \""..path.."/"..name.."\"" )
		aaa.dir.remove( path.."/"..name )
		self:free()
	end
end

function MU:ask_destroy( x,y )
	--aaa.print_fn()
	local meu = self:get_meu()
	local name = self:get_name()
	local str
	if meu:is_proto() then
		local nb =  meu:get_instances_nb()
		if nb == 1 then
			str = "Really want to destroy\na Prototype\n"..name.."\nNo Undo here for now"
		else
			ga:add_dialog_message( name.."\nis a prototype\ndestroy instances first", {x,y} )
		end
	else
		str = "Really want to destroy\n"..name.."\nNo Undo here for now"		
	end
	if str then
		ga:add_dialog_confirm( str, {x,y}, "Destroy", "No", true, self, "destroy" )
	end
end

function MU:__begin_dialog( action )
	local _,name_src = self:get_path_and_name()
	local name_dst = name_src
	if action == "instantiate" then
		local meu = self:get_meu()
		local nb = meu:get_instances_nb() 
		name_dst = name_dst..(nb+1)
	end
	local dialog_table = { title="MEU NEW instance name", mu=self, bu=self, param_type="string", value_begin=name_dst, b_simple=true, obj=self }
	dialog_table.__mu_dialog_table = { action=action, mu=self }
	dialog_table = ga:add_dialog_edit( dialog_table )
	if dialog_table then
		table.print( dialog_table, "dialog_table", 2 )
	end
end
function MU:ask_instantiate()
	self:__begin_dialog( "instantiate" )
end
function MU:ask_rename()
	self:__begin_dialog( "rename" )
end
function MU.__move_files( src_path, dst_path )
	return aaa.file.move( src_path, dst_path )
end
function MU.__copy_files( meu_src, src_path, dst_path )
	aaa.print_fn()

	local err = aaa.file.copy_dir( src_path, dst_path )

	if not err then
		-- remove script: we need it only in proto
		local proto = meu_src:get_proto()
		if not meu_src:is_proto() then
			local fname = proto:get_lua_filename() -- get the name in proto
			fname = dst_path.."/"..fname
				--aaa.print( "lua fname is "..lua_fname )
			if aaa.file.is_exist( fname ) then	-- test to avoid error message when already no script
				aaa.file.remove( fname )
			end
		end

		-- remove aaa.lua because qwe can't have the same name
		local fname = dst_path.."/aaa.lua"
		if aaa.file.is_exist( fname ) then	-- test to avoid error message when no aaa.lua
			aaa.file.remove( fname )
		end
	end

	return err
end

function MU:__copy_low( mus_dst, dir_dst,name_dst, rect )
	aaa.print_fn()
	local dir_src, name_src = self:get_path_and_name()
	dir_dst = dir_dst or dir_src

	if not MEU_CTX.cur:check_name_keep_type( name_dst, name_src, self.." Can't instantiate" ) then
		return
	end

	-- complete copy of dir
	local dir_src_full = dir_src.."/"..name_src
	local dir_dst_full = dir_dst.."/"..name_dst
	local meu_src = self:get_meu()
	local err = MU.__copy_files( meu_src, dir_src_full, dir_dst_full )

	if not err then
		-- Now we copy the MEU state here not the file on disk
		
		meu_src:store_preset( 0, dir_dst_full.."/" )
		--local name = MEU:make_preset_filename(0)
		--aaa.file.copy( dir_src_full.."/"..name, dir_dst_full.."/"..name )
		local mu_new = mus_dst:create_one_mu( name_dst )
		if mu_new then
			if rect then
				local sx = rect[3] or self:get_sx()
				local sy = rect[4] or self:get_sy()
				mu_new:set_sxy( sx,sy )
				mu_new:set_xy(  rect[1],rect[2] )
			end
			mu_new.__b_new_visible_forced  = true
			return mu_new
		end
	end
end

function MU:__do_instantiate( name_dst )
	local mus_dst = self:__get_mus_up()
	if mus_dst:check_by_name( name_dst, "instantiate" ) then
		local rect = self:__get_pos_rect_new_close()
		return self:__copy_low( mus_dst, nil,name_dst, rect )
	end
end

--[[
function MU:__do_copy( mus_dst )
	local mus_src = self:__get_mus_up()
	local dir_src, name_src = self:get_path_and_name()

	local dir_dst, name_dst

	if mus_dst == mus_src then
		name_dst = name_src.."Inst"
		dir_dst = dir_src
	else
		name_dst = name_src
	end
	--local tab = { dir_src=path, dir_dst=dir_dst, name_src=name_src, name_dst=name_dst } --	obj=self, bu=self

	if false then
		if mus_dst:check_by_name( name_dst, "copy" ) then
			local rect = self:__get_pos_rect_new_close()
			self:__copy_low( mus_dst, dir_dst,name_dst, rect )
		end
	end

end
--]]

function MU:__rename( name_new )
	local mus = self:__get_mus_up()
	if not mus:check_by_name( name_new, "__rename" ) then
		return
	end
	
	local meu = self:get_meu()
	local name_old = self:get_name()

	if not MEU_CTX.cur:check_name_keep_type(  name_new, name_old, meu.." Can't rename" ) then
		return
	end

	local name_old_lower = string.lower( name_old )
	mus:__unregister_mu( self, false )
	
	local cached = meu.__meu_cached
	if cached then
		cached[name_old_lower] = nil
	end
	local gp = self:get_gp()
	cached = gp.__meu_cached
	if cached then
		cached[name_old_lower] = nil
	end

	mus:move_mu( self, name_new )
	--todo solve name stuff once for all
	self:change_name( name_new )	-- change MU

	self:set_slider_text( name_new )	-- needed to change slider name

	meu:change_name( name_new )	-- change MEU

	mus:__register_mu( self ) 

	return name_old
end

function MU:rename( name_next )
	local name_prev = self:__rename( name_next )
	if name_prev ~= name_next then
		local undo = self:make_table_to_call_method( self, "__rename", name_prev	)
		local redo = self:make_table_to_call_method( self, "__rename", name_next	)
		ga:add_undo_redo( undo,redo )
	end
end

function MU:do_dialog_hook( what, value, dialog_table )
	local b_ret = true
	self:print( "MU:do_dialog_hook()" )
	aaa.print_method()
	local mu_dialog_table = dialog_table.__mu_dialog_table
	if what == "change_value" then
		self:print( "value asked by callback is \""..value.."\"" )
		local cleaned = MEU_CTX.cur:make_name_valid( value )	
		if cleaned == value then
			dialog_table.value_last = value
		else
			self:print( "  removed char we get \""..cleaned.."\"" )
			value = dialog_table.value_last
			param.set( dialog_table.param_ref, value )
			b_ret = false	
			-- if #value < 6 then
			-- 	param.set( dialog_table.param_ref, dialog_table.value_last )
			-- else
			-- 	dialog_table.value_last = value
			-- end
		end
		mu_dialog_table.name_dst = value
		--gol.get_error( "MU:do_dialog_hook() value" )	
	elseif what == "open" then
	elseif what == "close" then
		--gol.get_error( "MU:do_dialog_hook() close" )
		local action = mu_dialog_table.action
		self:print( "action \""..action.."\" dialog return with "..value )
		if value == "ok" then
			--table.print( mu_dialog_table, "dialog_table for mu", 1 )
			local name_dst = mu_dialog_table.name_dst
			if action == "instantiate" then
				self.__table_todo = self:make_table_to_call_method( self, "__do_instantiate",	name_dst )
			elseif action == "rename" then
				self.__table_todo = self:make_table_to_call_method( self, "rename",				name_dst )
			end
		end
	end
	return b_ret
end

-- function MU:make_ref()
-- 	local rect = self:__get_pos_rect_new_close()
-- 	self:__get_mus_up():create_mu_ref( self, rect[1],rect[2], 0,0 )
-- end

function MU:add_uif_zones_mu( zones, b_meu, dy )
	dy = dy or 0
-- Slider (shared canonical Slide / Min / Max from BU helper, same positions
-- whether the UIF opens from a MEU or from a MU slider)
	self:add_uif_zones_slide( zones, false )

-- MEU	
	zones.Focus		=	{	angle=3/4,			dangle=4/32,	dist=.7,	}

	zones.Lua		=	{	angle=28.5/32,		dangle=5/32,	dist=1.3,	dy=.05,	}
	zones.Dir		=	{	angle=28.5/32,						dist=2.2,	rect={0,0, 2},		}
	
	local OX = b_meu and -8.6 or -8.6
	local DX = b_meu and 1.7 or 1.7
	local DY = -1.2
	local OY = b_meu and DY or 0

	zones.About		=	{	type="button",		angle=3/4,		dist=.7,	rect={-3,.4, 3}	}

-- Uix
	local b_new_ui = true
	if b_new_ui then
		-- Ui zone takes the Ui5 slot (5th column). Panel follows the button: same x,
		-- bottom of panel glued to bottom of button.
		local panel_sx = 12	-- ideal x range -4..4 around ui_x
		local ui_x       = OX + 5*DX                -- Ui5 x in the row
		local ui_dist    = .7 + dy
		local ui_btm_y   = -(1 + ui_dist) + OY - 0.5    -- button bottom in DY units
		-- panel aspect comes from the UI camera (not the whole render viewport),
		-- so the panel matches the area where BU_MEU windows actually live
		-- local function get_ui_ratio_x()
		-- 	local cam = ga:get_cam()
		-- 	return cam and cam:get_ratio_x() or 9/16
		-- end
		local function get_ui_frame_xy_sxy()
			local cam = ga:get_cam()
			local sx,sy
			if cam then
				sx,sy = cam:get_ortho_max_size()
			else
				sx = 8
				sy = sx * 9/16
			end
			return 0,0, sx,sy
		end
		
		-- the default slot is the sub button placed under the mouse when the
		-- panel opens. It mirrors what the original Ui1..Ui6 buttons used as
		-- their default: def for normal MEUs, def_dir for MEUs derived from MEU_DIR_BASE
		local gp = self:get_gp()
		local meu = self:get_meu_used()
		local default_slot = gp:get_meu_ui_slot_def( meu )

		-- panel anchor: defaults to under the Ui button, overridden below if the
		-- default slot exists so that this slot's sub button lands on the mouse
		local panel_x = ui_x
		-- Ui button center y in DY units (mouse sits here when the panel opens)
		local ui_btn_cy = OY - 1 - ui_dist

		local sub_zones = {}

		-- Compute the bounding box of every thing that must appear in the panel:
		-- the UI frame (camera ortho rect) plus every BU_MEU. Origin (0,0) maps
		-- to the panel center. abs_x/abs_y are the max distances from origin.
		-- The frame contribution is inflated by 5% so the panel always has a
		-- small margin around the frame; a BU_MEU sitting further out still
		-- extends abs_x/abs_y as before.
		local FRAME_MARGIN = .1
		local frame_x,frame_y, frame_sx,frame_sy = get_ui_frame_xy_sxy()
		local max_x = math.max( math.abs(frame_x - frame_sx*.5), math.abs(frame_x + frame_sx*.5) ) + FRAME_MARGIN
		local max_y = math.max( math.abs(frame_y - frame_sy*.5), math.abs(frame_y + frame_sy*.5) ) + FRAME_MARGIN

		local slot_nb = gp:get_ui_slot_nb()
		local rects = {}
		if slot_nb > 0 then
			for i=1, slot_nb do
				local bu_meu = gp:get_ui_slot_bu_meu(i)
				if bu_meu then
					local cx,cy, csx,csy = bu_meu:get_xy_sxy()
					-- track whether the slot already holds a MEU so the panel
					-- button can be drawn in a different style (empty / attached)
					local b_empty = bu_meu.get_meu_attached and bu_meu:get_meu_attached() == nil or false
					rects[i] = { x=cx, y=cy, sx=csx, sy=csy, b_empty=b_empty }
					local ax = math.max( math.abs(cx-csx*.5), math.abs(cx+csx*.5) )
					local ay = math.max( math.abs(cy-csy*.5), math.abs(cy+csy*.5) )
					if ax > max_x then max_x = ax end
					if ay > max_y then max_y = ay end
				end
			end
		end

		-- Scale: panel_sx is the target width. Aspect of the panel adapts to the
		-- content bbox so everything fits without empty bands. Uniform f.
		local f
		local panel_sy
		if max_x > 0 and max_y > 0 then
			f = panel_sx*0.5 / max_x
			panel_sy = max_y * 2 * f
		else
			panel_sy = panel_sx	-- fallback square
		end
		local panel_y = ui_btm_y + panel_sy*0.5

		if f then
			-- shift the panel anchor so the default slot's sub button lands
			-- on the Ui button center (where the mouse is when the panel opens)
			local r_def = rects[default_slot]
			if r_def then
				panel_x = ui_x      - r_def.x * f
				panel_y = ui_btn_cy - r_def.y * f
			end

			local min_size = 0.5
			for i, r in pairs(rects) do
				local sx = r.sx * f
				local sy = r.sy * f
				local m = math.min(sx, sy)
				if m > 0 and m < min_size then
					local scale_up = min_size / m
					sx = sx * scale_up
					sy = sy * scale_up
				end
				local px = panel_x + r.x * f
				local py = panel_y + r.y * f
				local label = tostring(i)
				sub_zones[label] = { type="button", rect={ px,py, sx,sy }, b_empty = r.b_empty }
			end
		end

		-- "New" button row, sits between the title strip and the slot grid.
		-- The panel grows upward by new_bu_sy to make room; the slot grid stays
		-- at panel_y (unchanged) so its rects don't shift. Height matches the
		local new_bu_sy = 1 -- self:__get_uif_button_sy()
		local new_bu_sx = new_bu_sy * 2
		sub_zones.New = {
			type = "button",
			rect = {	panel_x, panel_y + panel_sy/2 + new_bu_sy/2 - FRAME_MARGIN/2,
						new_bu_sx, new_bu_sy },
		}

		-- panel backdrop encompasses [New button + slot grid]. Center shifted up
		-- by new_bu_sy*0.5 so the bottom edge stays where the slot grid expects.
		-- No color set -> the pre-pass falls back to __set_color_uif_back so the
		-- panel matches the back of the radial star menu.
		local panel_y_eff  = panel_y + new_bu_sy * 0.5
		local panel_sy_eff = panel_sy + new_bu_sy
		sub_zones.Panel = { type="panel", rect={ panel_x, panel_y_eff, panel_sx, panel_sy_eff }, title="Pick a Ui" }

		-- white outline of the UI frame (camera ortho area) drawn over all sub
		-- buttons, using the same scale f as the buttons. b_above pushes it to
		-- the post-pass so it renders on top of everything else in the panel.
		if f then
			sub_zones.UiFrame = {
				type	= "frame",
				rect	= { panel_x + frame_x*f, panel_y + frame_y*f, frame_sx*f, frame_sy*f },
				color	= {0,1,1, .4},
				b_above	= true,
			}
		end

		-- xy_sxy includes the title strip drawn on top of the panel so hovering
		-- the title still keeps the sub alive (b_transient close check)
		local title_sy = 0.15	-- mirrors uif.text_sy in BU_uif
		zones["Pick Ui"]	=	{	type="button",		angle=3/4,		dist=ui_dist,	rect={ui_x, OY, 2.25},
			sub_def = {
				b_transient = true,
				xy_sxy = { panel_x, panel_y_eff + title_sy*0.5, panel_sx, panel_sy_eff + title_sy },
				zones  = sub_zones,
			},
		}
	else
		local SX = 1.5
		--for i=1,b_meu and 5 or 4 do
		for i=1,5 do
			local x = OX+i*DX
			zones["Ui"..i]	=	{	type="button",		angle=3/4,		dist=.7+dy,	rect={x,OY,1.5}		}
	--		zones["Ui"..i]	=	{	type="button",		angle=3/4,		dist=.7+dy,	rect={OX+0*DX,OY+(nb-i)*DY,SX}	}
		end
		if b_meu then
			zones.Ui6		=	{	type="button",		angle=3/4,		dist=.7+dy,	rect={0,OY+DY,SX}	}
		else
			zones.Ui6		=	{	type="button",		angle=3/4,		dist=.7+dy,	rect={0,OY+DY,SX}	}
		end
	end

-- Obj
	local X = -7
	zones.Destroy		=	{	type="button",		angle=3/4,		dist=3+dy,	rect={X,OY+1.1,		3}		}
	zones.Rename		=	{	type="button",		angle=3/4,		dist=3+dy,	rect={X,OY+2.2,		3}		}
	zones.Instantiate	=	{	type="button",		angle=3/4,		dist=3+dy,	rect={X+3.5,OY+2.2,	3.8}	}
--	zones.New			=	{	type="button",		angle=3/4,		dist=3+dy,	rect={X+4.4,OY,		2}		}
--	self:__add_uif_zones_base( zones, 3, OY )

-- Hide
	if not b_meu then
		-- placed at the top-right of Dir (same angle/dist anchor): Dir is at
		-- rect={0,0,2} so its top-right corner sits at +1 in x and +0.5 in y;
		-- Hide is shifted +2 / +1 to land flush with that corner above-right.
		local zo		=  	{	angle=28.5/32,						dist=2.2,	rect={1,1,2},		}
		if self.__b_mu_hidden then	zones.Unhide = zo
		else						zones.Hide = zo
		end
	end

	self:active_link()
	return zones
end

function MU:get_uif_zones_for_slider()
	local d = 1
	local zones = self:add_uif_zones_mu( {}, false, d )
--	zones.Ui5		=	{	angle=3/4,			dangle=4/32,	dist=.7+d		}
	return zones
end

function MU:do_uif_command( uif )
	local command = BU:get_uif_name_sel( uif )
	aaa.print_fn()
	--aaa.debug.print_traceback()

	local x,y = uif.x,uif.y
	local b_used = true
	if		command == "dir"				then	self:open_dir()
	elseif	command == "hide"				then	self:set_hide( true )
	elseif	command == "unhide"				then	self:set_hide( false )

	elseif	command == "instantiate"		then	self:ask_instantiate()
	elseif	command == "rename"				then	self:ask_rename()
	elseif	command == "destroy"			then	self:ask_destroy( x,y )
	elseif	command == "make_ref"			then	self:make_ref()
	elseif	command == "about"				then	self:get_meu_used():do_about( x,y )
	
	elseif	command == "pick_ui"			then
		-- the panel sub button (1..6) names a ui slot, route to set_ui_slot
		-- like the original Ui1..Ui6 main buttons did. The extra "New" sub
		-- button maps to slot 0 (GP wraps it to the last slot).
		if uif.name_sel == "New" then
			self:set_ui_slot( 0 )
		else
			local slot = tonumber( uif.name_sel )
			if slot then
				self:set_ui_slot( slot )
			end
		end
	elseif	string.sub(command,1,2) == "ui"	then
		local slot = tonumber(string.sub(command,3))
		self:set_ui_slot( slot )
	else
		b_used = false
	end

	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, MU )
end

-- function MU:__build_name_instance( meu )
-- 	return meu:get_name()
-- end


function MU:create( rect, meu )

	local name = meu:get_name()
	local self = MU:create_instance( name, rect )
--	local label = meu:get_label()
--	self:set_label( label )

	local label = meu:get_label()
	if label then
		--self:box_debug( "going UPPER for label name\n"..label )
		label = string.upper( label )
	end

	local title
	if meu:is_proto() then
		
		--self:box_debug( "MU:create() proto\n"..name )
		title = label or meu:get_meu_type()
	else
		--self:box_debug( "MU:create()\n"..name )
		title = label or name
	end

	--attach meu
	self.__meu	=	meu
	meu.__mu	=	self

	self.__render_index = 0

	local bus = BUS:begin_window_raw( name.."_MU" )

		local sx,sy = rect[3],rect[4]
		bus:set_transfo( sx,sy, sx*.5, sy*.5 )
		bus:set_bu_pos_load_save( false )
		--bus:set_grid_draw( false )
		local SX,SY	= .7 * sx, sy
		local x,y	= .3*sx+SX*.5, SY*.5

		-- we shouldf do hide/unhide here on shift crtl chick like icon
		local slider = bus:add_slider( "mu_slider", {x,y, SX,SY } )
			slider:set_show_value( false )
			slider:set_meter( true )
			slider:set_rot( false )
			self.__mu_slider = slider
			self:set_slider_text( title, 1 )
			slider.get_uif_name = function(sli) return sli:get_name() .. " of " .. self:get_name() end
			slider:set_min_max( 0,1 )
			slider:set_min_max_strict( true )
			slider:associate_mu( self )

	bus:end_window_raw()

	self:transform_in_window_no_mini( bus )
	self:set_text_draw( false )

	self:set_pos_load_save( true )
	self:set_value_load_save( true )
	self:set_ui_top_size( true )
	self:set_mobile( true )


	self:set_method_on_click_triple( self, "open_dir" )
	local meu = self:get_meu()
	self:set_method_on_click_double( meu, "edit_lua" )

	if self.init then
		self:init()
	end
	self:set_active_direct( true )
	--table.print( self, self, 2 )
	--self:box_debug( "MU:create()" )
	return self
end

--todo clarify this
doc.get_meu		= {	"() return the MEU corresponding to the MU.",
						"MEU have a symetric MEU:get_mu() method."	}
function MU:get_meu()		return self.__meu					end
doc.get_meu_used = {	"() return the MEU rendered by the MU.",
						"It make sense for MEU reference (MEU_REF) who reference another MEU"	}
function MU:get_meu_used()	return self.__meu:get_meu_used()	end

--todo use it in drop and extend it
-- gp mus meu mus meu....
function MU:get_gp()
	local up = self:__get_mus_up()
	while true do

		up = up.__up			--	MEU or GP
		if up:is_class(GP) then
			return up
		end
		--this is a MEU
		up = up:get_mu():__get_mus_up()
	end
end
function MU:set_ui_slot( id )
	return self:get_gp():set_meu_ui_slot( self:get_meu_used(), self, id )
end

function MU:get_dir_rel()
	return self:get_name().."/"
end
function MU:get_dir_up()
	local mus =	self:__get_mus_up()
	return mus:get_dir_up()..self:get_name().."/"
end
-- function MU:get_dir_fpath()
--  	return app:get_dir_absolute()..self:get_dir_up()
-- end

function MU:open_dir()
--	self:print( "dir is "..self:get_dir_new()	)
	local dir = self:get_meu():build_dirname()
	self:print( dir	)
	aaa.os.open_dir( dir )
end

function MU:get_meu_dir()
	local mus =	self:__get_mus_up()
	if mus then	--2025 July 24 hack: test added to protect but fon't know origin of problem
		local up = mus:get_up()
		if not up:is_class(GP) then
			return up:get_mu()
		end
	end
end

function MU:draw_marked( lbrt )
	local ph = aaa.time.t*2. -- + self.__bu_id * .25
	--local sy = math.sin(aaa.time.t*10.)*.15+.35
	--gol.draw_triangles_2d( -.4,0, -.6,sy, -.6,-sy )
	--gol.draw_triangles_2d( .4,0, .6,-sy, .6,sy )
	gol.set_line_width( BU.__draw_text_line_width  )
	--local fn_color = gol.color_red
	--fn_color( .5 + ph * .5 )
	gol.color_red()
	ph = wrap_01( ph  )
	ph = 1 - ph
	local s = .5 + ph * 1
	--gol.draw_lines_2d( -.5,x, .5,x )
	local i = .5
	gol.draw_lines_2d( -s,-s, -i,-i,
						-s,s, -i,i,
						s,-s, i,-i,
						s,s, i,i )

end

function MU:mark_by_match_pattern( pat )
	local b_match = false
	if pat ~= "" then
		b_match = self:is_name_match(pat)
		-- if b_match then
		-- 	self:print( "match pattern "..pat )
		-- end
		if not b_match then
			--local meu = self:get_meu_used()
			local meu = self:get_meu()
			if meu.__get_meu_referred then	--hack for meu_ref
				local meu_ref = meu:__get_meu_referred()
				if meu_ref then
					b_match = meu_ref:is_name_match( pat )
					--self:print( "meu_ref is "..meu_ref.." pat is "..pat.." -> "..b_match )
				end
			end
			if not b_match then
				local mus = meu:get_mus_down()
				if mus then
					--self:print( "have a mus down "..mus )
					--self:print( "result is "..mus:mark_by_match_pattern( pat ) )
					if mus:mark_by_match_pattern( pat ) then 
						b_match = true
					end
				end
			end
		end
	end
	self:__set_draw_marked( b_match )
	return b_match
end
function MU:mark_by_type( meu_type )
	local meu = self:get_meu()
	local b_match = meu:is_meu_type( meu_type )	
	if not b_match then
		local mus = meu:get_mus_down()
		if mus then
			if mus:mark_by_type( meu_type ) then 
				b_match = true
			end
		end
	end
	self:__set_draw_marked( b_match )
	return b_match
end

--todo use delegate ?
--function MU:get_values(...)		return if select( "#", ... ) > 0 then self.__mu_slider:get_values(...)	end
--todo now value saved in preset 0 and in bu_pos (trouble on bu_pos and DIR/GRID)
--function MU:get_value()			return self.__mu_slider:get_values() 				end
--function MU:set_value(...)		return self.__mu_slider:set_values(...)			end

--function MU:get_value()			return self.__mu_slider:get_value()				end

--function MU:set_value( val )	--todo was ... instead of val
--	aaa.print_fn()
--	self.__mu_slider:set_value( val )
--end

function MU:get_value( id )
	if id==nil or id==1 then
		return self.__mu_slider:get_value(1)
	elseif id==2 then
		return self.__b_mu_hidden
	end
end
function MU:set_value( value, id )
	id = id or 1
	if id==1 then
		return self.__mu_slider:set_value( value )
	elseif id==2 then
		self.__b_mu_hidden = value
	end
end

function MU:get_values()
	local tab = { render=self.__mu_slider:get_value(), b_hidden=self.__b_mu_hidden }
	return tab
end
function MU:set_values( values )
	--aaa.debug.print_traceback( self.."set_values" )
	--table.print( values, self.."set_values" )
	if type(values)=="table" then
		self.__mu_slider:set_values(values.render)
		self.__b_mu_hidden = values.b_hidden
	else
		self.__mu_slider:set_value( values )
	end
end

--function MU:set_min_max( val_min, val_max )		self.__mu_slider:set_min_max( val_min, val_max )	end
--todo deal with id ?
function MU:set_target_param( ref )
	local slider = self:get_slider()
	--	because set_target_param change the value
	local val = slider:get_value()
	slider:set_target_param( ref )
	slider:set_value( val )
end

function MU:set_icon_color()
	if MEU.__b_draw_icon_pure then
		gol.color_white()
	else
		if		self:is_contact()	then	gol.color_green()
		elseif	self:is_bu_cur()	then	gol.color_magenta()
		else
			local v = self:get_value()
			gol.color( .5,1,1, v<=0 and .5 or 1 )
			-- if v<=0	then	gol.color_red(.5)
			-- else			SLIDER.gol_color_meter_def( )
			-- end
		end
		-- if GP.cur:is_mu_ui( self ) then
		-- 	if self.__b_on then
		-- 		gol.color_green( self:get_alpha_bu_to_draw() )
		-- 	else
		-- 		gol.color_red( self:get_alpha_bu_to_draw() )
		-- 	end
		-- else
		-- 	if self.__b_on then
		-- 		gol.color_green( self:get_alpha_bu_to_draw() )
		-- 	else
		-- 		gol.color( 0,.4,.4, self:get_alpha_bu_to_draw() )
		-- 	end
		-- end
	end
end

local	c1			=	{	.0,.4,.4,	.5	}
--local	c1			=	{	.0,.0,.0,	.5	}

local	c_text_off	=	{	0,1,1,		.5	}
local	c_text_on	=	{	0,0,0,		1	}

function MU:is_visible()
--	if true then return true end
	if not oo.getsuper(MU).is_visible( self ) then
		return false
	end
	if self.__b_mu_hidden and not MU.b_draw_hidden then
		return false
	end
	local b_on = self.__b_new_visible_forced 
	if b_on then	-- __b_on_forced is there so just Instantiated MU are always visible when rendered once they go back to normal
		if self.__b_on or MU.b_draw_unused then
			self.__b_new_visible_forced  = false
		end
	else
		b_on = self.__b_on
	end
	if b_on then	
		if MU.b_draw_used then
			return true
		end
	else
		if MU.b_draw_unused then
			return true
		end
	end

	--self:print(MU.b_draw_unused )

	return false
end

function MU:update()
	if self.__table_todo then
		self:do_table( self.__table_todo )
		self.__table_todo = nil
	end
	
--	self:print( self.b_mu_draw )
	self.__b_on = self:is_render()

	if not self.b_mu_draw or not self:is_visible() then
		self.__b_skip_draw = true
		return false
	end

	self.__b_skip_draw = false
	return oo.getsuper(MU).update(self)
end

-- optimization for MU:draw()
function MU:__transform_and_draw()
--	aaa.show( self.__b_mu_hidden, "BU:tad "..self )	
--	self:print( self.b_mu_draw )
	if self.__b_skip_draw then
		return false
	end

	--self:print( slider.." draw()" )

	if MU.__b_clip then
		local x,y = self:get_xy()
		local SX = ga.half_size_x + .2	--half size
		if outside( x, -SX, SX ) then
			return
		end
		local SY = ga.half_size_y + .05--half size
		if outside( y, -SY, SY ) then
			return
		end
		--local sx,sy = self:get_sxy()
		--self:print( sx.." "..sy )
	end

	return BU.__transform_and_draw( self )
end

-- function MU:draw()
-- end

local border_color_off = {1,0,0, 1}
local border_color_on  = {0,1,0, 1}
-- we draw active MU with this Fn but other MU are drawn faster
function MU:__draw_all()
--	aaa.show( "", "draw "..self )
	--self:print( "MU:draw()" )
	--aaa.debug.print_traceback()
	--if true then return end
	--aaa.debug.print_traceback()
	--self:print( MU.b_mu_draw )
	--aaa.toto = aaa.toto + 2

	if self.verbose >= 3 then aaa.print_method() end
	
	local slider = self.__mu_slider
--	local b_mu_ui = GP.cur:is_mu_ui(self)
	self.__line_color_forced = (slider:get_value() > 0) and border_color_on or border_color_off

	local b_fast = not self.__b_on and not self:is_contact() and not slider:is_contact()
	if b_fast then
		--if true then return end
		if GA.b_spy then aaa.spy.push_range( self.." draw_v1()", 2 ) end
		--if true then return end
		
		-- if b_mu_ui then
		-- 	gol.color_red()
		-- 	local inter = .5 + math.cos( math.pi2 * aaa.time.t ) * .5
		-- 	self:gol_draw_frame_cross_spe_light( inter )
		-- else			
		-- 	--faster simplier
		-- 	--local alpha = self:get_alpha_bu_to_draw()
		-- 	--gol.color( 0,.4,.4, alpha )
		-- 	--gol.draw_line_strip_2d(	-.2,-.5,	-.5,-.5,	-.5,.5,		-.2,.5	)
		-- end

	--]]
		--gol.set_texture_dim( 0 )
		--gol.color_cyan( .2 )

		--this is done on every BU drawn but we skip the slider draw here
		slider.__draw_ui_render_index = GP.cur:get_render_index()

		local alpha
		if self.__b_mu_hidden then
			gol.color( .5,0,0, .9 * self:get_alpha_bu_to_draw() )
		else
			gol.color( 0,.2,.2, .9 * self:get_alpha_bu_to_draw() )
			--gol.draw_lines_2d(	0,-.7, 	0,-.5, 	0,.7,  0,.5		)
		end
		local lbrt = {-.2, -.5, .5, .5}
		self:draw_rect_array_lbrt( lbrt )
	
		-- BORDER
		self:draw_border()

		slider:draw_fore( lbrt )

		if MU.__b_draw_text then
			--gol.color( .8, 1.	)
			--gol.color( .5, 1, 1, 1 )
			SLIDER.gol_color_meter_def(MU)	--todo make it simplier
			local str = slider:get_text()
			local SX = .63
			local sca_x = .2
			local sx_str_len = aaa.get_str_translate( str )
			if sx_str_len*sca_x > SX then
				-- too wide for the rect we scale down just to fit in
				sca_x = SX / sx_str_len
			end
				if GA.b_spy then aaa.spy.push_range( "draw_str_at", 1 ) end
			--	gol.compile_list( l )
					self:draw_text( str, -.16,-.45, sca_x,.9 )
			--	gol.end_list()
				if GA.b_spy then aaa.spy.pop_range() end
			--end
		end

		self:draw_fore()	-- imperfect but ok

		if aaa.keyboard.is_alt_only() then
			self:__draw_ui_top()
		end
	else
		if GA.b_spy then aaa.spy.push_range( self.." draw_v2()", 2 ) end
	
		BU.__draw_all(self)
		--oo.getsuper(MU).__draw_all(self)

	--	gol.set_texture_dim( 0 )	--	needed
	--	gol.set_line_width( 1 )
		-- work some details 
		
	-- 	if b_mu_ui then
	-- 		if self.__b_on then
	-- 			gol.color_green()
	-- 		else
	-- 			gol.color_red()
	-- 		end
	-- 		local inter = .5 + math.cos( math.pi2 * aaa.time.t ) * .5
	-- --		self:gol_draw_frame_cross( inter * .25, 1. - inter )
	-- 		-- Draw some kind of frame around MU ui
	-- 		self:gol_draw_frame_cross_spe_light( inter )
	-- 	else
	-- 		--local alpha = self:get_alpha()
	-- 		-- if self.__b_on then
	-- 		-- 	gol.color( 0, .8, 0, alpha )
	-- 		-- 	-- Draw outside corner around active MU
	-- 		-- 	self:gol_draw_frame_cross_light( .05, .2 )
	-- 		-- else
	-- 		-- 	gol.color( 0, .4, .4, alpha )
	-- 		-- 	--aaa.draw_rect_line( -.5, -.5, -.2, .5 )
	-- 		-- 	--self:gol_draw_frame_rect()
	-- 		-- 	-- Maa 2022 May don't see it....
	-- 		-- 	gol.draw_line_strip_2d(	-.2,-.5,	-.5,-.5,	-.5,.5,		-.2,.5	)
	-- 		-- end
	-- 	end

	 	--gol.draw_lines_2d(	0,-.7, 	0,-.5, 	0,.7,  0,.5		)
	end
	
-- ICON
	if MU.__b_draw_icon then
		if GA.b_spy then aaa.spy.push_range( "draw_icon", 2 ) end
			local meu = self:get_meu()
			meu:set_color_then_draw_icon()
			if meu:is_proto() then
				meu:draw_icon_mark_proto()
			end
		if GA.b_spy then aaa.spy.pop_range() end					
	end

	if GA.b_spy then aaa.spy.pop_range() end
end

--todo regroup ?
function MU:is_render()
	---if true then return true end
	--if self.verbose >= 2 then aaa.print_method() end
	local value = self.__mu_slider:get_value(1)
	if value==nil then
		self:print( "Maa we got a problem" )
		return false
	end
	return value > 0
end

function MU:set_render_skip_next( b )
	self.__b_skip_render_next = b
end

function MU:get_render_index()	return self.__render_index		end
--todo check use of this
function MU:is_rendered_in_this_frame()
	return self:get_render_index() == GP.cur:get_render_index()
end

function MU:render( alpha )
	--if true then return end
	if self.verbose >= 2 then aaa.print_method() end
	--self:print( "render alpha "..alpha )
	-- the fact to have to render is determined at a higher level
	--  and don't need to be checked here
	--	skip_frame allow to disable a MU after
	local mus = MUS.get_cur()
	if self.__b_skip_render_next then			--todo adapt to stereo and "multiple" rendering
		self.__b_skip_render_next = false
		mus:add_mu_rendered( self, false )
	else
		alpha = self:get_value(1) * alpha
		local meu = self:get_meu()
		--local ret
		--needed to be done before render or list will be incorrect for Begin MUsw
		mus:add_mu_rendered( self, true )
--		if meu:is_render( alpha ) then
		if GA.b_spy then aaa.spy.push_range( "MU-"..self..".render()", 1 ) end
		if GA.b_verbose_render then self:print( "\tMEU render() begin" ) end

			meu:render( alpha )
			local render_index = GP.cur:get_render_index()
			if meu:get_render_index() == render_index then
				self.__render_index = render_index
			end

		if GA.b_verbose_render then self:print( "\tMEU render() end" ) end
		if GA.b_spy then aaa.spy.pop_range() end
		--self:print( "ret "..ret )
		--todo deal with ret
		--mus:add_mu_rendered( self, (ret~=nil) and ret or true )
	end
end

function MU:set_hide( b )
	self.__b_mu_hidden = b
	aaa.show( self.."Hidden -> "..b, "UI" )
	local slider = self.__mu_slider
	if b then	slider:set_color_back( "hide" )
	else		slider:set_color_back()
	end
end
function MU:flip_hide()
	self:set_hide( not self.__b_mu_hidden )
end

-- need to clean C double/triple h first 
-- function MU:do_key(key)
-- 	self:print_do_key( "MU", key )

--  	local b_key_used = false
-- 	if key == 104 then
-- 		self:flip_hide()
-- 	else
-- 		b_key_used = false
-- 	end

--  	return b_key_used or oo.getsuper(MU).do_key( self, key )
-- end

function MU:do_mouse_move( x,y )
	if self.__b_wait_up then
		oo.getsuper(MU).do_mouse_move( self, x,y )
	end
end

function MU:do_click_down( x,y )
	--if aaa.keyboard.is_alt_only() then
	if aaa.keyboard.is_ctrl() and aaa.keyboard.is_shift() then
		self:flip_hide()
		self.__b_wait_up = true	--todo refine test and then move this locking mecanism to BU
		return
	end

	oo.getsuper(MU).do_click_down( self, x,y )

	ga:get_undo_redo():store_position_in_bu(self)

	local gp = GP.cur
	gp:set_mu_selected( self )

	-- we need to store the ui_slot id to avoid a default move when ckicking on a meu_dir in the default dir_slot
	--	to avoid a parasit move placing it outside of the BU_MEU
	local meu = self:get_meu_used()
	local ui_slot
--	self:print( "Meu is "..meu )
	if meu:is_derived_from_class(MEU_DIR_BASE) then
--		self:print( "This is a dir" )
		local mus_up = self:__get_mus_up()
		local up = mus_up:get_up()
--		self:print( "mus_up "..mus_up.." with "..up )
		if up:is_derived_from_class(MEU_DIR_BASE) then
--			self:print( "This is a dir in a dir" )
			ui_slot = up.__slot_id
--			self:print( "ui_slot is "..ui_slot )
		end
	end

	local i_old = self:get_meu_used():get_ui_slot()		--was  gp:find_ui_slot_by_meu()
	local i_new = self:set_ui_slot()
	local b_move = (not i_new) or (i_new ~= i_old)
	if b_move then		
--		self:print( "test "..ui_slot.." "..meu.__slot_id.." "..i_new )			
		if ui_slot and ui_slot == i_new then
			b_move = false 
		end
	end
	--	self:print( i_old.." to "..i_new.." : "..b_move  )
	return not b_move
end

-- function MU:do_click_up( x,y )
-- 	if self.__b_wait_up then
-- 		self.__b_wait_up = nil		
-- 		self.__b_wait_drop = true	-- drop is_afters
-- 	else
-- 		oo.getsuper(MU).do_click_up( self, x,y )	
-- 	ends
-- end

-- function MU:set_drop_receiver( drop_receiver )
-- 	self:get_bus_down().drop_receiver = self
-- end

function MU:drop_on( receiver, x_top,y_top ) --, ox,oy )
	local function verbose( str )
		if BU.verbose_drop_on > 0 then
			self:print_debug( "MU dropped "..str )
		end
	end
	local function verbose_b( str, b )
		verbose( str..", return "..tostring(b) )
		return b
	end

	verbose( "on receiver "..receiver.." at "..x_top..","..y_top )


	-- if self.__b_wait_drop then
	-- 	self.__b_wait_drop = nil
	-- 	return true
	-- end

	--self:print_inverse( "MU:drop_on( "..target.." )" )
	--todo move first part to caller BU:remove_contact_low()

	local b_copy = aaa.keyboard.is_ctrl() and not aaa.keyboard.is_shift()
--todo deal with the ref case
--	local b_ref	 = false	-- alt now move aaa.keyboard.is_alt()
	---todo show indication on screen
--	if b_copy and b_ref then
--		b_copy	=	false
--		b_ref	=	false
--	end
--	local b_move = not (b_copy or b_ref)
	local b_move = not b_copy
	if b_move and receiver == self:get_bus_up() then
		return verbose_b( "drop on same this just a move", true )
	end

	if not CLASS.is_gabu_obj( receiver ) then
		return verbose_b( "receiver not a GABU_OBJ", false )
	end

	-- if not receiver:is_derived_from_class( BUS ) then	--todo just a check
	-- 	self:print( "Not dropped on a BUS : ", receiver )
	-- 	return false
	-- end

	if receiver == app then
		receiver = GP.cur:get_mus_down()
		verbose( "receiver is app, and so replaced now by GP.cur:get_mus_down()" ) 
	end

	--	don't drop on self
	if b_move then
		local my_bus = self:get_meu().bus
		if receiver == my_bus then
			return verbose_b( "Can't drop on itself.", false )
		end
	end


	-- local receiver = target.__bus_receiver
	-- table.print( receiver, "drop receiver", 1 )
	-- if not receiver then
	-- 	self:print( "No drop receiver in "..receiver )
	-- end

	--table.print( target, "target", 2 )
	--local b_meu = receiver:is_derived_from_class( MEU )
	--self:print( "receiver is "..receiver.." derived from meu : "..b_meu )
	--if oo.isinstanceof( receiver, MEU ) then
	-- if b_meu then
	-- 	self:print( "Droped on a MEU" )
	-- 	receiver:do_drop( self )
	-- 	return false
	-- end


	-- if b_copy and my_bus == up.bus then
	-- 	self:print( "Can't drop it will create a cycle" )
	-- 	return false
	-- end
	
	local x,y
	local x_local,y_local
	if receiver:is_derived_from_class(MEU_DIR) then
		--x,y = nil,nil	--todo this is global coordoonate
		receiver = receiver:get_mus_down()
		x,y = 0,0
		verbose( "receiver is MEU_DIR, and so replaced now by its MUS down" )
	end

--	table.print( receiver, "receiver is "..receiver, 1 )

	if receiver:is_class( MUS ) then
		verbose( "receiver is MUS" ) 
		if not x then
			x,y = BU:convert_xy_top_to_bus( receiver.__bus_mus, x_top,y_top )
		end
		if not x_local then
			x_local,y_local = self:convert_xy_top_to_local( x_top,y_top )
		end
		verbose( "will drop at top -> "..x.." "..y.." local "..x_local.." "..y_local  ) 
		
		local mus_src = self:__get_mus_up()
		if b_copy then
			receiver:copy_mu( 				self,		x,y, x_local,y_local )
			--receiver:copy_mu( 			self,		x,y )
			return verbose_b( "Did a copy_mu", false )
		-- elseif b_ref then
		-- 	--	receiver:create_mu_ref(		self,		x,y, x_local,y_local )
		-- 	--	return false
		else
			if mus_src == receiver then
				return verbose_b( "mus_src is receiver", true )
			else
				-- here we move to differnt MUS				
				local b = receiver:move_mu( 	self, nil,	x,y, x_local,y_local )
				return verbose_b( "Did a move_mu", b )
			end
		end
	else
		-- 2025 Feb no drop_mu
		--return receiver:drop_mu( 			self,		x,y, x_local,y_local )
	end

	return verbose_b( "Did nothing", false )
end

function MU:active_link()
	aaa.print_fn()
	local gp = self:get_gp()
	gp:register_mu_to_draw_link( self )
end

function MU:set_bu_cur()
	--aaa.print_fn()
	oo.getsuper(MU).set_bu_cur(self)
	self:active_link()
end

function MU:save_to_dir( dirname, b_complete )
	local meu = self:get_meu()
	meu:save_to_dir( dirname, b_complete )
end

function MU:save_inside_dir( dirname, b_complete )
	self:save_to_dir( dirname..self:get_dir_rel(), b_complete )
end

