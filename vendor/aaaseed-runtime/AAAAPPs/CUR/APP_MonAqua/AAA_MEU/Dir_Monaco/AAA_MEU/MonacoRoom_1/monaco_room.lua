function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	--ui.bu_col = self:add_rgbf(	{1,8.2 } )
--	self:add_camera( nil, 16 ):set_preset_use( false )

	local ix = 1
	local iy = 1
	local SY = 1
	local DY = SY*.2

	iy = iy + SY

end

function MONACO_AQUA:get_kake_size()
	local c = self.const.mona
	return c.k_lar
end
function MONACO_AQUA:get_kake_center_to_entrance_from_index( index_to_entrance )
	local c = self.const.mona
	return c.k_lar/2 + c.dk * (index_to_entrance-1)
end

function MONACO_AQUA:define_room()
	local mona = {}
	mona.d_lr 			= 7.995			-- floor (Sol) image size from left to right side
	mona.k_hau			= 8.77			-- Kakemono Side image height
	mona.kc_hau			= mona.k_hau	-- Kakemono Central image height
	mona.k_lar			= 4.46			-- Kakemono Side image width
	mona.k_interval		= 0.75			-- interval between Kakemono Side image
	mona.kc_interval	= .663			-- interval between Kakemono Side image and Kakemono Central
	mona.dk = mona.k_lar + mona.k_interval
	mona.sol_s			= 20.753		-- floor (Sol) big dimension

	local wiss = {}
	wiss.d_lr			= 8.16			-- floor (Sol) image size from left to right side
	wiss.k_hau 			= 5.45			-- Kakemono Side image height
	wiss.kc_hau			= 5.16			-- Kakemono Central image height
	wiss.k_lar			= 4.14			-- Kakemono Side image width
	wiss.k_interval		= 1.11			-- interval between Kakemono Side image at wissous
	wiss.kc_interval	= .663		-- interval between Kakemono Side image and Kakemono Central
	wiss.dk = wiss.k_lar + wiss.k_interval
	wiss.sol_s			= 21

	self.const = { mona=mona, wiss=wiss }

	self.deco = {}
	local function kake_3ds( b_left, c, index_to_entrance )
		return { cen={ b_left and -c.d_lr/2 or c.d_lr/2, c.k_lar/2+c.dk*(index_to_entrance-1), c.k_hau/2  }, suv={ c.k_lar, c.k_hau } }
	end

	local t = self.deco
	local c
	--		  Zup Right Hand            						selector
	c = self.const.mona
	t.KL1 = { pos_3ds = kake_3ds( true,  c, 1 )				,sn="kl"	}
	t.KL2 = { pos_3ds = kake_3ds( true,  c, 2 )				,sn="kl"	}
	t.KL3 = { pos_3ds = kake_3ds( true,  c, 3 )				,sn="kl"	}
	t.KL4 = { pos_3ds = kake_3ds( true,  c, 4 )				,sn="kl"	}

	t.KR1 = { pos_3ds = kake_3ds( false, c, 1 )	,rot_y=.5	,sn="kr"	}
	t.KR2 = { pos_3ds = kake_3ds( false, c, 2 )	,rot_y=.5	,sn="kr"	}
	t.KR3 = { pos_3ds = kake_3ds( false, c, 3 )	,rot_y=.5	,sn="kr"	}
	t.KR4 = { pos_3ds = kake_3ds( false, c, 4 )	,rot_y=.5	,sn="kr"	}

	t.KC  = { pos_3ds = { cen={  0,      c.sol_s,	c.kc_hau/2	},	suv={c.d_lr,c.k_hau}	}	,rot_y=-.25	,sn="kc"	}
	t.SO  = { pos_3ds = { cen={  0, 	 c.sol_s/2,	0       	},	suv={c.sol_s,c.d_lr} 	}	,rot_x=-.25	,sn="sol"	}

	c = self.const.wiss
	t.KL1.pos_3ds_wis = kake_3ds( true,	 c, 1 )
	t.KL2.pos_3ds_wis = kake_3ds( true,	 c, 2 )
	t.KL3.pos_3ds_wis = kake_3ds( true,	 c, 3 )
	t.KL4.pos_3ds_wis = kake_3ds( true,	 c, 4 )

	t.KR1.pos_3ds_wis = kake_3ds( false, c, 1 )
	t.KR2.pos_3ds_wis = kake_3ds( false, c, 2 )
	t.KR3.pos_3ds_wis = kake_3ds( false, c, 3 )
	t.KR4.pos_3ds_wis = kake_3ds( false, c, 4 )

	t.KC.pos_3ds_wis  = { cen={  0,     c.sol_s,	c.kc_hau/2  },	suv={c.d_lr,c.kc_hau}	}
	t.SO.pos_3ds_wis  = { cen={  0, 	c.sol_s/2,	0       	},	suv={c.sol_s,c.d_lr} 	}

	for k,v in PAIRS(t) do
		--self:print( "processing "..k )
		v.pos		= { cen = self:tr_3ds_2_aaa( v.pos_3ds.cen)		,suv = v.pos_3ds.suv		}
		v.pos_wis	= { cen = self:tr_3ds_2_aaa( v.pos_3ds_wis.cen)	,suv = v.pos_3ds_wis.suv	}
	end

	return t
end


function MONACO_AQUA:tr_3ds_2_aaa( t )
	return { t[2], t[3], t[1] }
end

function MONACO_AQUA:define_cams()
	local c = self.const.mona
	local dz = c.d_lr/2
	local x = c.sol_s - c.kc_interval
	self.cams =
	{
		KL 	= { xyz_3ds={ 19.265, 10.045, 1.4 }
					,angle_deg = 10.8	,lens_shift_3ds=6
-- kisize but we minimize it for speed
--					,sxp=9384	,syp=4096,	k_sxp=2083,
					,sxp=5864	,syp=2560,	k_sxp=1300,
					focal_horizontal=48., focal_vertical=22.05, frustum_offset_y=-.1545,
					clip_close=19, clip_far=100.,
					i_src=3, i_dst=1, dir_src=-1,
					target= { {0,0,-dz}, {x,0,-dz} }
				},
		KR 	= { xyz_3ds={ -19.265, 10.045, 1.4 }	,rot_y=.5
					,angle_deg = 10.8	,lens_shift_3ds=6
-- kisize but we minimize it for speed
--					,sxp=9384	,syp=4096,	k_sxp=2083,
					,sxp=5864	,syp=2560,	k_sxp=1300,
					focal_horizontal=48., focal_vertical=22.05, frustum_offset_y=-.1545,
					clip_close=19, clip_far=100.,
					i_src=3, i_dst=1, dir_src=1,
					target= { {0,0,dz}, {x,0,dz} }
				},
		KC	= { xyz_3ds={ 0, 1.15, 1.4 }		,rot_y=.25
					,angle_deg = 10.8	,lens_shift_3ds=6.914
-- kisize but we minimize it for speed
--					,sxp=3732	,syp=4096,
					,sxp=2004	,syp=2200,
					focal_horizontal=23.75, frustum_offset_y=-.0765,
					clip_close=18, clip_far=100.,
					i_src=1, i_dst=3, dir_src=1,
					target= { {c.sol_s,0,-dz}, {c.sol_s,0,dz} }
				},
		SO	= { xyz_3ds={ 0, 10.365, 25.364 }
					,angle_deg = -90
-- kisize but we minimize it for speed
--					,sxp=9692	,syp=3732,
					,sxp=5320	,syp=2048,
					focal_vertical= 17.97,
					clip_close=23.0, clip_far=60.,
					--i_src=2, i_dst=1, dir_src=-1,
					target= { {0,0,-dz}, {c.sol_s,0,-dz}, {c.sol_s,0,dz}, {0,0,dz} }
				},
	}

	local function build_coef( v, target, pos )
		local t = {}
		t.beg = target[v.i_src]
		t.a = ( pos[v.i_dst] - target[v.i_dst] ) / ( pos[v.i_src] - target[v.i_src] )
		t.b = pos[v.i_dst] - pos[v.i_src] * t.a
		return t
	end
	for k,v in PAIRS(self.cams) do
		--self:print( "processing "..k )
		v.pos		= self:tr_3ds_2_aaa( v.xyz_3ds )
		if v.i_src then
			v.coefs = {}
			for kt,target in IPAIRS(v.target) do
				v.coefs[kt] = build_coef( v, target, v.pos )
			end
		end
	end

end


