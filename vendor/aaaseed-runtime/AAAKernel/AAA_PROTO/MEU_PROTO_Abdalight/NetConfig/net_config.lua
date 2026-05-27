function meu:define_meu_infos()
    return {
        author = "Abdalight", date = "2025",
        tags = { "Core", "Device", "Unfinished" },
        help = { "A module to configure network parameters, including IP, port, and connection status." }
    }
end

function meu:define_ui()
    local ref = self.ref
    local ui  = self.ui
    local ix, iy = 1, 1
    local sx, sy = 8, 1
    local dy = 0.2
    local bu

    bu = self:add_trig_method(  {ix+1,iy,            3,sy},     "Net Global Focus",     self,   "net_global_focus"      )
        bu:set_text("Focus")
        bu:set_color_back("focus")
    iy = iy + sy

    bu = self:add_button(       {ix,iy,         0.5*sx,sy},     "ActiveA",              self,   "b_net_active", false   )
        bu:set_text("Active")
        bu:set_target_param( ref.net_global_active )
        ui.bu_link_active = bu
    iy = iy + sy + dy


    bu = self:add_text(			{ix,iy,				sx,sy},		"Host Ip 0" 	)
        bu:set_text( param.get( ref.net_host_ip_0 ) )
		ui.bu_net_host_ip_0 = bu
    iy = iy + sy

    bu = self:add_text(			{ix,iy,				sx,sy},		"Host Ip 1" 	)
        bu:set_text( param.get( ref.net_host_ip_1 ) )
		ui.bu_net_host_ip_1 = bu
    iy = iy + sy

    bu = self:add_text(			{ix,iy,				sx,sy},		"Host Ip 2" 	)
        bu:set_text( param.get( ref.net_host_ip_2 ) )
		ui.bu_net_host_ip_2 = bu
    iy = iy + sy

    bu = self:add_text(			{ix,iy,				sx,sy},		"Host Ip 3" 	)
        bu:set_text( param.get( ref.net_host_ip_3 ) )
		ui.bu_net_host_ip_3 = bu
    iy = iy + sy + dy

    bu = self:add_button(       {ix,iy,         0.5*sx,sy},     "test",              self,   "b_test", false   )
        bu:set_text("yoyo")
    iy = iy + sy + dy

    ix, iy = 9, 1

    bu = self:add_slider(      {ix,iy,          0.5*sx,sy},    "Link Index",    self,   "link_index",       3,          0,16  )
        bu:set_value_type_integer( true )
        bu:add_values_def( 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 )
        ui.bu_link_index = bu
    iy = iy + sy

    self:add_network_link(  ix,iy,				sx,sy*1.75 )

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


function meu:net_global_focus()
	local ref = self.ref
	if ref.global_net then
        aaa.obj.set_focus_ui( ref.global_net )
	end
end

function meu:net_focus()
	local ref = self.ref
	if ref.netlink then
		aaa.obj.set_focus_ui( ref.netlink )
	end
end



function meu:init()
    local ref = self.ref
    local pgr = param.get_ref
    
    ref.global_net = aaa.obj.get_from_top_by_class( "net" )

    if ref.global_net then
        ref.net_global_active = pgr( ref.global_net,    "active"       )
        ref.net_host_ip_0     = pgr( ref.global_net,    "host_ip_0"    )
        ref.net_host_ip_1     = pgr( ref.global_net,    "host_ip_1"    )
        ref.net_host_ip_2     = pgr( ref.global_net,    "host_ip_2"    )
        ref.net_host_ip_3     = pgr( ref.global_net,    "host_ip_3"    )
    end
    self.link_index_cur = - 42
end

function meu:update_ui()
    local ref = self.ref
    local ui = self.ui

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


function meu:update_netlink()
	local ref = self.ref
	local ui  = self.ui
	local link_index = self.link_index --param.get( ref.link_index )
	
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
				param.set( ref.link_stop_trig,  true )
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
    local ps = param.set
    local pg = param.get

    --[[if ref.global_net then
        ps(  ref.net_host_ip_0, pg( ref.net_host_ip_0 ) )
        ps(  ref.net_host_ip_1, pg( ref.net_host_ip_1 ) )
        ps(  ref.net_host_ip_2, pg( ref.net_host_ip_2 ) )
        ps(  ref.net_host_ip_3, pg( ref.net_host_ip_3 ) )
    end]]
    
    self:update_netlink()
    if ref.netlink then
        ps(  ref.link_dst_ip,   pg( ref.link_dst_ip  ) )
        ps(  ref.link_port_nb,  pg( ref.link_port_nb ) )
    end
end
