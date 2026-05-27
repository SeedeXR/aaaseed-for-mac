
struct st_light_directional
{
	mat4	light_view_projection;

	vec3	direction;
	float	gamma;
	//float	ao_factor;

	vec3	color;
	int		b_shadow;					//	enable shadows

	vec2	shadow_map_size_over;
	float	shadow_filter_width;
	float	shadow_bias;

	int		b_intensity_map;			//	enable intensity_map
	int		intensity_map_mode;
	float	intensity_map_factor;
	int		dummy;						//OpenGl need 16B padding on struct

	vec2	intensity_map_scale;
	vec2	intensity_map_offset;
};

struct st_light_point
{
	vec3	position;
	float	radius;
	vec3	color;
	float	gamma;
	//float	ao_factor;
};

struct st_light_spot
{
	mat4	light_view_projection;

	vec3	position;
	float	radius;

	vec3	direction;
	float	gamma;
	//float	ao_factor;

	float   field_of_view;
	float   field_of_view_decay;
	vec2    dummy_2;

	vec3	color;
	int		b_shadow;					//	enable shadows

	vec2	shadow_map_size_over;
	float	shadow_filter_width;
	float	shadow_bias;

	int		b_intensity_map;			//	enable intensity_map
	int		intensity_map_mode;
	float	intensity_map_factor;
	int		dummy;						//OpenGl need 16B padding on struct

	vec2	intensity_map_scale;
	vec2	intensity_map_offset;
};


struct st_light_nb
{
    uint directional;
    uint point;
	uint spot;
};

//note, for now we use metallic workflow
//our surface data
struct st_surface
{
	vec3 	world_position;
	float 	roughness;
	vec4 	diffuse;				//vec4 to transfer alpha from diffuse texture
    vec3 	specular;				//For metallic workflow, store in x only
    float 	reflection_intensity;
	vec3 	emissive;
	float 	ambient_intensity;
	vec3 	normal;
	float 	ao_factor;				//1 = full strength, 0 = no AO
};

struct st_material
{
    vec3	emissive;
	float	roughness;
	vec3	diffuse;
	float	ao_factor;
	vec3	specular;
    float	reflection_intensity;
	float	ambient_intensity;
};

#if AAA_GOL_VERSION >= 420
	layout(binding = 13) uniform material_block
	{
		st_material material;
	} aaa_material;
#endif

#define METALLIC_WORKFLOW 0
#define SPECULAR_WORKFLOW 1
#define SHADING_WORKFLOW METALLIC_WORKFLOW


float NormalDistribution_GGX( float a, float NdH )
{
    // Isotropic ggx.
    float a2 = a*a;
    float NdH2 = NdH * NdH;

    float denominator = NdH2 * (a2 - 1.0) + 1.0;
    denominator *= denominator;
    denominator *= PI;

    return a2 / denominator;
}

float Geometric_Shadowing_Smith_Schlick_GGX( float a, float NdV, float NdL )
{
        // Smith schlick-GGX.
    float k = a * 0.5;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);

    return GV * GL;
}

vec3 fresnel_schlick( vec3 specular, vec3 h, vec3 v )
{
    float dvh = dot(v, h);
    dvh = clamp(dvh, 0.0, 1.0);
    return (specular + (1.0 - specular) * pow((1.0 - dvh), 5));
}

vec3 Specular( vec3 specular, vec3 h, vec3 v, vec3 l, float a, float NdL, float NdV, float NdH, float VdH, float LdV )
{
    float normal_dist = NormalDistribution_GGX( a, NdH );
    float gs = Geometric_Shadowing_Smith_Schlick_GGX( a, NdV, NdL );

	vec3 f = fresnel_schlick( specular, h, v );

    return (normal_dist * gs * f) / (4.0 * NdL * NdV + 0.0001);
}

vec3 Diffuse( in vec3 albedo )
{
    return albedo/vec3(PI);
}


vec3 compute_light_get_factor( st_surface surface, in vec3 light_dir, in vec3 view_dir )
{
    vec3 normal = surface.normal;
    float roughness = surface.roughness;

    // Compute some useful values.
    float NdL = saturate( dot(normal, light_dir) );
    float NdV = saturate( dot(normal, view_dir ) );
    vec3 h = normalize( light_dir + view_dir );
    float NdH = saturate( dot(normal, h) );
    float VdH = saturate( dot(view_dir, h) );
    float LdV = saturate( dot(light_dir, view_dir) );
    float a = max( 0.001, roughness * roughness );

    vec3 cDiff = Diffuse( surface.diffuse.rgb );
    vec3 cSpec = Specular( surface.specular, h, view_dir, light_dir, a, NdL, NdV, NdH, VdH, LdV );

    return NdL * (cDiff * (1.0 - cSpec) + cSpec);
	//return vec3(NdV);
}

vec3 compute_light( st_surface surface, in vec3 light_dir, in vec3 view_dir, in vec3 light_color )
{
    vec3 normal = surface.normal;
    float roughness = surface.roughness;

    // Compute some useful values.
    float NdL = saturate( dot(normal, light_dir) );
    float NdV = saturate( dot(normal, view_dir ) );
    vec3 h = normalize( light_dir + view_dir );
    float NdH = saturate( dot(normal, h) );
    float VdH = saturate( dot(view_dir, h) );
    float LdV = saturate( dot(light_dir, view_dir) );
    float a = max( 0.001, roughness * roughness );

    vec3 cDiff = Diffuse( surface.diffuse.rgb );
    vec3 cSpec = Specular( surface.specular, h, view_dir, light_dir, a, NdL, NdV, NdH, VdH, LdV );

    return light_color * NdL * (cDiff * (1.0 - cSpec) + cSpec);
	//return vec3(NdV);
}

void do_surface_material( inout st_surface surface, in st_material material )
{
	surface.diffuse.rgb	*= material.diffuse;
	surface.roughness 	*= material.roughness;

#if SHADING_WORKFLOW == METALLIC_WORKFLOW
	float metallic 		= material.specular.x * surface.specular.x;
	vec3  diffuse		= surface.diffuse.rgb;
    surface.diffuse.rgb	= diffuse * (1. - metallic);
    surface.specular	= mix( vec3(0.03), diffuse, metallic );	 // 0.03 default specular value for dielectric.
#elif SHADING_WORKFLOW == SPECULAR_WORKFLOW
	surface.specular	*= material.specular;
#endif
	surface.emissive	*= material.emissive;

	surface.ao_factor				= material.ao_factor;
	surface.reflection_intensity	= material.reflection_intensity;
	surface.ambient_intensity		= material.ambient_intensity;
}

#	ifndef PCF_BOX_KERNEL_SIZE
#		define PCF_BOX_KERNEL_SIZE 4
const float PCF_NB_FACTOR = 1./float((PCF_BOX_KERNEL_SIZE*2+1) * (PCF_BOX_KERNEL_SIZE*2+1));
#	endif

float shadow_no_filter( in sampler2D shadow_texture, in vec3 projected_tex_coord, in float bias )
{
	float light_depth_value = projected_tex_coord.z - bias;
	float depth_value =  texture( shadow_texture,  projected_tex_coord.xy ).r;
	//return vec3(depthValue);

	if( light_depth_value < depth_value )
		return 1;
	else
		return 0;
}

// HS stand for half size
#define PCF_FACTOR(HS) ( 1. / ( (2*HS+1) * (2*HS+1) ) )
const float pcf_factor[16] = float[16]
(
	PCF_FACTOR(0),		PCF_FACTOR(1),		PCF_FACTOR(2),		PCF_FACTOR(3),
	PCF_FACTOR(4),		PCF_FACTOR(5),		PCF_FACTOR(6),		PCF_FACTOR(7),
	PCF_FACTOR(8),		PCF_FACTOR(9),		PCF_FACTOR(10),		PCF_FACTOR(11),
	PCF_FACTOR(12),		PCF_FACTOR(13),		PCF_FACTOR(14),		PCF_FACTOR(15)
);

float shadow_pcf( in sampler2D shadow_texture, in vec3 projected_tex_coord, in float bias, in vec2 shadow_pixel_size_factor, in int pcf_size )	//in const int pcf_size )
{
	float percent_lit = 0;
	float light_depth_value = projected_tex_coord.z - bias;
	for( int i = -pcf_size; i <= pcf_size; i++ )
	{
		vec2 duv = vec2(i,0) * shadow_pixel_size_factor;
		float depth_value = texture( shadow_texture, projected_tex_coord.xy + duv ).r;
		if( light_depth_value < depth_value )
			percent_lit += 1.;	//todo make it depend on distance

		for( int j = 1; j <= pcf_size; j++ )
		{
			//todo make it depens on distance computed at center ?
			duv = vec2(i,-j) * shadow_pixel_size_factor;
			depth_value = texture( shadow_texture, projected_tex_coord.xy + duv ).r;
			if( light_depth_value < depth_value )
				percent_lit += 1.;	//todo make it depend on distance

			duv = vec2(i,j) * shadow_pixel_size_factor;
			depth_value = texture( shadow_texture, projected_tex_coord.xy + duv ).r;
			if( light_depth_value < depth_value )
				percent_lit += 1.;	//todo make it depend on distance
		}
	}

	return percent_lit * pcf_factor[pcf_size];
}

//	OLD SCHOOL LIGHT OpenGl 1.0/2.0
//

uniform	uint	aaa_lights;		//	8 x 4bits per lights

//	we accumulate the lights there
struct st_ads {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

#if !AAA_IS_AMD
// --------------------------------------------------------------------------
// POINTLIGHT FUNCTION
void pointLight( inout st_ads glo, const in int i, const in vec3 normal, const in vec3 eye, const in vec3 ecPosition3 )
{
	float nDotVP;			// normal . light direction
	float nDotHV;			// normal . light half vector
	float pf;				// power factor
	float attenuation;		// computed attenuation factor
	float d;				// distance from surface to light source
	vec3  VP;				// direction from surface to light position
	vec3  halfVector;		// direction of maximum highlights

	// Compute vector from surface to light position
	VP = vec3 (gl_LightSource[i].position) - ecPosition3;

	// Compute distance between surface and light position
	d = length(VP);

	// Normalize the vector from surface to light position
	VP = normalize(VP);

	// Compute attenuation
	attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
			  gl_LightSource[i].linearAttenuation * d +
			  gl_LightSource[i].quadraticAttenuation * d * d);
	halfVector = normalize(VP + eye);
	nDotVP = max(0.0, dot(normal, VP));
	nDotHV = max(0.0, dot(normal, halfVector));

	if (nDotVP == 0.0)
		pf = 0.0;
	else
		pf = pow(nDotHV, gl_FrontMaterial.shininess);

	glo.ambient		+= gl_LightSource[i].ambient * attenuation;
	glo.diffuse		+= gl_LightSource[i].diffuse * nDotVP * attenuation;
	glo.specular	+= gl_LightSource[i].specular * pf * attenuation;
}

// --------------------------------------------------------------------------
// SPOTLIGHT FUNCTION
void spotLight( inout st_ads glo, const in int i, const in vec3 normal, const in vec3 eye, const in vec3 ecPosition3 )
{
	float nDotVP;            // normal . light direction
	float nDotHV;            // normal . light half vector
	float pf;                // power factor
	float spotDot;           // cosine of angle between spotlight
	float spotAttenuation;   // spotlight attenuation factor
	float attenuation;       // computed attenuation factor
	float d;                 // distance from surface to light source
	vec3  VP;                // direction from surface to light position
	vec3  halfVector;        // direction of maximum highlights

	// Compute vector from surface to light position
	VP = vec3( gl_LightSource[ i ].position ) - ecPosition3;

	// Compute distance between surface and light position
	d = length( VP );

	// Normalize the vector from surface to light position
	VP = normalize( VP );

	// Compute attenuation
	attenuation = 1.0 / ( gl_LightSource[ i ].constantAttenuation + gl_LightSource[ i ].linearAttenuation * d + gl_LightSource[ i ].quadraticAttenuation * d * d );

	// See if point on surface is inside cone of illumination
	spotDot = dot( -VP, normalize( gl_LightSource[ i ].spotDirection ) );

	if( spotDot < gl_LightSource[ i ].spotCosCutoff )
		spotAttenuation = 0.0; // light adds no contribution
	else
		spotAttenuation = pow( spotDot, gl_LightSource[ i ].spotExponent );

	// Combine the spotlight and distance attenuation.
	attenuation *= spotAttenuation;

	halfVector = normalize( VP + eye );

	nDotVP = max( 0.0, dot( normal, VP ) );
	nDotHV = max( 0.0, dot( normal, halfVector ) );

	if( nDotVP == 0.0 )
		pf = 0.0;
	else
		pf = pow( nDotHV, gl_FrontMaterial.shininess );

	glo.ambient		+= gl_LightSource[ i ].ambient * attenuation;
	glo.diffuse		+= gl_LightSource[ i ].diffuse * nDotVP * attenuation;
	glo.specular	+= gl_LightSource[ i ].specular * pf * attenuation;
}

// --------------------------------------------------------------------------
// DLIGHT FUNCTION
void directionalLight( inout st_ads glo, const in int i, const in vec3 normal )
{
	float nDotVP; // normal . light direction
	float nDotHV; // normal . light half vector
	float pf;     // power factor

	nDotVP = max( 0.0, dot( normal, normalize( vec3( gl_LightSource[ i ].position ) ) ) );
	nDotHV = max( 0.0, dot( normal, vec3( gl_LightSource[ i ].halfVector ) ) );

	if( nDotVP == 0.0 )
		pf = 0.0;
	else
		pf = pow( nDotHV, gl_FrontMaterial.shininess );

	glo.ambient		+= gl_LightSource[ i ].ambient;
	glo.diffuse 	+= gl_LightSource[ i ].diffuse * nDotVP;
	glo.specular	+= gl_LightSource[ i ].specular * pf;
}

// --------------------------------------------------------------------------
// FLIGHT FUNCTION
//	compute light
vec4 flight_nb( const in vec3 normal, const in vec3 ecPosition3, const in int light_nb )
{
	vec3 eye = vec3 (0.0, 0.0, 1.0);

	st_ads glo;
	// Clear the light intensity accumulators
	glo.ambient  = vec4(0.0);
	glo.diffuse  = vec4(0.0);
	glo.specular = vec4(0.0);

	//	parse 4 bits by 4 bits ( 8 lights )
	uint lights = aaa_lights;
	for( int i_flight=0; i_flight<light_nb; ++i_flight )
	{
		int one = int(lights) & 0xf;
		if( one!=0 )
		{
			if( one == 1 )
				directionalLight( glo, i_flight, normal );
			else if( one == 2 )
				pointLight( glo, i_flight, normal, eye, ecPosition3 );
			else
				spotLight( glo, i_flight, normal, eye, ecPosition3 );
		}
		lights >>= 4u;
		if( lights==0u )	//	avoid rest of loop
			break;
	}
	//	in fixed path l'alpha est la
	vec4 color;
	color = gl_FrontLightModelProduct.sceneColor;
	//color = vec4( .0,.0,.0, 1 );
	color.rgb	+= glo.ambient.rgb	* gl_FrontMaterial.ambient.rgb;
	color.rgb	+= glo.diffuse.rgb	* gl_FrontMaterial.diffuse.rgb;
	color.rgb	+= glo.specular.rgb	* gl_FrontMaterial.specular.rgb;

	color = clamp( color, 0.0, 1.0 );

//	return vec4( 0,1,0, 1 );
	return color;
}

vec4 flight_nb( const in vec3 normal, const in vec3 ecPosition3, const in int light_nb, const in float alphaFade )
{
	vec4 color = flight_nb( normal, ecPosition3, light_nb );
	color.a *= alphaFade;
//	color.a = 1.0;
	return color;
}
#ifndef	LIGHT_NB	//to provoke redefinition error
#	define LIGHT_NB		8
#endif

vec4 flight( const in vec3 normal, const in vec3 ecPosition3 )
{
	return flight_nb( normal, ecPosition3, LIGHT_NB );
}

vec4 flight( const in vec3 normal, const in vec3 ecPosition3, const in float alphaFade )
{
	return flight_nb( normal, ecPosition3, LIGHT_NB, alphaFade );
}
#endif	//#if  !AAA_IS_AMD

































//end should be x * 100 lines
