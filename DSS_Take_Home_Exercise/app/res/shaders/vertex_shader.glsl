
R"(
#version 330

layout (location=0) in vec3 tilePos;
layout (location=1) in vec2 texCoord;
layout (location=2) in vec4 charPos;

uniform mat4 projection;

out vec2 tc;

uniform mat4 transform;
uniform sampler2D samp;
uniform bool textRender;

void main(void)
{	
	if(!textRender)
	{
		gl_Position = transform * vec4(tilePos, 1.0);
		tc = texCoord;
	}
	else
	{
		gl_Position = projection * vec4(charPos.xy, 0.0, 1.0);
		tc = charPos.zw;
	}
}
)"
