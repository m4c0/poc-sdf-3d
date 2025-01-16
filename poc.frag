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
  float circle = length(p) - 0.35;

  vec3 q = abs(p) - 0.3;
  float box = length(max(q, 0)) + min(max(q.x, max(q.y, q.z)), 0.0);

  return max(box, circle);
}

vec2 raymarch(vec3 ro, vec3 rd) {
  float t = 0;
  float x = 0;
  for (int i = 0; i < 256; i++) {
    float h = sdf(ro + rd * t);
    if (h < 0.001) return vec2(t, i);
    if (t > max_t) break;
    x = i;
    t += h;
  }
  return vec2(max_t, x);
}

vec3 normal(vec3 pos) {
  const vec2 e = vec2(1.0, -1.0);
  const float eps = 0.0001;
  return normalize(
    e.xyy * sdf(pos + e.xyy * eps) + 
    e.yyx * sdf(pos + e.yyx * eps) + 
    e.yxy * sdf(pos + e.yxy * eps) + 
    e.xxx * sdf(pos + e.xxx * eps));
}

void main() {
  const float fl = 1.0; // focal length

  vec3 ro = camera.xyz;
  vec3 rd = normalize(vec3(pos * fl, -1));

  float a = camera.w;
  mat2 cam = mat2(cos(a), -sin(a), sin(a), cos(a));
  rd.xz = cam * rd.xz;

  vec3 c = vec3(0);
  vec2 ti = raymarch(ro, rd);
  float t = ti.x;
  if (t < max_t) {
    vec3 pos = ro + rd * t;
    vec3 nor = normal(pos);

    vec3 light = vec3(0.5773);
    float sp_pwr = 20;

    float spec = pow(dot(nor, normalize((light + ro) / 2)), sp_pwr);
    float dif = clamp(dot(nor, light), 0.0, 1.0);
    float amb = 0.5 + 0.5 * dot(nor, vec3(0.0, 1.0, 0.0));
    c = vec3(0.3, 0.03, 0.02) * amb
      + vec3(0.8, 0.1, 0.07) * dif * spec;
  } else {
    c = mix(vec3(0.15), vec3(0.1), step(0, pos.y));
  }

  colour = vec4(c, 1);
  // colour = vec4(vec3(ti.y) / 256.0, 1); // iteration debug
  // colour = vec4(vec3(clamp(t, 0, 1)), 1); // depth
}
