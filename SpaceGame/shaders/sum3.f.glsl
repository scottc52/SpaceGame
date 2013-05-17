
uniform sampler2D source;
uniform float coefficients[3];
uniform float offsetx;
uniform float offsety;

varying vec2 f_texcoord;

void main(void)
{
    vec2 tc = f_texcoord;
    vec2 offset = vec2(offsetx, offsety);

	vec4 color;
    color  = coefficients[0] * texture2D(source, tc - offset);
    color += coefficients[1] * texture2D(source, tc);
    color += coefficients[2] * texture2D(source, tc + offset);

    gl_FragColor = color;
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
