#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

const int w = 80;
const int h = 30;

// draw sphere using distance + depth
void drawSphere(char screen[h][w], float cx, float cy, float cz, float r) {
        for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                        float px = (x - w / 2) / 10.0f;
                        float py = (y - h / 2) / 5.0f;

                        float dx = px - cx;
                        float dy = py - cy;

                        float d2 = dx * dx + dy * dy;
                        if (d2 > r * r)
                                continue;

                        float dz = sqrt(r * r - d2);
                        float z = dz + cz;

                        int shade = (int)((dz / r) * 5);
                        const char chars[] = " .:-=*#";

                        if (x >= 0 && x < w && y >= 0 && y < h)
                                screen[y][x] = chars[shade];
                }
        }
}

int main() {
        float t = 0;

        std::cout << "\033[2J\033[?25l";

        while (true) {
                char screen[h][w];
                for (int y = 0; y < h; y++)
                        for (int x = 0; x < w; x++)
                                screen[y][x] = ' ';

                // orbit pos
                float x1 = cos(t) * 1.5f;
                float y1 = sin(t) * 0.7f;

                float x2 = cos(t + 3.14f) * 1.5f;
                float y2 = sin(t + 3.14f) * 0.7f;

                // back sphere
                drawSphere(screen, x2, y2, -1.0f, 1.0f);
                // front sphere
                drawSphere(screen, x1, y1, 1.0f, 1.0f);

                std::cout << "\033[H";
                for (int y = 0; y < h; y++) {
                        for (int x = 0; x < w; x++)
                                std::cout << screen[y][x];
                        std::cout << "\n";
                }

                t += 0.05f;
                std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
}
