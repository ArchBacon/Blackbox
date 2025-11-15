#version 330 core

out vec4 outColor;

in vec3 vertexColor;
in vec2 texCoord;

uniform sampler2D texture1;
//uniform sampler2D texture2;

void main()
{
    outColor = texture(texture1, texCoord) * vec4(vertexColor, 1.0f);
//    outColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
}
