
if APP.DECLARE( "ROMANITE", APP_GP ) then
end

local L_APP = ROMANITE

function L_APP:init_app( ... )

	self:set_init_monitor_pass( false )

	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end
	self:lock(false)

	return true
end

local h = 2
--function L_APP:get_capture_suv_meter()		return h*4288/1200,	h		end
function L_APP:get_capture_suv_meter()		return 5,	h		end
function L_APP:get_capture_nb_uv()			return 2,	1		end

function L_APP:make_building_defs( tab )
	--aaa.print_fn()
	local sy_factor = .8
	local z_roma = {3,10}
	local inst_nb = 6

	tab.defs = {}
	local function add( t )
		if type(t.sy)=="table" then
			t.sy[1] = t.sy[1] * sy_factor
			t.sy[2] = t.sy[2] * sy_factor
		else
			t.sy = t.sy * sy_factor
		end
		table.insert( tab.defs, t )
	end
	add{	name="Arene",		fname="ARENES", 			sy=3,				z_range={6,25},		rot_y = 20,	field_type = 2 }
	add{	name="Musee",		fname="ROMANITE", 			sy=2,				z_range=z_roma,		rot_y = 70,	nb_by_unit=70, }
	add{	name="Horloge",		fname="TOUR_HORLOGE",		sy={3,4},			z_range={7,15},		}
	add{	name="Salle",		fname="SALLE_SPECTACLES",	sy={2,3},			z_range={10,20},	}
	add{	name="Ruine",		fname="RUINE",				sy={1,1.5},			z_range={3,7},		rot_y = 25,	}
	add{	name="Magne",		fname="TOUR_MAGNE",			sy={3,4},			z_range={6,20},		}
	add{	name="Cathe",		fname="CATHEDRALE",			sy={5,10},			z_range={2,22},		rot_y = 25,	}
	add{	name="Beaux",		fname="BEAUX_ARTS",			sy={2,3},			z_range={5,12},		rot_y = 25,	}
	add{	name="Gare",		fname="GARE",				sy=2,				z_range={10,15},	rot_y = 30,	}
	add{	name="Eglise_A",	fname="EGLISE",				sy={2.5,3.5},		z_range={4,12},		}
	add{	name="Eglise_B",	fname="EGLISE_ORANGE",		sy={3,4},			z_range={4,12},		rot_y = 30,	}
	add{	name="Morts",		fname="MONUMENT_MORTS",		sy={1,1.5},			z_range={1,10},		rot_y = 10,	}
	add{	name="Carre",		fname={ "CARRE_ART", "CARRE_ART_POTEAUX" },
															sy=2.5,				z_range=z_roma,		rot_y = 25,	}
	add{	name="Maison",		fname={ "MAISON_CARREE", "MAISON_CARREE_POTEAUX" },
															sy={2,3},			z_range=z_roma,		rot_y = 7,	}
	add{	name="Nemausus",	fname="NEMAUSUS",			sy={1.3,2.5},		z_range={8,20},		}
	add{	name="Fontaine",	fname="FONTAINE",			sy={1.5,2},			z_range={1,10},		rot_y = 25,	}
	add{	name="Sculpt_A",	fname="SCULPTURE", 			sy={.4,.8},			z_range={1,10},		}
	add{	name="Immeub_A",	fname="IMMEUBLES_A",		sy={2,3},			z_range={4,15},		}
	add{	name="Immeub_B",	fname="IMMEUBLES_B",		sy={2,3},			z_range={5,15},		}
	add{	name="Pont",		fname="PONT_GARD",			sy={1,3},			z_range={10,20},	rot_y = 45,	}

	add{	name="Arbre_A",		fname="ARBRE_A",			sy={1,1.6},			z_range={3,25},	b_tree=true,
															inst_nb=5,	displace=.2,	displace_gamma=.5,	rot_y = 20,	}
	add{	name="Arbre_B",		fname="ARBRE_B",			sy={.6,1.2},				z_range={3,25},	b_tree=true,
															inst_nb=5,	displace=.45,	displace_gamma=1.1,	rot_y = 20,	}
	add{	name="Arbre_C",		fname="ARBRE_C",			sy={1,1.6},			z_range={3,25},	b_tree=true,
															inst_nb=5,	displace=.2,	displace_gamma=2,	rot_y = 20,	}
	add{	name="Arbre_D",		fname="ARBRE_D",			sy={.6,1.2},		z_range={3,25},	b_tree=true,
															inst_nb=5,	displace=.5,	displace_gamma=.5,	rot_y = 20,	}
	add{	name="Arbre_E",		fname="ARBRE_E",			sy={1,1.8},			z_range={3,25},	b_tree=true,
															inst_nb=10,	displace=.5,	displace_gamma=.5,	rot_y = 20,	}
end

function L_APP:make_recit_defs( tab) 
	local sy_factor = .8	
	tab.defs = {}
	local function add( t )
		t.sy = sy_factor
		t.ox = t.ox or 0
		--t.oy = -.01
		table.insert( tab.defs, t )
	end
	add{	name= "Alexandre",	fname="ALEXANDRE",			vname="10_PORTRAIT_ALEXANDRE",	vid=10,	sy=1,	ox=-.095,	oy =-.01	}
	add{	name= "Apollon",	fname="APOLLON",			vname="2_APOLLON",				vid=2,	sy=.8,	ox=-.11,	oy =-.01	}
	add{	name= "Drusus",		fname="DRUSUS",				vname="3_DRUSUS",				vid=3,	sy=1.2,	ox=-.05,	oy =-.01	}
	add{	name= "Chien",		fname="ENFANT_CHIEN",		vname="4_ENFANT",				vid=4,	sy=1.2,	ox=-.05,	oy =-.01	}
	add{	name= "Foyer",		fname="FOYER",				vname="15_FOYER_PORTATIF",		vid=15,	sy=1.3,	ox=-.05,	oy =-.01	}

	add{	name= "Frise",		fname="FRISE",				vname="8_FRISE_AUX_AIGLES",		vid=8,	sy=.6,	ox=-.01,	oy =-.01	}
	add{	name= "Fronton",	fname="FRONTON",			vname="11_FRONTON_AUGUSTEUM",	vid=11,	sy=.85,	ox=-.0,		oy =-.0		}
	add{	name= "Grezan",		fname="GREZAN",				vname="1_GREZAN",				vid=1,	sy=1.3,	ox=-.02, 	oy =-.0		}
	add{	name= "Mosaique",	fname="MOSAIQUE_PALAIS",	vname="6_MOSAIQUE",				vid=6,	sy=.9,	ox=.04, 	oy =.01,	dr=4	}
	add{	name= "Neptune",	fname="NEPTUNE",			vname="13_FONTAINE_NEPTUNE",	vid=13,	sy=1.3,	ox=-.04,	oy =-.005	}

	add{	name= "Penthee",	fname="PENTHEE",			vname="14_MOSAIQUE_PENTHEE",	vid=14,	sy=1.08, ox=.04,	oy =.01		}
	add{	name= "Silene",		fname="SILENE",				vname="12_GROUPE_BEAUCAIRE",	vid=12,	sy=.9,	ox=-.19,	oy =-.02	}
	add{	name= "Stele",		fname="STELE",				vname="7_STELE_FUNERAIRE",		vid=7,	sy=1, 	ox=-.04,	oy =-.0		}
	add{	name= "Taureau",	fname="TAUREAU",			vname="9_RELIEFS",				vid=9,	sy=1.1,	ox=-.07,	oy =-.005	}
	add{	name= "Villa",		fname="VILLA_ROMA",			vname="5_VILLA_ROMA",			vid=5,	sy=1.05, ox=.04, 	oy =.01	}
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

