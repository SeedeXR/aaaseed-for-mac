--todo avoid GC and test it
-------------------------------------------------------------------------------------
--
--  QUATERNION
--
if CLASS.DECLARE( "QUATERNION" ) then
	QUATERNION:set_class_status_doc(	CLASS.STATUS.MATH,
										"quarternion math is here" )
end

function QUATERNION:create( w, x,y,z )--...)
    local obj = QUATERNION:create_instance_no_name()
    obj.x = x
    obj.y = y
    obj.z = z
    obj.w = w
    return obj
end

function QUATERNION:multiplyRight(q)
    local w,x,y,z
    w = self.w * q.w - self.x * q.x - self.y * q.y - self.z * q.z
    x = self.w * q.x + self.x * q.w + self.y * q.z - self.z * q.y
    y = self.w * q.y - self.x * q.z + self.y * q.w + self.z * q.x
    z = self.w * q.z + self.x * q.y - self.y * q.x + self.z * q.w
    return QUATERNION:create(w,x,y,z)
end

function QUATERNION:is_zero()
    -- are we the zero vector
    if self.w ~= 0 or self.x ~= 0 or self.y ~= 0 or self.z ~= 0 then
        return false
    end
    return true
end

function QUATERNION:len()
    return math.sqrt(math.pow(self.w,2) + math.pow(self.x,2) + math.pow(self.y,2) + math.pow(self.z,2))
end

function QUATERNION:lensq()
    return math.pow(self.w,2) + math.pow(self.x,2) + math.pow(self.y,2) + math.pow(self.z,2)
end
function QUATERNION:scale(l)
    self.w = self.w * l
    self.x = self.x * l
    self.y = self.y * l
    self.z = self.z * l
end

function QUATERNION:normalize()
    local l
    if self:is_zero() then
        self:print("Unable to normalise a zero-length quaternion")
        return
    end
    l = 1/self:len()
	self.w = self.w *l
	self.x = self.x *l
	self.y = self.y *l
	self.z = self.z *l
end

function QUATERNION:matrix_transform(out)
    local mat = out or {}  -- preallocated table

    local x, y, z, w = self.x, self.y, self.z, self.w
    local x2, y2, z2 = x*x, y*y, z*z

    -- Column-major order (OpenGL style)
    mat[1]  = 1 - 2*y2 - 2*z2
    mat[2]  = 2*x*y - 2*w*z
    mat[3]  = 2*x*z + 2*w*y
    mat[4]  = 0

    mat[5]  = 2*x*y + 2*w*z
    mat[6]  = 1 - 2*x2 - 2*z2
    mat[7]  = 2*y*z - 2*w*x  -- fixed sign
    mat[8]  = 0

    mat[9]  = 2*x*z - 2*w*y
    mat[10] = 2*y*z + 2*w*x  -- fixed sign
    mat[11] = 1 - 2*x2 - 2*y2
    mat[12] = 0

    mat[13] = 0
    mat[14] = 0
    mat[15] = 0
    mat[16] = 1

    return mat
end


function QUATERNION:get_euler()
	local test = self.x*self.y + self.z*self.w
	local	heading
	local	attitude
	local	bank
	if test > 0.499 then -- singularity at north pole
		heading = 2 * math.atan2( self.x, self.w )
		attitude = math.pi *.5
		bank = 0
		self:print_error( "north pole")
	elseif test < -0.499 then -- singularity at south pole
		heading = -2 * math.atan2(self.x, self.w )
		attitude = - math.pi *.5
		bank = 0
		self:print_error( "south pole")
	else
		local sqx = self.x*self.x
		local sqy = self.y*self.y
		local sqz = self.z*self.z
		heading = math.atan2(2*(self.y*self.w-self.x*self.z) , 1 - 2*sqy - 2*sqz)
		attitude = math.asin(2*test)
		bank = math.atan2(2*(self.x*self.w-self.y*self.z ), 1 - 2*sqx - 2*sqz)
	end
	return { heading, attitude, bank }
end


   -- void gl_matrix(double *m)
   --  {
   --      if(!m) return;
   --      m[0] = 1.0 - 2.0 * ( y * y + z * z );
   --      m[1] = 2.0 * (x * y + z * w);
   --      m[2] = 2.0 * (x * z - y * w);
   --      m[3] = 0.0;
   --      m[ 4] = 2.0 * ( x * y - z * w );
   --      m[ 5] = 1.0 - 2.0 * ( x * x + z * z );
   --      m[ 6] = 2.0 * (z * y + x * w );
   --      m[ 7] = 0.0;
   --      m[ 8] = 2.0 * ( x * z + y * w );
   --      m[ 9] = 2.0 * ( y * z - x * w );
   --      m[10] = 1.0 - 2.0 * ( x * x + y * y );
   --      m[11] = 0.0;
   --      m[12] = 0;
   --      m[13] = 0;
   --      m[14] = 0;
   --      m[15] = 1.0;
   --  };

function QUATERNION:slerp(t, from, to, out )
    local epsilon = 1e-6

    -- 4D dot product
    local cosomega =  from.w * to.w +  from.x * to.x +  from.y * to.y + from.z * to.z

    -- Copy 'to' locally (no allocation, just scalars)
    local tw, tx, ty, tz = to.w, to.x, to.y, to.z

    -- Shortest path correction
    if cosomega < 0.0 then
        cosomega = -cosomega
        tw = -tw
        tx = -tx
        ty = -ty
        tz = -tz
    end

    -- Clamp for numerical safety
    if cosomega > 1.0 then
        cosomega = 1.0
    elseif cosomega < -1.0 then
        cosomega = -1.0
    end

    local scale_from, scale_to

    if (1.0 - cosomega) > epsilon then
        local omega = math.acos(cosomega)
        local sinomega = math.sin(omega)

        scale_from = math.sin((1.0 - t) * omega) / sinomega
        scale_to   = math.sin(t * omega) / sinomega
    else
        -- Quaternions are very close → linear interpolation
        scale_from = 1.0 - t
        scale_to   = t
    end

	out = out or QUATERNION:create(0,0,0,0)
	out.w = scale_from * from.w + scale_to * tw
    out.x = scale_from * from.x + scale_to * tx
    out.y = scale_from * from.y + scale_to * ty
    out.z = scale_from * from.z + scale_to * tz
end


function QUATERNION.make_rotate( from, to )
	local sourceVector = V3.clone( from )
	local targetVector = V3.clone( to )

	local fromLen2 = V3.norm_squared( from )
	local fromLen
--	// normalize only when necessary, epsilon test
	if ((fromLen2 < 1.0 - 1e-7) or (fromLen2 > 1.0 + 1e-7)) then
		fromLen = math.sqrt(fromLen2);
		V3.scale( sourceVector, 1 / fromLen )
		--sourceVector /= fromLen;
	else
		fromLen = 1.0
	end

	local toLen2 = V3.norm_squared( to )
--	// normalize only when necessary, epsilon test
	if ((toLen2 < 1.0 - 1e-7) or (toLen2 > 1.0 + 1e-7)) then
		local toLen
		--// re-use fromLen for case of mapping 2 vectors of the same length
		if ((toLen2 > 1.0 - 1e-7) and (toLen2 < 1.0 + 1e-7)) then
			toLen = fromLen
		else
			toLen = math.sqrt(toLen2)
		end
		V3.scale( targetVector, 1 / toLen )
	end
--	// Now let's get into the real stuff
--	// Use "dot product plus one" as test as it can be re-used later on
	local dotProdPlus1 = 1.0 + V3.dot( sourceVector, targetVector )

	local x,y,z,w = 0,0,0,1
--	// Check for degenerate case of full u-turn. Use epsilon for detection
	if (dotProdPlus1 < 1e-7) then
		--// Get an orthogonal vector of the given vector
		--// in a plane with maximum vector coordinates.
		--// Then use it as quaternion axis with pi angle
		--// Trick is to realize one value at least is >0.6 for a normalized vector.
		if math.abs(sourceVector[1]) < 0.6 then
			local norm = math.sqrt(1.0 - sourceVector[1] * sourceVector[1])
			x = 0.0
			y = sourceVector[3] / norm
			z = -sourceVector[2] / norm
			w = 0.0
		elseif math.abs(sourceVector[2] ) < 0.6 then
			local norm = math.sqrt(1.0 - sourceVector[2] * sourceVector[2])
			x = -sourceVector[3] / norm
			y = 0.0
			z = sourceVector[1] / norm
			w = 0.0
		else
			local norm = math.sqrt(1.0 - sourceVector[2] * sourceVector[3])
			x = sourceVector[2] / norm
			y = -sourceVector[1] / norm
			z = 0.0
			w = 0.0
		end
	else
	--	// Find the shortest angle quaternion that transforms normalized vectors
	--	// into one other. Formula is still valid when vectors are colinear
		local s = math.sqrt(0.5 * dotProdPlus1)
	--	aaa.print( "s is "..s )
		local cross = V3.get_cross( sourceVector, targetVector )
	--	table.print( cross, "cross" )
	--	Vector3 c = v0.crossProduct(v1);
		V3.scale( cross, 1 / (2.0 * s) )
	--	table.print( cross, "cross2" )
		--tmp = sourceVector.getCrossed(targetVector) / (2.0 * s);
		x = cross[1]
		y = cross[2]
		z = cross[3]
		w = s
	--	aaa.print( "w "..x )
				--local s = math.sqrt( (1.+dot)*2. );
				--local invs = 1 / s;
	end
	return QUATERNION:create( w, x, y, z )
end

function QUATERNION.rotation_arc( vec_orig, vec_dest )
	-- V3.normalize( vec1 )
	-- V3.normalize( vec2 )

	-- local cross = V3.get_cross(vec1, vec2)

	-- local dot = V3.dot( vec1, vec2 )
	-- if dot >= 1.0 then
	-- 	return QUATERNION:create( 1.0, .0,.0,.0 )
	-- else
	-- 	local s = math.sqrt( (1+dot)*2.)
	-- 	V3.scale( cross, 1/s )
	-- 	local q = QUATERNION:create(  s / 2.0, cross[1], cross[2], cross[3] )
	-- 	return q
	-- end

    --    Quaternion q;
     --       // Copy, since cannot modify local
     --       Vector3 v0 = *this;
     --       Vector3 v1 = dest;

            V3.normalize( vec_orig )
            V3.normalize( vec_dest )

            local dot = V3.dot( vec_orig, vec_dest )
         --   Real d = v0.dotProduct(v1);
          -- // If dot == 1, vectors are the same
			if (dot >= 1.0) then
				return QUATERNION:create( 1, 0,0,0 )
			elseif (dot < (1e-6 - 1.0) ) then
			--// Generate an axis

				local axis = V3.get_cross( {1,0,0}, vec_orig)
			--	Vector3 axis = Vector3::UNIT_X.crossProduct(*this);
				if (V3.norm(axis) == .0 ) then --// pick another if colinear
					aaa.print( "colinear ?")
					axis = V3.get_cross( {0,1,0}, vec_orig)
				end
				V3.normalize(axis)
				return QUATERNION:create( math.pi, axis[1], axis[2], axis[3] )

			else
				local s = math.sqrt( (1.+dot)*2. );
				local invs = 1 / s;

				local cross = V3.get_cross(vec_orig, vec_dest)
			--	Vector3 c = v0.crossProduct(v1);
				V3.scale( cross, invs )

				return QUATERNION:create( s * 0.5, cross[1], cross[2], cross[3] )
--				q.normalise();
			end
end


function QUATERNION.arcball( start, stop )

	--table.print( start, "start" )
	--table.print( stop, "stop" )
	if (start[1] == stop[1]) and (start[2] == stop[2]) then
		-- no rotation
		aaa.print_error( "No rotaion")
		return QUATERNION:create( 1, 0,0,0 )
	end

	V3.normalize( start )
	V3.normalize( stop )

	local cos2a = V3.dot( start, stop )
	local sina = math.sqrt( (1.0 - cos2a) * 0.5 );
	local cosa = math.sqrt( (1.0 + cos2a) * 0.5 );


	local axis = V3.get_cross( start, stop )

	axis[1] = axis[1]*sina
	axis[2] = axis[2]*sina
	axis[3] = axis[3]*sina

	return QUATERNION:create( cosa, axis[1], axis[2], axis[3])
	-- -- use a dot product to get the angle between them
	-- -- use a cross product to get the vector to rotate around
	-- local cos2a = ab_start*ab_curr;
	-- local sina = sqrt((1.0 - cos2a)*0.5);
	-- local cosa = sqrt((1.0 + cos2a)*0.5);
	-- vec cross = (ab_start^ab_curr).unit() * sina;
	-- quaternion(ab_next,cross.x,cross.y,cross.z,cosa);
end