#version 450
#extension GL_GOOGLE_include_directive : require
#include "../glslinc/inigo.glsl"

layout(push_constant) uniform upc {
  vec4 camera;
  vec2 aspect;
  float time;
};

layout(location = 0) in vec2 pos;
layout(location = 0) out vec4 colour;

const float max_t = 100;

float sdf(vec3 p) {
  return length(p) - 0.3;
}

mat4 cam_matrix() {
  float rot = camera.w;

  const vec3 v_up = vec3(0, 1, 0);
  vec3 v_right = vec3(sin(rot), 0, cos(rot));
  vec3 v_front = normalize(cross(v_up, v_right));
  return mat4(
    vec4(v_right, 0),
    vec4(v_up, 0),
    vec4(v_front, 0),
    vec4(0, 0, 0, 1));
}

float raymarch(vec3 ro, vec3 rd) {
  float t = 0;
  for (int i = 0; i < 256; i++) {
    float h = sdf(ro + rd * t);
    if (h < 0.001) return t;
    if (t > max_t) break;
    t += h;
  }
  return max_t;
}

vec3 normal(vec3 pos) {
  const vec2 e = vec2(1.0, -1.0) * 0.5773; // 0.5773 ~= sqrt(3)/3
  const float eps = 0.0005;
  return normalize(
    e.xyy * sdf(pos + e.xyy * eps) + 
    e.yyx * sdf(pos + e.yyx * eps) + 
    e.yxy * sdf(pos + e.yxy * eps) + 
    e.xxx * sdf(pos + e.xxx * eps));
}

void main() {
  const float fl = 1.0; // focal length
  vec3 ro = camera.xyz;
  mat4 mat = cam_matrix();
  vec3 rd = (mat * vec4(pos, fl, 1)).xyz;

  vec3 c = vec3(0);
  float t = raymarch(ro, rd);
  if (t < max_t) {
    vec3 pos = ro + rd * t;
    vec3 nor = normal(pos);

    float dif = clamp(dot(nor, vec3(0.5773)), 0.0, 1.0);
    float amb = 0.5 + 0.5 * dot(nor, vec3(0.0, 1.0, 0.0));
    c = vec3(0.3, 0.03, 0.02) * amb + vec3(0.8, 0.1, 0.07) * dif;
  } else {
    c = mix(vec3(0.15), vec3(0.1), step(0, pos.y));
  }

  colour = vec4(c, 1);
}
