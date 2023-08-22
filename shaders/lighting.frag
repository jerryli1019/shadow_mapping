#version 330 core

in vec4 position; 
in vec3 normal;  

uniform mat4 modelview; 
uniform mat4 view;     

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emision;
uniform float shininess;

const int maximal_allowed_lights = 10;
uniform bool enablelighting;
uniform int nlights;
uniform vec4 lightpositions[ maximal_allowed_lights ];
uniform vec4 lightcolors[ maximal_allowed_lights ];

uniform sampler2D depthMap;
in vec4 ShadowCoord;

out vec4 fragColor;


void main (void){
    vec3 shadow_coord;
    float bias;
    if (!enablelighting){
        vec3 N = normalize(normal);
        fragColor = vec4(0.5f*N + 0.5f , 1.0f);
    } else {
        mat4 toWorld = inverse(view) * modelview ;
        vec4 eye = inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
        vec4 v = normalize(eye - toWorld * position);
        vec4 norm = vec4(normal, 1.0f);
        vec4 n = transpose(inverse(toWorld)) * norm;
        n = vec4(normalize(vec3(n[0], n[1], n[2])), 0.0f);
        fragColor = emision;
        for (int i = 0; i < nlights; i++) {
            float visibility = 1.0;
            bias = 0.005;
            shadow_coord = ShadowCoord.xyz * 0.5 + 0.5;
            if ( texture( depthMap, shadow_coord.xy ).r  <  shadow_coord.z - bias){
                visibility = 0.5;
            }
            vec4 l = normalize(lightpositions[i]);
            vec4 h = normalize(v + l);
            fragColor += ((ambient + visibility * diffuse * max(dot(n, l), 0) \
                      + visibility * specular * pow(max(dot(n, h), 0), shininess))) \
                      * lightcolors[i];
        }
    }
}