//check also https://www.shadertoy.com/view/tdG3Rw

float DE( vec2 pp, out bool blood, float t )
{
	pp.y += (
		.4 * sin(.5*2.3*pp.x+pp.y) +
		.2 * sin(.5*5.5*pp.x+pp.y) +
		0.1*sin(.5*13.7*pp.x)+
		0.06*sin(.5*23.*pp.x));

	pp += vec2(0.,0.4)*t;

	float thresh = 5.3;

	blood = pp.y > thresh;

	float d = abs(pp.y - thresh);
	// todo use proper implicit dist
	//d /= sqrt(1.+grad*grad);
	return d;
}

vec3 sceneColor( in vec2 pp )
{
	float endTime = 16.;
	float rewind = 2.;
	float t = mod( iTime, endTime+rewind );

	if( t > endTime )
		t = endTime * (1.-(t-endTime)/rewind);

	bool blood;
	float d = DE( pp, blood, t );

	if( !blood )
	{
		// floor. not really happy with this at the moment..
		vec3 floorCol = vec3(.25);

		floorCol = texture( iChannel0, -(vec2(pp.x/20.,pp.y/33.)+vec2(.5,.02)) ).xyz;
		float floori = (floorCol.x+floorCol.y+floorCol.z)/3.;

		floori = smoothstep(0.5,.53,floori) ;
		floori = smoothstep(-5.15,1.,floori);
		floori = 1.-floori;
		floorCol = vec3(floori);

		// ao from blood
		float ao = clamp( smoothstep(0.,.2,d), 0., 1.);
		return mix(1.,sqrt(ao),.25) * floorCol;
	}
	else
	{
		//blood. fake a 3d look
		//height
		float h = clamp( smoothstep(.0,.25,d), 0., 1.);
		h = 4.*pow(h,.2);
		// couldnt get matching results using finite diffs, need to sort this out.
		/*vec2 eps = vec2(0.,.01);
		float dx = clamp( smoothstep(.0,.25,DE(pp+eps.yx,blood,t)), 0., 1.);
		dx = (dx - h)/eps.y;
		float dy = clamp( smoothstep(.0,.25,DE(pp+eps.xy,blood,t)), 0., 1.);
		dy = (dy - h)/eps.y;*/
		//vec3 N = vec3(-dx, 1., -dy );

		// gradient instructions. easy but produces artifacts
		vec3 N = vec3(-dFdx(h), 1., -dFdy(h) );
		N = normalize(N);
		vec3 L = normalize(vec3(.5,.7,-.5));
		float res = pow(dot(N,L),6.);
		// make it more red hack
		//res += vec3(.4,.5,-0.3);

		//res = res * 2. ; //vec3(1.,.8,-0.3);

		return mix( vec3(1.,0.,1.), vec3(1.,.8,0.), 1.-res );
	}
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	uv.x /= iResolution.y/iResolution.x;

	fragColor.a = 1.0;
	fragColor.xyz = sceneColor(uv*4.);
}