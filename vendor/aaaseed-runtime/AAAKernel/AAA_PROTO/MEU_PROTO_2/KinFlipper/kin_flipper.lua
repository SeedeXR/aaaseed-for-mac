
function meu:define_meu_infos( )
	return { author = "Mâa",
			tags = { "2d", "Camera", "texture", "unfinished", "depreciated" },
			help = "Depreciated used in some Old applications"
		}
end

function meu:define_ui()
	local bu
	local par

	local ix, iy = 1, 2
	local dx = 2.5

	bu = self:add_button( {ix,	iy },			"Do Flip", self, "b_flip", false )

--[[
	bu = self:add_button( {ix+dx,	iy },			"Simul", app, "b_simul", false )
		bu:set_text_rect_ratio( 3 )
--]]
end


function meu:init_mu_meu()
	local meu_kin = self.meu_kin
	if not meu_kin then
		meu_kin = self:get_meu_by_name_no_error(		"Kinect_2"	)	--todo a more generic way to get cur video
		if not meu_kin then	return false end
		self.meu_kin = meu_kin
	end
	
	local meu_kin_move = self.meu_kin_move
	if not meu_kin_move then
		meu_kin_move = self:get_meu_by_name_no_error(	"KinMove_1"	)	--todo a more generic way to get cur video
		if not meu_kin_move then return false end
		self.meu_kin_move = meu_kin_move
		self.mu_kin_move = meu_kin_move:get_mu()
	end

	local mu_ref_1 = self.mu_ref_1
	if not mu_ref_1 then
		mu_ref_1 = self:get_mu_by_name_no_error(		"ref_fbo_f"..1	)	--todo a more generic way to get cur video
		if not mu_ref_1 then return false end
		self.mu_ref_1 = mu_ref_1
	end

	local mu_ref_2 = self.mu_ref_2
	if not mu_ref_2 then
		mu_ref_2 = self:get_mu_by_name_no_error(		"ref_fbo_f"..2	)	--todo a more generic way to get cur video
		if not mu_ref_2 then return false end
		self.mu_ref_2 = mu_ref_2
	end

	local mu_blur = self.mu_blur
	if not mu_blur then
		mu_blur = self:get_mu_by_name_no_error(			"blur_1"	)	--todo a more generic way to get cur video
		if not mu_blur then return false end
		self.mu_blur = mu_blur
	end

	return true
end

function meu:update()
	if not self:init_mu_meu() then return end

	local ref = self.ref

	local meu_kin = self.meu_kin
	local b_disp
	local b_f1 = false
	local b_f2 = false
	local frame_index = meu_kin:get_frame_count()
	if self.__frame_index ~= frame_index then
		self.__frame_index = frame_index
			--self:print( "New frame "..frame_index )

		self.meu_kin_move:set_bind_2d( meu_kin:get_depth_bind_used() )
		local name
		if self.b_flip then
			if frame_index % 2 == 0 then
				name = "A"
				b_f1 = true
			else
				name = "B"
				b_f2 = true
			end
		else
			name = "A"
			b_f1 = true
		end
		self.fbo_double = self:get_bind_by_name( name )

		self.mu_blur:get_meu_used():set_bind_2d( self.fbo_double )

		b_disp = true
	else
		--nothing to do kinect did not change
		--self:print( "No new frame" )
		if self.b_flip then
			b_disp = false
		else
		end
	end

	app:set_fbo_double_last( self.fbo_double )
	self.mu_kin_move:set_render_skip_next( not b_disp )
	self.mu_blur:set_render_skip_next( not b_disp )
	self.mu_ref_1:set_render_skip_next( not b_f1 )
	self.mu_ref_2:set_render_skip_next( not b_f2 )
end
