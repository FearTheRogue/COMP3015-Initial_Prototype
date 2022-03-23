#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTextureCoord;

// values to be passed to frag shader
out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

// defining uniforms
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void getCamSpaceValues(out vec3 n, out vec3 pos) 
{
    n = normalize(NormalMatrix * VertexNormal);
    pos = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
}

void main()
{
    getCamSpaceValues(Normal, Position);

    TexCoord = VertexTextureCoord;

    gl_Position = MVP * vec4(VertexPosition,1.0);
}
