#version 450
#extension GL_GOOGLE_include_directive : require
#include "../glslinc/inigo.glsl"

layout(push_constant) uniform upc {
  vec2 aspect;
  float time;
};

layout(location = 0) in vec2 pos;
layout(location = 0) out vec4 colour;

void main() {
  float d = sd_circle(pos, 0.5);
  vec3 c = vec3(1 - step(0, d));;
  colour = vec4(c, 1);
}
