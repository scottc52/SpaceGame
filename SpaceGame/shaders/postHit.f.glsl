uniform sampler2D fbo_texture;
uniform float hit_time;
uniform float damage;
varying vec2 f_texcoord;

uniform float tlstart;
uniform float tlend;
uniform float tlfade;
 
void main(void) {
	vec4 color = texture2D(fbo_texture, f_texcoord);
	vec2 texcoord = f_texcoord;
	if(hit_time<2500){
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
		texcoord.x += sin(texcoord.y * 30*2*3.14159 + hit_time/20.0) / 5.0 * val;
		
		
		//color = texture2D(fbo_texture, texcoord);
		color = color * (1.0-val) + vec4(1.0,1.0,1.0,1.0) *val;
	}
	
	//damage saturation
	//float bw = 0.21*color.r + 0.71*color.g + 0.07*color.b;
	float bw = (max(max(color.r, color.g),color.b) + min(min(color.r, color.g),color.b))/2.0;
	color = color * (1.0-damage) + vec4(bw, bw, bw, 1.0) * damage;
			
	// damage vignette
	float vig = sqrt( ( pow((texcoord.x-0.5)/ 0.5,2) + pow((texcoord.y-0.5)/ 0.5,2) )/2 ) * damage * 1.2;
	vig = min(vig, 1.0);
	color = color * (1.0-vig) + vec4(0.5,0.0,0.0,1.0) *vig;
	
	//level fade
	float fade = 0;
	if(tlstart<tlfade){fade += 1.0-tlstart/tlfade;}
	if(tlend<tlfade){fade += tlend/tlfade;}
	color = color * (1.0-fade) + vec4(1.0,1.0,1.0,1.0) *fade;
	
	
	gl_FragColor = color;
}