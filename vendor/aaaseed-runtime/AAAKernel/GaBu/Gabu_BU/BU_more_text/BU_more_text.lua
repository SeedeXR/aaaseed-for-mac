local doc = BU.doc
--
-- SET/GET
--
--todo deceide final names for these functions
function BU:set_text_font_type( val )		self.__text_obj:set_font_type( val )			return self		end
function BU:get_text_font_type()			return	self.__text_obj:get_font_type()			end

function BU:set_text_face_size( val )		self.__text_obj:set_face_size( val )			return self		end
function BU:get_text_face_size()			return	self.__text_obj:get_face_size()			end

function BU:set_text_face_dpi( val )		self.__text_obj:set_face_dpi( val )				return self		end
function BU:get_text_face_dpi()				return	self.__text_obj:get_face_dpi()			end

function BU:set_text_font( val )			self.__text_obj:set_font( val )					return self		end
function BU:get_text_font()					return	self.__text_obj:get_font()				end

function BU:set_text_justification( val )	self.__text_obj:set_justification( val )		return self		end
function BU:get_text_justification()		return	self.__text_obj:get_justification()		end

function BU:set_text_alignment( val )		self.__text_obj:set_alignment( val )			return self		end
function BU:get_text_alignment()			return self.__text_obj:get_alignment()			end

function BU:set_text_alignment_vert( val )	self.__text_obj:set_alignment_vert( val )		return self		end
function BU:get_text_alignment_vert()		return	self.__text_obj:get_alignment_vert()	end

function BU:set_text_interligne( val )		self.__text_obj:set_interligne( val )			return self		end
function BU:get_text_interligne()			return	self.__text_obj:get_interligne()		end

function BU:set_text_line_length( val )		self.__text_obj:set_line_length( val )			return self		end
function BU:get_text_line_length()			return	self.__text_obj:get_line_length()		end

function BU:set_text_draw_box( val )		self.__text_obj:set_draw_box( val )				return self		end
function BU:get_text_draw_box()				return	self.__text_obj:get_draw_box()			end

function BU:get_text_width( )				return self.__text_obj:get_width()				end
function BU:get_text_height()				return self.__text_obj:get_height()				end

--
--	ALIGNMENT TEXT
--
function BU:get_text_align_x()				return self.__text_align_x						end
function BU:set_text_align_x( align )		self.__text_align_x	= string.lower(align)		return self		end

function BU:get_text_align_y()				return self.__text_align_y						end
function BU:set_text_align_y( align )		self.__text_align_y	= string.lower(align)		return self		end


--
--	TEXT
--
function BU:set_text_x(			x )				self.__text_x = x								return self		end
function BU:set_text_y(			y )				self.__text_y = y								return self		end
function BU:set_text_xy(		x,y )			self.__text_x = x			self.__text_y = y	return self		end

--todo hide text and generalize use of this instead of direct allocation
function BU:set_text_fxy(		fx, fy )		self.__text_factor_x = fx	self.__text_factor_y = fy or fx		return self		end
function BU:set_text_factor(	f )				return self:set_text_fxy( f, f )	end

function BU:set_text_xyf(		x,y, f )		return self:set_text_xy( x,y ):set_text_factor( f )		end
function BU:set_text_xy_fxy(	x,y, fx,fy )	return self:set_text_xy( x,y ):set_text_fxy( fx, fy )	end

function BU:__set_text_type( s )				self.__s_text_type = s		return self	end
function BU:set_text_nice()						return self:__set_text_type(3)			end
function BU:set_text_maa()						return self:__set_text_type(2)			end
function BU:set_text_glut()						return self:__set_text_type(1)			end
function BU:__get_text_type()					return self.__s_text_type			end
function BU:is_text_nice()						return self.__s_text_type == 3		end

function BU:set_text( text, text_size_factor )
	self.__text = text
 	self.__text_len = text and string.len(text) or 0
	self:set_text_factor( text_size_factor )
	return self
end

function BU:push_text( new_text )
	self.text_old = self.__text and self.__text or "___nil___"
	self:set_text( new_text )
end
function BU:pop_text()
	self:set_text( self.text_old ~= "___nil___" and self.text_old or nil )
end
function BU:get_text()					return self.__text		end
function BU:get_text_len()				return self.__text_len	end
--BU.__b_text_draw = true
function BU:set_text_draw( b_on )			self.__b_text_draw = b_on			return self		end
function BU:set_text_draw_always( b_on )	self.__b_text_draw_always = b_on	return self		end
function BU:set_text_visible( b_on )		return self:set_text_draw( b_on )			end
function BU:is_text_draw()					return ( TEXT:is_active() or self.__b_text_draw_always )
													and (self.__text_len > 0)
													and (self.__b_text_draw or self:is_mini())
											end

function BU:set_value_no_draw( val )	self.__value_no_draw = val	return self		end
function BU:is_value_to_draw( val )		return self.__value_no_draw ~= val 			end

function BU:get_text_fxy()
	return self.__text_factor_x, self.__text_factor_y
end
--todo done for F Monitor an Ui Window, perhaps do better and more generic
function BU:set_text_xy_fxy_mini( x,y, fx,fy )
	self.__text_mini_x = x
	self.__text_mini_y = y
	self.__text_mini_factor_x = fx
	self.__text_mini_factor_y = fy
end
function BU:get_text_xy_fxy()
	--local X, Y, F = -.45, -.35, .65
	--local F, X, Y = 1, 0, 0
	local inter = self:get_transfo_inter_text()
	if not inter then
		if self:is_mini() then
			return self.__text_mini_x,self.__text_mini_y, self.__text_mini_factor_x, self.__text_mini_factor_y
		else
			--if self:get_text()=="C" then
			--	self:print( x.." "..y.." "..f )
			--end
			return self.__text_x, self.__text_y, self.__text_factor_x, self.__text_factor_y
		end
	end
	--aaa.print( inter )
	local x = self.__text_x
	if x~=0 then x = interpolate_to0( x, inter ) end
	local y = self.__text_y
	if y~=0 then y = interpolate_to0( y, inter ) end
	local fx = self.__text_factor_x
	if fx~=1 then fx = interpolate_to1( fx, inter ) end
	local fy = self.__text_factor_y
	if fy~=1 then fy = interpolate_to1( fy, inter ) end
	return x,y, fx,fy
end

--
--	TEXT COLOR
--
--todo use named color here too
function BU:set_text_color( r_or, g, b, a )
	local ty = type(r_or)
	if ty == "string" then
		local t = self:get_color_named( r_or )
		self.__color_text	=	{ t[1], t[2], t[3], t[4] }
	elseif not r_or or ty == "table" then
		--self:print( r_or_table )
		self.__color_text	=	r_or
	else
		self.__color_text	=	{ r_or or 1, g or 1, b or 1, a or 1 }
	end
	return self
end

function BU:set_text_color_busy( b_busy )
	if b_busy then
		return self:set_text_color( "busy" )
	end
	return self:set_text_color()	--todo not sure about this case
end
function BU:set_text_color_problem_info( b )
	if b then
		return self:set_text_color( "info" )
	end
	return self:set_text_color( "problem" )
end
function BU:set_text_color_problem_white( b )
	if b then
		return self:set_text_color( "white" )
	end
	return self:set_text_color( wrap_01(math.sin(aaa.time.t_real*10)*.5+.5)*.5+.5, 0, 0, 1 )
end
function BU:set_text_color_green_info( b )
	if b then
		return self:set_text_color( "green" )
	end
	return self:set_text_color( "info")
end


--lionel says : same fonctions ?
function BU:get_text_color_table()	return self.__color_text	end
-- function BU:get_text_color()
-- 	local t = self:get_text_color_table()
-- 	if t then
-- 		return t[1], t[2], t[3], t[4]
-- 	end
-- 	self:box_error( "__color_table ?" )
-- 	return 1, 0, 0, 1
-- end

--
--	TEXT DRAW
--
--todo unify with draw_text_color

function BU:draw_text_color()
	local t = self:get_text_color_table()
	gol.color( t[1], t[2], t[3], t[4] * self:get_alpha_bu_to_draw() )
end
function BU:draw_text_color_less()
	local t = self:get_text_color_table()
	gol.color( t[1]*.75, t[2]*.75, t[3]*.75, t[4] * self:get_alpha_bu_to_draw() )
end
function BU:draw_text_color_sel( b_less )
	--if self:is_contact() then
	--	gol.color_green()
	--else
	if b_less then
		self:draw_text_color_less()
	else
		self:draw_text_color()
	end
	--end
end
function BU:set_text_line_attr( b_wide )
--	gol.disable_lighting()
--	gol.set_logic_op_none()
--	gol.set_default()
--	gol.set_texture_dim( 0 )
	gol.set_line_smooth( true )
	--self:print( self.__draw_text_line_width )
	--gol.set_line_width( self.__draw_text_line_width )
	gol.set_line_width( b_wide and self.__draw_text_line_width*1.6 or self.__draw_text_line_width )
end


function BU:draw_text_line( str, x,y, ... )
	if GA.b_spy then aaa.spy.push_range( str, 6 ) end

		self:set_text_line_attr()
		--self:print( "str is "..str )
		--table.print( str, "str" )
		if type(str) == "table" then
			self:print( "str is table" )
			table.print( str, "str" )
		else
			aaa.draw_str_xy( str, x,y, ... )
		end

	if GA.b_spy then aaa.spy.pop_range() end
end
function BU:draw_text_line_maa( str, x,y, sx,sy )
	if GA.b_spy then aaa.spy.push_range( str, 6 ) end
		self:set_text_line_attr()
		aaa.draw_str_maa_xy( str, x,y-.3*sy, sx, sy )
	if GA.b_spy then aaa.spy.pop_range() end
end

function BU:draw_text_nice( ... )
	--aaa.print_fn()
	local text_obj = self.__text_obj
--	text_obj:set_font( 0 )
		--todofont avoid heavy fns
	--text_obj:set_font_type( "font_nice" )
	--text_obj.__font_type = 3
	text_obj:__draw_for_bu( self, ... )
end

function BU:draw_text_nice_shadow( txt, x,y,z, sx,sy, justification, align_x )
	local to = self.__text_obj
	to:set_next_shadow()
	to.__shadow_du = .75 * sx
	to.__shadow_dv = -.75 * sy
	to:__draw_for_bu( self, txt, x,y,z, sx,sy, justification, align_x )
end

function BU:draw_text( str, x,y, ... )
	-- local text = select( 1, ... )
	-- if text == "REC" then
	-- 	self:box_debug( text )
	-- end
--	gol.color_white(1)
--	gol.set_texture_dim(0)
--	self:draw_text_line( ... )
	if		self.__s_text_type==1 then	self:draw_text_line(		str, x,y, ... )
	elseif	self.__s_text_type==2 then	self:draw_text_line_maa(	str, x,y, ... )
	else								self:draw_text_nice(		str, x,y,0, ... )
	end
end


function BU:__compute_text_rect( le,ri, bo,to, align_x, b_spe )
	local rect = self.__text_rect
	if not rect then
		rect = {}
		self.__text_rect = rect
	end

	--Get offset and factor for text
	--if no specify return 0, 0, 1
	local offset_x,offset_y, fx,fy
	if b_spe then	offset_x,offset_y, fx,fy = 0,0, 1,1
	else			offset_x,offset_y, fx,fy = self:get_text_xy_fxy()
	end
	--offset_x,offset_y, fx,fy = self:get_text_xy_fxy()

--	if rypef then
--		self:print( "should be solve" )
--		f = .25
--	end
	--Find position
	--self:print( offset_x.." "..offset_y.." "..f )

	local sx = math.abs( ri-le ) * fx    
	local sy = math.abs( to-bo ) * fy

	if		align_x == "right" then
		le = ri - sx
	elseif	align_x == "center" then
		le = (le + ri - sx ) * .5
	else -- left case
	end

	le = le + offset_x
	bo = bo + offset_y

 	rect.l,rect.b, rect.sx,rect.sy = le,bo, sx,sy
	return le,bo, sx,sy
end

function BU:__get_text_rect()
	local text_rect = self.__text_rect
	if text_rect then
		return text_rect.l,text_rect.b, text_rect.sx,text_rect.sy
	else
		return nil
	end
end
local function draw_rect_debug( le,bo, sx,sy )
	--todo
	--gol.push_attrib()
	--gol.color_cyan( .5 )
	--aaa.draw_rect_line( le_in, bo_in, ri_in, to_in )
	local r,g,b, a = gol.get_color() 
		gol.color_orange( 1 )
		aaa.draw_rect_line( le,bo, le+sx,bo+sy )
	gol.color( r,g,b, a )
	--gol.pop_attrib()
end
function BU:draw_text_lrbt( str, le_in,ri_in, bo_in,to_in, align_x, b_spe )
--	if not TEXT:is_active() then return end
	if not str then return end

	local b_text_nice = self:is_text_nice()
	--local b_too_large
	local l,le,bo, sx,sy
	local sx_text

	-- if str == 'X' then
	-- 	self:print( str.." with align "..align_x )
	-- end
	align_x = align_x or self:get_text_align_x()
	--align_x = "left"
	-- if align_x == "center" then
	-- 	self:print( str.." is align_x : "..align_x )
	-- end
	local function compute()
		le,bo, sx,sy = self:__compute_text_rect( le_in,ri_in, bo_in,to_in, align_x, b_spe )

		local rx,ry
		if b_spe then	rx,ry = 1,1
		else			rx,ry = self:get_ratio_xy()
		end
		sx_text = sy*ry

		if GA:is_visu_text_draw_rect() then
			draw_rect_debug( le,bo, sx,sy )
		end

		if not b_text_nice then
		--Get the size of the text
			local sx_str_len = aaa.get_str_translate( str )	-- done for roman line font only
			--when nice it is done in TEXT
--			if b_text_nice then
--		 		sx_str_len = sx_str_len * 1.2 	-- pure hack: we estimate length of text from the roman line size
--			end
			if sx_str_len * sx_text > sx then
				-- too wide for the rect we scale down just to fit in
				sx_text = sx / sx_str_len
				--b_too_large = true
			end
			--if not b_text_nice then
			-- for line font only, we deal differently with other font
			-- we deal we alignment only if text is more narrow than the rect
			if align_x == "right" then
				le = le + sx - sx_text * sx_str_len
			elseif align_x == "center" then
			--self:print( "sx_text is : "..sx_text )
				le = le + (sx - sx_text * sx_str_len) * .5
			end
		end

			--end
		-- we deal we aligment only if text is more narrow than the rect
		-- sx_text * sx_str_len
		--l = le + sx_str_len * sx_text
		-- if align_x == "right" then
		-- 	l = le + sx - s
		-- elseif align_x == "center" then
		-- 	l = le + (sx - s) * .5
		-- else
		-- 	l = le
		-- end
	end	-- function compute()

	if false then		--faster but costly and we need special case when text is changing often
						--		which will explode cache size
		local info = self.__text_cache
		if not info then
			info = {}
			self.__text_cache = info
		end
		info = self.__text_cache[str]
		if not info then
			info = {}
			self.__text_cache[str] = info
		end 	
		if info.str~=str or info.le_in~=le_in or info.ri_in~=ri_in or info.bo_in~=bo_in or info.to_in~=to_in or info.align_x~=align_x then
			--self:print( str.." -> "..info.to.." "..to_in )
			info.str=str; info.le_in=le_in; info.ri_in=ri_in; info.bo_in=bo_in; info.to_in=to_in; info.align_x=align_x
			--	cache the rect (use for blob contacts)
			compute()
			info.le=le; info.bo=bo; info.sx=sx; info.sy=sy
		end
		self:draw_text( str, info.le, info.bo, info.sx, info.sy )
		--Viz the blue rect where the text is draw
	else
		--	cache the rect (use for blob contacts)
		compute()


		local text_obj = self.__text_obj
		
		if b_text_nice then
			local x
			-- if str=="Max" then
			-- 	--self:print( "sx_text is : "..sx_text )
			-- 	--self:print( " --- A got "..str.." ---" )
			-- 	self:print( " --- self.__s_text_type "..self.__s_text_type )
			-- 	aaa.print_fn()
			-- end
			
			--	text_obj:set_font( 0 )
					--todofont avoid heavy fns
				--text_obj:set_font_type( "font_nice" )
				--text_obj.__font_type = 3
			--if b_too_large or align_x == "center" then
			if align_x == "center" then
				text_obj:set_justification( "center" )
				text_obj:set_alignment( "center" )
				x = le + sx*.5
			elseif align_x == "right" then
				text_obj:set_justification( "right" )
				text_obj:set_alignment( "right" )
				x = le + sx*.5
			else
				text_obj:set_justification( "left" )
				text_obj:set_alignment( "left" )
				x = le
			end
			text_obj:set_next_fit( sx ) --		, to_in-bo_in )
			text_obj:__draw_for_bu( self, str, x,bo,0, sx_text,sy )
		else
			self:set_text_line_attr()
			text_obj:__draw_line( str,  le,bo,0, sx_text,sy )
		end
		
		--self:draw_text( str, le,bo, sx_text,sy )
		--Viz the blue rect where the text is draw
	end
end

function BU:draw_text_in_rect( str, rect, align_x )
	self:draw_text_lrbt( str, rect.l,rect.r, rect.b,rect.t, align_x )
end

function BU:format_value( val )
	if val then
		if self:is_value_type_bool() then
			return val and "true" or "false"
		elseif self:is_value_type_integer() then
			return string.format( "%d", val )
		else
			return self:format_number_in_text( val )
		end
	else
		return "No value"
	end
end

local function format_number_abs_less_than_1_in_char_3( val )
	local str
	if val < 0 then
		str = "-."
		val = -val
	else
		str = "."
	end
	val = math.floor( val * 1000 )
	if		val == 0	then	str = ".0"
	elseif	val < 10	then	str = str.."00"..val
	elseif	val < 100	then	str = str.."0"..val
	else
		str = str..val
	end
	return str
end
function BU:format_number_in_text( val )
	local str
	if		val == math.floor( val )		then	str = string.format( "%d.", val )
	elseif	val == 0 						then	str = ".0"
	elseif	val == 1 						then	str = "1.0"
	elseif	inside( val, -.9999, .9999 ) 	then	str = string.format( "%.3g", val )
												--str = format_number_abs_less_than_1_in_char_3( val )
	else										str = string.format( "%.4g", val )
	end
	return str
end
function BU:format_number_in_text_1( val )
	local str
	if		val == 0 then						str = ".0"
	elseif	val == 1 then						str = "1.0"
	elseif inside( val, -.9999, .9999 ) then	str = string.format( "%.3g", val )
												--str = format_number_abs_less_than_1_in_char_3( val )
	else										str = string.format( "%.1g", val )
	end
	return str
end
function BU:format_time_duration( t, dur )
	local str = BU:format_number_in_text_1( t )
	if dur then
		str = str.." / "..BU:format_number_in_text_1( dur )
	end
	return str
end

local tab = BU
