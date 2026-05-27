function meu:init()
	self.state = "get_close"

	local ref = self.ref
	local layer_all = ref.layer

	for i=1,12 do
		local layer = layer_all[i]
		if layer then
			local obj = layer.obj
			layer.bank_2d, layer.bind_2d = aaa.layer.get_bank_bind_2d_ref( obj )
			local model = aaa.obj.get_down_by_class_no_error( obj, "model" )
			if model then
				layer.size = param.get_ref( model, "size_factor" )
			end
		end
	end

	local ref = self.ref
	ref.text = aaa.obj.get_down( self:get_layer(12), "qw_face_countdown" )
		ref.text_str	= param.get_ref( ref.text, "text" )
		ref.text_y		= param.get_ref( ref.text, "origin_y" )
end

function meu:draw_neutral()
	local ref = self.ref
	aaa.obj.update_then_draw( self:get_layer(2) )
end

function meu:draw_layer( id, media, size, text_size )
	local ref = self:get_layer_ref_table(id)
	if media then
		param.set( ref.bank_2d, media.bank_2d )
		param.set( ref.bind_2d, media.bind_2d )
	end
	if size then
		param.set( ref.size, size )
	end
	if text_size then
		param.set( ref.alpha, 1. )
		text_size = 3 - text_size * 2.
		param.set( ref.size,    	text_size		)
		param.set( self.ref.text_y, 1-text_size*.4	)
	end
	aaa.obj.update_then_draw( self:get_layers() )
end
function meu:draw_layer_balance( id, media, balance )
	self:draw_layer( id, media, 2.5+balance*1.5 )
end

local states_all = {
	get_close={},
	free_play={},
	qw_play={},
	photo={},
	qw_publish={},
	published={}
}
function meu:set_seq( name )
	self.state = name
	self.tempo = 0
	self.balance = 0
	self.b_grab_photo = false
end

local function post_facebook( filename )
	local smtp = SMTPS.get( "QwartFace" )
	smtp:set_server( "smtp.gmail.com", 465, "aaa.quartz", "aaas33d88" )
	filename = aaa.file.get_dir_name( string.remove_trailing_slash( aaa.dir.get_def() ) ).."/"..filename
	smtp:send( 'aaa.quartz@gmail.com', 'trigger@ifttt.com', "#photo_qwartz_face", "", filename )
	aaa.print( "Post to facebook "..filename )
--	aaa.box_good( filename )
end

function meu:update( )
	local state = self.state
	local tempo = self.tempo or 0
	tempo = tempo + aaa.time.dt
	self.tempo = tempo

	--aaa.mess.show( state.." "..tempo  )
	if not app.b_coverage then
		--aaa.mess.show( "o coverage "..state.." "..tempo  )
		if state ~= "get_close" then
			self:set_seq( "get_close" )
		end
	else
		-- b_coverage is on
		if state == "get_close" then
			self:set_seq( "free_play" )
			self.b_first = true
		elseif	state == "free_play" then
			if self.b_first and tempo >= 5. then
				self:set_seq( "qw_play" )
			end
		elseif	state == "qw_play" then
			if tempo >= 1. then
				local bal = self.balance
				bal = bal + ( app.coverage[3]-app.coverage[1] ) * aaa.time.dt * 2.
				bal = clamp( bal, -1, 1 )
				self.balance = bal
				--bal = -bal
				if outside( bal, -.9999, .9999 ) then
					if bal < 0 then
						self:set_seq( "free_play" )
						self.b_first = false
					else
						self:set_seq( "photo" )
					end
				end
			end
		elseif	state == "qw_publish" then
			if tempo >= 1. then
				local bal = self.balance
				bal = bal + ( app.coverage[3]-app.coverage[1] ) * aaa.time.dt * 2.
				bal = clamp( bal, -1, 1 )
				self.balance = bal
				--bal = -bal
				if outside( bal, -.9999, .9999 ) then
					if bal < 0 then
						self:set_seq( "photo" )
					else
						self:set_seq( "published" )
						local date = aaa.time.year_start.."_"..aaa.time.month_start.."_"..aaa.time.day_start
						local filename = "Media/Qwartz/Face/Out/"..date.."/QwartzFace_"..date.."_"..aaa.time.get_str_hms()
						aaa.img.save( self:get_bind_by_name( "F8" ),	"../../"..filename )
						filename = filename..".png"
						post_facebook( filename )
					end
				end
			end
		elseif	state == "published" then
			if tempo >= 3. then
				self:set_seq( "free_play" )
				self.b_first = true
			end
		end
	end
end

function meu:set_grab_photo( b )
	local fbo = app.meus.dif:get_fbo()
	fbo:set_attachment_active( 2, b )
	self.b_grab_photo = b
end

function meu:draw( )
	local medias = app:get_table( "__medias" )
	local state = self.states_all

	local ref = self.ref
	self:draw_layers_begin()

	aaa.obj.update_then_draw( self:get_layer(1) )

	local state = self.state
	if state == "get_close" then
		--	aaa.mess.show( state.." "..self.tempo  )
		if self.tempo >= 3. then
			self:draw_layer( 2, medias.approcher )
		end
	elseif	state == "free_play" then
		self:draw_layer( 3, medias.neutre )
	elseif	state == "qw_play" then
		self:draw_layer( 3, medias.qu_pris )
		if self.tempo >= 1. then
			self:draw_layer_balance( 4, medias.amuser, -self.balance )
			self:draw_layer_balance( 5, medias.oui, self.balance )
		end
	elseif	state == "photo" then
		local delay = 8.
		if self.tempo <= delay then
			local number =  math.floor( delay - self.tempo ) + 1
			--param.set( self.ref.text_str, number )
			--self:draw_layer( 12, nil, nil, self.tempo / delay 	)
			self:draw_layer( 6, medias.__count[number] )
			self.flash = 0
		else
			if self.flash == 0 then
				if not self.b_grab_photo then
					self:set_grab_photo( true )
				else
					--self:draw_layer( 3, medias.neutre )
					self:set_grab_photo( false )
					self.flash = self.flash + aaa.time.dt
				end
			elseif self.flash < .35 then
				self.flash = self.flash + aaa.time.dt
				self:draw_layer( 7 )
			else
				self:set_seq( "qw_publish" )
			end
		end
	elseif	state == "qw_publish" then
		self:draw_layer( 9 )
		self:draw_layer( 3, medias.qu_publier )
		if self.tempo >= 1. then
			self:draw_layer_balance( 4, medias.non, -self.balance )
			self:draw_layer_balance( 5, medias.oui, self.balance )
		end
	elseif	state == "published" then
		self:draw_layer( 3, medias.publiee )
	elseif	state == "" then
	end

	--param.set( self.ref.text_str,8 )
	--self:draw_layer( 12, nil, nil, 1 	)

	self:draw_layers_end()
end