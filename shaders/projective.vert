# version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 depthMVP;

out vec4 position;
out vec3 normal;
out vec4 ShadowCoord;

void main(){
    gl_Position = projection * modelview * vec4( vertex_position, 1.0f );
    // forward the raw position and normal in the model coord to frag shader
    position = vec4(vertex_position, 1.0f );
    normal = vertex_normal;
    ShadowCoord = depthMVP * vec4(vertex_position,1);
}
