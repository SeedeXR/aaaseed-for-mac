aaa.lua.global.declare_table( "mpv" )


function mpv.init_pays()
	mpv.pays = {	"France",	"Monaco",	"Italy", "Great Britain", "Russie", "Espagne", "Germany", "Pays-Bas", "Belgique",
					"USA",
					"Japon",	"China",	"Pakistan", "Inde", "Tibet",
					"Suisse",
					"Australie",
					}
	mpv.pays_nb = table.getn( mpv.pays )
	mpv.ville = {	"Paris",	"Saint-Tropez",	"Meg�ve"	,
					"Monaco",
					"Milan", "Rome",
					"London",
					"Barcelone", "Madrid",
					"Berlin",
					"Amsterdam",
					"Bruxelles",
					"Moscow",	"Saint-P�tersbourg",
					"New York",	"Los Angeles", 	"LA",	"Santa Fe", "Miami", "Aspen",
					"Tokyo",	"Kyoto",
					"P�kin",	"Beijing",	"Shanghai",
					"Karachi",
					"New Delhi",
					"Gen�ve",	"Basel",
					"Sydney",	"Melbourne"
					}
	mpv.ville_nb = #mpv.ville
end

function mpv.init_ref()
	mpv.ref = {}
	local ref = mpv.ref
	ref.bdd_text = aaa.obj.get( "mpv_bdd_text" )
		ref.text = param.get_ref( ref.bdd_text, "text" )

	mpv.init_pays	()
end
if not mpv.ref then mpv.init_ref() end
--mpv.init_ref()
if not mpv.pays then mpv.init_pays() end
--mpv.init_pays()

function mpv.set_pays( i )
	i = i % mpv.pays_nb + 1
	param.set( mpv.ref.text, mpv.pays[i] )
end
function mpv.set_ville( i )
	i = i % mpv.ville_nb + 1
	param.set( mpv.ref.text, mpv.ville[i] )
end
function mpv.set_pv( i )
	i = i % ( mpv.pays_nb + mpv.ville_nb )
	if i < mpv.pays_nb then
		param.set( mpv.ref.text, mpv.pays[i+1] )
	else
		param.set( mpv.ref.text, mpv.ville[i-mpv.pays_nb+1] )
	end
end

local multiple = aaa.get_multiple_cur()
mpv.multiple_last = mpv.multiple_last or -1
if multiple then
	if mpv.multiple_last ~= multiple then
		--	aaa.print( "toto" )
		mpv.multiple_last = multiple
		mpv.ref.multiple_index = param.get_ref( multiple, "index" )
	end
	local	index = param.get( mpv.ref.multiple_index )
	mpv.set_ville( index )
else
	mpv.set_pays( 2 )
end
