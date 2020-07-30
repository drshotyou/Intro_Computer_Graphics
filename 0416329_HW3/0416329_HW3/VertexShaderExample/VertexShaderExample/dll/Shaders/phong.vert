#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 lightm;
uniform mat4 projection, modelview;
uniform mat3 normalMat;
uniform vec3 lightPos;
out vec3 normalInterp;
out vec3 vertPos;  
out vec3 newLightPos;



out vec4 color;


void main() {  
 //mat4 normalMat = transpose(inverse(modelview));
 vec4 vertPos4 = modelview * vec4(position,1.0);
 vertPos = vec3(vertPos4);
 normalInterp = vec3(normalMat * normal);
 vec4 tempLightPos = (lightm * vec4(lightPos,1.0));
 newLightPos = vec3(tempLightPos);
 gl_Position = projection * vertPos4;
}
