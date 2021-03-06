/*
{
"parameters" : [
	   {
		  "name" : "offsetVal",
		  "range" : {
			 "x" : 0,
			 "y" : 40
		  },
		  "show" : true,
		  "type" : "float",
		  "value" : 30
      }
	]
}
*/
#version 150
#pragma include "includes/hue.glsl"

uniform sampler2D _MainTexture;
uniform vec2 _Resolution;

uniform float offsetVal;

uniform vec4 bgColor;
uniform vec4 letterColor;
uniform float _Time;

in vec2 uv;
in vec2 texCoord;

out vec4 outputColor;

float box(vec2 uv, vec2 s) {
    vec2 q = abs(uv) - s;
    return length(max(q,0.0)) + min(max(q.x,q.y),0.0);
}

void main()
{
    const int iter = 8;

    // Add all of the samples to a total, which you divide by the number of samples later to compute the average
    vec3 result = vec3(0.);

    // This is the radius of the circle
    float taper = texture(_MainTexture, texCoord).a;
	vec2 texel = vec2(1.)/_Resolution;

    vec2 uv_c = uv * 2.0 - 1.0;
    for (int i = 0; i < iter; i++) {

        // Compute the angle of the current circle point
        float angle = float(i)/float(iter)*2.*3.14159;

        // Calculate the offset
        vec2 offs = vec2(cos(angle + _Time), sin(angle+_Time))*offsetVal*texel;

	    offs = mix(offs, normalize(uv_c)*0.05, vec2(clamp(1.-taper,0., 1.)));

        // Sample the texture and hue shift it based on the angle around the circle
        vec3 col1 = texture(_MainTexture, texCoord + offs.xy).rgb;
        result += hueShift(vec3(0.,col1.g,col1.b), angle)*8.;
    }

    vec3 base = result/float(iter) ;
    vec4 col = vec4(base , 1.0);
    //col = vec4(vec3(taper) , 1.0);

    //col.rgb = pow(col.rgb, vec3(.5));
    outputColor = col;
}
