uniform sampler2D source;
uniform float coefficients[5];
uniform float offsetx1;
uniform float offsety1;
uniform float offsetx2;
uniform float offsety2;

varying vec2 f_texcoord;

void main(void)
{
    vec2 tc = f_texcoord;
    vec2 offset1 = vec2(offsetx1, offsety1);
	vec2 offset2 = vec2(offsetx2, offsety2);

	vec4 color;
    color  = coefficients[0] * texture2D(source, tc - offset2);
    color += coefficients[1] * texture2D(source, tc - offset1);
    color += coefficients[2] * texture2D(source, tc);
	color += coefficients[3] * texture2D(source, tc + offset1);
	color += coefficients[4] * texture2D(source, tc + offset2);
	
    gl_FragColor = color;
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
