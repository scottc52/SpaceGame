uniform sampler2D fbo_texture;
uniform float hit_time;
varying vec2 f_texcoord;
 
void main(void) {
	if(hit_time>2500){
		gl_FragColor = texture2D(fbo_texture, f_texcoord);
	}else{
		float val;
		float peak1 = 300.0;
		float peak2 = 500.0;
		float trough = 2500;
		if(hit_time<peak1){
			val = hit_time/peak1;
		}else if(hit_time< peak2){
			val = 1.0;
		}else{
			val = 1.0 - (hit_time-peak2)/(trough-peak2);
		}
		vec2 texcoord = f_texcoord;
		texcoord.x += sin(texcoord.y * 30*2*3.14159 + hit_time/20.0) / 5.0 * val;
		
		
		vec4 color = texture2D(fbo_texture, texcoord);
		gl_FragColor = color * (1.0-val) + vec4(1.0,1.0,1.0,1.0) *val;
	}
}