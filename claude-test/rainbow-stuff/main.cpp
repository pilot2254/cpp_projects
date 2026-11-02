// console_demo.cpp - True-color demoscene-style console animation (Windows 10+)
//
// Build (MSVC):   cl /O2 /EHsc /std:c++17 console_demo.cpp
// Build (MinGW):  g++ -O2 -std=c++17 console_demo.cpp -o console_demo.exe
//
// Controls:  SPACE = next scene,  ESC / Q = quit
// Tip: run in Windows Terminal, maximize it and shrink the font (Ctrl + -)
//      for a much higher "resolution".

#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif

static const float PI  = 3.14159265358979f;
static const float TAU = 6.28318530717958f;

struct RGB { uint8_t r, g, b; };
struct F3  { float r, g, b; };

static inline bool operator!=(RGB a, RGB b) { return a.r != b.r || a.g != b.g || a.b != b.b; }
static inline uint8_t clamp8(float v) { return (uint8_t)std::clamp(v * 255.f, 0.f, 255.f); }
static inline RGB toRGB(F3 c) { return { clamp8(c.r), clamp8(c.g), clamp8(c.b) }; }

// Inigo Quilez style cosine palette
static inline F3 pal(float t, F3 d) {
    return { 0.5f + 0.5f * cosf(TAU * (t + d.r)),
             0.5f + 0.5f * cosf(TAU * (t + d.g)),
             0.5f + 0.5f * cosf(TAU * (t + d.b)) };
}

static inline uint32_t hash32(uint32_t x) {
    x ^= x >> 16; x *= 0x7feb352du;
    x ^= x >> 15; x *= 0x846ca68bu;
    x ^= x >> 16; return x;
}

static std::atomic<bool> g_running{ true };
static BOOL WINAPI CtrlHandler(DWORD) { g_running = false; return TRUE; }

// ---------------------------------------------------------------- scenes ----

using Buffer = std::vector<RGB>;

// 1) Classic plasma
static void scenePlasma(Buffer& buf, int W, int PH, float t) {
    const F3 d{ 0.00f, 0.33f, 0.67f };
    for (int y = 0; y < PH; ++y) {
        float v = (y - PH * 0.5f) / PH;
        for (int x = 0; x < W; ++x) {
            float u  = (x - W * 0.5f) / PH;
            float v1 = sinf(u * 10.f + t);
            float v2 = sinf(10.f * (u * sinf(t * 0.5f) + v * cosf(t * 0.33f)) + t);
            float cx = u + 0.5f * sinf(t * 0.2f);
            float cy = v + 0.5f * cosf(t * 0.33f);
            float v3 = sinf(sqrtf(100.f * (cx * cx + cy * cy) + 1.f) + t);
            float val = (v1 + v2 + v3) / 3.f;
            buf[y * W + x] = toRGB(pal(val * 0.5f + t * 0.05f, d));
        }
    }
}

// 2) Infinite twisting tunnel
static void sceneTunnel(Buffer& buf, int W, int PH, float t) {
    const F3 d{ 0.80f, 0.90f, 0.30f };
    float ox = 0.30f * sinf(t * 0.6f);
    float oy = 0.20f * cosf(t * 0.8f);
    for (int y = 0; y < PH; ++y) {
        for (int x = 0; x < W; ++x) {
            float px = (x - W * 0.5f) / PH - ox;
            float py = (y - PH * 0.5f) / PH - oy;
            float r  = sqrtf(px * px + py * py) + 0.0001f;
            float a  = atan2f(py, px) / TAU;           // -0.5 .. 0.5
            float depth = 0.35f / r + t * 1.5f;
            float ang   = a + depth * 0.04f + t * 0.05f; // twist
            int checker = ((int)floorf(depth * 2.f) + (int)floorf(ang * 16.f)) & 1;
            F3 c = pal(depth * 0.08f + ang, d);
            float shade = std::min(1.f, r * 1.6f);       // far = dark
            float k = shade * (0.55f + 0.45f * checker);
            buf[y * W + x] = toRGB({ c.r * k, c.g * k, c.b * k });
        }
    }
}

// 3) Lit, z-buffered rotating torus over a twinkling starfield
static void sceneTorus(Buffer& buf, int W, int PH, float t) {
    static std::vector<float> zbuf, cosP, sinP;
    const float phiStep = 0.005f, thetaStep = 0.015f;
    if (cosP.empty()) {
        for (float p = 0; p < TAU; p += phiStep) { cosP.push_back(cosf(p)); sinP.push_back(sinf(p)); }
    }
    zbuf.assign((size_t)W * PH, 0.f);

    // background
    for (int y = 0; y < PH; ++y) {
        float g = (float)y / PH;
        for (int x = 0; x < W; ++x) {
            F3 c{ 0.02f + 0.05f * g, 0.01f + 0.03f * g, 0.06f + 0.12f * g };
            uint32_t h = hash32((uint32_t)x * 73856093u ^ (uint32_t)y * 19349663u);
            if (h % 450 == 0) {
                float tw = 0.5f + 0.5f * sinf(t * 3.f + (h >> 8) % 628 / 100.f);
                c.r += tw * 0.9f; c.g += tw * 0.9f; c.b += tw;
            }
            buf[y * W + x] = toRGB(c);
        }
    }

    const float R1 = 1.f, R2 = 2.f, K2 = 5.f;
    const float K1 = std::min(W, PH) * K2 * 3.f / (8.f * (R1 + R2));
    const float A = t * 0.9f, B = t * 0.55f;
    const float cA = cosf(A), sA = sinf(A), cB = cosf(B), sB = sinf(B);
    const F3 d{ 0.00f, 0.10f, 0.20f };

    for (float th = 0; th < TAU; th += thetaStep) {
        float ct = cosf(th), st = sinf(th);
        float cx = R2 + R1 * ct, cy = R1 * st;
        for (size_t i = 0; i < cosP.size(); ++i) {
            float cp = cosP[i], sp = sinP[i];
            float x = cx * (cB * cp + sA * sB * sp) - cy * cA * sB;
            float y = cx * (sB * cp - sA * cB * sp) + cy * cA * cB;
            float z = K2 + cA * cx * sp + cy * sA;
            float ooz = 1.f / z;
            int xp = (int)(W * 0.5f + K1 * ooz * x);
            int yp = (int)(PH * 0.5f - K1 * ooz * y);
            if (xp < 0 || xp >= W || yp < 0 || yp >= PH) continue;
            size_t idx = (size_t)yp * W + xp;
            if (ooz <= zbuf[idx]) continue;
            zbuf[idx] = ooz;

            float L = cp * ct * sB - cA * ct * sp - sA * st
                    + cB * (cA * st - ct * sA * sp);
            float diff = std::max(0.f, L / 1.41421f);
            float spec = powf(diff, 24.f);
            F3 c = pal(i * phiStep / TAU * 2.f + th / TAU * 0.5f + t * 0.08f, d);
            float k = 0.12f + 0.88f * diff;
            buf[idx] = toRGB({ c.r * k + spec * 0.9f, c.g * k + spec * 0.9f, c.b * k + spec * 0.9f });
        }
    }
}

struct Scene { const char* name; void (*render)(Buffer&, int, int, float); };
static const Scene kScenes[] = {
    { "PLASMA", scenePlasma },
    { "TUNNEL", sceneTunnel },
    { "TORUS",  sceneTorus  },
};
static const int   kSceneCount   = sizeof(kScenes) / sizeof(kScenes[0]);
static const float kSceneSeconds = 12.f;
static const float kFadeSeconds  = 0.8f;

// --------------------------------------------------------------- output -----

static inline void appendNum(std::string& s, int v) {
    char tmp[12]; int n = 0;
    if (v == 0) { s.push_back('0'); return; }
    while (v > 0) { tmp[n++] = char('0' + v % 10); v /= 10; }
    while (n) s.push_back(tmp[--n]);
}

static inline void appendColor(std::string& s, bool fg, RGB c) {
    s += fg ? "\x1b[38;2;" : "\x1b[48;2;";
    appendNum(s, c.r); s.push_back(';');
    appendNum(s, c.g); s.push_back(';');
    appendNum(s, c.b); s.push_back('m');
}

static void writeOut(HANDLE h, const std::string& s) {
    DWORD written = 0;
    WriteConsoleA(h, s.data(), (DWORD)s.size(), &written, nullptr);
}

int main() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD oldMode = 0;
    GetConsoleMode(hOut, &oldMode);
    if (!SetConsoleMode(hOut, oldMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN)) {
        std::printf("This console doesn't support VT sequences (needs Windows 10+).\n");
        return 1;
    }
    UINT oldCP = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCtrlHandler(CtrlHandler, TRUE);

    // alternate screen, hide cursor, clear
    writeOut(hOut, "\x1b[?1049h\x1b[?25l\x1b[2J");

    Buffer buf;
    std::string out;
    int W = 0, H = 0;
    int scene = 0;
    float fps = 60.f;

    using clock = std::chrono::steady_clock;
    auto start      = clock::now();
    auto sceneStart = start;
    auto lastFrame  = start;

    while (g_running) {
        // input
        while (_kbhit()) {
            int k = _getch();
            if (k == 27 || k == 'q' || k == 'Q') g_running = false;
            else if (k == ' ') { scene = (scene + 1) % kSceneCount; sceneStart = clock::now(); }
            else if (k == 0 || k == 224) _getch(); // swallow arrow/function keys
        }
        if (!g_running) break;

        // size
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        int nw = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int nh = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        if (nw != W || nh != H) {
            W = std::max(nw, 1); H = std::max(nh, 1);
            buf.assign((size_t)W * H * 2, RGB{ 0, 0, 0 });
            writeOut(hOut, "\x1b[0m\x1b[2J");
        }
        const int PH = H * 2; // two pixels per character cell

        // timing
        auto now = clock::now();
        float t      = std::chrono::duration<float>(now - start).count();
        float tScene = std::chrono::duration<float>(now - sceneStart).count();
        float dt     = std::chrono::duration<float>(now - lastFrame).count();
        lastFrame = now;
        if (dt > 0.f) fps = fps * 0.95f + (1.f / dt) * 0.05f;
        if (tScene >= kSceneSeconds) {
            scene = (scene + 1) % kSceneCount;
            sceneStart = now; tScene = 0.f;
        }

        // render
        kScenes[scene].render(buf, W, PH, t);

        float fade = std::min({ 1.f, tScene / kFadeSeconds, (kSceneSeconds - tScene) / kFadeSeconds });
        if (fade < 1.f) {
            fade = std::max(fade, 0.f);
            for (auto& p : buf) {
                p.r = (uint8_t)(p.r * fade); p.g = (uint8_t)(p.g * fade); p.b = (uint8_t)(p.b * fade);
            }
        }

        // encode frame: upper-half-block, fg = top pixel, bg = bottom pixel
        out.clear();
        out.reserve((size_t)W * H * 24);
        RGB lastF{}, lastB{};
        bool first = true;
        for (int row = 0; row < H; ++row) {
            out += "\x1b["; appendNum(out, row + 1); out += ";1H";
            const RGB* top = &buf[(size_t)(row * 2) * W];
            const RGB* bot = &buf[(size_t)(row * 2 + 1) * W];
            for (int x = 0; x < W; ++x) {
                if (first || top[x] != lastF) { appendColor(out, true,  top[x]); lastF = top[x]; }
                if (first || bot[x] != lastB) { appendColor(out, false, bot[x]); lastB = bot[x]; }
                first = false;
                out += "\xE2\x96\x80"; // U+2580 upper half block
            }
        }

        // HUD
        char hud[128];
        std::snprintf(hud, sizeof(hud), " %s | %d fps | %dx%d px | SPACE next | ESC quit ",
                      kScenes[scene].name, (int)(fps + 0.5f), W, PH);
        if ((int)std::strlen(hud) <= W) {
            out += "\x1b[1;1H\x1b[0;38;2;235;235;245;48;2;15;15;25m";
            out += hud;
            out += "\x1b[0m";
        }

        writeOut(hOut, out);

        // ~60 fps cap
        auto frameTime = clock::now() - now;
        auto target = std::chrono::microseconds(16667);
        if (frameTime < target) std::this_thread::sleep_for(target - frameTime);
    }

    // restore everything
    writeOut(hOut, "\x1b[0m\x1b[?25h\x1b[?1049l");
    SetConsoleOutputCP(oldCP);
    SetConsoleMode(hOut, oldMode);
    return 0;
}
