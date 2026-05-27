if not dev11 then
	dev11 = GABU_OBJ{}
end

function dev11:init()
	if self.ref then return end
	self.ref = {}
	local ref = self.ref
	ref.boid	= aaa.obj.get( "dev11_boid" )
	ref.curve	= aaa.obj.get( "dev11_curve3d" )
	for i=1,256 do
		aaa.bdd.clear_control_points( ref.curve, i )
	end
	self.sto = {}
end

local b_restart = true
function dev11:test()
	dev11:init()

	local	ref		= self.ref
	local	boid	= ref.boid
	local	curve	= ref.curve

	local	nb = aaa.bdd.get_point_nb( boid )
	local	pt = V3.new()
	--aaa.print( nb )
	local get_point = aaa.bdd.get_point_and_id
	for i=1,nb do
		local id
		pt[1], pt[2], pt[3], id = get_point( boid, i )
		local dum
		dum, id = poid.split_id( id )
		--aaa.print( id.." "..x )

		local pt_last = self.sto[id] or V3.new_from( 0, 0, 0 )
		if V3.dist_squared( pt, pt_last ) > .1 then
			self.sto[id] = V3.clone( pt )
			aaa.bdd.push_control_point_back( curve, id, pt[1], pt[2], pt[3] )

			local nb_pt = aaa.bdd.get_control_point_nb( curve, id )
			while nb_pt > 12 do
				local len
				if b_restart then
				--	len = aaa.bdd.get_len( curve, id )
				end
				aaa.bdd.pop_control_point_front( curve, id )
				if b_restart then
				--	len = len - aaa.bdd.get_len( curve, id )
				--	self:print( len )
				--	b_restart = false
				end
				nb_pt = nb_pt - 1
			end
		end
	end


end


dev11:test()