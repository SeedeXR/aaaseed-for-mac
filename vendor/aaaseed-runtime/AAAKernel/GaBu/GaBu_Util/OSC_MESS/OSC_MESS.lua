if CLASS.DECLARE( "OSC_MESS" ) then
	OSC_MESS:set_class_status_doc(	CLASS.STATUS.CORE,
									"receive Open Sound Control messages" )
end

function OSC_MESS:create()
	local self = OSC_MESS:create_instance_no_name()
	--self:init()
	self.off_tag = 0
	self.off_arg = 0
	return self
end
function OSC_MESS:get_messages( start )
	local ms = {}
	repeat
		local tab = aaa.net.osc_take_by_start( start )
		if tab and #tab>0 then
			for i=1,#tab do
				local m = OSC_MESS:create()
				m.tags = tab[i].tags or {}
				m.args = tab[i].args or {}
				table.insert( ms, m )
				--m:dump( i )
			end
		else
			break
		end
	until false
	return ms
end
function OSC_MESS:process_messages( start, fn, str, ... )
	local ms = OSC_MESS:get_messages( start )
	if ms then
		for i=1,#ms do
			fn( ms[i], str, ... )
		end
	end
end
function OSC_MESS:take_messages_and_dump( start, str, ... )
	OSC_MESS:process_messages( start, OSC_MESS.dump, str, ... )
end

function OSC_MESS:dump( str )
	local function concat_tag( str, tab )
		for i=1,#tab-self.off_tag do
			str = str.." "..tab[i+self.off_tag]
		end
		return str
	end
	local function concat_arg( str, tab )
		--self:print( "tab have "..#tab.." "..self.off_arg )  
		for i=1,#tab-self.off_arg do
			if i % 64 == 0 then
				aaa.print( i.." ---------------- "..str )
			end
			str = str.." "..tab[i+self.off_arg].."("..type(tab[i+self.off_arg])..")"
		end
		return str
	end
	--table.print( tags, "tags" )
	--table.print( args, "args" )
	if str then str = "OSC "..str.." : "
	else 		str = "OSC : "
	end
	local str = concat_tag( str, self.tags )
	aaa.print( str )

	if self.args then
		--local str = concat_arg(  str.." --->", self.args  )
		--aaa.print( str )
		local tab = self.args 
		local str = ""
		local id_end = #tab-self.off_arg 
		for i=1,id_end do
			str = str.." "..tab[i+self.off_arg].."("..type(tab[i+self.off_arg])..")"
			if i % 4 == 0 then
				aaa.print( "Arg "..(i-4).."- : "..str )
				str = ""
			end
		end
		if str ~= "" then
			aaa.print( "Arg -"..id_end.." : "..str )
		end
	end
	
end

function OSC_MESS:add_table( dst, src )
	if src then
		if not dst then dst = {} end
		local id = #dst
		for is=1,#src do
			dst[id+is] = src[is]
		end
	end
	return dst
end



function OSC_MESS:get_tag( i )		return self.tags[ (i or 1) + self.off_tag ]	end
function OSC_MESS:get_arg( i )		return self.args[ (i or 1) + self.off_arg ]	end
function OSC_MESS:get_arg_tab()
	local tab = {}
	local args = self.args
	for i=1,#args-self.off_arg do
		tab[i] = args[i+self.off_arg]
	end
	return tab
end

function OSC_MESS:pop_tag()
	local ret = self:get_tag()
	self.off_tag = self.off_tag + 1
	return ret
end
function OSC_MESS:pop_arg()
	local ret = self:get_arg()
	self.off_arg = self.off_arg + 1
	return ret
end