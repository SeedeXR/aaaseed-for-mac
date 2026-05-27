APP.DECLARE( "ROMA", APP_GP )

function ROMA:set_media_dir()
	MEDIA.set_dir_media( "AAARom/Demo" )
end
function ROMA:load_medias()
	self:set_media_dir()
	self.imgs = {}
	local imgs = self.imgs
	imgs.acd		= IMGS.get_img( "ACD.png"	)
	imgs.emb		= IMGS.get_img( "EMB.png"	)
	imgs.pns		= IMGS.get_img( "PNS.png"	)
	imgs.romanite	= IMGS.get_img( "romanite.png"	)
	imgs.arrow		= IMGS.get_img( "arrow.png" )
	gol.set_wrap_2d_edge()

	self.sections =
	{
		{	section_id		= 1,
			alpha			= 1,
			name 			= "museo",
			name_full		= "Muséographie",
			names_sub	= { "Cicada", "Epidemik", "Terzetto", "Chanel" },
			bats =
			{
				{	name_full = "Musée Romanité", 			x=-33.5,	y=18.8, 	z=-104.4	},
				{	name_full = "Musée Arênes", 			x=2.5,		y=18.8, 	z=5			},
				{	name_full = "Lycée Alphonse Daudet", 	x=-165.8,	y=25, 		z=68.4		},
				{	name_full = "Cour d’appel", 			x=140.13,	y=25, 		z=22.15		},
			},
		},
		{	section_id		= 2,
			alpha			= 1,
			name 			= "public",
			name_full		= "Art Public",
			names_sub	= { "Aquarium", "HBL", "NML", "Qwartz Move" },
			bats =
			{
				{	name_full = "Maison carrée", 			x=-184.5,	y=19, 		z=416.6		},
				{	name_full = "Carré d’Art", 				x=-279,		y=25, 		z=408.2		},
				{	name_full = "Eglise Saint Paul", 		x=-246.8,	y=47.4, 	z=231.4		},
				{	name_full = "Square Antonin", 			x=-196,		y=11.3, 	z=552.7		},
			},
		},
		{	section_id		= 3,
			alpha			= 1,
			name 			= "motion",
			name_full		= "Motion Design",
			names_sub	= { "Intuitive", "Paris", "GE", "FileChat" },
			bats =
			{
				{	name_full = "Temple de Diane", 			x=-733,		y=10, 		z=734		},
				{	name_full = "Tour Magne", 				x=-584.8,	y=45, 		z=1032.6	},
				{	name_full = "Jardins de la source", 	x=-654.5,	y=8.7, 		z=548		},
				{	name_full = "Midi libre", 				x=-615.5,	y=17.2, 	z=474.9		},
			},
		},
	}

	local subs_all = {}
	for i=1,3 do
		local section = self.sections[i]
		local name = section.name
		MEDIA.set_dir_media( "AAARom/Demo/"..name )
		section.item =  IMGS.get_img( name..".png" )
		imgs[name] = section.item

		local x, y, z = 0, 0, 0
		local names_sub = section.names_sub
		local subs = {}
		local nb = #names_sub
		for i=1,nb do
			local sub_name = names_sub[i]
			local sub = { name=sub_name }
			sub.picto = IMGS.get_img( sub_name.." Picto.png" )
			gol.set_wrap_2d_edge()
			sub.title = IMGS.get_img( sub_name.." Bandeau.png" )
			VIDEOS:set_dir_media( "AAARom/Demo/"..name )
			sub.video = VIDEOS:get( sub_name..".mp4" )
			MEDIA.set_dir_media( "AAARom/Demo/"..name )
			local bat = section.bats[i]
			bat.x, bat.y, bat.z = bat.x/100, bat.y/100, bat.z/-100
			x = x + bat.x
			y = y + bat.y
			z = z + bat.z
			sub.bat = bat
			sub.section = section
			table.insert( subs, sub )
			table.insert( subs_all, sub )
		end

		section.center = { x=x/nb, y=y/nb, z=z/nb }
		section.subs = subs
	end
	self.subs_all = subs_all
end

function ROMA:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	if not oo.getsuper(ROMA).init_app( self, ... ) then return end

	self.colors =
	{
		{
			{	100/255,	144/255,	127/255 },
			{	54/255,		63/255,		59/255 },
			{	154/255,	172/255,	152/255 },
			{	190/255,	201/255,	195/255 },
		},
		{
			{	204/255,	88/255,		75/255	},
			{	85/255,		29/255,		24/255	},
			{	141/255,	61/255,		50/255	},
			{	235/255,	160/255,	155/255	},
		},
		{
			{	43/255,		97/255,		151/255	},
			{	23/255,		59/255,		84/255	},
			{	66/255,		146/255,	204/255	},
			{	132/255,	195/255,	234/255	},
		},
		{
			{	217/255,	162/255,	62/255	},
			{	114/255,	70/255,		24/255	},
			{	177/255,	117/255,	35/255	},
			{	246/255,	208/255,	121/255	},
		},
	}


	self.b_vdebug = false
	--gol.color( 14/255, 51/255, 78/255, .75 )
	self:load_medias()

	return true
end
