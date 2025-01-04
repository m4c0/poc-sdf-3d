export module input;
import casein;
import dotz;

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
static int in_w_axis() { return in_axis(W_BACK, W_FRONT); }

export void in_update_camera(dotz::vec4 & c, float ms) {
  auto a = c.w -= in_r_axis() * ms / 1000.0f;
  auto s = in_s_axis() * ms / 1000.0f;
  auto w = in_w_axis() * ms / 1000.0f;
  c.x += s * dotz::cos(a) - w * dotz::sin(a);
  c.z -= s * dotz::sin(a) + w * dotz::cos(a);
}

export void in_setup() {
  using namespace casein;
  in_setup_btn(K_LEFT,  R_LEFT);
  in_setup_btn(K_RIGHT, R_RIGHT);

  in_setup_btn(K_A, S_LEFT);
  in_setup_btn(K_D, S_RIGHT);
  in_setup_btn(K_W, W_FRONT);
  in_setup_btn(K_S, W_BACK);
}
