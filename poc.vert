#version 450

layout(push_constant) uniform upc {
  vec2 aspect;
  float time;
};

layout(location = 0) in vec2 pos;
layout(location = 0) out vec2 f_pos;

void main() {
  vec2 p = pos * 2.0 - 1.0;
  gl_Position = vec4(p, 0, 1);

  f_pos = p * aspect;
}
