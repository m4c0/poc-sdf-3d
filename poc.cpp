#pragma leco app
#pragma leco add_shader "poc.vert"
#pragma leco add_shader "poc.frag"

import casein;
import dotz;
import sitime;
import vee;
import voo;

struct upc {
  dotz::vec2 aspect;
  float time;
} g_pc;

struct thread : voo::casein_thread {
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
      ots_loop(dq, sw, [&](auto cb) {
        g_pc.time = t.millis() / 1000.0f;
        vee::cmd_push_vert_frag_constants(cb, *pl, &g_pc);
        oqr.run(cb, sw.extent());
      });
    });
  }
} t;
