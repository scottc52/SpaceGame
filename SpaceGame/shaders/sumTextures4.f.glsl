
uniform sampler2D sourceBase;

uniform sampler2D source0;
uniform sampler2D source1;
uniform sampler2D source2;
uniform sampler2D source3;

varying vec2 f_texcoord;

void main(void)
{
	vec4 tb = texture2D(sourceBase, f_texcoord);
    vec4 t0 = texture2D(source0, f_texcoord);
    vec4 t1 = texture2D(source1, f_texcoord);
    vec4 t2 = texture2D(source2, f_texcoord);
    vec4 t3 = texture2D(source3, f_texcoord);
    gl_FragColor = tb + t0 + t1 + t2 + t3;
	//gl_FragColor = t3;
}
