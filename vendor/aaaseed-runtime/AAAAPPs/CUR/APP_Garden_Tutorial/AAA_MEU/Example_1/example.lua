
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local sha = self:get_shading()

	local ix, iy = 1,1
	local SY = .8
	local DY = .2

	self:add_shading_ui( {1,1} )
	self:add_camera()
	self:add_rendering()
	iy = iy + 1	
	local y_begin = iy + DY

--------------------
	self:set_tab_key( "Main" )
	ix, iy = 1, y_begin

	bu = self:add_trig_method(	{ix,iy,		4,SY},		"Test 1", 		self,		"do_test", 1	 )
	bu = self:add_trig_method(	{ix+4,iy,	4,SY},		"Test 2", 		self,		"do_test", 3	 )
	iy = iy + SY
	
	bu = self:add_trig_method(	{ix,iy,		4,SY},		"Load", 		self,		"load_media"	 )
	iy = iy + SY + DY

	self:add_bu_texture_target_unit( {ix,iy, 8,6} )

	ix,iy = 9,5
	local SX = 8/3
	bu = self:add_slider(		{ix,iy,	8,SY},	"Size", 		self,	"size",			0,	0,1		)
		iy = iy + SY*2
end

function meu:init()
	local sha = self:add_shading( 2 )
end

function meu:do_test( s_how )
	aaa.camera.cam_to_world( 0,0,0 )

	local bind = 1
	aaa.img.set_cpu_keep( bind, true )
	--todo deal with this case where image is on gpu
	--local fn_name = aaa.img.get_bind_filename(bind)
	--self:print( "filename is "..fn_name )
	--local b = aaa.img.read( bind, fn_name )
	-- self:print( "result is "..b )
	--aaa.img.move_to_gpu( v )
	self:print( "write pixels on bind "..bind )
	if s_how==1 then
		for i=1,512 do
			--self:print(i)
			aaa.img.set_color_xy( bind, i,i, 1. )
			aaa.img.set_color_xy( bind, i,i-1, 1. )
			aaa.img.set_color_xy( bind, i,i+2, 1. )
		end
	else
		for x=0,1023 do
			for y=0,1023 do
				local r,g,b = aaa.img.get_color_xy( bind, x,y )
				--self:print(i)
				aaa.img.set_color_xy( bind, x,y, 1.-r,1.-g,1.-b )
			end
		end
	end
end

function meu:load_media()
	local is = IMG_SEQ:create_from_dir( "Marion", aaa.dir.make_media_path("Marion/DecoupageXXX/"), "PXL_*.tga" )
	self:print( " bind "..is:get_bind(1).." to "..is:get_bind(is:get_nb()) )
	self.img_seq = is
end