#version 430

in vec3 normalInterp;  
in vec3 vertPos;   
in vec3 newLightPos;    

uniform float Kd;   
uniform float Ks;   
void main() {
  vec3 N = normalize(normalInterp);
  vec3 L = normalize(newLightPos - vertPos);

  float lambertian = max(dot(N, L), 0.0);
  float specular = 0.0;
  if(lambertian > 0.0) {
    vec3 R = reflect(-L, N);      
    vec3 V = normalize(-vertPos);
    float specAngle = max(dot(R, V), 0.0);
    specular = pow(specAngle, 5.6);
  }

  gl_FragColor =   vec4(0.5 ,0.8 ,1 ,1 )*vec4((0.5 * vec3(0.2)) +
                      (Kd * lambertian * vec3(0.5)) +
                      (Ks * specular * vec3(0.8)), 1.0);
    
}
