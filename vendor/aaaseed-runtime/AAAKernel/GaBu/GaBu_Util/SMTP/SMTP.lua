CLASS.DECLARE( "SMTP", nil, {
	-- default field values
	--duration = 1.0,
	--time = 0.0,

	--BIND_VIDEO_START = 256,
	SMTP_NB = 16,
	} )
--------------------
--	STATIC


function SMTP.__build_ref( ref, smtp )
	ref.smtp = smtp

	ref.host				=	param.get_ref(	smtp,	"host"					)
	ref.port				=	param.get_ref(	smtp,	"port"					)

	ref.username			=	param.get_ref(	smtp,	"username"				)
	ref.password			=	param.get_ref( 	smtp,	"password"				)

	ref.from				=	param.get_ref( 	smtp,	"from"					)
	ref.to					=	param.get_ref( 	smtp,	"to"					)

	ref.subject				=	param.get_ref(	smtp,	"subject"				)
	ref.body				=	param.get_ref(	smtp,	"body"					)

	ref.attach_use			=	param.get_ref(	smtp,	"attachment_use"		)
	ref.attach_filename		=	param.get_ref(	smtp,	"attachment_filename"	)

	ref.send				=	param.get_ref(	smtp,	"send_trig"				)

end
------------------

function SMTP:init()
	self.host				= ""
	self.port				= 465
	self.username			= ""
	self.password			= ""
	self.from				= ""
	self.to					= ""
	self.subject			= ""
	self.body				= ""
	self.attach_filename	= nil
end

function SMTP:create( i_or_ref )
	local self = SMTP:create_instance_no_name()
--	self.id = i -- was unuse
	self.ref = {}
	local ref = self.ref

	local smtp_ref
	if aaa.obj.is_ref_no_error( i_or_ref ) then
		smtp_ref = i_or_ref
	else
		if not SMTP.smtp_ref then
			local layer = aaa.obj.get_by_name_symbo( "gabu_smtp_layer" )
			SMTP.smtp_ref = aaa.obj.get_branch_by_class( layer, "bdd_smtp" )
		end
		--	create a bdd_smtp object from the one in the layer
		smtp_ref = app:clone_obj( SMTP.smtp_ref )
	end
	SMTP.__build_ref( self.ref, smtp_ref )

	self.cur = self

	self:init()

	return self
end

function SMTP:set_focus()
	aaa.obj.set_focus_ui( self.cur.ref.smtp )
end

--
--	VIDEO TIME
--
function SMTP:set_host( host )				self.host 			= host			end
function SMTP:set_port( port )				self.port 			= port			end
function SMTP:set_username( username )		self.username 		= username		end
function SMTP:set_password( password )		self.password 		= password		end
function SMTP:set_from( from )				self.from 			= from			end
function SMTP:set_to( to )					self.to 			= to			end
function SMTP:set_subject( subject )		self.subject 		= subject		end
function SMTP:set_body( body )				self.body 			= body			end
function SMTP:set_attachment( filename )	self.filename 		= filename		end
function SMTP:set_attachment_use( b_use )	self.b_attach_use 	= b_use			end

function SMTP:set_low()
	local ref = self.cur.ref
	param.set( ref.host, self.host )
	param.set( ref.port, self.port )
	param.set( ref.username, self.username )
	param.set( ref.password, self.password )
	param.set( ref.from, self.from )
	param.set( ref.to, self.to )
	param.set( ref.subject, self.subject )
	param.set( ref.body, self.body )
	if self.filename ~= nil then
		param.set( ref.attach_filename, self.filename )
		param.set( ref.attach_use, self.b_attach_use )
	end
end

function SMTP:do_send()
	self:set_low()
	local ref = self.cur.ref
	param.set( ref.send, 1 )
	self:update()
end

function SMTP:update()
	aaa.obj.update( self.cur.ref.smtp )
end

function SMTP:set( from, to, subject, body, filename )
	-- body
	self:set_to( to )
	self:set_from( from )
	self:set_subject( subject )
	self:set_body( body )
	if filename ~= nil then
		self:set_attachment_use( true )
		self:set_attachment( filename )
	else
		self:set_attachment_use( false )
	end
end

function SMTP:set_server( host, port, user, pass )
	self:set_host( host )
	self:set_port( port )
	self:set_username( user )
	self:set_password( pass )
end

function SMTP:send( from, to, subject, body, filename )
	-- body
	self:set( from, to, subject, body, filename )
	--table.print( self, "self" )
	self:do_send()
end

