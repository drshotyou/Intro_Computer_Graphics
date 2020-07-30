#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 projection, modelview,lightm;
out vec3 normalInterp;
out vec3 vertPos;  
  
uniform float Kd;   
uniform float Ks;   

uniform vec3 lightPos; 
uniform mat3 normalMat;
out vec4 color;


void main() {  
 //mat4 normalMat = transpose(inverse(modelview));
 vec4 vertPos4 = modelview * vec4(position, 1.0);
  vertPos = vec3(vertPos4);
  normalInterp = vec3(normalMat * normal);
  gl_Position = projection * vertPos4;
 vec4 tempLightPos = lightm * vec4(lightPos,1.0);
  vec3 N = normalize(normalInterp);
  vec3 L = normalize(vec3(tempLightPos) - vertPos);
  float lambertian = max(dot(N, L), 0.0);
  float specular = 0.0;
  if(lambertian > 0.0) {
    vec3 R = reflect(-L, N);    
    vec3 V = normalize(-vertPos); 
    float specAngle = max(dot(R, V), 0.0);
    specular = pow(specAngle, 5.6);
  }
  color = vec4(0.5 ,0.8 ,1 ,1 )*vec4((0.5 * vec3(0.2)) +
                      (Kd * lambertian * vec3(0.5)) +
                      (Ks * specular * vec3(0.8)), 1.0);
}
