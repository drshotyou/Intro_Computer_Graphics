#version 430

in vec2 frag_UV;
in float alpha2;
in float life2;
in int mode2; 
in float particleAlpha2;
uniform float alpha;
uniform int mode;
out vec4 frag_color;

uniform sampler2D ourTexture;
void main() {
	if(mode==0)
	{
		vec4 temp_frag_color = texture(ourTexture,frag_UV);
		temp_frag_color.w = alpha2;
		frag_color = temp_frag_color;
	}  
		else if (mode==1)
	{
		if(life2 > 0.5)
		{
			vec4 temp_frag_color = vec4(0.0,0.0,1.0,1.0);\
			temp_frag_color.w = particleAlpha2;
			frag_color = temp_frag_color;
		}
		else if(life2 > 0.3 && life2 < 0.5)
		{
			vec4 temp_frag_color = vec4(0.0,1.0,0.5,1.0);
			temp_frag_color.w = particleAlpha2;
			frag_color = temp_frag_color;
		} else {
			frag_color = vec4(0.0,1.0,0.5,0.0);
		}

	} else if(mode==2) {
		if(life2 > 0.5)
		{
			vec4 temp_frag_color = vec4(0.98,0.83,0.25,1.0);\
			//temp_frag_color.w = particleAlpha2;
			frag_color = temp_frag_color;
		}
		else if (life2 > 0.3 && life2 < 0.5)
		{
			vec4 temp_frag_color = vec4(1.0,0.23,0.25,1.0);\
			//temp_frag_color.w = particleAlpha2;
			frag_color = temp_frag_color;
		}
	}
		

	
  //frag_color = vec4(1.0, 0.5, 0.5, alpha2);
}
