#include "Draw.h"

void dot(int x, int y) {
    if (x < 0) x += SCREEN_WIDTH;
    if (x >= SCREEN_WIDTH) x -= SCREEN_WIDTH;
    if (y < 0) y += SCREEN_HEIGHT;
    if (y >= SCREEN_HEIGHT) y -= SCREEN_HEIGHT;
    buffer[y][x] = 0xFFFFFFFF;
}

// based on ammeral
void draw_line(const Vec& from, const Vec& to) {
    int x_incl = 1;
    int y_incl = 1;
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    if (dx < 0) {
        dx = -dx;
        x_incl = -1;
    }
    if (dy < 0) {
        dy = -dy;
        y_incl = -1;
    }
    int vertlonger = 0;
    if (dy > dx) {
        vertlonger = 1;
        int tmp = dx;
        dx = dy;
        dy = tmp;
    }

    int denom = dx << 1; // same as *2
    int t = dy << 1;
    int e = -dx;
    int x = from.x;
    int y = from.y;

    if (vertlonger) {
        while (dx-- >= 0) {
            dot(x,y);
            if ((e += t) > 0) {
                x += x_incl;
                e -= denom;
            }
            y += y_incl;
        }
    } else {
        while (dx-- >= 0) {
            dot(x,y);
            if ((e += t) > 0) {
                y += y_incl;
                e -= denom;
            }
            x += x_incl;
        }
    }

}