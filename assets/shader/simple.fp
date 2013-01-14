#version 330
in vec3 v_normal_world;

out vec4 out_color;

void main(void)
{
  // set biased normal color as output
  out_color = vec4((v_normal_world+1.0f)*0.5f, 1.0f);
}
