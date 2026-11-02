// =====================================================================================
//   G A R G A N T U A
//   A real-time Schwarzschild black hole, ray traced inside your console.
//
//   Every character cell is two pixels (the upper-half-block trick with 24-bit color).
//   For every pixel a photon is fired backwards from the camera and its path is
//   integrated through curved spacetime around a non-rotating black hole. Light bends,
//   so the far side of the accretion disk shows up above AND below the shadow, the
//   background stars smear into arcs and Einstein rings, and a thin photon ring
//   hugs the event horizon.
//
//   Physics (units: Schwarzschild radius r_s = 1, so G = c = 1, M = 1/2)
//   - Null geodesics: in Schwarzschild spacetime a photon's orbit obeys the Binet equation
//         u'' + u = (3/2) r_s u^2          (u = 1/r)
//     which is exactly reproduced by integrating the Cartesian pseudo-force
//         a = -(3/2) h^2 * p / |p|^5       (h = |p x v|, the conserved angular momentum)
//     This gives the exact photon sphere at r = 1.5 r_s and the exact shadow size.
//   - Event horizon: r = 1 r_s.  Accretion disk: from the ISCO (3 r_s) outward.
//   - Disk gas follows Keplerian orbits, beta = sqrt(M/r), angular speed ~ r^(-3/2),
//     so the inner disk swirls much faster than the outer disk (differential rotation).
//   - Relativistic Doppler beaming: D = 1 / (gamma (1 - beta cos(theta))),
//     intensity ~ D^3.5, which makes the approaching side much brighter and bluer.
//   - Gravitational redshift: sqrt(1 - r_s / r).
//   - Thin-disk temperature profile T ~ r^(-3/4).
//
//   Windows 10/11 only (uses the VT/ANSI console mode and 24-bit color).
//   Best in Windows Terminal: maximize the window and shrink the font (Ctrl + minus).
//
//   Build:
//     MSVC  :  cl /O2 /EHsc /std:c++17 gargantua.cpp
//     MinGW :  g++ -O3 -march=native -std=c++17 gargantua.cpp -o gargantua.exe
//
//   Keys:  SPACE pause   + / - zoom   A antialiasing   H HUD   Q / ESC quit
// =====================================================================================

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

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

// ----------------------------------------------------------------------------- math
struct V3 { float x, y, z; };
static inline V3 operator+(V3 a, V3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
static inline V3 operator-(V3 a, V3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
static inline V3 operator*(V3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }
static inline V3 operator*(V3 a, V3 b) { return {a.x * b.x, a.y * b.y, a.z * b.z}; }
static inline float dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline V3 cross(V3 a, V3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
static inline float len(V3 a) { return std::sqrt(dot(a, a)); }
static inline V3 norm(V3 a) { return a * (1.0f / len(a)); }
static inline float clampf(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }
static inline V3 mix(V3 a, V3 b, float t) { return a + (b - a) * t; }
static inline float smooth(float e0, float e1, float x) {
    x = clampf((x - e0) / (e1 - e0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}
static inline uint32_t hash3(int x, int y, int z) {
    uint32_t h = (uint32_t)x * 73856093u ^ (uint32_t)y * 19349663u ^ (uint32_t)z * 83492791u;
    h ^= h >> 13; h *= 0x5bd1e995u; h ^= h >> 15; h *= 0x27d4eb2du; h ^= h >> 16;
    return h;
}

// ----------------------------------------------------------------------------- scene constants
constexpr float PI       = 3.14159265f;
constexpr float RS       = 1.0f;    // event horizon (Schwarzschild radius)
constexpr float DISK_IN  = 3.0f;    // innermost stable circular orbit
constexpr float DISK_OUT = 15.0f;
constexpr float ESCAPE_R = 32.0f;

// ----------------------------------------------------------------------------- sky
// Stars live in a 3D grid of cells around the unit sphere. Light bent by the black hole
// samples the sky from a different direction, so the starfield visibly warps.
static V3 sky(V3 d) {
    V3 col{0, 0, 0};

    // Milky-way-like band, tilted across the view
    const V3 bandN = norm(V3{0.75f, 0.55f, 0.36f});
    float bd   = dot(d, bandN);
    float band = std::exp(-bd * bd * 14.0f);
    float n1 = 0.5f + 0.5f * std::sin(d.x * 9.0f + std::sin(d.y * 7.0f + d.z * 3.0f) * 2.5f);
    float n2 = 0.5f + 0.5f * std::sin(d.z * 13.0f + std::sin(d.x * 11.0f) * 1.7f + d.y * 5.0f);
    float dust = smooth(0.35f, 0.9f, n2);                       // dark dust lanes
    col = col + V3{0.30f, 0.26f, 0.52f} * (band * (0.06f + 0.16f * n1) * (1.0f - 0.7f * dust));
    col = col + V3{0.65f, 0.40f, 0.30f} * (band * band * band * 0.10f * n1);

    // faint large-scale nebula glow away from the band
    float neb = 0.5f + 0.5f * std::sin(d.y * 4.0f + std::sin(d.x * 3.0f) * 2.0f) * std::sin(d.z * 3.5f + 1.0f);
    col = col + V3{0.10f, 0.03f, 0.12f} * (neb * neb * 0.35f);

    // stars
    const float K = 85.0f;
    V3 q = d * K;
    uint32_t h = hash3((int)std::floor(q.x), (int)std::floor(q.y), (int)std::floor(q.z));
    float chance = (h & 0xFFFF) / 65535.0f;
    if (chance < 0.016f + 0.05f * band) {
        float b = ((h >> 16) & 0xFF) / 255.0f;
        b = 0.07f + 3.5f * b * b * b * b * b * b;
        float tint = ((h >> 24) & 0xFF) / 255.0f;
        V3 c = tint < 0.30f ? V3{0.65f, 0.78f, 1.0f}
             : tint < 0.78f ? V3{1.0f, 0.97f, 0.92f}
                            : V3{1.0f, 0.75f, 0.50f};
        col = col + c * b;
    }
    return col;
}

// ----------------------------------------------------------------------------- accretion disk
static V3 blackbody(float k) {  // 0 = cool red ... 1 = hot blue-white
    const V3 c0{1.00f, 0.16f, 0.02f}, c1{1.00f, 0.48f, 0.10f}, c2{1.00f, 0.85f, 0.60f}, c3{0.72f, 0.84f, 1.00f};
    k = clampf(k, 0.0f, 1.0f);
    if (k < 0.40f) return mix(c0, c1, k / 0.40f);
    if (k < 0.78f) return mix(c1, c2, (k - 0.40f) / 0.38f);
    return mix(c2, c3, (k - 0.78f) / 0.22f);
}

static void diskSample(V3 hit, float hr, V3 rayDir, float t, V3& emit, float& alpha) {
    float ang = std::atan2(hit.z, hit.x);
    float lr  = std::log(hr);

    // Keplerian swirl. Differential rotation would wind the texture up forever, so two
    // flow layers are cross-faded (a flow-map), which keeps the motion going without the
    // pattern turning into noise after a few minutes.
    float omega = 3.2f * std::pow(hr, -1.5f);
    const float P = 9.0f;
    float tex = 0.0f;
    for (int k = 0; k < 2; k++) {
        float tk = std::fmod(t + k * P * 0.5f, P);
        float w  = std::sin(PI * tk / P); w *= w;
        float ph = ang - omega * tk + k * 2.4f;
        float n = 0.55f
                + 0.22f * std::sin(3.0f * ph + 7.0f * lr)
                + 0.15f * std::sin(8.0f * ph - 13.0f * lr + 1.3f)
                + 0.10f * std::sin(17.0f * ph + 29.0f * lr + 2.1f)
                + 0.07f * std::sin(31.0f * ph - 47.0f * lr + 0.7f)
                + 0.10f * std::sin(43.0f * lr + 0.5f * std::sin(5.0f * ph));
        tex += w * n;
    }
    tex = clampf(tex, 0.05f, 1.5f);

    float x = DISK_IN / hr;
    float I = std::pow(x, 2.1f)
            * smooth(DISK_IN, DISK_IN + 0.7f, hr)
            * smooth(DISK_OUT, DISK_OUT - 5.0f, hr);

    // relativistic Doppler beaming + gravitational redshift
    V3 tang     = V3{-hit.z, 0.0f, hit.x} * (1.0f / hr);
    float beta  = clampf(std::sqrt(0.5f / hr), 0.0f, 0.7f);
    float gamma = 1.0f / std::sqrt(1.0f - beta * beta);
    float cosT  = -dot(tang, rayDir);
    float D     = 1.0f / (gamma * (1.0f - beta * cosT));
    float g     = std::sqrt(1.0f - RS / hr);
    float s     = D * g;

    float boost = clampf(std::pow(s, 3.5f), 0.08f, 6.0f);
    float temp  = 0.85f * std::pow(x, 0.75f) + (s - 1.0f) * 0.55f;

    emit  = blackbody(temp) * (I * tex * boost * 3.2f);
    alpha = clampf(I * tex * 2.2f, 0.0f, 0.93f);
}

// ----------------------------------------------------------------------------- the photon
static V3 trace(V3 p, V3 v, float t) {
    V3 L = cross(p, v);
    float h2 = dot(L, L);
    V3 col{0, 0, 0};
    float T = 1.0f;  // transmittance through the semi-transparent disk

    for (int i = 0; i < 600; i++) {
        float r2 = dot(p, p);
        float r  = std::sqrt(r2);
        if (r < RS) return col;                                     // swallowed: pure black
        if (r > ESCAPE_R && dot(p, v) > 0.0f) return col + sky(norm(v)) * T;

        float dt = clampf(0.045f * r, 0.004f, 2.5f) / len(v);
        V3 a  = p * (-1.5f * h2 / (r2 * r2 * r));
        V3 pp = p;
        v = v + a * dt;
        p = p + v * dt;

        // faint hot glow hugging the photon sphere
        col = col + V3{1.0f, 0.55f, 0.25f} * (T * 0.012f * dt * len(v) / (r2 * r2) * 1.5f);

        if ((pp.y > 0.0f) != (p.y > 0.0f)) {                       // crossed the disk plane
            float f  = pp.y / (pp.y - p.y);
            V3 hit   = pp + (p - pp) * f;
            float hr = len(hit);
            if (hr > DISK_IN && hr < DISK_OUT) {
                V3 e; float a2;
                diskSample(hit, hr, norm(v), t, e, a2);
                col = col + e * T;
                T  *= (1.0f - a2);
                if (T < 0.01f) return col;
            }
        }
    }
    return col;
}

// ----------------------------------------------------------------------------- camera + frame
struct Camera { V3 pos, fwd, right, up; float fov; };

static Camera makeCamera(float t, float zoom) {
    float phi  = t * 0.11f;
    float el   = 0.09f + 0.20f * (0.5f + 0.5f * std::sin(t * 0.19f));        // gently rises and dips
    float dist = 23.0f * zoom * (1.0f + 0.12f * std::sin(t * 0.13f));
    Camera c;
    c.pos   = V3{dist * std::cos(el) * std::cos(phi), dist * std::sin(el), dist * std::cos(el) * std::sin(phi)};
    c.fwd   = norm(c.pos * -1.0f);
    V3 r    = norm(cross(c.fwd, V3{0, 1, 0}));
    V3 u    = cross(r, c.fwd);
    float roll = 0.18f * std::sin(t * 0.17f);
    c.right = r * std::cos(roll) + u * std::sin(roll);
    c.up    = u * std::cos(roll) - r * std::sin(roll);
    c.fov   = 0.36f;
    return c;
}

static inline uint8_t toByte(float c) {
    c = 1.0f - std::exp(-c * 1.35f);              // filmic-ish exposure
    c = std::pow(clampf(c, 0.0f, 1.0f), 0.92f);
    return (uint8_t)(c * 255.0f + 0.5f);
}

// renders W x H pixels into rgb (3 bytes per pixel), using all CPU cores
static void renderFrame(int W, int H, float t, float zoom, int ss, std::vector<uint8_t>& rgb) {
    rgb.resize((size_t)W * H * 3);
    Camera cam = makeCamera(t, zoom);
    unsigned nThreads = std::max(1u, std::thread::hardware_concurrency());
    std::atomic<int> nextRow{0};

    auto worker = [&]() {
        for (;;) {
            int y = nextRow.fetch_add(1);
            if (y >= H) break;
            for (int x = 0; x < W; x++) {
                V3 acc{0, 0, 0};
                for (int sy = 0; sy < ss; sy++)
                for (int sx = 0; sx < ss; sx++) {
                    float px = x + (sx + 0.5f) / ss, py = y + (sy + 0.5f) / ss;
                    float u =  (2.0f * px - W) / H * cam.fov;
                    float v = -(2.0f * py - H) / H * cam.fov;
                    V3 dir = norm(cam.fwd + cam.right * u + cam.up * v);
                    acc = acc + trace(cam.pos, dir, t);
                }
                acc = acc * (1.0f / (ss * ss));
                uint8_t* o = &rgb[((size_t)y * W + x) * 3];
                o[0] = toByte(acc.x); o[1] = toByte(acc.y); o[2] = toByte(acc.z);
            }
        }
    };
    std::vector<std::thread> pool;
    for (unsigned i = 0; i < nThreads; i++) pool.emplace_back(worker);
    for (auto& th : pool) th.join();
}

// ----------------------------------------------------------------------------- console output
static inline void appendNum(std::string& s, int n) {
    char buf[12]; int i = 0;
    if (n == 0) { s += '0'; return; }
    while (n > 0) { buf[i++] = char('0' + n % 10); n /= 10; }
    while (i > 0) s += buf[--i];
}
static inline void appendRGB(std::string& s, const uint8_t* c) {
    appendNum(s, c[0]); s += ';'; appendNum(s, c[1]); s += ';'; appendNum(s, c[2]); s += 'm';
}

// two pixel rows per text row: top pixel = foreground of the upper-half block, bottom = background
static void buildFrame(std::string& out, const std::vector<uint8_t>& rgb, int cols, int textRows) {
    out.clear();
    for (int cy = 0; cy < textRows; cy++) {
        out += "\x1b["; appendNum(out, cy + 1); out += ";1H";
        int lastTop = -1, lastBot = -1;
        for (int x = 0; x < cols; x++) {
            const uint8_t* top = &rgb[((size_t)(2 * cy) * cols + x) * 3];
            const uint8_t* bot = &rgb[((size_t)(2 * cy + 1) * cols + x) * 3];
            int kt = (top[0] << 16) | (top[1] << 8) | top[2];
            int kb = (bot[0] << 16) | (bot[1] << 8) | bot[2];
            if (kt != lastTop) { out += "\x1b[38;2;"; appendRGB(out, top); lastTop = kt; }
            if (kb != lastBot) { out += "\x1b[48;2;"; appendRGB(out, bot); lastBot = kb; }
            out += "\xE2\x96\x80";   // U+2580 UPPER HALF BLOCK
        }
    }
}

#ifdef _WIN32
// ============================================================================= Windows main
static std::atomic<bool> g_quit{false};
static BOOL WINAPI onCtrl(DWORD) { g_quit = true; return TRUE; }

int main() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode) ||
        !SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT)) {
        std::printf("This console does not support VT sequences. Use Windows 10+ / Windows Terminal.\n");
        return 1;
    }
    UINT oldCP = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCtrlHandler(onCtrl, TRUE);

    auto write = [&](const std::string& s) {
        DWORD w; WriteFile(hOut, s.data(), (DWORD)s.size(), &w, nullptr);
    };
    write("\x1b[?1049h\x1b[?25l\x1b[2J");   // alternate screen, hide cursor

    std::vector<uint8_t> rgb;
    std::string out;
    out.reserve(1 << 22);

    using clk = std::chrono::steady_clock;
    auto last = clk::now();
    float t = 0.0f, zoom = 1.0f, fps = 0.0f;
    bool paused = false, hud = true;
    int ss = 1, lastCols = 0, lastRows = 0;

    while (!g_quit) {
        // input
        while (_kbhit()) {
            int ch = _getch();
            if (ch == 0 || ch == 224) { _getch(); continue; }
            if (ch == 27 || ch == 'q' || ch == 'Q') g_quit = true;
            else if (ch == ' ') paused = !paused;
            else if (ch == '+' || ch == '=') zoom = std::max(0.35f, zoom * 0.9f);
            else if (ch == '-' || ch == '_') zoom = std::min(3.0f, zoom / 0.9f);
            else if (ch == 'a' || ch == 'A') ss = (ss == 1) ? 2 : 1;
            else if (ch == 'h' || ch == 'H') { hud = !hud; write("\x1b[2J"); }
        }

        // console size
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        if (cols != lastCols || rows != lastRows) {
            write("\x1b[2J");
            lastCols = cols; lastRows = rows;
        }
        int textRows = std::max(1, rows - (hud ? 1 : 0));

        // time
        auto now = clk::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;
        if (!paused) t += dt;
        if (dt > 0.0f) fps = fps * 0.9f + (1.0f / dt) * 0.1f;

        // render + present
        renderFrame(cols, textRows * 2, t, zoom, ss, rgb);
        buildFrame(out, rgb, cols, textRows);

        if (hud) {
            char line[512];
            float camR = 23.0f * zoom * (1.0f + 0.12f * std::sin(t * 0.13f));
            std::snprintf(line, sizeof line,
                " GARGANTUA | Schwarzschild geodesic ray tracer | horizon 1 r_s  photon sphere 1.5 r_s  ISCO 3 r_s"
                " | camera %.1f r_s | %dx%d px | %.0f fps%s | [SPACE] pause [+/-] zoom [A] AA:%s [H] hud [Q] quit",
                camR, cols, textRows * 2, fps, paused ? " | PAUSED" : "", ss > 1 ? "on" : "off");
            std::string s(line);
            if ((int)s.size() > cols - 1) s.resize(std::max(0, cols - 1));
            out += "\x1b[0m\x1b["; appendNum(out, rows); out += ";1H\x1b[2K\x1b[38;2;255;170;80m";
            out += s;
        }
        out += "\x1b[0m";
        write(out);
    }

    write("\x1b[0m\x1b[?25h\x1b[?1049l");      // restore everything
    SetConsoleOutputCP(oldCP);
    return 0;
}

#else
// ============================================================================= preview build (non-Windows)
// Renders a few frames to PPM images so the look can be checked without a console.
int main(int argc, char** argv) {
    int W = argc > 1 ? std::atoi(argv[1]) : 320;
    int H = argc > 2 ? std::atoi(argv[2]) : 180;
    std::vector<uint8_t> rgb;
    const float times[] = {0.0f, 7.0f, 15.0f, 31.0f};
    for (float t : times) {
        auto t0 = std::chrono::steady_clock::now();
        renderFrame(W, H, t, 1.0f, 1, rgb);
        float ms = std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - t0).count();
        char name[64]; std::snprintf(name, sizeof name, "frame_%02d.ppm", (int)t);
        FILE* f = std::fopen(name, "wb");
        std::fprintf(f, "P6\n%d %d\n255\n", W, H);
        std::fwrite(rgb.data(), 1, rgb.size(), f);
        std::fclose(f);
        std::printf("%s  %.1f ms\n", name, ms);
    }
    return 0;
}
#endif
