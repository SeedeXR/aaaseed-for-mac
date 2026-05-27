

--	TAB Key (tab == onglet)
--
--todo push pop

function BUS:set_tab_key( key )
	self.__bus_tab_key = key
--	if key then	self.__bus_tab_key = key
--	else		self:set_tab_key_def()
--	end
end
function BUS:get_tab_key()			return self.__bus_tab_key		end
function BUS:set_tab_key_def()
	self:set_tab_key( "Main" )
end


function BUS:activate_bu_by_tab_key( key )
	self:apply_fn_down(
		function(bu)
			local tab_key = bu.__bu_tab_key
			if tab_key then
				bu:set_active( tab_key == key )
				--bu:print( bu_key.." "..key )
			end
		end
	)
end
function BUS:get_bu_tab_key()
	local tab = {}
	local keys = {}
	self:apply_fn_down(
		function(bu)
			local key = bu.__bu_tab_key
			if key then
				if not keys[key] then
					keys[key] = true
					table.insert( tab, key )
				end
			end
		end
	)
	return tab
end

--
--	BU_GROUP han dled un add_bu
--
-- these 2 fns will be called by BUS when bu is added
-- this one when there is a current bu_group
--  store the asked name in syno field
--	but then transform the name by adding the group prefix to the name

--todo deal with recursuvity
function BUS:begin_bu_group( name )
	local gr = self:get_bu_group_always( name )
	local bg =  self.__bu_group
	bg.cur = gr
	bg.cur_name = name
	bg.cur_prefix = name.."."
	return gr
end
function BUS:end_bu_group()
	local bg = self.__bu_group
	if bg then
		local gr = bg.cur
		if gr then
			bg.cur = nil
			bg.cur_name = nil
			bg.cur_prefix = nil
			return gr
		else
			aaa.debug.show_warning( "BUS:end_bu_group() No bu_group current" ) 
			self:print_error( "BUS:end_bu_group() No bu_group current" )
			aaa.debug.print_traceback()
		end
	else
		self:print_error( "BUS:end_bu_group() No bu_group" )
	end
end
function BUS:get_bu_group( name )
	local bg = self.__bu_group
	if bg then
		return bg.groups[name]
	end
end
function BUS:get_bu_group_always( name )
	local bg = self.__bu_group
	if not bg then
		bg = { groups= {} }
		self.__bu_group = bg
	end
	local gr = bg.groups[name]
	if not gr then
		gr = {}
		bg.groups[name] = gr
	end
	return gr
end
-- todo : improve this concept of bu_group to toggle, it exists in BU_DOC, kin_move and here...
-- for example enable a simple way to provide nested button groups to build
function BUS:set_bu_group_active( name, b_active )
	local gr = self:get_bu_group(name)
	if gr then
		array.apply_method( gr, "set_active", b_active )
	end
end


--
--	ADD_BU
--

function BUS:__add_bu_before( bu )
	--aaa.print_fn()
	local bg = self.__bu_group
	if bg then
		local gr = bg.cur
		if gr then
			local name = bu:get_name()
			bg.__syno_tmp = name
			bu:change_name( bg.cur_prefix..name )
			bu:set_text( name ) --todo use syno in draw instead so don't have the group prefix
			return name
		end
	end
end

--create a field in the bu with a synomym using the group affiliation
-- this will be used later to populate a table
function BUS:__add_bu_after( bu )
	local bg = self.__bu_group
	if bg then
		local gr = bg.cur
		if gr then
			table.insert( gr, bu )
			local syno = bg.__syno_tmp
			--bg.__syno_tmp = nil
			bu:set_syno( syno )
			bu:set_bu_group( gr )
		end
	end
end

function BUS:add_bu(bu)
	if not bu then
		self:print_error( "try to add nil as a bu" )
		return nil
	end

	-- eventually we do stuff before on and with name (MEU need to do it at least)
	--local obj = self.__add_bu_caller
	--if obj then
	self:__add_bu_before(bu)
	--end
	self:push_for_change()

		bu.__bu_tab_key = self:get_tab_key()
		if self.verbose >= 1 then
			--table.print( self, "bus in BUS:add_bu()", 3 )
			--table.print( BUS, "BUS in BUS:add_bu()", 3 )
			self:print( "BUS.verbose is "..BUS.verbose )
			self:box_debug( "stop" )
			self:print( "add_bu( " .. bu .. " )" )
		end
		self:add_down(bu)

		local dplane = self:find_dplane_by_depth(0)
		--todo why we consider the default only
		--todo why we don't go thru the dplane fns
		-- Add bu in dplane 0 default
		table.insert( dplane.__bu, bu )

		bu:init_post() --todo counter part in other cases ?

	self:pop_for_change()
	-- eventually we do stuff after using the bu (MEU need to do it at least)
	--if obj then
	self:__add_bu_after(bu)
	--end

	--todo register unregister multiple before and after
	-- so it will merge with __add_bu_before/after
	if app and app.add_bu_after then
		app:add_bu_after( bu )
	end

	return bu
end

--todo in the remove processes make sure that current interaction don't use part of what we destroy
function BUS:remove_bu(bu)
	if not bu then
		self:print_error("try to remove nil as a bu")
		return nil
	end

	self:push_for_change()

		if self.verbose >= 1 then
			self:print("remove_bu( " .. bu .. " )")
		end
		self:remove_down(bu)
		-- Remove also the BU from the dplane table
		self:remove_bu_from_dplane(bu)

	self:pop_for_change()

	return bu
end
function BUS:remove_bu_all()
	self:push_for_change()

		if self.verbose >= 1 then
			self:print( "remove_bu_all()" )
		end
		local down_old = self:remove_all_down()
		-- Reinit the dplane's table
		self:init_dplane()

	self:pop_for_change()
end
function BUS:__destroy_all_bu()
	self:push_for_change()

		if self.verbose >= 1 then
			self:print( "__destroy_all_bu()" )
		end
		local down_old = self:remove_all_down()
	--	table.print( down_old, "down_old", 1 )
		table.apply_method( down_old, "free" )

		-- Reinit the dplane's table
		self:init_dplane()

	self:pop_for_change()
end

--
-- HELPER create
--
function BUS:create_add_bu(	class, name, rect, ... )
	--aaa.print_fn()
	if not CLASS.is_class(class) then
		self:box_debug( "No Class" ) 
	end
	if type(name)~="string" then
		if name==nil then
			name = "NIL"
			self:box_debug( "name is nil: Using \"" ..name.."\" as name" )
		else
			self:box_debug( "name is not a string" )
		end 
	end
	if rect ~= nil and type(rect)~="table" then
		self:box_debug( "rect is nor a nil, nor a table" ) 
	end

	local bu = self:add_bu(	class:create( name, rect, ... ) )

	return bu
end
function BUS:create_add_bu_no_name(	class, rect, ... )
	--aaa.print_fn()
	if not CLASS.is_class(class) then
		self:box_debug( "No Class" ) 
	end
	if rect ~= nil and type(rect)~="table" then
		self:box_debug( "rect is nor a nil, nor a table" ) 
	end
	return self:add_bu(	class:create( rect, ... ) )
end

-- generic helper
function BUS:add_button(				name, rect )		return self:create_add_bu( BUTTON,				name, rect )		end
function BUS:add_slider(				name, rect )		return self:create_add_bu( SLIDER,				name, rect )		end
function BUS:add_slider_two(			name, rect )		return self:create_add_bu( SLIDER_TWO,			name, rect )		end
function BUS:add_slider_xy(				name, rect )		return self:create_add_bu( SLIDER_XY,			name, rect )		end
function BUS:add_slider_multi(			name, rect, ... )	return self:create_add_bu( SLIDER_MULTI,		name, rect, ... )	end
function BUS:add_slider_multi_curve(	name, rect, ... )	return self:create_add_bu( SLIDER_MULTI_CURVE,	name, rect, ... )	end
function BUS:add_selector(				name, rect )		return self:create_add_bu( SELECTOR,			name, rect ) 		end
function BUS:add_preset(				name, rect )		return self:create_add_bu( PRESET,				name, rect )		end
function BUS:add_text(					name, rect )		return self:create_add_bu( BU_TEXT,				name, rect )		end

BUS.doc.add_monitor = "( name, rect, bind, b_fix ) fix monitor can't be moved"			
function BUS:add_monitor(         		name, rect, bind, b_fix )	return self:create_add_bu( BU_MONITOR,	name, rect, bind, b_fix ) end

-- more specific fns
BUS.doc.add_trig = "( name, rect ) trig button go back to OFF state (value false) as soon they are release"	
function BUS:add_trig(					name, rect )		return self:add_button(	name, rect ):set_trig()				end
function BUS:add_dial(					name, rect )		return self:add_slider(	name, rect ):set_dial( true )		end
function BUS:add_text_here(				name )				return self:add_text(   name )								end
function BUS:add_text_info(				name, rect )		return self:add_text(   name, rect ):make_text_info()		end

function BUS:add_slider_target( name, rect, tab_or_ref,target_name, val, vmin,vmax )
	local bu = self:add_slider( name, rect )
	bu:set_target( tab_or_ref, target_name ) --la val )
	bu:set_min_max_value( vmin,vmax, val )
	return bu
end

--todo clear the no rect situation here
function BUS:add_selector_target_lua( name, rect, tab,field_name, v )
	local bu = self:add_selector(     name, rect )
	if v~=nil then tab[field_name] = v end
	bu:set_target_lua( tab, field_name )
	return bu
end


function BUS:add_image( name, rect, filename_or_bind_or_img, b_auto )
	--self:print_debug( "add image : "..select(1,...) )
	return self:add_bu( BU:create_image( name, rect, filename_or_bind_or_img, b_auto ) )
end
function BUS:add_image_bind( bind, rect, b_auto )
	--self:print_debug( "add image : "..select(1,...) )
	return self:add_bu( BU:create_image( "Bind_"..bind, rect, bind, b_auto ) )
end

--	x, y, sx,sy are the coordonates in the bus referenciel
function BUS:add_video( filename, rect )
	self:print_debug( "add video "..filename )

	local bu
	local video = VIDEOS:get( filename )
	if video then
		if	rect[1]==nil or rect[2]==nil	then
			rect[1],rect[2] = self:get_next_bu_pos( true )
		end
		bu = BU:create_video( filename, video, rect )
		if bu then
			if self.verbose >= 1 then self:print( "add_video()" ) end
		else
			return nil
		end
	else
		self:print_error( "Can't add_video(): couldn't get video object for "..filename )
		--TODO refine this proxy thing
		local x,y = self:get_next_bu_pos( false )
		local s = .8
		bu = self:add_image_bind( TEXS:get_bind_by_name("PB"), {x,y, s,s*.75} )
	end
	self:add_bu( bu )
	return bu
end

function BUS:add_bu_page_trs( bu )
	bu:set_kind_page_trs()
	bu:set_visible( false )
	bu = self:add_bu( bu )
	bu:set_dplane( -42 )
	return bu
end

function BUS:add_www( rect )
	if self.verbose >= 1 then aaa.print_method() end
	if BU_WWW then
		return self:add_bu( BU_WWW:create( rect ) )
	else
		self:print( "no BU_WWW class setting texture instead" )
		return self:add_image_bind( IMGS.BIND_WWW, rect )
	end
end

function BUS:add_boucle( filename )
	if self.verbose >= 1 then aaa.print_method() end
	local bu = self:add_video( filename, {0,0, 7.9,4.45} )
	if bu then
		bu:set_ui_active( false )
		bu:set_border( false )
		bu:set_video_play_on_page( true )
		bu:set_video_loop( true )
	end
	return bu
end

function BUS:add_file( filename, b_image, b_video, b_click_double_full_page )
	local bu
	if MEDIA.is_name_image( filename ) then
		if b_image then
			bu = self:add_image( "BU_"..filename, {0,0, 2,2}, filename, true )
		end
	elseif MEDIA.is_name_video( filename ) then
		if b_video then
			--VIDEOS:set_dir( dir_name.."/" )
			bu = self:add_video( filename, {0,0, nil,1} )
			if bu then
				bu:set_video_button( true )
			end
		end
	else
		self:print_debug( "add_files() not recognized  : "..filename )
	end

	if b_click_double_full_page and bu then
		bu:set_method_on_click_double( bu, "__set_window_state", "flip_full" )
	end
	return bu~=nil
end

function BUS:add_files( dir_name_or_table, b_image, b_video, b_click_double_full_page )
	local t_file
	local nb = 0
	local b_table

	if type( dir_name_or_table ) == "string" then
		if self.verbose >= 1 then self:print( "add_files(\""..dir_name_or_table.."\" )" ) end
		t_file = aaa.dir.get_files( dir_name_or_table )
		if not t_file then return 0 end

		if t_file[1] and t_file[1] == "aaa_list.lua" then
			tab = nil
			aaa.dofile( dir_name_or_table.."/".."aaa_list.lua" )
			if tab then
				t_file = tab
				b_table = true
			else
				return 0
			end
		else
			--todo refine bus should find his app (root..)
			if app and app.lang then
				app.lang:filter_out_translation( t_file )
			end
		end
	else
		t_file = dir_name_or_table
		b_table = true
	end

	for t, u in pairs( t_file ) do
		--self:print_debug( "file : "..t.." .. "..u )
		local filename = b_table and u or dir_name_or_table.."/"..u
		if self:add_file( filename, b_image, b_video, b_click_double_full_page ) then
			nb = nb + 1
		end
	end
	if nb > 0 then
		self:print_debug( "added "..nb.." bu" )
	end
	return nb
end

function BUS:add_dir_no_recur( dir_name_or_table, b_image, b_video, b_click_double_full_page )
	if b_image == nil					then b_image = true						end
	if b_video == nil					then b_video = true						end
	if b_click_double_full_page == nil	then b_click_double_full_page = false	end

	--todo push/pop movie_dir
	if self.verbose >= 1 then
		if type( dir_name_or_table ) == "string" then
			self:print( "add_dir (\""..dir_name_or_table.."\" )" )
		else
			self:print( "add_dir() but with a table" )
		end
	end
	local nb = self:add_files( dir_name_or_table, b_image, b_video, b_click_double_full_page  )
	return nb
end

function BUS:add_dir( dir_name_or_table, b_image, b_video, b_click_double_full_page, b_button )
	if b_image == nil					then b_image = true						end
	if b_video == nil					then b_video = true						end
	if b_button == nil					then b_button = false					end
	if b_click_double_full_page == nil	then b_click_double_full_page = false	end

	--todo push/pop movie_dir
	local nb = self:add_dir_no_recur( dir_name_or_table, b_image, b_video, b_click_double_full_page )
	if nb > 0 then
		if b_button then self:add_page_buttons() end
		self:inc_page()
	end

	if type( dir_name_or_table ) == "string" then
		local t_dir = aaa.dir.get_dirs( dir_name_or_table )
		self:print( "dir  / dir : "..tostring( t_dir ) )
		if t_dir then
			dir_name_or_table = dir_name_or_table.."/"
			for v, w in pairs( t_dir ) do
				nb = nb + self:add_dir( dir_name_or_table..w, b_image, b_video, b_click_double_full_page, b_button  )
			end
		end
	end
	return nb
end

function BUS:add_dir_only_video( dir_name )	return self:add_dir( dir_name, false, true, true, false )	end
function BUS:add_dir_only_image( dir_name )	return self:add_dir( dir_name, true, false, true, false )	end

function BUS:bu_keyboard_open( keyboard )
	if keyboard == nil then return end
	if keyboard.b_is_open == nil  or keyboard.b_is_open == true then
		keyboard:close()
	else
		keyboard:open()
	end
end
function BUS:add_keyboard( bus )
	local keyboards = self:get_table_always( "__keyboards" )

	local id = #keyboards + 1 -- + 1 because we insert it in the table just after creation
	if id > 32 then return end

	local bu = KEYBOARD:create( "PC", id )
	table.insert( keyboards, bu )
--	bu:set_button_close_active( true )
	local sx = 2
	local sy = .5
	local x = -1.75 + math.floor( ( id - 1 ) / 11 ) * 2.1
	local y = 2.8 - ( id - 1 ) % 11 * .6
	bu = bus:add_trig( "PC_"..id, {x,y, sx,sy} )
	bu:set_method_on_click( self, "bu_keyboard_open", keyboards[id] )
end
function BUS:add_bu_keyboard( name, rect )
	local bus = BUS:create( "KEYBOARD" )

	bus:init_begin()

		local bu = bus:add_trig( "Add keyboard", {-2,3.5, 3,.75} )
			:set_method_on_click( self, "add_keyboard", bus )
			self:add_keyboard( bus ) -- adding first keyboard so don't have to do it from bu_text (when click on a bu_text editable)
			self.__keyboards[1]:close()

	bus:init_end()
	bu = self:add_bu( BU:create_window_center( "Keyboard", rect, bus ) 	)
	bus:set_transfo( 8 )
	bu:__set_window_state("mini")
	return bu
end

--todo
--	make sure we just have one instance (singleton ?)

function BUS:__process_wiz_rect( rect )
	rect = rect or {}
  	if not rect[1] or not rect[2] then
		local sx,sy = self:get_transfo_sxy()
		rect[1] = rect[1] or ( sx and math.min( sx*.9 , 2 ) or 2. )
		rect[2] = rect[2] or ( sy and (sy - .2) or 3.8 )
	end
	rect[4] = rect[4] or rect[3]
end
function BUS:add_wiz_local_low( name_end, rect, ... )
	self:__process_wiz_rect( rect )
	local class = _G["BU_"..name_end]
	local bu = self:create_add_bu_no_name( class, rect, ... )
	--ga:add_bu_wiz( name_end:lower(), bu )
	bu:set_sxy_min( 1/8,1/8 )
	return bu
end
function BUS:add_wiz_low( name_end, rect, ... )
	self:__process_wiz_rect( rect )
	local class = _G["BU_"..name_end]
	local bu = self:create_add_bu_no_name( class, rect, ... )
	ga:add_bu_wiz( name_end:lower(), bu )
	bu:set_sxy_min( 1/8,1/16 )
	return bu
end


function BUS:add_wiz_cam(		rect )				return	self:add_wiz_low( "CAM",	rect ) end
function BUS:add_wiz_send(		rect )				return	self:add_wiz_low( "SEND",	rect ) end
function BUS:add_wiz_fps(		rect )				return	self:add_wiz_low( "FPS",	rect ) end
function BUS:add_wiz_watch(		rect )				return	self:add_wiz_low( "WATCH",	rect ) end
function BUS:add_wiz_blob(		rect )				return	self:add_wiz_low( "BLOB",	rect ) end
function BUS:add_wiz_power(		rect )				return	self:add_wiz_low( "POWER",	rect ) end
function BUS:add_wiz_maaeb(		rect )				return	self:add_wiz_low( "MAAEB",	rect ) end
function BUS:add_wiz_alive(		rect )				return	self:add_wiz_low( "ALIVE",	rect ) end
function BUS:add_wiz_eye(		rect, b_one_eye )	return  self:add_wiz_low( "EYE",	rect, b_one_eye ) end
function BUS:add_wiz_pb(		rect )				return	self:add_wiz_local_low( "PB",		rect ) end
--function BUS:add_wiz_pb_local(	rect )				return	self:add_wiz_low( "PB",		rect ) end
function BUS:add_wiz_show( 		rect )				return 	self:add_wiz_low( "SHOW",  	rect ) end
function BUS:add_wiz_memory(	rect )				return 	self:add_wiz_low( "MEMORY",	rect ) end

function BUS:add_wiz_mess(	rect )
	local bus = BUS:create( "Mess_in" )
	local bu
	bus:init_begin()
		local SXS = .2
		--local SX = 1/8
		local sx,sy = rect[3],rect[4]
		bu = bus:add_wiz_low( "MESS", {sx-SXS/2,sy, sx*2-SXS, sy*2} )

		--todo resucitate
		
		local scro = bus:add_slider( "Scroller", {sx*2-SXS*.5,.5, 1,SXS, -.25 } )
			bu.__scroller = scro
			scro:set_meter( false )
			scro:set_value( 1 )
	bus:init_end()

	local bu = bus_cur:add_window( "Mess", bus,		rect )
		bu:set_back_alpha( .6 )
	return bu
end


--
--	BUTTON GENERIC
--
function BUS:add_but_flip_fn(     name, rect, ... )	return self:add_button(          name, rect ):set_function_on_click( ... )		end
function BUS:add_but_trig_fn(	  name, rect, ... )	return self:add_but_flip_fn(     name, rect, ... ):set_trig()				end
function BUS:add_but_flip_method( name, rect, ... )	return self:add_button(          name, rect ):set_method_on_click( ... )	end
function BUS:add_but_trig_method( name, rect, ... ) return self:add_but_flip_method( name, rect, ... ):set_trig()				end
function BUS:add_but_target_lua(  name, rect, tab, name_target, val )
	local bu = self:add_button(   name, rect )
	--la next line should not be a problem here
	--if val~=nil then tab[name] = val end
	bu:set_target_lua( tab, name_target, val )
	return bu
end
function BUS:add_but_target_param( name, rect, ref, val, val_min,val_max )
	local bu = self:add_button(    name, rect )
	bu:set_target_param( ref )
	bu:set_min_max_value( val_min,val_max, val )
	return bu
end
function BUS:add_button_image( name, rect, filename_or_bind_or_img, b_auto )
	local bu = self:add_image( name, rect, filename_or_bind_or_img, b_auto )
	bu:set_border( false )
	bu:disable_mobile()
	return bu
end
function BUS:add_button_image_on_method( name, rect, image, key, obj, method, ... )
	if self.verbose >= 1 then self:print( image ) end
	local bu = self:add_button_image( name, rect, image )
	bu:set_method( key, obj, method, ... )
	return bu
end

--	better with method
--	still there for Partdieu
-- function BUS:add_button_image_on_fn( name, rect, key, fn, arg1, arg2, arg3 )
-- 	if self.verbose >= 1 then self:print( image ) end
-- 	local bu = self:add_button_image( name, rect, image )
-- 	bu:set_function( key, fn, arg1, arg2, arg3 )
-- 	return bu
-- end

function BUS:add_button_image_on_click(			name, rect, image, ... )
	return self:add_button_image_on_method( 	name, rect, image, "click", ... )
end
function BUS:add_button_image_on_click_double(	name, rect, image, ... )
	return self:add_button_image_on_method(		name, rect, image, "click_double", ... )
end
function BUS:add_button_image_on_click_triple(	name, rect, image, ... )
	return self:add_button_image_on_method(		name, rect, image, "click_triple", ... )
end

--
--	BUTTON SPECIFIC
--
--[[
function BU:__update_color_wiz( color_ref )
--	local color = self.__color_wiz_color
	if not color_ref then
		self:print( "try to update using nil color ref" )
		return
	end
	color_ref:draw_rgba()
		aaa.draw_rect( -.5, -.5, 0., .5 )
		--self:gol_draw_rect_plain()
	color_ref:draw_rgb()
		aaa.draw_rect( 0., -.5, .5, .5 )
		--self:gol_draw_rect_line()
end

function BUS:add_color_wiz( name, color_ref, ... )	return	self:add_button_image_on_click(	name, {but_x,-2, .3,.3}, BUS.__image_name.refresh,	self, "reset_page_current" )	end
	local bu = self:add_button( nil, ... )
	--self:print_inverse( "tototo "..color_ref )
	--bu.color_ref = color_ref
	if aaa.obj.is_ref_no_error( color_ref ) then
		color_ref = COLOR_REF:create( name, color_ref )
	else
		self:print_error( color_ref.." should be a color ref, but is not." )
	end
	bu:set_ui_active( true )
	bu:set_text( name )
	bu:set_text_draw( false )
--	bu:set_trig()

--	bu.__color_wiz_color = color_ref
	bu:set_method( "draw_custom", bu, "__update_color_wiz", color_ref )

	return bu
end
]]--

local but_x = 3.5
local but_y = 0
function BUS:add_button_refresh()				return	self:add_button_image_on_click(	"refresh",	{but_x,	-2,			.3,.3},	BUS.__image_name.refresh, self, "reset_page_current" )	end
function BUS:add_button_home()					return	self:add_button_image_on_click(	"home",		{but_x,	but_y,		.3,.3}, BUS.__image_name.home,	  self, "set_page", 0 )			end
function BUS:add_button_next()					return	self:add_button_image_on_click(	"next",		{but_x,	but_y-.5,	.3,.3}, BUS.__image_name.next,	  self, "inc_page" )				end
function BUS:add_button_prev()					return	self:add_button_image_on_click(	"prev",		{but_x,	but_y-.85,	.3,.3}, BUS.__image_name.prev,	  self, "dec_page" )				end
function BUS:add_scroll( min_y, max_y )			return	self:add_bu_page_trs( BU:create( "scroll",	{0, (max_y+min_y)*.5, 10000, (max_y-min_y)} ) )							end
function BUS:add_fond( filename, rect )
	local bu
	if not rect then
		rect = {0,0}
	end
	if not rect[3] then
		rect[3],rect[4] = ga.cam:get_ortho_max_size()
	end
	bu = self:add_image( "fond"..filename, rect, filename )
	bu:set_ui_active( false )
	bu:set_border( false )
	return bu
end

function BUS:add_page_buttons()
	local DPLANE = 1000
	local bu
	bu = self:add_button_home()
		bu:set_pos_load_save( false )
		bu:set_dplane(DPLANE)
	bu = self:add_button_refresh()
		bu:set_pos_load_save( false )
		bu:set_dplane(DPLANE)
	bu = self:add_button_next()
		bu:set_pos_load_save( false )
		bu:set_dplane(DPLANE)
	bu = self:add_button_prev()
		bu:set_pos_load_save( false )
	--	self:move_to_front( bu )
		bu:set_dplane(DPLANE)
	return bu
end

--
--	BUP
--
function BUS:set_bup( bup )
	self.__bup = bup
	--self:box_debug( "set_bup "..bup )
end
function BUS:add_bup( ... )
	local bup = BUP:create( self:get_name(), ... )
	self:set_bup( bup )
	return bup
end
function BUS:get_bup()
	--self:box_debug( "get_bup "..self.__bup )
	return self.__bup
end

function BUS:set_xy(	x,y		)	self.__bup:set_xy(			x,y	)	end
function BUS:move_next(		... )	self.__bup:move_next(		...	)	end
function BUS:move_prev(		... )	self.__bup:move_prev(		...	)	end
function BUS:move_left(		... )	self.__bup:move_left(		...	)	end
function BUS:move_right(	... )	self.__bup:move_right(		...	)	end
function BUS:move_x(	x  		)	self.__bup:move_x(			x	)	end
function BUS:move_y(	y  		)	self.__bup:move_y(			y	)	end
function BUS:move_xy(	x,y  	)	self.__bup:move_xy(			x,y )	end
function BUS:move_next_col( ... )	self.__bup:move_next_col(	... )	end

--
--	WINDOW stuff
--
function BUS:begin_window_raw( name )
	local self = BUS:create( name )
	self:init_begin()
	return self
end
function BUS:begin_window( name, sx,sy )
	local self = BUS:begin_window_raw( name )

	self:set_grid_draw( true )
	local sx = sx or 4.
	local sy = sy or 4.
	local fox = .05
	local foy = .05
	local fx = .925
	local fy = .9
	self:add_bup( {sx*fox,sy*foy, sx*fx,sy*fy} ):set_size( sx*.05,sy*.05 )

--todo reintegrate
--	self:add_fps()

	--START
	--local bu = self:add_text_info(	{name.." "..aaa.net.machine,sx*fox*.5,	sy*4.,sx*.5}, sy*.25		)

	return self
end


function BUS:end_window_raw()
	self:init_end()
end
function BUS:end_window()
	self:end_window_raw()
	if not self:get_transfo() then
		self:set_transfo( 4,4, 2,2 )
	end
end

function BUS:add_window( name, bus_down, rect )
	self:push()

		local bu = BU:create_window_center( name, rect, bus_down )
		local sx,sy = rect[3],rect[4]
		if not bus_down:get_transfo() then
			bus_down:set_transfo( sx*2,sy*2, sx,sy )	-- here we set the coor system
		end
		
		self:add_bu( bu )
	self:pop()
	return bu
end

function BUS:add_window_with_define_method( rect, name, target, method, ... )
	rect = rect or {}

	rect.sx		=	rect[3] or self.__window_def_sx
	rect.sy		=	rect[4] or self.__window_def_sy
	if not rect[1] then
		rect[1]	=	self.__window_def_x
		self.__window_def_x = self.__window_def_x + self.__window_def_step_x
	end
	rect[2]		=	rect[2] or self.__window_def_y
	local bus_down = target[method]( target, rect, name, ... )
	local bu
	if bus_down then
		--local side = math.min( rect[3], rect[4] ) * .1
		bu = self:add_window(	name, bus_down,	rect )
		bu:set_pos_mini(		{rect[1], rect[2],		1/8,1/8} )
		bu:__set_window_state( "mini" )
	end
	return bu
end


