
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu


	local ix = 1
	local iy = 2
	local SY = 1
	local DY = .2

	iy = iy + SY * 1.4
end


function MONACO_AQUA:init_presence_SO()
	self.presence_SO = {}
	for i = 1, 8 do
		local item = {}
		item.x = 0
		item.z = 0
		item.u = 0
		item.v = 0
		item.coverage = 0
		local name = self:get_location_by_index(i)
		self.presence_SO[ name ] = item
	end
end

function MONACO_AQUA:get_presence_SO( name )
	if not self.presence_SO then
		self:init_presence_SO()
	end
	if name == nil then
		-- return the full table
		return self.presence_SO
	elseif self.presence_SO[ name ] then
		-- return entry
		return self.presence_SO[ name ]
	end
	return nil
end

function MONACO_AQUA:show_coverage( name, coverage, u,v )
	if coverage~=0 then
		local function round3(v) return math.floor( v*1000 ) * .001 end
		local function round2(v) return math.floor( v*100 ) * .01 end
		aaa.show( round3(coverage).." with "..round2(u).." , "..round2(v), name )
	end
end

function MONACO_AQUA:update_presence_SO( index, coverage, x,z, u,v )
	if self:is_party() then x,z, u,v, coverage = 0,0,0,0,0 end
--	self:print( "update_presence_SO "..index.." : "..coverage.." at "..x..", "..z)
	--return
	if not self.presence_SO then
		self:init_presence_SO()
	end

	local name = ""
	if index < 9 then
		name = self:get_location_by_index(index)
	else
		-- clam ?
	end

	self:show_coverage( "Cov_"..index, coverage, u, v )

	-- get table entry
	local item = self.presence_SO[ name ]
	if not item then
		-- if entry doesn't exists, then add it to the table in case we need more items (like the clam)
		item = {}
		self.presence_SO[ name ] = item
	end

	--update entry
	item.x = x
	item.z = z
	item.u = u
	item.v = v
	item.coverage = coverage
--table.print( self.presence_SO, "self.presence_SO", 3 )
--	table.print( self.presence_SO[ "KR3" ], "self.presence_SO", 3 )
--	local tab = self:get_presence_SO()
--	table.print( tab, "tab", 3 )
end

function MONACO_AQUA:build_presence_clam_SO()
	self.presence_clam_SO = {}
	for i = 1, 7 do
		local item = {}
		item.x = 0.0
		item.z = 0.0
		item.coverage = 0.0
		self.presence_clam_SO[ i ] = item
	end
end

function MONACO_AQUA:update_presence_clam_SO( index, coverage, x, z )
	if self:is_party() then x,z, coverage = 0,0,0 end

	if not self.presence_clam_SO then
		self:build_presence_clam_SO()
	end
	self:show_coverage( "Clam_"..index, coverage, x, z )

	if self.presence_clam_SO[ index ] then
		-- update table entry
		self.presence_clam_SO[ index ].x = x
		self.presence_clam_SO[ index ].z = z
		self.presence_clam_SO[ index ].coverage = coverage
	else
		-- if entry doesn't exists, then add it to the table in case we need more items (like the clams)
		local item = {}
		item.x = x
		item.z = z
		item.coverage = coverage
		self.presence_clam_SO[ index ] = item
	end

end

function MONACO_AQUA:get_presence_clam_SO( index )
	if not self.presence_clam_SO then
		self:build_presence_clam_SO()
	end
	if index == nil then
		-- return the full table
		return self.presence_clam_SO
	elseif self.presence_clam_SO[ index ] then
		-- return entry
		return self.presence_SO[ index ]
	end
	return nil
end


function MONACO_AQUA:build_presence_nemo_SO()
	self.presence_nemo_SO = {}
	for i = 1, 3 do
		local item = {}
		item.x = 0.0
		item.z = 0.0
		item.coverage = 0.0
		self.presence_nemo_SO[ i ] = item
	end
end

function MONACO_AQUA:get_presence_nemo_SO( index )
	if not self.presence_nemo_SO then
		self:build_presence_nemo_SO()
	end
	if index == nil then
		-- return the full table
		return self.presence_nemo_SO
	elseif self.presence_nemo_SO[ index ] then
		-- return entry
		return self.presence_nemo_SO[ index ]
	end
	return nil
end
--update_presence_nemo_SO
function MONACO_AQUA:update_presence_nemo_SO( index, coverage, x, z )
	if self:is_party() then x,z, coverage = 0,0,0 end

	--	self:print( "update_presence_nemo_SO "..index.." : "..coverage.." at "..x..", "..z)
		--return
	if not self.presence_nemo_SO then
		self:build_presence_nemo_SO()
	end
	self:show_coverage( "Nemo_"..index, coverage, x, z )

	if self.presence_nemo_SO[ index ] then
		-- update table entry
		self.presence_nemo_SO[ index ].x = x
		self.presence_nemo_SO[ index ].z = z
		self.presence_nemo_SO[ index ].coverage = coverage
	else
		-- if entry doesn't exists, then add it to the table in case we need more items (like the clams)
		local item = {}
		item.x = x
		item.z = z
		item.coverage = coverage
		self.presence_nemo_SO[ index ] = item
	end
	--table.print( self.presence_SO, "self.presence_SO", 3 )
--	table.print( self.presence_SO[ "KR3" ], "self.presence_SO", 3 )
--	local tab = self:get_presence_SO()
--	table.print( tab, "tab", 3 )
end

