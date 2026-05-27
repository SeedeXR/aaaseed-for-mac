function meu:define_meu_infos( )
	return	{ author = "Mâa",
				help= { "Send Image over the network, 1 channel image can be compressed in a very efficient way",
						"Often used to send image from a camera capture process to the machine doing the graphic out." },
				tags = { "2d", "core", "coregraphic", "texture", "input", "output" }
			}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix,iy = 1,4.5
	local layer = self:get_layer(1)
	local SY = 1.
	local DY = .2
	local sxm,sym = 8,SY*5.8

	iy = 1
	ui.bu_count =
		self:add_text_info( {1,		iy,			8,SY}, 		"process_index" )
	self:add_trig_fn(		{9,		iy,			3,SY},		"Focus",	aaa.obj.set_focus_ui, ref.bdd_img_send	 )
	iy = iy + SY

	--self:add_button( 		{1,		iy,			4,SY},		"Force",	self, "b_force",	false )
	bu = self:add_button(	{ix,	iy,			4,SY},		"Constraint"				)
		bu:set_multiple( { "On change", "Limit", "Force" },	"constraint_sel", 1 )
		ui.bu_constraint_sel = bu
	bu = self:add_slider(	{ix+4,	iy,			4,SY},		"Interval",	self,			"interval",	.04,	.001,10	)
		ui.bu_delay = bu
	self:add_button(		{ix+8,	iy, 		2,SY},		"Mono", 	ref.mono,		nil,		true	)
	self:add_button(		{ix+8+2,iy, 		2,SY},		"Compress", ref.compress,	nil,		true	)
	ui.bu_frame_info =
		self:add_text_info(	{ix+8+4,iy,			4,SY*.5}, 	"frame_info" )
	iy = iy + SY + DY

	self:add_bu_texture(	{ix+8,	iy,			sxm,sym},	"Sent",		1 )
	self:add_bu_texture(	{ix+8,	iy+sym,		sxm,sym},	"Received",	3 )
	self:add_bu_texture(	{ix,	iy+sym,		sxm,sym},	"Tmp",		2 )

	ix,iy = 1, 3 + DY

	self:add_network_link(	ix,iy,				8,SY*1.75 )
	
end

function meu:add_network_link( ix,iy,	sx,sy )
	local ref = self.ref
	local ui  = self.ui

	local ix,iy = ix,iy
	local sxb
	sxb, sy = sx/3, sy/1.75
	local bu

	bu = self:add_text(			{ix,iy,				sx*.4,sy*.75},		"Dst ip" 	)
		ui.bu_link_dst_ip = bu
	bu = self:add_text(			{ix+sx*.4,iy,	  	sx*.05,sy*.75},		":" 		)	--:set_text(":")
		:set_ui_active( false )
	bu = self:add_text(			{ix+sx*.45,iy,		sx*.2166,sy*.75},	"Port nb" 	)
		ui.bu_link_port_nb = bu
	bu = self:add_trig_method(	{ix+sxb*2,iy,		sxb,sy*.75},		"Net_Focus",	self,	"net_focus"				)
		:set_text("Focus"):set_color_back("focus")
	iy = iy + sy*.75

	bu = self:add_button(		{ix,iy,				sxb,sy},			"Active Link",	self,	"b_link_active", false	)
		bu:set_text("Active")
		ui.bu_link_active = bu
	bu = self:add_button(	 	{ix+sxb,iy,			sxb,sy},			"run",			self,	"b_link_run",	false	)
	bu = self:add_text_info(	{ix+sxb*2,iy,		sxb,sy},			"img_sent_nb"	)
		bu:set_text_color( "red" )
		ui.bu_img_sent_nb = bu
	iy = iy + sy
end

function meu:init()
	--self:print_error( "meu:init()" )
	local ref = self.ref

	local bdd = self:get_layer_bdd(1)
	ref.bdd_img_send	= bdd
	ref.process_index	= param.get_ref( bdd, "process_index"	)
	ref.force_compute	= param.get_ref( bdd, "force_compute"	)
	ref.img_src 		= param.get_ref( bdd, "image_src"		)
	ref.img_tmp 		= param.get_ref( bdd, "image_dst_base"	)
	ref.img_dst 		= param.get_ref( bdd, "out_bind_dst"	)
	ref.mono 			= param.get_ref( bdd, "mono"			)	
	ref.compress 		= param.get_ref( bdd, "compress"		)
	ref.link_index		= param.get_ref( bdd, "link_index"		)
	ref.channel_id		= param.get_ref( bdd, "channel_id"		)
	ref.frame_blk_nb	= param.get_ref( bdd, "blk_nb" 			)
	ref.frame_size		= param.get_ref( bdd, "memory_size"		)

	self.link_index_cur = - 42
end

function meu:net_focus()
	local ref = self.ref
	local ui  = self.ui
	if ref.netlink then
		aaa.obj.set_focus_ui( ref.netlink )
	end
end

function meu:get_binds()
	return self:get_texture_bind_2d(1), self:get_texture_bind_2d(2), self:get_texture_bind_2d(3)
end

function meu:update_ui()
	local ref = self.ref
	local ui = self.ui

	local bind_src, bind_tmp, bind_dst = self:get_binds()
	ui.bu_count:set_text( bind_src.."("..param.get(ref.process_index)..")->"..bind_tmp.."-> "..bind_dst )

	ui.bu_frame_info:set_text( param.get(ref.frame_blk_nb).." Blks:\n"..param.get(ref.frame_size).." bytes" )

	local bu = ui.bu_constraint_sel
	local s_constraint = bu:get_value()
	local b_on = s_constraint>1
	bu:set_color_back( b_on and "bu_on" or "bu_off" ) 

	ui.bu_delay:interpolate_alpha_bu( b_on and 1 or .6 )

	if ref.netlink then
		local b_run = param.get_bool( ref.link_running ) and param.get_bool( ref.link_active ) and param.get_bool( ref.link_enable )
		if b_run then
			ui.bu_img_sent_nb:set_text_color( "yellow" )
		else
			ui.bu_img_sent_nb:set_text_color( "red" )
		end
		--if not self.b_link_active then ui.bu_link_sent_nb:set_text_color( "red" ) end
		ui.bu_img_sent_nb:set_text( param.get( ref.img_sent_nb ) )
	end
end

function meu:set_link_index( index )
	param.set( self.ref.link_index, index )
end

function meu:update_netlink()
	local ref = self.ref
	local ui  = self.ui
	local link_index = param.get( ref.link_index )	
	
	if self.link_index_cur ~= link_index then
		self:print_debug( "change netlink to "..link_index )
		ref.netlink	= aaa.net.get_link_ref( link_index )	-- can't do it with param.get_obj_attached(	ref.link_index ) bevause of param ui
		if ref.netlink then
			ref.link_active		= param.get_ref(	ref.netlink, "active"		)		
			ref.link_enable		= param.get_ref(	ref.netlink, "enable"		)
			ref.link_dst_ip		= param.get_ref(	ref.netlink, "dst_ip"		)
			ref.link_port_nb	= param.get_ref(	ref.netlink, "port_nb"		)
			ref.link_sent_nb	= param.get_ref(	ref.netlink, "blk_sent_nb"	)
			ref.link_start_trig	= param.get_ref(	ref.netlink, "start_trig"	)
			ref.link_stop_trig	= param.get_ref(	ref.netlink, "stop_trig"	)
			ref.link_running	= param.get_ref(	ref.netlink, "running"		)
			ref.img_sent_nb		= param.get_ref(	ref.netlink, "img_sent_nb" 	)

			ui.bu_link_active:set_target_param(	    ref.link_active    ) 
			ui.bu_link_dst_ip:set_text(  param.get( ref.link_dst_ip  ) )
			ui.bu_link_port_nb:set_text( param.get( ref.link_port_nb ) )

			self.link_index_cur = link_index
		end
	elseif ref.netlink then
		param.set( ref.link_dst_ip,  ui.bu_link_dst_ip:get_text() )
		param.set( ref.link_port_nb, ui.bu_link_port_nb:get_text())

		local b_run = param.get_bool( ref.link_running )
		--self:print( "b_run "..b_run ) 
		if b_run then
			if not self.b_link_run then
				param.set( ref.link_stop_trig, true )
			end
		else
			if self.b_link_run then
				param.set( ref.link_start_trig, true )
			end
		end
	end
end

function meu:update()
	
	local ref = self.ref
	local ui  = self.ui

	local bind_src, bind_tmp, bind_dst = self:get_binds()
	param.set( ref.img_src, bind_src )
	param.set( ref.img_tmp, bind_tmp )
	param.set( ref.img_dst, bind_dst )

	self:update_netlink()
end


function meu:draw()
	--aaa.print_fn()
	local ref = self.ref
	local ui = self.ui
	local b_draw = true
	local b_force_compute = false
	local bu = ui.bu_constraint_sel
	local s_constraint = bu:get_value()
	if s_constraint>1 then
		local delay = (self.delay or 0) + aaa.time.dt
		if s_constraint==2 then --limit
			if delay > self.interval then
				delay = math.max( delay - self.interval, 0 )
			else	-- we don;t draw
				b_draw = false
			end
		else	--force
			if delay > self.interval then
				delay = 0
				b_force_compute = true
			end
		end
		self.delay = delay
	end

	if b_draw then
		param.set( ref.force_compute, b_force_compute )
		MEU.draw( self )
	end
end
