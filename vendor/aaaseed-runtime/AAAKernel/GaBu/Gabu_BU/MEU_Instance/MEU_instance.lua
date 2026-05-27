

function MEU:get_inst_last_by_type( meu_type )
	return MEU_CTX.cur.__inst_last_by_type[string.lower(meu_type)]
end


--
-- NAME
--
MEU.doc.get_meu_type = { "() return the meu type as string,", "which is the part before the first underscore." }
function MEU:get_meu_type()				return self.__meu_type					end
MEU.doc.get_meu_type_lower = "() return the meu type as a lowercase string."
function MEU:get_meu_type_lower()		return string.lower( self.__meu_type )	end
MEU.doc.is_meu_type = "( name ) return true if the meu_type is name (case is ignored)."
function MEU:is_meu_type( name )		return string.lower( self.__meu_type ) == string.lower( name )	end

MEU.doc.get_inst_key = { "() return the instance part of the meu name as string,", "which is the part after the first underscore." }
function MEU:get_inst_key()				return self.__inst_key					end	--todo bad name
MEU.doc.get_meu_type_inst_key = "() return in one call MEU:get_meu_type() and MEU:get_inst_key()."
function MEU:get_meu_type_inst_key()	return self.__meu_type, self.__inst_key	end

MEU.doc.get_instances_nb = "() return the number of instances for this MEU type."
function MEU:get_instances_nb()
	local proto = self:get_proto()
	return MEU_CTX.cur:get_instances_nb( proto )
end
MEU.doc.get_instances_array = "() build and return an array with all the instances of this MEU type."
function MEU:get_instances_array()
	local proto = self:get_proto()
	return MEU_CTX.cur:get_instances_array( proto )
end
MEU.doc.apply_instances = { "(  method_name, ... ) apply method using arguments .... to all the instances of this MEU type." }
function MEU:apply_instances( method_name, ... )
	local proto = self:get_proto()
	MEU_CTX.cur:apply_instances( proto, method_name, ... )
end


--
--	PROTO
--
MEU.doc.is_proto_isolated = "() proto isolated means the prototype is not an instance"
function MEU:is_proto_and_isolated()		return self.__inst_key == "proto"		end	--	from a proto dir
function MEU:is_proto()						return self.__proto == self				end
function MEU:get_proto()					return self.__proto						end


--
--	FIND
--
--todo do it by going up and up instead of starting from top
MEU.doc.get_mu_by_name = {
	"( name_or_array ) return the first element with the requested name.",
	"The search start with the MEUs/MUs at the same level (Hold by the same MUS),",
	"  then recursively on branches (folder/MEU_DIR) down the MUS,",
	"  then it try recursively up.",
	"If the argument is an array of string, names are use one by one until a MU is found.",
	"no_error version of these methods don't print or trigger any error, use it for test.",
 }
MEU.doc.get_mu_by_name_no_error		= MEU.doc.get_mu_by_name
MEU.doc.get_meu_by_name				= MEU.doc.get_mu_by_name
MEU.doc.get_meu_by_name_no_error	= MEU.doc.get_mu_by_name
function MEU:get_mu_by_name( name_or_array )			return self:get_mu():__get_mus_up():get_mu_by_name( name_or_array )				end
function MEU:get_mu_by_name_no_error( name_or_array )	return self:get_mu():__get_mus_up():get_mu_by_name_no_error( name_or_array )	end
function MEU:get_meu_by_name( name_or_array )			return self:get_mu():__get_mus_up():get_meu_by_name( name_or_array )			end
function MEU:get_meu_by_name_no_error( name_or_array )	return self:get_mu():__get_mus_up():get_meu_by_name_no_error( name_or_array )	end
MEU.doc.get_meu_by_name_cached = {
	"( name ) return the first meu with the requested name as get_meu_by_name_no_error().",
	"The search start with the MEUs/MUs at the same level (Hold by the same MUS),",
	"  then recursively on branches (folder/MEU_DIR) in the MUS.",
	"  then it try recursively up.",
	"The search is cached, so subsequent call are faster",
	"a second value (boolean)is return when the call trigger the caching.",
	"cached meu are checked to have been freed, in this case the searching is done again.",
}
function MEU:get_meu_by_name_cached( name )
	local cached = self.__meu_cached
	if not cached then
		cached = {}
		self.__meu_cached = cached
	end

	--self:print( "name is "..name )
	name = string.lower( name )
	local m = cached[name]
	if m then return m, false end

	m = self:get_meu_by_name_no_error( name )
	-- get_meu_by_name_no_error() search up now (2025 March)
	-- if not m then	--if not found at the same level or below search from the top
	-- 	m = app:get_meu_by_name_no_error( name )
	-- end

	if m then
		cached[name] = m
		return m, true
	else
		self:show_error( "MEU:get_meu_by_name_cached() Can't find "..name )
	end
end
