if CLASS.DECLARE( "BU", nil,  {
	-- default field values
	__text_x					= 0,
	__text_y					= 0,
	__text_factor_x				= 1,
	__text_factor_y				= 1,
	-- same when in mini state
	__text_mini_x				= 0,
	__text_mini_y				= 0,
	__text_mini_factor_x		= 1,
	__text_mini_factor_y 		= 1,

	__contact_nb				= 0,
	__b_draw_lua				= true,
	__b_mobile					= true,
	__b_visible					= true,
	__b_video_play_on_page		= false,
	__b_video_play_on_page_strict = false,
	__b_video_loop				= false,

	__b_move_to_back			= true,
	__b_move_to_front			= true,

	__alpha_bu					= 1.,			-- different from __bu_alpha used by BU_COLOR
	__b_text_draw				= true,
	__b_tex_draw				= nil,
	__b_highlight 				= true,
	__side_bounce_factor		= .5,
	__b_inertia					= true,
	__b_rot						= true,
	__b_scale					= true,
--2023 July Maa commented implementation of alignment is buggy and incomplete
--	__align_x 					= "center",
--	__align_y 					= "center",
	__text_align_x 				= "left",
	__text_align_y 				= "center",	--todo
--	__kind						= "none",	--default bu kind
	__b_drop_can_receive		= false,
	__b_border					= true,
	__b_pos_load_save			= true,

	__b_inside_main_rect_only	= false,

	__b_preset_use				= false,
	__b_bus_down_active_ui		= true,

	__b_video_scrub				= true,
	__video_scrub_sy			= 1.,
	__page						= 0,
	__text_len					= 0,

	__dplane					= 0,  --default dplane

	__uif_verbose				= 0,
	__b_uif_enabled				= true,
	__b_uif_on_shift			= true,

	__back_alpha				= 1,
	__b_ui_active				= true,

	__border_line_color_red				=	{	1,0,0,	1	},
	__border_line_color_contact			=	{	0,1,0,	1	},
	__border_line_color_contact_no		=	{	0,1,1,	1	},
	__border_line_color					=	{	0,1,1,	1	},	--	Used to avoid creation destruction of table on the fly

	__color_text				= {	1,1,1,	1	},
	__sx_min					= .01,
	__sy_min					= .01,

	__b_uif_outside				= true,
	__b_undo_redo_using_values	= false,		-- we need it for selector multiple for examples

	__class_alpha_factor		= 1.,

	__tex_led_on				= nil,
	__tex_led_off				= nil,
	__b_top_blink 				= false,
	__top_blink_period 			= .25,

	__draw_text_line_width 		= 2,

	__render_index_cur			= -1	-- set by GP but this avoid tests when no GP

	} ) then

		BU:set_class_status_doc(	CLASS.STATUS.GABU,
									"BU means Box User: anything with and interaction with a user"
								)

	BU.__color_named = {}
	local col = BU.__color_named
	col.white		=	{	1,	1,	1,		1	}
	col.red			=	{	1,	0,	0,		1	}
	col.green		=	{	0,	1,	0,		1	}
	col.blue		=	{	0,	0,	1,		1	}
	col.yellow		=	{	1,	1,	0,		1	}
	col.cyan		=	{	0,	1,	1,		1	}
	col.violet		=	{	1,	0,	1,		1	}
	col.grey 		=	{	.5,	.5,	.5,		1	}
	col.alpha 		=	{	.5,	.8,	.8,		1	}
	col.saturation	=	{	1,	1,	1,		1	}
	col.value		=	{	.5,	.5,	.5,		1	}
	col.uif_active 	=	{	1,	0,	1,		1	}
	col.uif 		=	{	1,	1,	1,		1	}
	col.restart		=	{	1.,	.5,	0.,		1	}
	col.running		=	{	1,	1,	0,		1	}
	col.opened		=	{	1,	.5,	0,		1	}
	col.closed		=	{	1,	0,	0,		1	}
	col.error		=	{	1,	0,	0,		1	}
	col.info		=	{	1,	1,	0,		1	}
	col.record		=	{	1,	0,	0,		1	}
	col.valid		=	{	0,	1,	1,		1	}
	col.invalid		=	{	1,	0,	0,		1	}
	col.inactive	=	{	0,	.5,	.5,		1	}
	col.r  			= 	col.red
	col.g  			= 	col.green
	col.b			= 	col.blue
	col.a			= 	col.alpha
	col.gray		= 	col.grey
	col.sat 		= 	col.saturation
	col.val			= 	col.value
	col.busy 		= 	col.red
	col.problem 	= 	col.red				-- make it live
	col.info 		=	col.yellow
	
	local cm = .4
	BU.__color_back_named = {}
	local col = BU.__color_back_named
	col.bu			=	{	.5, 	.5,		.5,		.2 	}
	col.bui_pale	=	{	0,		.25,	.25,	.8	}
	col.bui			=	{	0,		.3,		.3,		.6	}
	col.bu_text		=	{ 	0,		0,		0, 		0 	}
	col.slider 		=	{	.0,		.65,	.65,	.5	}
	col.busy		= 	{	0,		1,		0,		1  	}
	col.lua			=	{	0,		.5,		0,		.8	}
	col.focus		=	{	.4,		0,		.4,		.8	}
	col.restart		=	{	.5,		.25,	0.,		.8	}
	col.save		=	{	1.*.7,	.3*.7,	.0,		.8	}
	col.x			=	{	.8,		.5,		.5,		.8	}
	col.y			=	{	0,		.9,		.5,		.8	}
	col.z			=	{	0,		.5,		.9,		.8	}
	col.red			=	{	cm,		0,		0,		1	}
	col.green		=	{	0,		cm,		0,		1	}
	col.blue		=	{	0,		0,		cm,		1	}
	col.none 		=	{	0,		0,		0,		0	}
	col.grey 		=	{	.7,		.7,		.7,		.5	}
	col.red_full	=	{	1,		0,		0,		1	}
	col.green_full	=	{	0,		1,		0,		1	}
	col.blue_full	=	{	0,		0,		1,		1	}
	col.cyan		=	{	0,		1,		1,		1	}
	col.magenta		=	{	1,		0,		1,		1	}
	col.yellow		=	{	1,		1,		0,		1	}
	col.u			=	{	cm*2,	cm,		cm,		1	}
	col.v			=	{	cm,		cm*2,	cm,		1	}
	col.axe			=	{	cm,		cm,		cm*2,	1	}
	col.factor		=	{	.5,		.5,		.5,		.8	}
	col.off			=	{	1.,		.0,		.0,		.5	}
	col.on			=	{	.0,		1.,		.0,		.5	}
	col.bu_off		=	{	1,		0,		0,		.45	}
	col.bu_on		=	{	0,		.5,		0,		.5	}
	col.hide		=	{	.5,		.0,		.0,		.9	} 
	col.r			= 	col.red
	col.g			= 	col.green
	col.b			= 	col.blue
	col.load		= 	col.save
	col.redef		= 	col.save
	col.reload		= 	col.restart
	col.reset		= 	col.restart
	col.action		= 	col.restart
	col.swap		=	col.restart
	col.edit		= 	col.lua
	col.w			=	col.axe

	function BU:get_color_named_no_error( name )
		return self.__color_named[string.lower(name)]
	end
	function BU:get_color_named( name )
		local col = self:get_color_named_no_error(name)
		if not col then
			col = self:get_color_back_named(name)
		end
		return col
	end
	function BU:get_color_back_named_no_error( name )
	--self:print( "name is "..name )
		name = name and string.lower(name) or "bui"
		return self.__color_back_named[ name ]
	end
	function BU:get_color_back_named( name )
		local col = self:get_color_back_named_no_error(name)
--Maa 2020 Jan commented to avoid too many message at start
--		if not col then	self:print_error( "no Back Color named "..name ) end
		return col
	end

	BU.__color_back	= BU:get_color_back_named( "BU" )
	--todo perhaps this a slow strategy reusing same obj on all
	BU.__text_obj	=	TEXT:create( "BU_TEXT_OBJ" )
	BU.__text_obj:set_font_texture( 0 )
	
	function BU.c_init_generator()	BU.id_generator	= 0	end
	BU.c_init_generator()

	local s	=	.2
	BU.__UI_TOP_LEFT	=	-.5 + s
	BU.__UI_TOP_RIGHT	=	 .5 - s
	BU.__UI_TOP_DOWN 	=	BU.__UI_TOP_LEFT
	BU.__UI_TOP_UP		=	BU.__UI_TOP_RIGHT

	BU.__UI_ON_TOP_INTER =	{	"scale_left_up",	"scale_up",		"scale_right_up",
								"scale_left",		"move",			"scale_right",
								"scale_left_down",	"scale_down",	"scale_right_down"
							}
end

local doc = BU.doc

function BU:gol_color_link( alpha )
	gol.color_cyan( alpha )
end

--
-- CLASS
--
function BU.c_init()
	BU.c_init_generator()
	BU.__scale_min = .2
	BU.__scale_max = 20
	BU.c_init_draw()
end

function BU.c_update()
	if aaa.keyboard.is_alt_only() then 
		BU.__class_alpha_factor = .4
	else
		local f = BU.__class_alpha_factor
		if f < 1 then
			BU.__class_alpha_factor = math.min( f + aaa.time.dt_real * 8, 1 )
		end 
	end
	BU:__update_draw_stuff()
end

--
-- CURRENT BU
--
doc.is_bu_cur = "() return if this bu is the current one."
function BU:is_bu_cur()
	return BU.__cur == self
end
--todo add mecanism to clear element which stop and a bug time and don't unregister
doc.set_bu_cur = "() set this bu as the current one, bu will get the keyboard events first."
function BU:set_bu_cur()
	--GA.dump_verbose_state()
	--self:print( "self.verbose is "..self.verbose.." "..BU.verbose )
	if BU.verbose > 0 then
		self:print( "will be current bu now" )
	end
	BU.__cur = self
	if BU.__b_debug_bu_cur then
		local str = self.." with id "..self:get_gabu_obj_id()
		aaa.show( str, "BU cur is" )
		aaa.print( "BU cur is "..str )
	end
end
doc.get_bu_cur = "() return the current bu."
function BU:get_bu_cur()
 	return BU.__cur
end

doc.save_pos_value_more = "( file ) is called by BUS:save_bu_pos() for every bu it contains\n"..
							 "define it to custom save data for your bu"
function BU:save_pos_value_more( file )
end

-- UI_TOP done with ALT
--
doc.__get_ui_top_interaction = "( x,y ) "
function BU:__get_ui_top_zone( x,y )
	local sub_x
	if		outside( x, -.5,.5 )	then
	elseif	x <= BU.__UI_TOP_LEFT	then	sub_x =	x/BU.__UI_TOP_LEFT											x = 1	
	elseif	x <  BU.__UI_TOP_RIGHT	then	sub_x =	(x-BU.__UI_TOP_LEFT)/(BU.__UI_TOP_RIGHT-BU.__UI_TOP_LEFT)	x = 2
	else									sub_x =	(x-BU.__UI_TOP_RIGHT)/(.5-BU.__UI_TOP_RIGHT)				x = 3
	end
	local sub_y
	if		outside( y, -.5,.5 )	then
	elseif	y <= BU.__UI_TOP_DOWN	then	sub_y =	y/BU.__UI_TOP_DOWN											y = 1
	elseif	y <  BU.__UI_TOP_UP		then	sub_y =	(y-BU.__UI_TOP_DOWN)/(BU.__UI_TOP_UP-BU.__UI_TOP_DOWN)		y = 2
	else									sub_y =	(y-BU.__UI_TOP_UP)/(.5-BU.__UI_TOP_UP)						y = 3
	end
	return x,y, sub_x,sub_y	--todo clarify sub_x,sub_y, still used by
end
doc.__get_ui_top_interaction = "( x,y ) "
function BU:__get_ui_top_interaction( x,y )
	--aaa.print_fn()
	local interaction
	if self:is_ui_top_size() then
		local lx,ly = self:__get_ui_top_zone( x,y )
		if lx and ly then
			local id = 3*(3-ly) + lx
			interaction = BU.__UI_ON_TOP_INTER[ id ]
		end
	elseif self:is_ui_top_move() then
		interaction = "move"
	end
	aaa.show( interaction, "BU interaction is" )
	--self:print( "interaction is "..interaction )
	return interaction
end

--
--	DEBUG
--
function BU:dump_contacts()
	local nb = self:get_contact_nb()
	if nb > 1 then
		for i = 1, nb do
			local blob = self.__blob_start[i]
			table.print( blob, i.."/ blob in bu" )
		end
		return true
	end
	return false
end

--
--	FLAG
--
--	UI_ON_TOP
--	ui on top is now for move and resize
function BU:__update_ui_top()
	self.__b_ui_top = self.__b_ui_top_move or self.__b_ui_top_size
end
function BU:set_ui_top_move( b )
	self.__b_ui_top_move = b
	self:__update_ui_top()
	return self
end
function BU:is_ui_top_move()				return self.__b_ui_top_move		end

function BU:set_ui_top_size( b )
	self.__b_ui_top_size = b
	self:__update_ui_top()
	return self
end
function BU:is_ui_top_size()				return self.__b_ui_top_size		end

function BU:set_ui_top( b )					self.__b_ui_top = b				return self		end
function BU:is_ui_top()						return self.__b_ui_top			end



function BU:set_ui_active( b )				self.__b_ui_active = b			return self		end
function BU:is_ui_active()					return self.__b_ui_active		end

function BU:set_visible( b )				self.__b_visible = b			return self		end
function BU:is_visible()					return self.__b_visible			end
function BU:is_invisible()					return not self.__b_visible		end

function BU:set_mobile( b )					self.__b_mobile = b				return self		end
function BU:disable_mobile()				self:set_mobile( false )		return self		end
function BU:is_mobile()						return self.__b_mobile			end


function BU:set_size_ratio_fix( b )			self.__b_size_ratio_fix = b		return self		end
function BU:is_size_ratio_fix()				return self.__b_size_ratio_fix	end

--todo move with __positions ?
function BU.set_pos_load_save_default( b )	BU.__b_pos_load_save_default = b	end
function BU:set_pos_load_save( b )			self.__b_pos_load_save = b		return self		end
function BU:is_pos_load_save()				return self.__b_pos_load_save	end
--todo regroup value and pos
function BU:set_value_load_save( b )		self.__b_value_load_save = b	return self end
function BU:is_value_load_save()			return self.__b_value_load_save	end
--todo regroup preset and pos value load save

--todo add global flag
function BU:set_highlight( b )				self.__b_highlight = b 			return self		end
--function BU:is_highlight() 				return self.__b_highlight		end

function BU:set_preset_use( b )				self.__b_preset_use = b			return self		end
function BU:is_preset_use()					return self.__b_preset_use		end

function BU:set_move_to_front( b )			self.__b_move_to_front = b		return self		end
function BU:is_move_to_front()				return self.__b_move_to_front	end

--todo 2023 October was done in MU but we need it for BU_RECT transform when BUS ans MUS merged
function BU:__set_mus_up( mus )				self.__mus_up = mus				end
function BU:__get_mus_up()					return self.__mus_up			end
--
--	BORDER
--
function BU:set_border( b )					self.__b_border = b				return self		end
function BU:is_border()						return self.__b_border			end

function BU:set_border_line( b )			self.__b_border_line = b		return self		end
function BU:is_border_line()				return self.__b_border_line		end

--
--	METHOD ON
--
--todo now we use result fron fn/method for click_double/triple/long check the fns return the right stuff
function BU:set_function_on_click(			fn,				... )	return self:set_function(	"click",		fn,					... )		end
function BU:set_function_on_click_double(	fn,				... )	return self:set_function(	"click_double",	fn,					... )		end
function BU:set_function_on_click_triple( 	fn, 			...	)	return self:set_function(	"click_triple", fn, 				...	)		end
doc.set_function_on_click_long = "( fn, ... ) fn should return true if it execute correctly or it will be call continuously"
function BU:set_function_on_click_long( 	fn, 			...	)	return self:set_function(	"click_long", 	fn, 				...	)		end

function BU:set_method_on_click(		obj, method_name,	... )	return self:set_method(		"click",		obj, method_name,	... )		end
function BU:set_method_on_click_double(	obj, method_name,	... )	return self:set_method(		"click_double",	obj, method_name,	... )		end
function BU:set_method_on_click_triple( obj, method_name, 	...	)	return self:set_method( 	"click_triple", obj, method_name, 	...	)		end
doc.set_function_on_click_long = "( obj, method_name, ... ) method should return true if it execute correctly or it will be call continuously"
function BU:set_method_on_click_long( 	obj, method_name, 	...	)	return self:set_method( 	"click_long", 	obj, method_name, 	...	)		end

--
--	DEPTH / DPLANE
--
function BU:set_dplane( depth )
	if not depth then return end

	local bus = self:get_bus_up()
	--	remove the BU from it's actual dplane
	bus:remove_bu_from_dplane( self )
	--	add the BU to the new dplane
	bus:add_bu_to_dplane( self, depth )

	--	update the BU dplane
	self.__dplane = depth
end
function BU:get_dplane()			return self.__dplane 	end

function BU:move_to_front()
	local bus = self:get_bus_up()
	bus:move_to_front( self )
end
function BU:move_to_back()
	local bus = self:get_bus_up()
	bus:move_to_back( self )
end

--
--	KIND
--
local	BU_SIGNATURE_VIDEO		=	"vid"
local	BU_SIGNATURE_IMAGE		=	"img"
local	BU_SIGNATURE_PAGE_TRS	=	"page_trs"

function BU:set_kind( str )			self.__kind = str							end
--function BU:set_kind_none()		self.__kind = "none"						end
function BU:set_kind_video()		self:set_kind( BU_SIGNATURE_VIDEO		)	end
function BU:set_kind_image()		self:set_kind( BU_SIGNATURE_IMAGE 		)	end
function BU:set_kind_page_trs()		self:set_kind( BU_SIGNATURE_PAGE_TRS	)	end

function BU:is_video()				return self.__kind == BU_SIGNATURE_VIDEO	end
function BU:is_img()				return self.__kind == BU_SIGNATURE_IMAGE	end
function BU:is_page_trs()			return self.__kind == BU_SIGNATURE_PAGE_TRS	end

function BU:get_dialog_name()
	local title = self:get_name()..""
	local m = self:get_meu_up()
	if m then
		title = title .. " In " .. m
	else
		local bus = self:get_bus_up()
		if bus then
			title = title .. " In " .. bus
		end
	end
	return title
end
--
--	MOUSE FNS
--
function BU:do_mouse_move( x,y )
	--aaa.print_fn()

	if self:is_uif_running() then
		self:__make_uif_data( x,y, false )
	else
		self:begin_uif( x,y, "mouse_move" )
	end
end

function BU:is_undo_redo_using_values()
	return self.__b_undo_redo_using_values
end

--todo deal with several interactions at the same time
-- these 2 fn should partly be in BUI
function BU:add_to_undo_history( value_prev, id )
	--aaa.debug.print_traceback()
	if self.__b_undo_redo_skip then
		return
	end

	local ur = ga:get_undo_redo()
	local verbose = ur:get_verbose()
	if verbose > 0 then
		aaa.print_fn()
	end
	--aaa.debug.print_traceback()
	local storage
	if value_prev then
		storage =	{	value_prev = value_prev,
						id = id or self:get_id_interacting()
					}
		self.__undo_redo_storage = storage
	else
		storage = self.__undo_redo_storage
	end
	if verbose > 1 then
		table.print( storage, "undo_redo_storage" )
	end

	if storage then
		if storage.pos_prev then
			ur:add_pos_to_undo_history( self )
		else
			local undo,redo, b_store
			--if value_prev~=nil and value_prev ~= value_next then
			if self:is_undo_redo_using_values() then
				local values_prev = storage.values_prev
				local values_next = self:get_values()
				local nb_prev = values_prev and #values_prev
				local nb_next = #values_next
				if nb_prev ~= nb_next then
					b_store = true
				else
					for i=1,nb_prev do
						if values_prev[i]==nil then aaa.debug.print_traceback( self..":add_to_undo_history() value_prev["..i.."] is nil" ) end
						if values_next[i]==nil then aaa.debug.print_traceback( self..":add_to_undo_history() value_next["..i.."] is nil" ) end							
						if values_prev[i] ~= values_next[i] then
							b_store = true
							break
						end
					end
				end
				if b_store then
					values_next = table.copy_simple(values_next)
					undo = self:make_table_to_call_method( self, "set_values", values_prev	)
					redo = self:make_table_to_call_method( self, "set_values", values_next	)
				end
			else
				local value_next,value_prev
				if self.get_value then
					value_next = self:get_value( storage.id )
					value_prev = storage.value_prev
				end
				if value_prev ~= value_next then
					if value_prev==nil then aaa.debug.print_traceback( self..":add_to_undo_history() value_prev is nil" ) end
					if value_next==nil then aaa.debug.print_traceback( self..":add_to_undo_history() value_next is nil" ) end
					self:print( value_prev .. " " .. value_next )
					--undo_redo.values_next = table.copy_simple(self:get_values())
					undo = self:make_table_to_call_method( self, "set_value", value_prev, storage.id )
					redo = self:make_table_to_call_method( self, "set_value", value_next, storage.id )
				end
			end
			if undo then
				ur:add_undo_redo( undo, redo )
			end
		end
	end
	self.__undo_redo_storage = nil
end

function BU:do_click_double( x,y )
	aaa.print_fn()
	self:print_debug( "BU:do_click_double() don't do anything for now" )
	self:do_flip_full_page_and_mini( nil, y )
end

function BU:do_click_down( x,y )
	--self:print( "xy "..x.." "..y )
	--aaa.debug.print_traceback()
--	self:print( "BU:do_click_down() at "..x..", "..y)
	--table.print( self.__positions, self.."__positions", 2 )
	--table.print( self, "DUMP "..self, 1 )

	self:set_bu_cur()

	ga:get_undo_redo():store_values_in_bu(self)

	-- local bus = self:get_bus_up()
	-- if bus then
	--  	x = x + bus.__x_offset_exp
	-- 	y = y + bus.__y_offset_exp
	-- end
	self:begin_uif( x,y, "action_down" )
end

function BU:do_click_up( x,y )
--	self:print( "do_click_up at "..x..", "..y )
-- if we do it the SLIDER don't catch it
	local uif = self:get_uif_data()
	local b_do_undo = true
	if uif then
		local name_sel = BU:get_uif_name_sel( uif )
		--aaa.print_fn()
		if name_sel then
			self:do_uif_command( uif )
			b_do_undo = not uif.b_undo_done	--hack for swap
			--b_do_undo = not 
			--self:print( "b_do_undo is "..b_do_undo )
		end
		self:end_uif( false, "do_click_up" )
	else
		if self.drop_on then
			BU.verbose_drop_on = 1
			local function verbose( str )
				if BU.verbose_drop_on > 0 then
					self:print_debug( "--- BU DROP "..str )
				end
			end

			-- we do that to get the right coor
			local x_top,y_top = self:convert_xy_local_to_top( x,y )
			if BU.verbose_drop_on > 0 then
				self:print_inverse( "--- BU DROP at "..x_top..","..y_top )
			end
			--table.print( blob_cur, "blob_cur ", 1  )
			
			-- search a receiver for the drop
			-- push visible attribute so we can't find self
			local b_visible = self:is_visible()
			self:set_visible( false )
			local bu_found
				--todo we should use the ga here no ?
				local finder
				--if true then
					finder = BUS_CTX:get_cur()
				--else
				--	finder = self:get_up_by_class( BUSS )
				---end
				bu_found = finder:find_bu_active_by_pos( x_top,y_top )
			-- pop visible attribute
			self:set_visible( b_visible )

			local drop_receiver
			if bu_found then
				local function verbose( str )
					if BU.verbose_drop_on > 0 then
						self:print_debug( "--- BU DROP "..str )
					end
				end

				if BU.verbose_drop_on > 0 then
					verbose( "found_bu is "..bu_found )
				end 
				while bu_found do -- check bu theb vus_up for receiver if not go up one level
					drop_receiver = bu_found:get_drop_receiver()
					if drop_receiver then
						verbose( "found drop_receiver "..drop_receiver.." from bu "..bu_found )
						break
					end

					local bus = bu_found:get_bus_up()
					verbose( "up to bus "..bus )
					-- if bus == bus_up then
					--	verbose( "dropped on my own bus forget it" )
					-- 	break
					-- end

					drop_receiver = bus:get_drop_receiver()
					if drop_receiver then
						verbose( "found drop_receiver "..drop_receiver.." from bus "..bus )
						break
					end

					bu_found = bus:get_bu_up()
					verbose( "up to bu "..bu_found )
				end
			else
				verbose( "No BU found_bu use "..app.." as receiver" )
				drop_receiver = app
			end

			local ret
			--if receiver:can_drop_receive() then
			if drop_receiver then
				--local x_local,y_local = self:convert_xy_bus_up_to_local( blob_cur.x,blob_cur.y )

				--ox = ox - self:get_x()
				--oy = oy - self:get_y()

				ret = self:drop_on( drop_receiver, x_top,y_top ) --, x_local,y_local  )
				--aaa.box_good( "ret is "..ret )
			else
				ret = false
				verbose( "no drop_receiver" )
			end

			verbose( "result is "..ret.." "..(ret and "move" or "cancel move").." pos "..self:get_x().." "..self:get_y() )
			if ret then	
				--local sx,sy = self:get_sxy()
				--self:print( "sxy is "..sx.." "..sy )
				--self:print( "drop on "..receiver.." ret is "..ret )	
			--self:cpy_cur_to_all( "BU DROP" )	--todo refine
				--local sx,sy = self:get_sxy()
				--self:print( "sxy is "..sx.." "..sy )
			else
				self:cpy_reference_to_cur( "BU DROP" )
				--self:cpy_orig_to_cur( "BU DROP" )
				--self:cpy_cur_to_all( "BU DROP" )
				--self:cpy_cur_to_orig( "BU DROP" )
			end
		end	
	end
	if b_do_undo then
		self:add_to_undo_history()
	end
end

--
--	FNS
--
function BU:init_post()
	local video = self:get_video()
	if video then
		if self.__b_video_play_on_page_strict then
			video:add_play_on_page( self.__page )
		end
		if self.__b_video_play_on_page then
			local page = self.__page
			video:add_play_on_page( page-1 )
			video:add_play_on_page( page )
			video:add_play_on_page( page+1 )
		end
	end
end

function BU:set_to_start()
	if self.verbose >= 1 then aaa.print_method() end
	self:cpy_orig_to_cur( "BU:set_to_start" )

	if self:is_video() then
		local video = self:get_video()
		if video then
			video:set_restart()
		end
	end
end

function BU:set_page( page )
	self.__page = page
end

--todo what we do to copy second value of a SLIDER_TWO
function BU:do_copy()
	BUI.__clipboard = self:get_values()
	self:print( "clipboard is now "..GABU_OBJ:serialize_to_line( "", BUI.__clipboard) )
	return true
end
function BU:do_copy_shift()
	self:print( "Shift Copy Asked" )
	local bu_list = self:get_bu_list()
	if bu_list then
		--table.print( bu_list, "Shift Copy Src", 1 )
		local t = {}
		local nb = 0
		for k,bu in pairs_sorted(bu_list) do
			local val = bu:get_values()
			--self:print( k.." value is "..val )
			if val~=nil then
				nb = nb + 1
				--table.print( val, self..":do_copy_shift()" )
				local name = bu:get_name_lowercase()
				t[name] = val
				self:print( GABU_OBJ:serialize_to_line( name.." = ", val ) )
			end
		end
		--table.print( t, "t", 1 )
		--array.apply_method( bu_list, "print", "toto" )
		if nb > 0 then
			BUI.__clipboard_bu_list = t
			return true
		else
			aaa.debug.show_warning( "Shift copy: List of Bu is empty" )
			--self:print( "Shift copy: List of Bu is empty")
		end
	else
		aaa.debug.show_warning( "Shift copy: no List of Bu" )
		--self:print_error( "Shift copy: no List of Bu" )
	end
end

function BU:do_paste()
	local clip = BUI.__clipboard
	if clip then
		if self.set_values_ui then
			ga:get_undo_redo():store_values_in_bu(self)
			self:set_values_ui( clip )
			self:print( "pasted "..GABU_OBJ:serialize_to_line( "", clip ) )
			return true
		else
			aaa.debug.show_warning( "paste : this BU have no set_values_ui" )
			--self:print( "paste : this BU have no set_values_ui" )
		end
	end
end

function BU:do_paste_shift()
	self:print( "Shift Paste Asked" )
	local clip = BUI.__clipboard_bu_list
	if clip then
		--table.print( clip, "__clipboard_bu_list")
		local bu_list = self:get_bu_list()
		if bu_list then
			for _,bu in pairs_sorted(bu_list) do
				local name = bu:get_name_lowercase()
				local val = clip[name]
				--self:print( "name is "..name.." val is "..val )
				if val~=nil and bu.set_values_ui then
					bu:set_values_ui(val)
					--self:print( GABU_OBJ:serialize_to_line( name.." <- ", val ) )
				end
			end
			return true
			--array.apply_method( bu_list, "print", "toto" )
		else
			aaa.debug.show_warning( "No Group to Shift paste" )
			--self:print( "No Group to Shift paste" )
		end
	else
		aaa.debug.show_warning( "No clipboard to Shift paste" )
		--self:print( "No clipboard to Shift paste")
	end
end

function BU:__is_movable_by_key()
	return (self.__positions.b_window or self:is_ui_top_size()) and (not self.__b_fix)
end

function BU:do_key( key )
	self:print_do_key( "BU", key )

	local b_key_used = false
	if key == 3 or key == 24 then	-- CTRL c or CTRL x
		-- if no get_values we fallback on shift version 2024 Sep
		if not self.get_values or aaa.keyboard.is_shift() then
			b_key_used = self:do_copy_shift()
		else
			b_key_used = self:do_copy()
		end
	elseif key == 22 then	-- CTRL v
		if aaa.keyboard.is_shift() then --todo this should be passed wuth the key ?
			b_key_used = self:do_paste_shift()
		else
			b_key_used = self:do_paste()
		end
	elseif key==32 then --space
		if self.__positions.b_window then
			if key == 32 then
				self:set_window_state( aaa.keyboard.is_ctrl() and "flip_mini" or "flip_full" )
				b_key_used = true
			end
		end
	elseif aaa.keyboard.is_alt() then
		if self:__is_movable_by_key() then
			if key==43 or key==45 or key==42 or key==47 then
		--		if not self:is_mini() and not self.__b_fix then
				local sx,sy = self:get_sxy()
				local f
				if key==43 or key==45 then
					f = 1 + 1/8
					f = key==43 and f or 1/f
				else
					f = key==42 and 2 or 1/2
				end
				self:set_sxy( sx*f,sy*f )
				b_key_used = true
			end
		else
			self:show_error_ui( "is not movable" )
		end
		
	elseif key==127 then
		local bu = self:get_bu_up()
		if bu then
			self:print( "try to pass key "..key.." up" )
			b_key_used = bu:do_key( key )
		end
	end
	
	return b_key_used or oo.getsuper(BU).do_key( self, key )
end

function BU:do_key_special( key )
	self:print_do_key_special( "BU", key )

	local b_key_used = false

	local bu_up = self:get_bu_up()
	if bu_up then
		self:print( "BU:do_key_special() try with bu up "..bu_up )
		b_key_used = bu_up:do_key_special( key )
	end

	if not b_key_used then
		--move or change qwith arrow apply only to cur BU
		if self:is_bu_cur() then
			local bus = self:get_up()
			if bus then
				if aaa.keyboard.is_alt() then
					if self:__is_movable_by_key() then
						if inside( key, 272, 275 ) then
							local x,y = self:get_xy()
							local dx,dy = self:get_sxy()
							local f = 1/16
							dx,dy = dx*f,dy*f
							if 		key == 274 then	x = x - dx	-- ARROW_LEFT
							elseif	key == 275 then	x = x + dx	-- ARROW_RIGHT
							elseif	key == 273 then	y = y - dy	-- ARROW_DOWN
							elseif	key == 272 then	y = y + dy	-- ARROW_UP
							end
							self:set_xy( x,y )
							b_key_used = true
						end
					else
						self:show_error_ui( "is not movable" )
					end
				else
					local direction
					if	   key == 272 or key == 273 then			-- ARROW_UP ARROW_DOWN
						direction = (key == 273) and "down" or "up"	
					elseif key == 274 or key == 275 then			-- ARROW_LEFT ARROW_RIGHT
						direction = (key == 274) and "left" or "right"
					end
					if direction then
						local bu_next = bus:found_bu_next( self, direction )
						if bu_next then
							bu_next:set_bu_cur()
							b_key_used = true
						end
					end
				end
			end
		end
	end

	return b_key_used or oo.getsuper(BU).do_key_special( self, key )
end

--
--	VALUES
--
-- rest is in BUI but some BU are not BUI but can deal with values 
function BU:bus_set_value( value )
	--if value~=nil and self:is_value_load_save() then
	if self:is_value_load_save() then
		if self.set_values then
			self:set_values(value)
		elseif self.set_value then
			self:set_value(value)
			--self:update()
		else
			self:box_debug( "set_values() or set_value() required here" ) 
		end
	end
end

function BU:get_values_as_str()
	local str
	if self.get_values then -- refine test on bu.get_value
		local values = self:get_values()
		str = self:serialize_to_line( "", values )
	elseif self.get_value then -- refine test on bu.get_value
		local value = self:get_value()
		if type(value) == "table" then
			str = "{ "
			for k,v in pairs(value) do
				str = str .. k .."="..v..","
			end
			str = str .. " }"
		else
			str = "" .. value
		end
	end
	return str
end
--used now todo the send for Monaco
function BU:get_values_as_str_monaco()
	local values = self:get_values()
	--table.print( values, "values", 2 )
	local values_type = type(values)
	local str
	if values_type=="table" then
		local nb = #values
		str = "{"
		if nb >= 0 then
			for i = 1, nb do
				self:print( i )
				if i~=1 then	str = str .. ","	end
				local val = values[i]
				if type(val)=="string" then 	str = str.."\""..val.."\""
				else							str = str..val
				end
			end
		end
		str = str.."}"
	else
		if values_type=="string" then 	str = "\""..values.."\""
		else							str = values
		end
	end
	return str
	-- local balues = self:__get_balues()
	-- table.print(balues,"balues", 2)
	-- local nb = #balues
	-- local str = "{"
	-- if nb >= 0 then
	-- 	for i = 1, nb do
	-- 		self:print( i )
	-- 		if i~=1 then	str = str .. ","	end
	-- 		local bal = balues[i]
	-- 		local val = self:get_value(i)
	-- 		if bal then
	-- 			val = bal:get_value(1)
	-- 		else
	-- 			self:print_error( "no balue in balues for id "..i.."\n using 0" )
	-- 			val = 0
	-- 		end
	-- 		if type(val)=="string" then 	str = str.."\""..val.."\""
	-- 		else							str = str..val
	-- 		end
	-- 	end
	-- end
	-- str = str.."}"
	-- return str
end

--
--	CREATE INIT
--
--[[
function BU:__init_new() --text, text_size )
--	self.__blob_start = {} --todo
--	self.__blob_cur = {}
--	self.color = {}
end
--]]
function BU:__init_new( rect )
	local bu_id = BU.id_generator + 1
	BU.id_generator = bu_id
	self.__bu_id = bu_id

	-- if self:get_name_lowercase() == "first" then
	-- 	table.print( rect, self.." rect" )
	-- end

	rect = BUP:__make_rect( rect, self )

	self:__init_pos(rect)
end

function BU.create_instance( class, name, rect )
	local self = oo.getsuper(BU).create_instance( class, name )
	-- if self:get_name_lowercase() == "first" then
	-- 	table.print( rect, self.." rect" )
	-- end
--	self:print( "self.verbose is "..self.verbose.." "..BU.verbose )
--	self:box_debug( "BU before" )

	self:__init_new( rect )

	self:set_text( name )
	self:set_active_direct( true )
	if BU.__b_pos_load_save_default ~= nil then
		self:set_pos_load_save( BU.__b_pos_load_save_default )
	end
	--use to test
	--self:set_back_bind(33)

--	self:print( "self.verbose is "..self.verbose.." "..BU.verbose )
--	self:box_debug( "BU after" )
	return self
end
function BU:create( name, rect )
	local self = BU:create_instance( name, rect )
	return self
end
function BU:create_image( name, rect, filename_or_bind_or_img, b_auto )
	--aaa.print_fn()

	local img, b_err = self:get_image_common( filename_or_bind_or_img )
	if b_auto then
		if rect[4] then
			rect[3] = rect[4] * img:get_rx()
		elseif rect[3] then
			rect[4] = rect[3] * img:get_ry()
		else
			--hack
			rect[3] = img:get_rx()
			rect[4] = 1
		end
	end
	local self = BU:create( name, rect )
	self:set_img( img )
	self:set_kind_image()
	self:set_back_bind( 33 )
	
	return self
end

--todo rethink all this
function BU:free()
	--oo.getsuper(BU).free( self )
end

-- 2024 Nove Mâa removed it: too confusing
-- function BU:rename( name )
-- 	self:set_text( name )
-- 	self:set_color_back( name )
-- end

--
--	TEX
--
function BU:__assign_post_common( sxp,syp )
	local bind = self:get_texture_bind_2d()
	if sxp or syp then
		if sxp then
			if not syp then
				local sx, sy = aaa.img.get_size( bind )
				syp = sxp * sy / sx
			end
		else
			local sx, sy = aaa.img.get_size( bind )
			sxp = syp * sx / sy
		end
		local cur = self.__positions.used.cur
		self:set_pos_orig( {cur.x,cur.y, sxp,syp, cur.angle} )
	end
	self:set_kind_image()
	return self

end
function BU:assign_texture( bind, sxp,syp )
	self:detach_video()
	if bind then self:set_texture_bind_2d( bind ) end
	return self:__assign_post_common( sxp,syp )
end
function BU:assign_img( img, sxp,syp )
	self:detach_video()
	if img then	self:set_img( img ) end
	return self:__assign_post_common( sxp,syp )
end

function BU:get_image_common( filename_or_bind_or_img )
	local img, b_err
	local t = type( filename_or_bind_or_img )
	if t == "string" then
		img = IMGS.get_img( filename_or_bind_or_img )
	elseif t == "number" then
		img = IMGS.get_img_from_bind( filename_or_bind_or_img )
	elseif filename_or_bind_or_img == nil then
		--if self.verbose >= 1 then
		aaa.debug.print_traceback()
		self:print_error( "can't find an image from nil" )
	else
		img = filename_or_bind_or_img
	end

	if not img then
		img = TEXS:get_bind_by_name("PB")
		b_err = true
		--if self.verbose >= 1 then
			--aaa.debug.print_traceback()
		--	self:print_debug( "We use the PB Texture, because could not get a bind from image "..filename_or_bind_or_img )
		--end
	end

	return img, b_err
end
function BU:add_image( filename_or_bind_or_img )
	local img, b_err = self:get_image_common( filename_or_bind_or_img )

	local images = self.__images
	if not images then
		self.__images = {}
		images = self.__images
		if self.__img then
			images[1] = self.__img
		end
		self.__images_cur = 1
	end
	--table.print( images, "avant" )
	table.insert( images, img )
	--table.print( images, "apres" )
end

--
--	BROWSE
--
function BU:change_ratio_affect_x()
	local bind = self:get_texture_bind_2d()
	local sx, sy = aaa.img.get_size( bind )
	local rx = sx/sy
	local pos = self.__positions.used
	pos.orig.sx			=	pos.orig.sy			* rx
	self:set_sxy( pos.cur.sy * rx, pos.cur.sy )
	pos.cur.sx			=	pos.cur.sy			* rx
	pos.reference.sx	=	pos.reference.sy	* rx
	pos = self.__positions.memory
	if pos then
		pos.sx = pos.sy	* rx
	end
end
function BU:set_media_index( index )
	if not self.__images_cur then return end
	local nb = #self.__images
	index = ((index - 1) % nb ) + 1
	local img = self.__images[ index ]
	if self.verbose >= 1 then self:print( index.." / "..nb ) end
	self.__images_cur = index
	local sx,sy = self:get_sxy()
	self:assign_img( img, nil, sy )
	self:change_ratio_affect_x()
end
function BU:next_media()
	if not self.__images_cur then return end
	self:set_media_index( self.__images_cur + 1 )
end
function BU:prev_media()
	if not self.__images_cur then return end
	self:set_media_index( self.__images_cur - 1 )
end

function BU:set_focus()
	if self:is_video() then
		local video = self:get_video()
		if video then
			video:set_focus()
		end
	else
		oo.getsuper(BU).set_focus(self)
	end
end

--
--	BUS / UP / DOWN
--
function BU:get_bus_down()	return self:get_down_no_error(1)	end
function BU:add_bus_down( bus )
	if not bus then
		self:print_error( "calling add_bus_down( bus ) with a nil bus, this should not happen" )
		return
	end
	if self:get_down_no_error(1) then
		self:print_error( "calling add_bus_down( bus ) when the BU already have a bus_down, this should not happen" )
		return
	end
	if bus:get_bu_up() then
		self:print_error( "calling add_bus_down( bus ) when the bus already attached to a BU, this should not happen" )
		return
	end
	self:add_down(bus)
end
function BU:remove_bus_down()
	local bus = self:get_bus_down()
	if not bus then return end	--	this way if we detach too much we don't crash

	self:remove_down(bus)
	return bus
end

function BU:find_bu_by_name_lowercase( bus_name, bu_name )
	--self:print( "going in BU" )
	local bus = self:get_bus_down()
	if not bus then return end
	--self:print( "have a bus down "..bus )
	return bus:find_bu_by_name_lowercase( bus_name, bu_name )
end

function BU:get_bus_up_no_error()	return self:get_up_by_class( BUS )	end
function BU:get_bus_up()
	local bus_up = self:get_up_by_class( BUS )
	if not bus_up then
		self:print_error( "__up is "..self.__up )
		aaa.debug.print_traceback( "this bu have no bus" )
		--debug.debug()
	end
	return bus_up
end
function BU:get_bu_up()
	local bus_up = self:get_bus_up()
	if bus_up then
		return bus_up:get_bu_up()
	end
end

--todo perhaps we have to go up several level here
function BU:get_meu_up()
	local bus = self:get_bus_up()
	if bus then
		local meu = bus.__meu_owner
		return bus.__meu_owner
	end
end

function BU:is_name_match( pat )
	local b_match = oo.getsuper(BU).is_name_match( self, pat )
	if (not b_match) and self.get_text then
		local text = self:get_text()
		if text then
			text = string.lower( text )
			b_match = string.find( text, pat ) ~= nil
		end
	end
--	self:print( "match is "..b_match )
	return b_match
end
function BU:mark_by_match_pattern( pat )
	self:apply_method_down( "mark_by_match_pattern", pat )
	
	local b_match = false
	if pat ~= "" then
		b_match = self:is_name_match(pat)
		if b_match then
			--self:print( "match pattern "..pat )
		end
	end
	self:__set_draw_marked( b_match )
end


function BU:set_syno( syno )
	self.__syno = string.lower(syno)
end
function BU:set_bu_group( gr )
	self.__bu_group = gr
end

doc.get_bu_list = {	"() return a table the bu is part of,",
						"  can be the bu of bu_group or from a MEU. "}
function BU:get_bu_list()
	local gr = self.__bu_group
	if gr then
		--table.print( gr, self..":get_bu_list()" )
		return gr
	end

	local meu = self.__meu_attached
	if meu then
		return meu:__get_bu_for_preset(0)
	end
end
--
--	DROP
--
function BU:set_drop_receiver( drop_receiver )
	self.__drop_receiver = drop_receiver
end
function BU:get_drop_receiver()
	local receiver = self.__drop_receiver
	if receiver then
		return receiver
	end
	local bus = self:get_bus_down()
	if bus then
	 	return bus:get_drop_receiver()
	end
end

--
--	CONTACT
--
function BU:find_by_blob_id( blob_id )
	if self.verbose >= 2 then
		aaa.print_method()
	end
	local bus_down = self:get_bus_down()
	if bus_down then
		local found = bus_down:find_by_blob_id( blob_id )
		if found then return found end
	end
	local contact_nb = self:get_contact_nb()
	if contact_nb > 0 then
		local blob = array.find_elt_by_id( self.__blob_start, contact_nb, blob_id )
		if blob then
			if self.verbose >= 1 then self:print_inverse( "found by blob id "..blob_id ) end
			return self
		end
	end
	---[[
	if self.verbose >= 2 then
		if not self:dump_contacts() then
			self:print( "\tno contact" )
		end
	end
	--]]
end
function BU:set_contact_nb( nb )	self.__contact_nb = nb				end
function BU:get_contact_nb()		return self.__contact_nb			end
function BU:is_contact()			return self:get_contact_nb() > 0	end
function BU:get_contact_nb_down()
	local nb = self:get_contact_nb()
	local bus_down = self:get_bus_down()
	if bus_down then
		nb = nb + bus_down:get_contact_nb_down()
	end
	return nb
end
function BU:clean_contact()
	self:set_contact_nb( 0 )
	local bus_down = self:get_bus_down()
	if bus_down then
		bus_down:clean_contact()
	end
end

doc.__update_no_finger = "() deal with the BU update when there is no contact, called by BU:update()"
function BU:__update_no_finger()
	--	self:print( "BU:__update_no_finger()" )
	--	only one contact : translation
	--	self:print( "interact_update index = "..index )
	--	self:print( "dx = "..dx..", dy = "..dy )
	if self:is_page_trs() then
	else
		if ga.__b_inertia and self:is_inertia() then
			self:do_inertia_trs()
			--todo it also when in mono contact after a bitouch
			self:do_inertia_sca()
		end
	end
	--we do tests here because this code path is time critical
	local fx = self.__fx_on_click
	if fx then
		local ph = fx.ph
		if ph then
			self:__update_fx()
		end
	end
end

doc.__update_1_finger = "() deal with the BU update when there is 1 contact, called by BU:update()"
function BU:__update_1_finger()
--	aaa.print_fn()
	if self.verbose >= 3 then aaa.print_method() end

	--self:print( "__update_1_finger "..self.__ui_interaction_type )
	local bx,by = self.__blob_cur[1].x, self.__blob_cur[1].y
	--hack	removed test for n5
	if self.__ui_interaction_type == "custom" then
		--self:print( "do_mouse_move blob "..bx..", "..by.." )" )
		local x,y = self:convert_xy_bus_up_to_local( bx,by )
		--self:print( "do_mouse_move( "..x..", "..y.." )" )
		self:do_mouse_move( x,y )
		return
		--if not self.__ui_interaction_type == "move" then return end
	elseif self.__ui_interaction_type == "scrub" then
		local x,y = self:convert_xy_bus_up_to_local( bx,by )
		self:get_video():scrub_cano( x + .5 )
		return
	end

	--	only one contact
	local dx = bx - self.__blob_start[1].x
	local dy = by - self.__blob_start[1].y

	--self:print( aaa.keyboard.is_shift() )
	--	self:print( "BU:__move_dxy() "..dx.." "..dy )
	--hack dior for uv_edit
	--self:print( "BU:__update_1_finger() __ui_interaction_type is "..self.__ui_interaction_type )
	if aaa.keyboard.is_ctrl() and self.__ui_interaction_type~="move" then --test on "move added for MU (should we refine ?)"
		local f = 4
		if aaa.keyboard.is_shift() then	dx,dy = dx*f, dy*f
		else							dx,dy = dx/f, dy/f
		end
	end
	self:__move_dxy( dx,dy, aaa.keyboard.is_shift(), not self.__blob_cur[1].b_ghost )
end

doc.__update_2_finger = "() deal with the BU update when there is 2 contacts, called by BU:update()"
function BU:__update_2_finger()
	if self.verbose >= 3 then aaa.print_method() end

	if not (self.__ui_interaction_type == "move") then return end

	local bx1, by1 = self.__blob_cur[1].x, self.__blob_cur[1].y
	local bx2, by2 = self.__blob_cur[2].x, self.__blob_cur[2].y

	--	2 contact : translation, scaling, rotation
	--	only one contact : translation
	-- get original vector at start of the double interaction
	local st_x = self.__blob_start[2].x - self.__blob_start[1].x
	local st_y = self.__blob_start[2].y - self.__blob_start[1].y

	--todo isolate rest with a method in BU_pos
	-- get current vector
	local cu_x = bx2 - bx1
	local cu_y = by2 - by1
	-- get new angle
	local new_angle

	local reference = self.__positions.used.reference
	if ga.b_rotate and self:is_rot() then
		new_angle = math.get_angle( cu_x,cu_y ) - math.get_angle( st_x,st_y ) + reference.angle
	else
		new_angle = 0.
	end
	-- get new_scale
	local scale
	if self:is_scale() then
		scale = math.get_length( cu_x,cu_y ) / math.get_length( st_x,st_y )
	else
		scale = 1
	end

	local scale_x, scale_y = self:constraint_scale( scale * reference.sx, scale * reference.sy )

	local bus = self:get_bus_up()
	-- get original vector at start of the double interaction	
	--	1/ translation
	local vx = self.__blob_start[1].x - reference.x - bus.__x_offset_exp
	local vy = self.__blob_start[1].y - reference.y - bus.__y_offset_exp
	--	2/ rotation
	vx, vy = math.rotate_ab_turn( vx, vy, -reference.angle )
	--	3/ scale
	vx = vx / reference.sx
	vy = vy / reference.sy
	--then back
	--	3/ scale
	vx = vx * scale_x
	vy = vy * scale_y
	--	2/ rotate
	vx, vy = math.rotate_ab_turn( vx, vy, new_angle )
	--	1/ translation
	vx = bx1 - vx
	vy = by1 - vy
	vx = vx - bus.__x_offset_exp
	vy = vy - bus.__y_offset_exp
--	self:print( vx  )

--	if self.bu_container then
--		vx, vy = self.bu_container:convert_xy_local_to_bus_up( vx, vy )
--	end

	--	self:print( "image cur x = "..self.image_cur.x..", y = "..self.image_cur.y )
	--	self:print( "cx = "..cx..", cy = "..cy )
	self:__set_rect_inertia(	{vx,vy, scale_x,scale_y, new_angle},
								(not self.__blob_cur[1].b_ghost) or (not self.__blob_cur[2].b_ghost)
					 			)
end

--	carefull this called by bui SLIDER ... as SUPER
doc.update = "() deal with the interaction all the derived class should call it thru a cascading necanism"
function BU:update()
	-- if CLASS.is_gabu_obj( self, BU ) then
	-- 	aaa.print_fn( true )
	-- end

	-- done by the caller
	--if not self:is_active() then return	end

	--hack
	if self.verbose >= 3 then aaa.print_method() end
	
	if self.__fn_table then
		self:do_fn( "update_before" )
	end

	--update all the blob in contact
	local contact_nb = self:get_contact_nb()
	if contact_nb > 0 then
		local to_remove = {}
		for i = 1, contact_nb do

			local click = self.__click
			if not click then
				--todo perhaps we should change strategy to avoid creation of table 
				click = { time = -42., x=-1000., y=-1000. }
				self.__click = click
			end
			
			-- click long
			local t = aaa.time.t_real
			local d_time = t - click.time
			click.b_long = d_time > ga.click_long_time

			--self:print( "get_ready_for_update "..bu.." "..bu.__blob_start[j] )
			--self:print( blobs:find_by_id( bu.__blob_start[j].id ) )
			local blob_id = self.__blob_start[i].id
			local blobs = ga:get_blobs()
			local blob = blobs:find_by_id( blob_id )
			--self:print( self:get_contact_nb_down() )
			if blob then
				local b_do_it = true
				if click.b_long then
					if self:is_uif_running() then
						b_do_it = false
					elseif self:is_class_name( "BU_TEXT" ) then
						if self:is_editable() then
							if self:is_editable_active() then
								b_do_it = false
							else
								b_do_it = true
							end
						end
					else
						b_do_it = true
					end
					--self:print("There is a click long")
					if b_do_it then
						local b_have, ret = self:do_fn( "click_long" )
						if b_have then
							if ret then
								--self:print("remove contact")
								table.insert( to_remove, blob_id )
								bus_cur:dispatch_click( self, 1 ) --	last arg for number of click
								--self:print( "click_long done")
							end
						else
							if self.verbose >= 2 then self:print( "\tno fn for click_long" ) end
						end
					end
				end
				--	self:print( "Updating "..j )
				--	self:print( table.show( bu.__blob_start[j], "blob_start" ) )
				--	self:print( table.show( bu.__blob_cur[j], "blob_cur" ) )
				--	self:print( table.show( blobs.blob[index], "blobs" ) )
				local blob_cur = self.__blob_cur
				if not blob_cur then
					self.__blob_cur = {}
					blob_cur = self.__blob_cur
				end
				blob_cur[i] = {}
				blob:copy_to( blob_cur[i] )
				self:convert_blob_from_top_to_bus_up( blob_cur[i] )
				--self:print( blob.."Blob updated " )
			else
				self:print_debug( self.." can't find contact blob "..blob_id )
				self:print_debug( "---> will remove contact for security" )
				table.insert( to_remove, blob_id )
			end
		end

		for i=1, #to_remove do
			self:remove_contact( to_remove[i] )
		end
		contact_nb = self:get_contact_nb()
	end

	--[[
	if contact_nb > 0 then
		self:print( "nb contact = "..contact_nb )
		self:print( "self:is_page_trs() return " .. self:is_page_trs() )
	end
	--]]

	if contact_nb==0 then
		self:__update_no_finger()
	elseif contact_nb==1 or ga.max_finger_interact==1 or self:is_page_trs() then
		self:__update_1_finger()
	elseif contact_nb >= 2 or ga.max_finger_interact==2 then
		self:__update_2_finger()
	end

	if self:is_mini() then
	else 
		local bus = self:get_bus_down()
		if bus then
			bus:update()
		end
	end
end

function BU:remove_contact_low( blob_id )
--	aaa.print_fn( true )

	local nb_prev = self:get_contact_nb()
	--self:print( "toto" )
	if nb_prev == 1 then
		local bus_up = self:get_bus_up()
		local x,y
		local blob_cur
		if self.__blob_cur then
			blob_cur = self.__blob_cur[1]
			x,y = self:convert_xy_bus_up_to_local( blob_cur.x - bus_up.__x_offset_exp, blob_cur.y - bus_up.__y_offset_exp )
		end
		local blob = self.__blob_start[1]
		--self:print( "before do_click_up()" )
		self:do_click_up( x,y )
		if blob_cur and blob.id == blob_id then --hack bigue blob_cur crash on sleep MU
			--self:print( "Remove last contact, id = "..id )
		else
			self:print_debug( "WEIRD wasn't touching "..self.." : we ignore" )
		end

		self:set_contact_nb( 0 )
		--self:cpy_cur_to_reference( "BU:remove_contact_low" )
		self.__blob_start = nil
		self.__blob_cur = nil
		if self.__ui_interaction_type == "scrub" then
			self:get_video():set_scrub( false )
		end
		self.__ui_interaction_type = nil
	else
		--find
		local i = array.find_index_by_id( self.__blob_start, nb_prev, blob_id )
		--remove
		if i then
			table.remove( self.__blob_start, i )
			self:set_contact_nb( nb_prev - 1 )
		end
		local blobs = ga:get_blobs()

		for i=1,self:get_contact_nb() do
			-- to restart interaction the cur became the start
			local blob_start = self.__blob_start[i]
			local blob = blobs:find_by_id( blob_start.id )
			if blob then
				--maa	self.__blob_start[i] = table.deepcopy( blob )
				blob:copy_to( blob_start )
				self:convert_blob_from_top_to_bus_up( blob_start )
			end
		end
	end



	if self.verbose >= 1 then self:print( "removed contact now "..self:get_contact_nb() ) end
end

function BU:remove_contact( blob_id )
	if not self then aaa.print_error( "remove_contact() called with a nil bu" ) return end

	local contact_nb = self:get_contact_nb()
	if self.verbose >= 1 then self:print( " Remove contact blob "..blob_id.." from contact_nb "..contact_nb ) end
	if contact_nb > 0 then	--hack to avoid problem when putting MU to sleep
		if contact_nb == 1 then
			if self:is_page_trs() then
				local bus = self:get_bus_up()
				local x = bus.__x_offset_exp
				local p = math.floor( -x/bus.__page_sx + .5 )
				bus_cur:set_page( p )
				local y = bus.__y_offset_exp
				p = math.floor( y/6.+.5 )
			end
			local fx = self.__fx_on_click
			if fx and fx.b_on_click then
				self:begin_fx()
			end
		else
		end
		self:remove_contact_low( blob_id )
	end
end

function BU:__refresh_old_touch()
	local dst_index = 0
	local blobs = ga:get_blobs()
	for i= 1, self:get_contact_nb() do
		local blob = blobs:find_by_id( self.__blob_start[i].id )
		if blob then
			dst_index = dst_index + 1
			local blob_start = self.__blob_start[dst_index]
			blob:copy_to( blob_start )
			self:convert_blob_from_top_to_bus_up( blob_start )
			--blobs:add_used( blob )
		else
			self:set_contact_nb( self:get_contact_nb() - 1 )
			self:print_error( "contact "..i.." does not exist anymore!!!!" )
		end
	end
end


function BU:remove_contact_all()
	for i = self:get_contact_nb(),1,-1 do
		self:remove_contact( self.__blob_start[1].id ) -- 1 because we remove any in the fn
	end
	--self:box_debug( self:get_contact_nb() )
end

function BU:reset()
	self:remove_contact_all()
	self:set_to_start()
end

function BU:__add_contact( blob, ui_type )
--	aaa.print_fn()

	blob.__b_consumed = true	--	to avoid using the same blob twice ( on two bus )

	self:__refresh_old_touch()
	self:set_contact_nb( self:get_contact_nb() + 1 )
	local blob_start = self.__blob_start
	if not blob_start then
		self.__blob_start = {}
		blob_start = self.__blob_start
	end
	--self:print( "------------------ "..blob.x.." "..blob.y )
	--self:print( "__add_contact "..ui_type )

	local contact_nb = self:get_contact_nb()
	if not blob_start[contact_nb] then
		blob_start[contact_nb] = {}
	end
	blob:copy_to( blob_start[contact_nb] )
	--maa self.__blob_start[self.contact_nb] = table.deepcopy( blob )
	--ga:get_blobs():add_used( blob )
	self:cpy_cur_to_reference( "BU:__add_contact" )
	if ui_type then
		--self:print( "switch ui_type to "..ui_type )
		self.__ui_interaction_type = ui_type
	end
	if self:is_page_trs() then
		--self.bus:print( "toto" )
		local bus = self:get_bus_up()
		bus.__x_offset_start = bus.__x_offset_exp
		bus.__y_offset_start = bus.__y_offset_exp
	elseif self:is_move_to_front() then
		self:move_to_front()
	end
	if self.verbose >= 1 then self:print( "Add_contact_low "..blob.." for "..ui_type.." : now "..self:get_contact_nb().." contacts" ) end
	if self.verbose >= 3 then table.print( self.__blob_start, "add_contact" ) end
	--self:print( table.show( self, "BU:add_contact done now" ) )
end

function BU:is_click_double()	return self.__click and self.__click.b_double end
--function BU:is_click_triple() 	return self.__click and self.__click.b_double and self.__click.b_triple end
function BU:is_click_triple() 	return self.__click and self.__click.b_triple end
function BU:get_click_xy()
	local click = self.__click
	if click then
		return click.x, click.y
	end
end

--todo redo it with BALUE
function BU:set_send( b )
	self.__b_no_send = not b
	return self
end
function BU:is_send()
	return not self.__b_no_send
end

BU.trapped_method_send = { click=true, click_double=true,  click_triple=true, click_long=false }
function BU:do_fn( method_name, ... )
	local b_send = BU.trapped_method_send[method_name]
	if b_send then
	--	self:print( "BU:do_fn "..method_name )
	 	local fn = app and app.send_bu_do_method
	 	if fn then
	 		fn( app, self, method_name, ... )
	 	end
	end
--	return self:do_fn( method_name )
--	oo.getsuper(BU).do_fn( self, method_name )	
	-- if method_name == "click_double" then
	-- 	self:print_debug( "will now call method "..method_name.." using GABU_OBJ" )
	-- 	table.print( self.__fn_table, "fn_table" )
	-- end
	return GABU_OBJ.do_fn( self, method_name, ... )
end
--BU.do_fn_trapped = nil

function BU:__print_value( pre )
	local str
	if self.get_value then
		str = "value is "..self:get_value()
		if self.get_value_cano then
			str = str.." / "..self:get_value_cano()
		end
	else
		str = "no self.get_value"
	end
	self:print( pre.." "..str )
end
function BU:do_click()
--	self:print( "BU:do_click() begin" )
	--self:__print_value( "before self:do_fn( \"click\" )" )
	self:do_fn( "click" )
	--self:__print_value( "after self:do_fn( \"click\" )" )
--	GABU_OBJ.do_fn( self, "click" )
--	self:print( "BU:do_click() end" )
	--todo check why not this
	bus_cur:dispatch_click( self, 1 ) --	last arg for number of click
end

--todoq clean and document
function BU:clear_click_prev()
	self.__click = nil
end

--	here the blob is always in the containing bus local coordinates
--	in multiuser we have to deceide a strategy
--		locking all the bu_up position ?
function BU:add_contact( blob )
	--aaa.debug.print_traceback()
	if self.__b_gdebug_dump then
	 	if self.get_value then
	 		self:print( "-------------- DUMP value is "..self:get_value() )
	 	end
	-- 	self:dump_up( 0 )
	-- 	table.print( self, "DUMP "..self, self.__gdebug_dump_level )
	end

	--if true then return end
	--self:print( "add_contact{ "..blob.." ) was touched nb"..self:get_contact_nb() )
	--self:print( table.show( self, "bu_add_touch before" ) )
	if self.verbose >= 3 then self:print( table.show( blob, "add_contact of this" ) ) end

--	self.bu_container = self:get_bu_up()
	local b_used = false
	local b_scrub = false

	if self:get_contact_nb() == 0 then
		local fx = self.__fx_on_click
		if fx and fx.b_on_click then
			self:begin_fx()
		end
		--todo do it even if more contact
		--	store info on click
		local click_prev = self.__click
		local click
		if not click_prev then
			click_prev =  { time=-42., x=-1000., y=-1000.,  blob_x=-1000., blob_y=-1000. }
			click = {}
		else
			click = self.__click_prev or {}
		end
		local t = aaa.time.t_real
		click.time = t --	store the click for the next time
		local d_time = t - click_prev.time
		click.b_double = false
		click.b_triple = false
		local x,y = blob.x, blob.y
		click.blob_x, click.blob_y = x,y
		local bus = self:get_bus_up()
		-- convert xy to local coordonate now
		--x,y = self:convert_xy_bus_up_to_local( x + bus.__x_offset_exp, y + bus.__y_offset_exp )
		x,y = self:convert_xy_bus_up_to_local( x,y )
		--table.print( self.__trs, self..".__trs" )
	--	self:print( "BU:add_contact() local coor are ".. x..", "..y )
		click.x, click.y = x,y
		--self:print( d_time )

		--this a problem here i don't know why self.verbose exist and is 0
		--self:print( "BU.verbose "..BU.verbose )
		--self:print( "self.verbose "..self.verbose )

		--	do a first contact
		--	deal with video firstself:print( "BU.verbose "..BU.verbose )
		if self:is_video() then
			if self.__b_gdebug_bu_contact_set_focus then
				self:set_focus()
			end
			if self:is_video_button() then
				--todo use xy computed at beginning using bis offset ?
				--check it is ok now
				--local x,y = self:convert_xy_bus_up_to_local( blob.x, blob.y )
				local i = self:get_but_in( x,y )
				--hack
				if i and i > 1 then
					self:do_but_video( i )
					b_used = true
				end
				if not b_used then
					if self:is_video_scrub() then
						if y < (-.5 + self.__video_scrub_sy * .1) then
							b_used = true
							b_scrub = true
							local video = self:get_video()
							video:set_scrub( true )
							--self:print( x+.5 )
							video:scrub_cano( x + .5 )
						end
					end
				end
			end
		end

		local b_double = false
		local b_triple = false
		if d_time <= ga.click_double_time or (d_time-.0001) < aaa.time.dt_real then
			local dist = V2.dist_from_xyxy( blob.x, blob.y, click_prev.blob_x, click_prev.blob_y )
			if dist <= ga.click_double_dist then
				if self:is_click_double() then
					if self.verbose >= 1 then self:print( "this is a click_triple" ) end
					b_triple = true
				else
					if self.verbose >= 1 then self:print( "this is a click_double" ) end
					b_double = true
				end
			else
				if self.verbose >= 1 then self:print( "no click multiple : not close enought "..dist.." for "..ga.click_double_dist ) end
			end
		else
			if self.verbose >= 1 then self:print( "no click multiple : to long "..d_time.." for "..ga.click_double_time ) end
		end
		click.b_double = b_double
		click.b_triple = b_triple

		self.__click, self.__click_prev = click, click_prev

		if b_used then
			if b_scrub then
--				self.b_scrub_video_playing =
				self:__add_contact( blob, "scrub" )
			end
		end

		if not b_used then
			if self:is_click_triple() then
				--table.print( click, "click triple", 3 )
				local b_have, ret = self:do_fn( "click_triple", click.x, click.y )
				if b_have then
					if ret then
						bus_cur:dispatch_click( self, 3 ) --	last arg for number of click
						self:__add_contact( blob, "custom" )
						--self:print( "click_triple done")
					end
					b_used = true	-- e.g. avoid selector to keep changing after a file dialog
				else
					if self.verbose >= 1 then self:print( "\tno fn for click_triple" ) end
				end
			end
		end
	
		if not b_used then
			if self:is_click_double() then
				--table.print( click, "click double", 3 )
				--self:print_debug( "will call do_fn( \"click_double\" )" )
				local b_have, ret = self:do_fn( "click_double", click.x, click.y )
				--self:print_error( b_have.." "..ret )
				if b_have then
					if ret then
						bus_cur:dispatch_click( self, 2 ) --	last arg for number of click
						self:__add_contact( blob, "custom" )
						--self:print( "click_double done")
					end
					b_used = true	-- e.g. avoid selector to keep changing after a file dialog
				else
					--self:box_debug( "false" )
					if self.verbose >= 1 then self:print( "\tno fn for click_double" ) end
				end
			end
		end

		if not b_used then
			--	this the ui to move and scale
			if self:is_ui_top() and aaa.keyboard.is_alt_only() then
				local ui_type = self:__get_ui_top_interaction( x,y )
				--self:print( x.." "..y )
				--self:print( "before __add_contact "..ui_type )
				if ui_type then
					self:__add_contact( blob, ui_type )
					if self:get_contact_nb() == 1 then
						ga:get_undo_redo():store_position_in_bu(self)
					end
					b_used = true
				end
			end
		end

		if not b_used then
			-- or do a simple click
			--todo this a hack for chanel was done after with no test
			--self:print( blob.x, blob.y )
			if self.b_video_on_click then
				--self:print( "toggle video" )
				local video = self:get_video()
				if video then
					video:toggle_and_reset_at_stop()
				end
			else

				--self:print( "for b_move do_click_down( "..x..", "..y.." )" )
				--self:__print_value( "before self:do_click_down( x, y )" )
				local b_move = not self:do_click_down( x,y )
				--self:__print_value( " after self:do_click_down( x, y )" )
				--aaa.print_fn()
				--if b_move then
					if self:is_mobile() and b_move then
						self:__add_contact( blob, "move" )
					else
						self:__add_contact( blob, "custom" )
					end
				--end
				if self.verbose >= 1 then self:print( "do single click" ) end
				if not self.__b_disabled_action then
					self:do_click()
				end
			end
		end

	else -- deal with several contacts
		--todo add a limitation by bu
		if self:get_contact_nb() < ga.max_touch_by_object  then
			self:__add_contact( blob )
			local nb_to_reset = ga.touch_nb_to_reset
			if nb_to_reset and self:get_contact_nb() >= nb_to_reset then
				self:reset()
			end
		end
	end

	--self:print( BU.verbose )
	if self.verbose >= 1 then self:print( "added contact now "..self:get_contact_nb() ) end
	--debug if self:get_contact_nb() == 2 then error( "double contact" ) end

--	table.print( self, "after add_contact()", 1 )
	--self:print( "toto" )
--	self:print( table.show( bu, "bu_add_touch after" ) )
--	self:print( "contact now "..self:get_contact_nb() )
end
