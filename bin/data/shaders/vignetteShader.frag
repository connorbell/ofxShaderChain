/*
{
"parameters" : [
   {
      "name" : "strength",
      "range" : {
         "x" : 0,
         "y" : 1
      },
      "show" : true,
      "type" : "float",
      "value" : 0.6326530575752258
   },
   {
      "name" : "power",
      "range" : {
         "x" : 0,
         "y" : 1
      },
      "show" : true,
      "type" : "float",
      "value" : 0.5
   }
]
}
*/
#version 150

uniform sampler2D _MainTexture;
uniform vec2 _Resolution;

uniform float strength;
uniform float power;

in vec2 uv;
in vec2 texCoord;

out vec4 outputColor;

void main()
{
    vec2 uv_c = uv * 2. - 1.;
    vec4 c = texture(_MainTexture, texCoord);
    vec4 col = vec4(pow(c.rgb, vec3(power)), 1.0);

    outputColor = mix(vec4(.0,0.,0.,1.0), col, clamp(1. - smoothstep(0.,1., length(uv_c) * strength),0., 1.));
}
