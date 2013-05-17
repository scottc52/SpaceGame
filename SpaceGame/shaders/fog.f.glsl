
uniform float far_planef;
uniform float near_planef;
uniform float offsetf;
uniform float scalef;
uniform float powf;
uniform float minf;
uniform float maxf;
uniform vec4 color;
uniform sampler2D texturemap;
uniform sampler2D depth;

varying vec2 f_texcoord;
 
void main(void) 
{
	float depthval = (2 * near_planef) / (far_planef + near_planef - texture2D(depth, f_texcoord).r * (far_planef - near_planef));
	float worldDepth = near_planef + depthval * (far_planef-near_planef);
	vec4 base = texture2D(texturemap, f_texcoord);
	
	
	float val = pow(max(depthval - offsetf, 0.0), powf) / scalef;
	//float val = pow(max(worldDepth - offsetf, 0.0), powf) / scalef;
	val = min(max(val, minf), maxf);
	
	
	gl_FragColor = base * (1.0-val) + color *val;
	
	/*
	if(depthval > 0.010){//texture2D(depth, f_texcoord).g == texture2D(depth, f_texcoord).b){
		//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	*/

}
