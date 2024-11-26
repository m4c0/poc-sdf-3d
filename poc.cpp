#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import casein;
import dotz;
import sitime;
import vee;
import voo;

struct upc {
  dotz::vec4 camera { 0, 0, 2, 0 };
  dotz::vec2 aspect;
  float time;
} g_pc;

enum in_button {
  R_LEFT = 0,
  R_RIGHT,
  S_LEFT,
  S_RIGHT,
  W_FRONT,
  W_BACK,
  IN_BTN_MAX,
};
static bool in_state[IN_BTN_MAX] {};
static auto in_down(in_button b) { return [=] { in_state[b] = true; }; }
static auto in_up(in_button b) { return [=] { in_state[b] = false; }; }

static void in_setup_btn(casein::keys k, in_button b) {
  using namespace casein;
  handle(KEY_DOWN, k, in_down(b));
  handle(KEY_UP,   k, in_up  (b));
}

static inline int in_axis(in_button n, in_button p) {
  if (in_state[n] && !in_state[p]) return -1;
  if (!in_state[n] && in_state[p]) return 1;
  return 0;
}
static int in_r_axis() { return in_axis(R_LEFT, R_RIGHT); }
static int in_s_axis() { return in_axis(S_LEFT, S_RIGHT); }
static int in_w_axis() { return in_axis(W_FRONT, W_BACK); }

struct thread : voo::casein_thread {
  thread() : casein_thread {} {
    using namespace casein;
    in_setup_btn(K_LEFT,  R_LEFT);
    in_setup_btn(K_RIGHT, R_RIGHT);

    in_setup_btn(K_A, S_LEFT);
    in_setup_btn(K_D, S_RIGHT);
    in_setup_btn(K_W, W_FRONT);
    in_setup_btn(K_S, W_BACK);
  }
  void run() override {
    sitime::stopwatch t {};
    main_loop("poc", [&](auto & dq, auto & sw) {
      auto pl = vee::create_pipeline_layout({ vee::vert_frag_push_constant_range<upc>() });
      voo::one_quad_render oqr { "poc", &dq, *pl };

      auto aspect = sw.aspect();
      if (aspect > 1.0) {
        g_pc.aspect = { aspect, 1.0f };
      } else {
        g_pc.aspect = { 1.0f, 1.0f / aspect };
      }

      sitime::stopwatch frame {};
      ots_loop(dq, sw, [&](auto cb) {
        g_pc.camera.w += in_r_axis() * frame.millis() / 1000.0f;
        g_pc.time = t.millis() / 1000.0f;
        vee::cmd_push_vert_frag_constants(cb, *pl, &g_pc);
        oqr.run(cb, sw.extent());
        frame = {};
      });
    });
  }
} t;
