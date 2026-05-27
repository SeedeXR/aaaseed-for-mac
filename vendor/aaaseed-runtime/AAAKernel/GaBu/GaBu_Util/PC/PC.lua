--
--	PC
--
if CLASS.DECLARE( "PC" ) then
	PC:set_class_status_doc(	CLASS.STATUS.CORE,
								"Abstraction for a Personal Computer and its graphic output with cameras" )
end

function PC:create( name )
	local self = PC:create_instance( name )
	self.__ref = {}
	self.screen = {}
	return self
end

function PC:set_screen_names( tab )		self.screen_names = tab		end
function PC:get_screen_names()			return self.screen_names	end

function PC:set_machine_name( name )	self.machine_name = name	end
function PC:get_machine_name()			return self.machine_name	end

function PC:get_screen_nb( id )
	return #self.screen
end
function PC:get_screen( id )
	local s = self.screen[id]
	if not s then
		s = {}
		self.screen[id] = s
	end
	return s
end
function PC:set_screen( id, tab )		self.screen[id] = tab		end

function PC:get_rect()					return self.rect			end
function PC:set_rect( tab )				self.rect = tab				end
function PC:get_rect_ratio_x()
	local rect = self.rect
	return (rect.right - rect.left) / (rect.top - rect.bottom)
end
function PC:get_rect_ratio_y()
	local rect = self.rect
	return (rect.top - rect.bottom) / (rect.right - rect.left)
end

function PC:set_cam( cam )
	local r = self.__ref
	r.cam = cam
	r.tra_z			=	param.get_ref( cam, "tra_z" )
	r.focal			=	param.get_ref( cam, "focal" )
	r.frustum_x		=	param.get_ref( cam, "frustum_offset_x" )
	r.frustum_y		=	param.get_ref( cam, "frustum_offset_y" )
end
function PC:set_cam_frustum( fx, fy )
	local ref = self.__ref
	param.set( ref.frustum_x, fx )
	param.set( ref.frustum_y, fy )
end
function PC:set_cam_tra_z( z )	param.set( self.__ref.tra_z, z )		end
function PC:set_cam_focal( f )	param.set( self.__ref.focal, f )		end

function PC.compute_cam_focal_from_rect_low( dist, rect )
	local angle	= math.atan( (rect.right - rect.left) *.5 / ( 1. * dist ) )
	--local angle	= math.atan( rect.right * .5 / dist ) - math.atan( rect.left * .5 / dist )
	return math.deg( angle * 2. )
end
function PC:compute_cam_focal_from_rect( dist )
	return PC.compute_cam_focal_from_rect_low( dist, self.rect )
end

function PC.compute_cam_frustum_from_rect_low( rect )
	local	fx		=	( (rect.right + rect.left) * .5 )	/	(rect.right - rect.left)
	local	fy		=	( (rect.top + rect.bottom) * .5 )	/	(rect.top - rect.bottom)
	--.9988	--todo why we need to correct
	local	correc_x	=	1
	--local correc_y = 1.005
	local	correc_y	=	1.0005
	return fx*correc_x, fy*correc_y
end
function PC:compute_cam_frustum_from_rect( rect )
	return PC.compute_cam_frustum_from_rect_low( self.rect )
end

function PC:set_cam_focal_from_rect( dist )	self:set_cam_focal( self:compute_cam_focal_from_rect( dist ) )	end
function PC:set_cam_frustum_from_rect()
	local fx, fy = self:compute_cam_frustum_from_rect()
	self:set_cam_frustum( fx, fy )
end

function PC:update()
end
function PC:draw()
end
function PC:draw_screens( line_size, b_screen, b_nb, b_pc, b_rect, b_bezel )
	gol.set_default()

	gol.set_line_width( line_size )
--	gol.disable_lighting()

	gol.color_green()
	local ST	=	.2
	local nb_screen = self:get_screen_nb()
	for i=1,nb_screen do
		local screen = self:get_screen(i)
		local phy	= screen.phy
		local sxh	= phy.size_x_m * .5
		local syh	= phy.size_y_m * .5

		local STH	=  .1	--syh * .25
		--todo compute rect coor directly
		--todo	have an offset for inperfections in the wall real setup
		gol.push_matrix()
			gol.translate( screen.x, screen.y )
			if b_nb then
				if b_pc then
					gol.color_red()
					aaa.draw_str_maa_xy( screen.name_virtual, -STH, -2.*STH, ST*.5, ST*.5 )
				end
				gol.color_green()
				aaa.draw_str_maa_xy( screen.name, -STH, -STH, ST, ST )
			end
			if b_screen then
				aaa.draw_rect_line( -sxh, -syh, sxh, syh )
			end
		gol.pop_matrix()
	end

	if b_pc then
		gol.color_red()
		gol.begin_line_strip()
			for i=1,nb_screen do
				local screen = self:get_screen(i)
				gol.vertex2( screen.x, screen.y )
			end
		gol.do_end()
		gol.set_line_width( line_size )
		gol.color_red( .5 )
		--gol.begin_line_strip()
			--	local x, y = pc.screen[1].x, pc.screen[1].y
			--	gol.vertex2( x, y )
			--	x = pc.rect.x
			--	gol.vertex2( x, 1.2 )
		--gol.do_end()
		gol.set_line_width( line_size )
		local screen = self:get_screen(1)
		aaa.draw_str_maa_xy( "PC", screen.x-.15, screen.y+.1, ST, ST )
	end

	if b_rect then
		--self:print( "zobu" )
		gol.color_cyan( .8 )
		local r = self.rect
		aaa.draw_rect_line( r.left, r.bottom, r.right, r.top )
		gol.draw_lines_2d(	r.left, r.bottom,
							r.right, r.top,
							r.left, r.top,
							r.right, r.bottom )
	end

	if b_bezel then
		gol.color_black()
		for i=1,nb_screen do
			local screen = self:get_screen(i)
			local phy	=	screen.phy
			local sx	=	phy.size_x_m * .5
			local sy	=	phy.size_y_m * .5
			local d		=	phy.dist_m * .5
			local sxd	=	sx + d
			local syd	=	sy + d
			--todo compute rect coor directly
			--todo	have an offset for inperfections in the wall real setup
			gol.push_matrix()
				gol.translate( screen.x, screen.y )
				gol.draw_quad_strip_2d(	sx, sy,
										sxd, syd,
										-sx, sy,
										-sxd, syd,
										-sx, -sy,
										-sxd, -syd,
										sx, -sy,
										sxd, -syd,
										sx, sy,
										sxd, syd )
			gol.pop_matrix()
		end
	end
end

--
--	PCS
--
if CLASS.DECLARE( "PCS" ) then
	PCS:set_class_status_doc(	CLASS.STATUS.CORE,
								"once created and set, manipulate several PC as one" )
end

function PCS:create( name, nb )
	local self = PCS:create_instance( name )
	self.pc = {}
	local pc = self.pc
	nb = nb or 0
	self.pc_nb = nb
	for i=1,nb do
		pc[i] = PC:create( name.."_"..i )
	end
	return self
end
function PCS:get_pc( id )	return self.pc[id]	end
function PCS:get_pc_nb()		return self.pc_nb	end

function PCS:draw_screens( line_size, b_screen, b_nb, b_pc, b_rect, b_bezel )
	local pc = self.pc
	for i=1,self.pc_nb do
		pc[i]:draw_screens( line_size, b_screen, b_nb, b_pc, b_rect, b_bezel )
	end
end
function PCS:update()
	local pc = self.pc
	for i=1,self.pc_nb do
		pc[i]:update()
	end
end
function PCS:draw()
	local pc = self.pc
	for i=1,self.pc_nb do
		pc[i]:draw()
	end
end


