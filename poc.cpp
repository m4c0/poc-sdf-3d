#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import casein;
import dotz;
import input;
import sitime;
import vee;
import voo;

struct upc {
  dotz::vec4 camera { 0, 0, 3, 0 };
  dotz::vec2 aspect;
  float time;
} g_pc;

static void update_camera(float ms) {
  in_update_camera(g_pc.camera, ms);
}

struct thread : voo::casein_thread {
  thread() : casein_thread {} {
    in_setup();
  }
  void run() override {
    sitime::stopwatch t {};
    main_loop("poc", [&](auto & dq, auto & sw) {
      auto pl = vee::create_pipeline_layout({ vee::vert_frag_push_constant_range<upc>() });
      voo::one_quad_render oqr { "poc", &dq, *pl };

      // Flipping Y aspect to keep positive Y on top
      auto aspect = sw.aspect();
      if (aspect > 1.0) {
        g_pc.aspect = { aspect, -1.0f };
      } else {
        g_pc.aspect = { 1.0f, -1.0f / aspect };
      }

      sitime::stopwatch frame {};
      ots_loop(dq, sw, [&](auto cb) {
        update_camera(frame.millis());
        frame = {};

        g_pc.time = t.millis() / 1000.0f;
        vee::cmd_push_vert_frag_constants(cb, *pl, &g_pc);
        oqr.run(cb, sw.extent());
      });
    });
  }
} t;
