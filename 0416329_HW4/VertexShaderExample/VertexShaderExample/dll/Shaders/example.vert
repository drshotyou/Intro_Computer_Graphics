#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 positionParticle;
layout(location = 3) in float life;
layout(location = 4) in vec3 positionParticle2;
layout(location = 5) in float lifeTwo;

out vec2 frag_UV;
out float alpha2;
out float life2;
out int mode2;
out float particleAlpha2;
uniform float alpha;
uniform mat4 Projection,rotation;
uniform mat4 ModelView;
uniform int mode;
uniform float particleAlpha;
uniform float time;
//out int modeOut;

void main() {  
    if (mode==0)
    {
        gl_Position = Projection * ModelView * vec4(position, 1.0);
        life2=life;
    } else if (mode==1) {
        gl_Position = Projection * ModelView * vec4(positionParticle,1.0);
        life2=life;
    } else if(mode==2) {
         gl_Position = Projection * ModelView * vec4(positionParticle2,1.0);
         life2=lifeTwo;
    }
    mode2=mode;
    alpha2=alpha;
    particleAlpha2=particleAlpha;
    frag_UV = texcoord;
    
 // modeOut = mode;
}
