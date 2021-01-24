R"(
#version 330

in vec2 tc;
out vec4 color;

uniform sampler2D samp;
uniform vec3 textColor;
uniform bool textRender;

void main(void)
{	
  if(!textRender)
  {
	color = texture(samp, tc);
  }
  else
  {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(samp, tc).r);
    color = vec4(textColor, 1.0) * sampled;
  }
}
)"