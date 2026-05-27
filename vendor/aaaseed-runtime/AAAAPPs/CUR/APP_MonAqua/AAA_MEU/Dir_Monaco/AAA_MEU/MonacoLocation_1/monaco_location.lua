
MONACO_AQUA.__location_to_location_side =
{
	KL1="KL",	KL2="KL",	KL3="KL",	KL4="KL",
	KR1="KR",	KR2="KR",	KR3="KR",	KR4="KR",
	KC="KC",
	SO="SO"
}
MONACO_AQUA.__location_syno =
{
	KL12	= { 	"KL1",	"KL2"	},
	KL23	= { 	"KL2",	"KL3"	},
	KL34	= { 	"KL3",	"KL4"	},
	KL123	= { 	"KL1",	"KL2",	"KL3"	},
	KL234	= { 	"KL2",	"KL3",	"KL4"	},
	KL		= { 	"KL1",	"KL2",	"KL3",	"KL4"	},
	KR12	= { 	"KR1",	"KR2"	},
	KR23	= { 	"KR2",	"KR3"	},
	KR34	= { 	"KR3",	"KR4"	},
	KR123	= { 	"KR1",	"KR2",	"KR3"	},
	KR234	= { 	"KR2",	"KR3",	"KR4"	},
	KR		= { 	"KR1",	"KR2",	"KR3",	"KR4" },
	SO1		= {		"SO"	},
	SO2		= {		"SO"	},
	SO3		= {		"SO"	},
	SO4		= {		"SO"	},
	KALL	= { 	"KL1",	"KL2",	"KL3",	"KL4",	"KR1",	"KR2",	"KR3",	"KR4", "KC" },
}


-- name / IP
--
function MONACO_AQUA:build_pc_info()
--	local pre = "192.168.1." -- ip_str

	local pc_info_all = {}
	local pc_info_all_index = {}
	local function add( info )
		-- build location field (eg KC, KR1 )only if there is a location_side
		if info.location_side then
			info.location = info.location_side
			if info.location_index then
				info.location = info.location .. info.location_index
			end
			--build name only for the fun
			info.name = "BERTHA-"..info.location
		end

		info.b_dev		= info.b_dev		or false
		info.b_master	= info.b_master		or false
		info.b_dif		= info.b_dif		or false
		info.b_kake		= info.b_kake		or false
		info.b_kl		= info.b_kl			or false
		info.b_kr		= info.b_kr			or false
		info.b_kc		= info.b_kc			or false
		info.b_so		= info.b_so			or false

		pc_info_all[info.key] = info
		table.insert( pc_info_all_index, info )
	end

	--berthas	1-11
	for i=1,4 do	add	{	key="kl"..i,	ip="21"..i,	location_side="KL",		location_index=i,	group="bertha",	b_dif=true,	b_kake = true,  b_kl=true		}	end
	for i=1,4 do	add	{ 	key="kr"..i,	ip="22"..i,	location_side="KR",		location_index=i,	group="bertha",	b_dif=true,	b_kake = true,  b_kr=true		}	end
	add					{	key="kc",		ip="210",	location_side="KC",							group="bertha",	b_dif=true,	b_kake = true,  b_kc=true		}
	add 				{	key="so",		ip="205",	location_side="SO",							group="bertha",	b_dif=true,					b_so=true		}
	add 				{	key="master",	ip="200",	name="BERTHA-MAS",							group="bertha",	b_master=true								}
	--tablets	12
	add					{	key="tablet",	ip="202",	name="Immersion-tablette",					group="tablet" }
	--nucs		13-20
	for i=1,4 do	add	{	key="nucl"..i,	ip="23"..i,	name="NUCL"..i,								group="nuc" }		end
	for i=1,4 do	add	{	key="nucr"..i,	ip="24"..i,	name="NUCR"..i,								group="nuc" }		end
	--done

	self.__pc_info_all			= pc_info_all
	self.__pc_info_all_index	= pc_info_all_index
--	table.print( self.__pc_info_all, "pc_info_all", 3 )

end

function MONACO_AQUA:init_pc_info()
	self:build_pc_info()

	local function get_info()
		for _,info in PAIRS(self.__pc_info_all) do
			if info.ip and aaa.net.is_ip_str( "192.168.1."..info.ip ) then
				return info
			end
		end
	end

	local info
	if aaa.net.is_ip_str( "127.0.0.1" ) then -- no network we say tablet
		info = self.__pc_info_all.tablet
	else
		info = get_info()
	end
	if not info then	--case pc_dev we can customize here
		if aaa.net.machine=="ALIENMAA" then	--force tablet here
			info = self.__pc_info_all.tablet
		elseif aaa.net.machine=="MAAREO" then
			--info = self.__pc_info_all.tablet	--force tablet here
			info = { b_master=true, b_dev=false, b_dif=true, b_kake=true, b_kr=true, location_index=1, b_kl=false, b_kc=false, b_so=false  }
		else
			info = self.__pc_info_all.so
			info = { b_master=true, b_dev=true, b_dif=true, b_kake=true, b_kr=true, b_kl=true, b_kc=true, b_so=false  }
			--info = { b_master=true, b_dev=true, b_dif=false, b_kake=false, b_kr=false, b_kl=false, b_kc=false, b_so=false  }
		end
		--info = { b_master=false, b_dev=false, b_dif=true, b_kake=true, b_kr=true, location_index=1, b_kl=false, b_kc=false, b_so=false  }
		--info = { b_master=true, b_dev=true, b_dif=true, b_kake=true, b_kr=true, b_kl=true, b_kc=true, b_so=false  }
	end

	self.__b_pc_dev		= info.b_dev
	self.__b_pc_mas		= info.b_master
	self.__b_pc_dif		= info.b_dif
	self.__b_pc_tablet	= info.group=="tablet"

	self.__pc_info		= info

--	self:box_debug( "STOP and read" )
end

function MONACO_AQUA:get_location_by_index(index)	return self.__pc_info_all_index[index].location	end

function MONACO_AQUA:get_location()					return self.__pc_info.location					end
function MONACO_AQUA:is_location( name )			return self.__pc_info.location==name			end

function MONACO_AQUA:get_location_side()			return self.__pc_info.location_side				end
function MONACO_AQUA:is_location_side( name )		return self.__pc_info.location_side==name		end

function MONACO_AQUA:get_location_index()			return self.__pc_info.location_index		end
--function MONACO_AQUA:is_location_index( index )	return self.__pc_info.location_index==index	end

function MONACO_AQUA:find_location( names )
	if type(names)~="table" then
		local tab = self.__location_syno[names]
		if tab then		names = tab
		else			names = { names }
		end
	end
	local loc = self:get_location()
	for _,val in PAIRS(names) do
 		if loc==val then
			return loc
		end
	end
end

function MONACO_AQUA:is_kl( index )
	if index then return self.__pc_info.location==("KL"..index) end
	return self.__pc_info.b_kl
end
function MONACO_AQUA:is_kr( index )
	if index then return self.__pc_info.location==("KR"..index) end
	return self.__pc_info.b_kr
end
function MONACO_AQUA:is_kc()		return self.__pc_info.b_kc		end
function MONACO_AQUA:is_so()		return self.__pc_info.b_so		end
function MONACO_AQUA:is_kake()		return self.__pc_info.b_kake	end

function MONACO_AQUA:is_pc_mas()	return self.__b_pc_mas			end
function MONACO_AQUA:is_pc_dif()	return self.__b_pc_dif			end
function MONACO_AQUA:is_pc_dev()	return self.__b_pc_dev			end
function MONACO_AQUA:is_tablet()	return self.__b_pc_tablet		end

function MONACO_AQUA:check_grea_location( grea_name, names )
	if self:is_pc_dev() then return true end
	if self:is_pc_mas() then return true end	--refine for SO

	local loc = self:find_location(names)
 	if loc then
		self:print( grea_name.." -> find location "..loc )
		return true
	end
	self:print( grea_name.." location not on this machine" )
	return false
end

function MONACO_AQUA:__get_index_klr( x )
	local index = 0
--	self:print( "Finding screen for x = "..x)
	-- local function is_inside( x, i )
	-- 	if index > 1 then
	-- 		if x > app.const.mona.k_lar * ( i - 1 ) + app.const.mona.kc_interval * ( i - 1 ) and x < app.const.mona.k_lar * i + app.const.mona.kc_interval * ( i - 1 ) then

	-- 		end
	-- 	else
	-- 		if x > 0 and x < app.const.mona.k_lar then
	-- 			return 1
	-- 		end
	-- 	end
	-- 	return 0
	-- end
	if x > 0 then
		if x < app.const.mona.k_lar + app.const.mona.kc_interval * 0.5 then
			index = 1
		elseif x < app.const.mona.k_lar * 2 + app.const.mona.kc_interval * 1.5 then
			index = 2
		elseif x < app.const.mona.k_lar * 3 + app.const.mona.kc_interval * 2.5 then
			index = 3
		elseif x < app.const.mona.k_lar * 4 + app.const.mona.kc_interval * 3.5 then
			index = 4
		end
	else

	end
	return index
end

function MONACO_AQUA:get_screen_from_position( position )
	local str = ""
	if self.const then
		--table.print( app.const, "const", 5 )
		local z_2 = self.const.mona.d_lr * .5
		if position.z > z_2 then
		--	self:print( "Object is in KR")
			str = "KR"
			local index = self:__get_index_klr( position.x )
			if index > 0 then
				str = str..index
			end
		elseif position.z < -z_2 then
		--	self:print( "Object is in KL")
			str = "KL"
			local index = self:__get_index_klr( position.x )
			if index > 0 then
				str = str..index
			end
		else
			if position.x > self.const.mona.sol_s then
				str = "KC"
			--	self:print( "object is in KC")
			elseif position.x > .0 then
				str = "SO"
			--	self:print( "object is in SO")
			else
				self:print_error( "Object not on any PC")
			end
		end
	end
	return str
end
