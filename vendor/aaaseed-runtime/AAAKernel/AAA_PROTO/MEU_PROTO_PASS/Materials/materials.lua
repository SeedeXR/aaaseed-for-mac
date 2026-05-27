function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "3d", "Core", "CoreGraphic", "draw", "renderpass" },
			help = "this MEU is the UI to edit the Materials used in the OpenGL or Gbuffer rendering"
		}
end

function meu:define_material_modern( ix, iy, sx, sy, id )
	local r = self.mats[id]
	if r then
		local idx = self.mat_start + id - 1
		local pre = "M"..idx.."_"

		sy = sy / 5

		local bu
		iy = iy  + sy

		bu = self:add_text_info(	{ix,iy,			sx*1/4,sy},		"M"..idx	 )
		bu = self:add_rgbf(			{ix+sx/4,iy,	sx*3/4,sy}, 	pre.."Ambient",			r, "Ambient_" ):set_text( "Ambient" )
		iy = iy  + sy

		bu = self:add_trig_fn(		{ix,iy,			sx*1/4,sy}, 	pre.."Focus",			aaa.obj.set_focus_ui, r ):set_text( "Focus" ):set_color_back("focus")
		bu = self:add_rgbfa(		{ix+sx/4,iy,	sx*3/4,sy}, 	pre.."Diffuse",			r, "Diffuse_" ):set_text( "Diffuse" )
		iy = iy  + sy

		bu = self:add_rgbf(			{ix+sx/4,iy,	sx*3/4,sy}, 	pre.."Specular",		r, "Specular_" ):set_text( "Specular" )
		iy = iy  + sy

		bu = self:add_rgbf(			{ix+sx/4,iy,	sx*3/4,sy}, 	pre.."Emission",		r, "Emission_" ):set_text( "Emission" )
		iy = iy  + sy

		local SX = sx/3
		bu = self:add_slider(		{ix,iy,			SX,sy},		pre.."Ambient_Occlusion",	r, "Ambient_Occlusion", 0.5, 0, 1 ):set_text( "AO Factor" )
		bu = self:add_slider(		{ix+SX,iy,		SX,sy},		pre.."Shininess",			r, "Shininess", 		0.5, 0, 1 ):set_text( "Shininess" )
		bu = self:add_slider(		{ix+SX*2,iy,	SX,sy},		pre.."Reflection",			r, "Reflection", 		0.5, 0, 1 ):set_text( "Reflection" )
		iy = iy  + sy
	end
end

function meu:define_ui()
	local ix, iy = 1.2, 1
	local SX, SY = 5, 1

	SX,SY = 7.5,4.7
	local dx = (16 - SX*2) / 3
	local dy = (14 - iy - SY*2) / 3

	for i=0,self.mat_nb,4 do
		local idx = i / 4 + 1
		self:set_tab_key( ""..idx.."" )
		self:define_material_modern( ix,		iy, 	 	SX,SY, i + 1 )  -- 1,5,9..
		self:define_material_modern( ix+SX+dx,	iy, 		SX,SY, i + 2 )	-- 2,6,10..
		self:define_material_modern( ix,		iy+SY+dy,	SX,SY, i + 3 )  -- 3,7,11..
		self:define_material_modern( ix+SX+dx,	iy+SY+dy,	SX,SY, i + 4 )  -- 4,8,12...
	end
end

function meu:init()
	local mat_nb    = 32
	local mat_start = 32
	local mat_end   = 32 + mat_nb - 1


	--local dir = app:get_dir().."material/"
	--local dir = "material/"
	--self:box_debug( "For material will use Dir :\n"..dir )
	local module = app:get_module()
	local materials = aaa.obj.get_branch_by_class( module, "materials" )
	local t = aaa.obj.get_branchs_by_class( materials, "material" )

	local mats_by_filename = {}
	local i = 1
	for key,obj in PAIRS(t) do
		local fname = aaa.obj.get_filename(obj)
		if fname then
			self:print_debug( i.." material filename is "..fname )
			mats_by_filename[fname] = obj
		else
			self:print_error( i.." material obj "..obj.." have no filename" ) 
			self:print_error( i.." material "..aaa.obj.get_name(obj).." have no filename" ) 
		end
		i = i + 1
		--self:print( key .." -- ".. aaa.obj.get_filename(obj) )
	end
	--table.print( mats_by_filename, "mats_by_filename" )
	--aaa.box_debug( "Toto" )

	--todo make it function with real names and not just default one
	local mats = {}
	for i = mat_start,mat_end do
--		local obj = aaa.obj.get( "material/mat_0"..i..".mat" )	--we pick material 32 to 63
		local fname = "material/mat_0"..i..".mat"
		--local obj = aaa.obj.get( dir.."mat_0"..i..".mat" )	--we pick material 32 to 63
		local obj = mats_by_filename[fname]
		mats[ i - mat_start + 1 ] = obj						--we store 1 to 32
		--self:print( dir.."mat_0"..i..".mat\n\tname\t"..aaa.obj.get_name(obj).."\n\tfilename\t"..aaa.obj.get_filename(obj) )
	end
	--self:box_debug( "For material used Dir :\n"..dir )

	self.mat_nb 	= mat_nb
	self.mat_start 	= mat_start
	self.mat_end 	= mat_end
	self.mats 		= mats

	-- table.print( self.mats, "mats", 2 )
end

function meu:get_preset_nb() 	return 32	end
