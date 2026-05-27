
if CLASS.DECLARE( "BU_COLOR", BU, {	--we use BUI because we have a get/set_values()
	__b_uif_outside		= true,
	} ) then
	BU_COLOR:set_class_status_doc(	CLASS.STATUS.GABU,
									"regroup several BU in one to pick color" )
end

BU_COLOR.COLORS =
{		--	r			g				b				a			name
	{	r =	.5,			g = .5,			b = .5,			a = 1., 	name = "grey"						},
	{	r =	1.,			g = 1.,			b = 1.,			a = 1., 	name = "white"						},
	{	r =	1.,			g = .0,			b = .0,			a = 1., 	name = "red"						},
	{	r =	.0,			g = 1.,			b = .0,			a = 1., 	name = "green"						},
	{	r =	.0,			g = .0,			b = 1.,			a = 1., 	name = "blue"						},
	{	r =	.0,			g = 1.,			b = 1.,			a = 1., 	name = "cyan"						},
	{	r =	1.,			g = .0,			b = 1.,			a = 1., 	name = "magenta"					},
	{	r =	1.,			g = 1.,			b = .0,			a = 1., 	name = "yellow"						},
	{	r =	0.,			g = 0.,			b = 0.,			a = 1., 	name = "black"						},
	{	r =	0.941176,	g = 0.972549,	b = 1.,			a = 1., 	name = "alice blue"					},
	{	r =	0.980392,	g = 0.921569,	b = 0.843137,	a = 1., 	name = "antique white"				},
	{	r =	.0,			g = 1.,			b = 1.,			a = 1., 	name = "aqua"						},
	{	r =	0.498039,	g = 1.,			b = 0.831373,	a = 1., 	name = "aquamarine"					},
	{	r =	0.941176,	g = 1.,			b = 1.,			a = 1., 	name = "azure"						},
	{	r =	0.960784,	g = 0.960784,	b = 0.862745,	a = 1., 	name = "beige"						},
	{	r =	1.,			g = 0.894118,	b = 0.768627,	a = 1., 	name = "bisque"						},
	{	r =	1.,			g = 0.921569,	b = 0.803922,	a = 1., 	name = "blanched almond"			},
	{	r =	0.541176,	g = 0.168627,	b = 0.886275,	a = 1., 	name = "blue violet"				},
	{	r =	0.647059,	g = 0.164706,	b = 0.164706,	a = 1., 	name = "brown"						},
	{	r =	0.870588,	g = 0.721569,	b = 0.529412,	a = 1., 	name = "burly wood"					},
	{	r =	0.372549,	g = 0.619608,	b = 0.627451,	a = 1., 	name = "cadet blue"					},
	{	r =	0.498039,	g = 1.,			b = .0,			a = 1., 	name = "chartreuse"					},
	{	r =	0.823529,	g = 0.411765,	b = 0.117647,	a = 1., 	name = "chocolate"					},
	{	r =	1.,			g = 0.498039,	b = 0.313726,	a = 1., 	name = "coral"						},
	{	r =	0.392157,	g = 0.584314,	b = 0.929412,	a = 1., 	name = "cornflower blue"			},
	{	r =	1.,			g = 0.972549,	b = 0.862745,	a = 1., 	name = "corn silk"					},
	{	r =	0.862745,	g = 0.0784314,	b = 0.235294,	a = 1., 	name = "crimson"					},
	{	r =	.0,			g = .0,			b = 0.545098,	a = 1., 	name = "dark blue"					},
	{	r =	.0,			g = 0.545098,	b = 0.545098,	a = 1., 	name = "dark cyan"					},
	{	r =	0.721569,	g = 0.52549,	b = 0.0431373,	a = 1., 	name = "dark golden rod"			},
	{	r =	0.662745,	g = 0.662745,	b = 0.662745,	a = 1., 	name = "dark grey"					},
	{	r =	.0,			g = 0.392157,	b = .0,			a = 1., 	name = "dark green"					},
	{	r =	0.741176,	g = 0.717647,	b = 0.419608,	a = 1., 	name = "dark khaki"					},
	{	r =	0.545098,	g = .0,			b = 0.545098,	a = 1., 	name = "dark magenta"				},
	{	r =	0.333333,	g = 0.419608,	b = 0.184314,	a = 1., 	name = "dark olive green"			},
	{	r =	1.,			g = 0.54902,	b = .0,			a = 1., 	name = "dark orange"				},
	{	r =	0.6,		g = 0.196078,	b = 0.8,		a = 1., 	name = "dark orchid"				},
	{	r =	0.545098,	g = .0,			b = .0,			a = 1., 	name = "dark red"					},
	{	r =	0.913725,	g = 0.588235,	b = 0.478431,	a = 1., 	name = "dark salmon"				},
	{	r =	0.560784,	g = 0.737255,	b = 0.560784,	a = 1., 	name = "dark sea Green"				},
	{	r =	0.282353,	g = 0.239216,	b = 0.545098,	a = 1., 	name = "dark slate blue"			},
	{	r =	0.184314,	g = 0.309804,	b = 0.309804,	a = 1., 	name = "dark slate grey"			},
	{	r =	.0,			g = 0.807843,	b = 0.819608,	a = 1., 	name = "dark turquoise"				},
	{	r =	0.580392,	g = .0,			b = 0.827451,	a = 1., 	name = "dark violet"				},
	{	r =	1.,			g = 0.0784314,	b = 0.576471,	a = 1., 	name = "deep pink"					},
	{	r =	.0,			g = 0.74902,	b = 1.,			a = 1., 	name = "deep sky blue"				},
	{	r =	0.411765,	g = 0.411765,	b = 0.411765,	a = 1., 	name = "dim grey"					},
	{	r =	0.117647,	g = 0.564706,	b = 1.,			a = 1., 	name = "dodger blue"				},
	{	r =	0.698039,	g = 0.133333,	b = 0.133333,	a = 1., 	name = "fire brick"					},
	{	r =	1.,			g = 0.980392,	b = 0.941176,	a = 1., 	name = "floral white"				},
	{	r =	0.133333,	g = 0.545098,	b = 0.133333,	a = 1., 	name = "forest green"				},
	{	r =	1.,			g = .0,			b = 1.,			a = 1., 	name = "fuchsia"					},
	{	r =	0.862745,	g = 0.862745,	b = 0.862745,	a = 1., 	name = "gainsboro"					},
	{	r =	0.972549,	g = 0.972549,	b = 1.,			a = 1., 	name = "ghost white"				},
	{	r =	1.,			g = 0.843137,	b = .0,			a = 1., 	name = "gold"						},
	{	r =	0.854902,	g = 0.647059,	b = 0.12549,	a = 1., 	name = "golden rod"					},
	{	r =	0.678431,	g = 1.,			b = 0.184314,	a = 1., 	name = "green yellow"				},
	{	r =	0.941176,	g = 1.,			b = 0.941176,	a = 1., 	name = "honey dew"					},
	{	r =	1.,			g = 0.411765,	b = 0.705882,	a = 1., 	name = "hot pink"					},
	{	r =	0.803922,	g = 0.360784,	b = 0.360784,	a = 1., 	name = "indian red"					},
	{	r =	0.294118,	g = .0,			b = 0.509804,	a = 1., 	name = "indigo"						},
	{	r =	1.,			g = 1.,			b = 0.941176,	a = 1., 	name = "ivory"						},
	{	r =	0.941176,	g = 0.901961,	b = 0.54902,	a = 1., 	name = "khaki"						},
	{	r =	0.901961,	g = 0.901961,	b = 0.980392,	a = 1., 	name = "lavender"					},
	{	r =	1.,			g = 0.941176,	b = 0.960784,	a = 1., 	name = "lavender blush"				},
	{	r =	0.486275,	g = 0.988235,	b = .0,			a = 1., 	name = "lawn green"					},
	{	r =	1.,			g = 0.980392,	b = 0.803922,	a = 1., 	name = "lemon chiffon"				},
	{	r =	0.678431,	g = 0.847059,	b = 0.901961,	a = 1., 	name = "light blue"					},
	{	r =	0.941176,	g = 0.501961,	b = 0.501961,	a = 1., 	name = "light coral"				},
	{	r =	0.878431,	g = 1.,			b = 1.,			a = 1., 	name = "light cyan"					},
	{	r =	0.980392,	g = 0.980392,	b = 0.823529,	a = 1., 	name = "light golden rod yellow"	},
	{	r =	0.827451,	g = 0.827451,	b = 0.827451,	a = 1., 	name = "light grey"					},
	{	r =	0.564706,	g = 0.933333,	b = 0.564706,	a = 1., 	name = "light green"				},
	{	r =	1.,			g = 0.713726,	b = 0.756863,	a = 1., 	name = "light pink"					},
	{	r =	1.,			g = 0.627451,	b = 0.478431,	a = 1., 	name = "light salmon"				},
	{	r =	0.12549,	g = 0.698039,	b = 0.666667,	a = 1., 	name = "light sea green"			},
	{	r =	0.529412,	g = 0.807843,	b = 0.980392,	a = 1., 	name = "light sky blue"				},
	{	r =	0.466667,	g = 0.533333,	b = 0.6,		a = 1., 	name = "light slate grey"			},
	{	r =	0.690196,	g = 0.768627,	b = 0.870588,	a = 1., 	name = "light steel blue"			},
	{	r =	1.,			g = 1.,			b = 0.878431,	a = 1., 	name = "light yellow"				},
	{	r =	.0,			g = 1.,			b = .0,			a = 1., 	name = "lime"						},
	{	r =	0.196078,	g = 0.803922,	b = 0.196078,	a = 1., 	name = "lime green"					},
	{	r =	0.980392,	g = 0.941176,	b = 0.901961,	a = 1., 	name = "linen"						},
	{	r =	0.501961,	g = .0,			b = .0,			a = 1., 	name = "maroon"						},
	{	r =	0.4,		g = 0.803922,	b = 0.666667,	a = 1., 	name = "medium aquamarine"			},
	{	r =	.0,			g = .0,			b = 0.803922,	a = 1., 	name = "medium blue"				},
	{	r =	0.729412,	g = 0.333333,	b = 0.827451,	a = 1., 	name = "medium orchid"				},
	{	r =	0.576471,	g = 0.439216,	b = 0.858824,	a = 1., 	name = "medium purple"				},
	{	r =	0.235294,	g = 0.701961,	b = 0.443137,	a = 1., 	name = "medium sea green"			},
	{	r =	0.482353,	g = 0.407843,	b = 0.933333,	a = 1., 	name = "medium slate blue"			},
	{	r =	.0,			g = 0.980392,	b = 0.603922,	a = 1., 	name = "medium spring green"		},
	{	r =	0.282353,	g = 0.819608,	b = 0.8,		a = 1., 	name = "medium turquoise"			},
	{	r =	0.780392,	g = 0.0823529,	b = 0.521569,	a = 1., 	name = "medium violet red"			},
	{	r =	0.0980392,	g = 0.0980392,	b = 0.439216,	a = 1., 	name = "midnight blue"				},
	{	r =	0.960784,	g = 1.,			b = 0.980392,	a = 1., 	name = "mint cream"					},
	{	r =	1.,			g = 0.894118,	b = 0.882353,	a = 1., 	name = "misty rose"					},
	{	r =	1.,			g = 0.894118,	b = 0.709804,	a = 1., 	name = "moccasin"					},
	{	r =	1.,			g = 0.870588,	b = 0.678431,	a = 1., 	name = "navajo white"				},
	{	r =	.0,			g = .0,			b = 0.501961,	a = 1., 	name = "navy"						},
	{	r =	0.992157,	g = 0.960784,	b = 0.901961,	a = 1., 	name = "old lace"					},
	{	r =	0.501961,	g = 0.501961,	b = .0,			a = 1., 	name = "olive"						},
	{	r =	0.419608,	g = 0.556863,	b = 0.137255,	a = 1., 	name = "olive drab"					},
	{	r =	1.,			g = 0.647059,	b = .0,			a = 1., 	name = "orange"						},
	{	r =	1.,			g = 0.270588,	b = .0,			a = 1., 	name = "orange red"					},
	{	r =	0.854902,	g = 0.439216,	b = 0.839216,	a = 1., 	name = "orchid"						},
	{	r =	0.933333,	g = 0.909804,	b = 0.666667,	a = 1., 	name = "pale golden rod"			},
	{	r =	0.596078,	g = 0.984314,	b = 0.596078,	a = 1., 	name = "pale green"					},
	{	r =	0.686275,	g = 0.933333,	b = 0.933333,	a = 1., 	name = "pale turquoise"				},
	{	r =	0.858824,	g = 0.439216,	b = 0.576471,	a = 1., 	name = "pale violet red"			},
	{	r =	1.,			g = 0.937255,	b = 0.835294,	a = 1., 	name = "papaya whip"				},
	{	r =	1.,			g = 0.854902,	b = 0.72549,	a = 1., 	name = "peach puff"					},
	{	r =	0.803922,	g = 0.521569,	b = 0.247059,	a = 1., 	name = "peru"						},
	{	r =	1.,			g = 0.752941,	b = 0.796078,	a = 1., 	name = "pink"						},
	{	r =	0.866667,	g = 0.627451,	b = 0.866667,	a = 1., 	name = "plum"						},
	{	r =	0.690196,	g = 0.878431,	b = 0.901961,	a = 1., 	name = "powder blue"				},
	{	r =	0.501961,	g = .0,			b = 0.501961,	a = 1., 	name = "purple"						},
	{	r =	0.737255,	g = 0.560784,	b = 0.560784,	a = 1., 	name = "rosy brown"					},
	{	r =	0.254902,	g = 0.411765,	b = 0.882353,	a = 1., 	name = "royal blue"					},
	{	r =	0.545098,	g = 0.270588,	b = 0.0745098,	a = 1., 	name = "saddle brown"				},
	{	r =	0.980392,	g = 0.501961,	b = 0.447059,	a = 1., 	name = "salmon"						},
	{	r =	0.956863,	g = 0.643137,	b = 0.376471,	a = 1., 	name = "sandy brown"				},
	{	r =	0.180392,	g = 0.545098,	b = 0.341176,	a = 1., 	name = "sea green"					},
	{	r =	1.,			g = 0.960784,	b = 0.933333,	a = 1., 	name = "sea shell"					},
	{	r =	0.627451,	g = 0.321569,	b = 0.176471,	a = 1., 	name = "sienna"						},
	{	r =	0.752941,	g = 0.752941,	b = 0.752941,	a = 1., 	name = "silver"						},
	{	r =	0.529412,	g = 0.807843,	b = 0.921569,	a = 1., 	name = "sky blue"					},
	{	r =	0.415686,	g = 0.352941,	b = 0.803922,	a = 1., 	name = "slate blue"					},
	{	r =	0.439216,	g = 0.501961,	b = 0.564706,	a = 1., 	name = "slate grey"					},
	{	r =	1.,			g = 0.980392,	b = 0.980392,	a = 1., 	name = "snow"						},
	{	r =	.0,			g = 1.,			b = 0.498039,	a = 1., 	name = "spring green"				},
	{	r =	0.27451,	g = 0.509804,	b = 0.705882,	a = 1., 	name = "steel blue"					},
	{	r =	0.823529,	g = 0.705882,	b = 0.54902,	a = 1., 	name = "tan"						},
	{	r =	.0,			g = 0.501961,	b = 0.501961,	a = 1., 	name = "teal"						},
	{	r =	0.847059,	g = 0.74902,	b = 0.847059,	a = 1., 	name = "thistle"					},
	{	r =	1.,			g = 0.388235,	b = 0.278431,	a = 1., 	name = "tomato"						},
	{	r =	0.25098,	g = 0.878431,	b = 0.815686,	a = 1., 	name = "turquoise"					},
	{	r =	0.933333,	g = 0.509804,	b = 0.933333,	a = 1., 	name = "violet"						},
	{	r =	0.960784,	g = 0.870588,	b = 0.701961,	a = 1., 	name = "wheat"						},
	{	r =	0.960784,	g = 0.960784,	b = 0.960784,	a = 1., 	name = "white smoke" 				},
	{	r =	0.603922,	g = 0.803922,	b = 0.196078,	a = 1., 	name = "yellow green"				},
}

--generalize BU with sub BUs
function BU_COLOR:set_alpha_bu( a )
	oo.getsuper(BU_COLOR).set_alpha_bu( self, a )
	
	local t = self.__bu_hsv
	if t then
		t[1]:set_alpha_bu( a )
		t[2]:set_alpha_bu( a )
		t[3]:set_alpha_bu( a )
	end
	local t = self.__bu_rgb
	if t then
		t[1]:set_alpha_bu( a )
		t[2]:set_alpha_bu( a )
		t[3]:set_alpha_bu( a )
	end
	if self.__b_grey then
		self.__bu_grey:set_alpha_bu( a )
	end
	if self.__b_alpha then
		self.__bu_alpha:set_alpha_bu( a )
	end

	return self
end

function BU_COLOR:set_gamma( b_gamma )
	for _,bu in IPAIRS(self.__bu_rgb) do
		bu:set_draw_gamma( b_gamma ):add_values_def(1)
		bu:set_min_max( 0,8 )
	end
	local bu = self.__bu_grey
	if bu then
		bu:set_draw_gamma( b_gamma ):add_values_def(1)
		bu:set_min_max( 0,8 )
	end
	self.__b_draw_gamma = b_gamma
	return self
end

function BU_COLOR:__build_color_pos( i, nb )
	local t = self.__lb_sxy

	local x,y = t.l,t.b
	local sx,sy = t.sx,t.sy

	if i < 0 then	--used for alpha
		x = x + sx
		sx = self.__sx_alpha
	else
		sx = sx / nb
		x = x + i * sx
	end
	return { x+sx*.5,y+sy*.5, sx*(15/16),sy }
end

function BU_COLOR:__make_bui_name( post )
	local pre = self.__pre
	return pre and pre..post or post
end
function BU_COLOR:__add_slider( rect, name, min,max, b_do_target, val )
	local cref = self.__color_ref
	local param_ref = b_do_target and (cref and cref:get_param_ref( name ))
	local bui_name = self:__make_bui_name(name)
	--self:print( "Light bu "..bu_name )
	--local bu = self.__meu:add_param( rect, bui_name, param_ref, min,max, val )

	local bu = bus_cur:add_slider(	bui_name, rect  )
	if param_ref then
		--param.set_comment( param_ref, bui_name )
		bu:set_target_param( param_ref, 1, bui_name )
		--todo this thru only if use thru a meu
		param.set_save( param_ref, false )	--hack we try : if the meu store the value we don't need to store it in the object
	end
	bu:set_min_max_value( min,max, val )

	--bu:set_color_back( bui_name  )

	bu:set_text_draw( false )
	bu:set_value_no_draw( 1.0 )
	name = string.lower(name)
	bu:set_meter_color_named( name )
	bu:set_color_back_named_no_error( name )

	return bu
end
function BU_COLOR:__add_rgb_one( i, name, v )
	local rect = self:__build_color_pos( i, self.__nb_slider_rgb )
	local bu = self:__add_slider( rect, name, 0,1, true, v or 1 )
	local col = bu:get_color_named_no_error( string.lower( name ) )
	local gr = .5
	local f = .5
	local r = col[1] * f
	local g = col[2] * f
	local b = col[3] * f
	bu:set_text_color( { gr-r+g+b, gr+r-g+b, gr+r+g-b, 1 } )
	return bu
end
function BU_COLOR:__add_hsv_one( i, name, min,max )
	local rect = self:__build_color_pos( i, self.__nb_slider_hsv )
	local bu = self:__add_slider( rect, name, min,max, true )
	--bu:set_active( false )
	bu:set_visible( false )
	bu:set_text( string.sub(name,1,1) )
	bu:set_text_visible(true)
	return bu
end

--todo solve the rgb hsv bug by remaking the hsv strategy
--todo show when value outside of [0,1] 
function BU_COLOR:add_rgb_hsv( b_hsv )
	local cref = self.__color_ref
	local b_ref_hsv = b_hsv and (cref and cref:has_hsv()) or nil

	--hack done quicly for monaco
	local r,g,b
	if cref then
		r,g,b = unpack(cref:get_rgb())
	end
	--todo hsv should exist also when no color ref but this not the case for now
	if b_ref_hsv then
		self.__bu_hsv =
		{
			self:__add_hsv_one( 0,	"Hue",	 0, 2 ),
			self:__add_hsv_one( 1,	"Sat",	 0, 1 ),
			self:__add_hsv_one( 2,	"Value", 0, 1 )
		}
	end

	self.__bu_rgb =
	{
		self:__add_rgb_one( 0, "Red", 	r	),
		self:__add_rgb_one( 1, "Green",	g	),
		self:__add_rgb_one( 2, "Blue", 	b	)
	}

	if self.__b_grey	then self.__bu_grey	 = self:__add_rgb_one(  3, "Grey"  ) end
	if self.__b_alpha	then self.__bu_alpha = self:__add_rgb_one( -1, "Alpha" ) end
end

function BU_COLOR:create( name, rect, color_ref, b_alpha, b_grey, b_hsv )
	local rect_new = table.copy_simple(rect)
	local fsx = 2/8
	local l = rect[1] - rect[3] * .5
	rect_new[3] = rect_new[3] * fsx
	rect_new[1] = l + rect_new[3] * .5

	local self = BU_COLOR:create_instance( name, rect_new )
	self.__color_ref = color_ref

	self:set_ui_active(			true  )
	self:set_mobile(			false )
	self:set_text_draw(			false )

	--todo for now should be refined
	self:set_preset_use(     	false )
	self:set_pos_load_save(   	false )
	self:set_value_load_save(	false )

	self:set_method_on_click(			self, "flip_values_show" )
	self:set_method_on_click_double(	self, "flip_hsv_mode" )

	if name then
		name = self:get_text() --so we get without the potential group name 
		if string.sub( name, -6 ) == "_color" then
			self.__pre = string.sub( name, 1, -6 )
		else
			--this was a test
			--if name ~= "" then
			self.__pre = name.."_"
			--end
		end
	end
	
	self.__b_grey		= b_grey
	self.__b_alpha		= b_alpha
	
	self.__nb_slider_rgb	= 3 + (b_grey and 1 or 0) 
	self.__nb_slider_hsv	= 3

	local t = {}
	t.l = l + rect_new[3]
	t.sx = rect[3]-rect_new[3]
	t.sy = rect[4]
	t.b = rect[2] - t.sy *.5
	if b_alpha then
		self.__sx_alpha = t.sx / 5
		t.sx = t.sx - self.__sx_alpha
	end
	self.__lb_sxy = t

--	self.__meu = meu
	self:add_rgb_hsv( b_hsv )
	self:set_hsv_mode( false )
	return self
end


-- GET / SET Fns
--	regroup in a global COLOR_PICKER bu and clean (it does too much)
function BU_COLOR:get_rgb()
	local bu
	if self.__b_hsv_mode then
		bu = self.__bu_hsv
		return aaa.color.hsv_to_rgb( bu[1]:get_value(), bu[2]:get_value(), bu[3]:get_value() )
	else
		bu = self.__bu_grey
		local grey = bu and bu:get_value() or 1.
		bu = self.__bu_rgb
		if bu then
			return bu[1]:get_value() * grey, bu[2]:get_value() * grey, bu[3]:get_value() * grey
		end
	end
end
function BU_COLOR:get_rgba()
--todo refine : it was wrong because if the color obj_ui was not updated the conversion was not done
--	local color_ref = self.__color_ref
--	if color_ref then
--		local t = color_ref:get_rgba()
--		self:print( "zobibeter color ref    "..t[2] )
--		return t[1], t[2], t[3], t[4]
--	else
		local r,g,b = self:get_rgb()
		local bu = self.__bu_alpha
		return r,g,b, bu and bu:get_value() or 1.
--	end
end
function BU_COLOR:set_rgba( r,g,b, a, f )
	--todo check if the color ref is niot dealt with the bu infact
	local color_ref = self.__color_ref
	if color_ref then
		if f then
			color_ref:set_rgba(  r*f, g*f, b*f, a )
		else
			color_ref:set_rgba(  r,g,b, a )
		end
	end
	local bu
	if self.__b_hsv_mode then
		local h,s,v = aaa.color.rgb_to_hsv( r or 0, g or 0, b or 0 )
		bu = self.__bu_hsv
		bu[1]:set_value( h )
		bu[2]:set_value( s )
		bu[3]:set_value( v )
	else
		bu = self.__bu_grey
		if bu then bu:set_value( f or 1 ) end
		bu = self.__bu_rgb
		if r then bu[1]:set_value( r ) end
		if g then bu[2]:set_value( g ) end
		if b then bu[3]:set_value( b ) end
	end
	if a then
		bu = self.__bu_alpha
		if bu then bu:set_value( a ) end
	end
end
function BU_COLOR:set_rgb( r,g,b )		self:set_rgba( r,g,b )							end
function BU_COLOR:set_rgbf( r,g,b, f )	self:set_rgba( r,g,b, nil, f )					end
function BU_COLOR:set_rgba_t( tab )		self:set_rgba( tab[1],tab[2],tab[3], tab[4] )	end
function BU_COLOR:set_alpha( a )
	if not a then return end

	--la us a double now we have ni duplication of values
	local color_ref = self.__color_ref
	if color_ref then
		color_ref:set_alpha(  a )
	end
	local bu = self.__bu_alpha
	if bu then bu:set_value( a ) end
end
function BU_COLOR:get_alpha()
	local bu = self.__bu_alpha
	return bu and bu:get_value() or 1.
end

function BU_COLOR:get_grey()
--refine
--[[	local color_ref = self.__color_ref
	if color_ref then
		self:print( "zobibeter color ref" )
		local t = color_ref:get_rgba()
		return t[1], t[2], t[3], t[4]
	else
--]]
		local bu = self.__bu_grey
		local grey = bu and bu:get_value() or 1.
		return grey
--	end
end

function BU_COLOR:set_grey( g )
	local bu = self.__bu_grey
	local grey = bu:set_value(g)
	return grey
end

function BU_COLOR:update()
	oo.getsuper(BU_COLOR).update(self)

	if self.__color_ref then
	else
		--la we should decide a strategy
		if self.__b_hsv_mode then
			local r,g,b, a = self:get_rgba()
			local bu = self.__bu_rgb
			bu[1]:set_value( r )
			bu[2]:set_value( g )
			bu[3]:set_value( b )
			local bu_grey = self.__bu_grey
			if bu_grey then
				bu_grey:set_value(1)
			end
		else
		end
	end
end

local function format_number( v )
	--return math.min( math.floor( v * 256 ), 255 )
	return v==1. and 255 or math.floor( v * 256 )
end

function BU_COLOR:draw()
	if GA.b_spy then aaa.spy.push_range( "BU_COLOR", 3 ) end

--	self:__update_hsv()
	local r,g,b,a = self:get_rgba()
	local ab = self:get_alpha_bu_to_draw()
	--local alpha = self:get_alpha()
	gol.color( r,g,b, ab )
		aaa.draw_rect( -.5,0, .5,.5 )
	gol.color( r,g,b, a*ab )
		aaa.draw_rect( -.5,-.5, .5,0 )

	--refine color change
	--todo could be optimized
	--		make generic integrate in BI then
	if GA.b_spy then
		aaa.spy.pop_range()
		aaa.spy.push_range( "compute", 4 )
	end

	if self.__b_draw_gamma then
		self:set_text_color( 0,0,0, 1 )
	else
		local h,s,v = aaa.color.rgb_to_hsv( r,g,b )
		if s > .7 then
			local v2 = v * a
			--self:print( v2 )
			if		v2 > .3	then	self:set_text_color( 0,0,0, 1 )
			elseif	v < .7	then	self:set_text_color( 1,1,1, 1 )
			else					r,g,b = aaa.color.hsv_to_rgb( h+.4, .5, v )
									self:set_text_color( r,g,b, 1 )
			end
		else
			local v2 = v * a
			--self:print( v2 )
			if		v2 > .3	then	self:set_text_color( 0,0,0, 1 )
			elseif	v < .7	then	self:set_text_color( 1,1,1, 1 )
			else					--v = (v2+v) * .5
									--self:set_text_color( v,v,v,1 )
									r,g,b = aaa.color.hsv_to_rgb( h+.4, .5, v )
									self:set_text_color( r,g,b, 1 )
			end
		end
	end
	if GA.b_spy then
		aaa.spy.pop_range()
		aaa.spy.push_range( "text", 5 )
	end
	self:draw_text_color_sel()
	if self.__b_values_show then
		if self.__b_draw_gamma then
			self:draw_text_lrbt( self:format_number_in_text(r).." "..self:get_text().."\n "
									..self:format_number_in_text(g).."\n "
									..self:format_number_in_text(b),
								-.4, .5,	.22, .5	)
		else
			self:draw_text_lrbt( format_number(r).." "..self:get_text().."\n "..format_number(g).."\n "..format_number(b),	-.4, .5,	.22, .5	)
		end
	else
		self:draw_text_lrbt( self:get_text(),	-.5, .5,	.27, .52	)
	end
	--local str = self:get_text().."\n "..format_number(r).."\n "..format_number().."\n "..format_number(b)

--	self:draw_text_lrbt( format_number(r),	-.3, .5,	.05, .26	)
--	self:draw_text_lrbt( format_number(g),	-.3, .5,	-.20, .01	)
--	self:draw_text_lrbt( format_number(b),	-.3, .5,	-.45, -.24	)
--faster

	if GA.b_spy then aaa.spy.pop_range() end
end

-- RGB / HSV
--
function BU_COLOR:flip_hsv_mode()
	if aaa.keyboard.is_ctrl() then
		local src = BU_COLOR.__bu_click_last
		if src then
			local r,g,b, a = src:get_rgba()
			self:set_rgba( r,g,b, a )
		end
	else
		BU_COLOR.__bu_click_last = self
	end
	self:print( "flip_hsv_mode()" )
	self:set_hsv_mode( not self.__b_hsv_mode )
end
function BU_COLOR:flip_values_show()
	self.__b_values_show = not self.__b_values_show
end
--todo we do that because of activation on off by tab
--refine
function BU_COLOR:__set_hsv_bu( b )

	local bu_hsv = self.__bu_hsv
	if not bu_hsv then return end

	local bu_rgb = self.__bu_rgb
--	if not bu_rgb then return end

	--aaa.print_fn()

	--todo why active is not enough at init ?
	for i=1,3 do
		bu_hsv[i]:set_active( 	b		):set_visible(b)
		bu_rgb[i]:set_active(	not b	):set_visible(not b)
	end

	local bu_grey = self.__bu_grey
	if bu_grey then
		bu_grey:set_active( not b ):set_visible(not b)
	end
end

function BU_COLOR:set_hsv_mode( b )
	if b == nil then b = false end
	local bu_hsv = self.__bu_hsv
	if not bu_hsv then b = false end
	--aaa.print_fn()
	--table.print( self, "BU_COLOR in set_hsv_mode ", 2 )

	self.__b_hsv_mode = b
	local color_ref = self.__color_ref
	if color_ref then
		--self:print( "color_ref is "..color_ref )
		--table.print( color_ref, "color_ref", 4 )
		color_ref:set_hsv_mode( b )
	end

	if bu_hsv then
		local bu_rgb = self.__bu_rgb
		if bu_hsv and bu_rgb then
			local bu_grey = self.__bu_grey
			--self:print( "fghgfghfg" )
			-- convert info
			local bu = b and bu_rgb or bu_hsv
			local vec = {}
			--	read info
			for i=1,3 do	vec[i] = bu[i]:get_value()	end
			--	convert
			if b then
				if bu_grey then
					local grey = bu_grey:get_value()
					for i=1,3 do	vec[i] = vec[i] * grey	end
				end
				vec[1], vec[2], vec[3]	=	aaa.color.rgb_to_hsv( vec[1], vec[2], vec[3] )
			else
				vec[1], vec[2], vec[3]	=	aaa.color.hsv_to_rgb( vec[1], vec[2], vec[3] )
			end

			--	write info
			bu = b and bu_hsv or bu_rgb
			for i=1,3 do bu[i]:set_value( vec[i] )	end
			if bu_grey and not b then
				bu_grey:set_value(1)
			end

			self:__set_hsv_bu( b )
		end
	end
	return self
end

function BU_COLOR:get_values()
	return pack( self:get_rgba() )
end
function BU_COLOR:set_values( tab )
	--aaa.print_fn()
	return self:set_rgba( unpack(tab) )
end
