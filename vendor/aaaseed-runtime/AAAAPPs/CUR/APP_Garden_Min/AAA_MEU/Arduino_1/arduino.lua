function meu:define_meu_infos( )
	return { 	author = "Mâa ",
				tags = { "device", "input", "output", "unfinished", "tutorial" },
				help = {	"Example of MEU reading information from an arduino device."
					}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local ui = self.ui

	local ix, iy = 1,1
--	bu = self:add_slider(	{ix+1,iy,	7,1}, 	"speed",	self, "speed" )
--	self:add_trig_restart(	{ix,iy+1}		)

	ui.bu_line			=	self:add_text_info(	{ix,iy,	8,1}, "Line" )
	iy = iy + 1
	local t = {}
	local S = 16./9.
	for i = 1,9 do
		bu = self:add_button( {ix+(i-1)*S,iy, S, S },	"ON"..i ):set_text_visible( false )
			bu:set_check_mul(true)
			t[i] = bu
	end
	ui.bu_on = t

	bus_cur:set_method( "draw_after",	self,	"draw_ui_after"	)
end

function meu:draw_ui_after()

	local nb = math.min( #self.bufs, self.nb_frame )
	if nb < 0  then return end

	local f = .7 / nb
	local function draw( i, j )
		local x = (i-.5) / 9. - .5
		local by = .32
		aaa.draw_line( x, by - (j-1) * f, x, by - j * f )
	end
	gol.set_texture_dim( 0 )
	gol.push_matrix()
		gol.color_white()
		gol.set_line_width( 4 )
		--gol.set_line_width( 16 * ( 1-j/nb) )

		--self:print( #self.bufs )

		for i=1,9 do
			for j=1,nb do
				local t = self.bufs[j]
				if t[i]==1 then
					draw( i, j )
				end
			end
		end
	gol.pop_matrix()
end

function meu:init()
	local ref = self.ref

	ref.bdd = self:get_layer_bdd(1)
	ref.received = param.get_ref( ref.bdd, "received" )
	ref.line_received_nb = param.get_ref( ref.bdd, "line_received_nb" )
	ref.line_received = {}
	for i=1,8 do
		ref.line_received[i] = param.get_ref( ref.bdd, "line_received_"..i )
	end

	self.nb_frame = 128
	self.bufs = {}
end

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref

--	ui.bu_line:set_text( param.get( line_ref[i_line] ) )
end

function meu:draw()
	local ref = self.ref
	local ui = self.ui

	MEU.draw( self )

	--self:print( param.get( ref.received ) )

	local nb = param.get( ref.line_received_nb )
	local t = { }
	if nb > 0 then
		local line_ref = ref.line_received
		for i_line=1,nb do
			local str = param.get( line_ref[i_line] )
			--self:print( "line "..i_line.." : "..str )
			for i=1,9 do
				local i_sub = 1 + (i-1)* 2
				local c = string.sub( str, i_sub, i_sub )
				t[i] = t[i] or c=='1'
				--self:print( i.." -- "..c.." "..t[i] )
			end
		end
	end
	for i=1,9 do
		ui.bu_on[i]:set_value( t[i] and 1 or 0 )
		local v = t[i] and 1 or 0
		t[i] = v
		--if v < .9 then
			v = interpolate( (self.v_last or 0.), v, .01 )
		--end
		self.v_last = v
		aaa.midi.set_control( 4, i, v )
	end
	table.insert( self.bufs, 1, t )

	self.nb_frame = 128
	if #self.bufs > self.nb_frame then
		table.remove( self.bufs )
	end
end
