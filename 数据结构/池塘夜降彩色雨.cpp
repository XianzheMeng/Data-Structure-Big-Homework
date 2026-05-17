/*
Coded by deepseek. I've done nothing but copy.
*/

#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winmm.lib")

#define WIDTH   800
#define HEIGHT  600
#define WATER_Y 500
#define MAX_RAIN 60
#define TIMER_ID 1
#define FRAME_MS 30

// 雨点状态
typedef enum { FALLING, HIT, RIPPLE } RainState;

// 雨点结构
typedef struct RainDrop {
    float x, y;
    float startX;
    float speedY;
    COLORREF color;
    float rippleRadius;
    float maxRipple;
    float rippleAlpha;
    RainState state;
    int onLeaf;
    struct RainDrop* next;
} RainDrop;

// 溅起的小水珠
typedef struct {
    float x, y;
    float vx, vy;
    int life;
    COLORREF color;
} DropSplash;

// 荷叶
typedef struct {
    int x, y, radius;
} LotusLeaf;

// 全局变量
RainDrop* rainList = NULL;
LotusLeaf leaves[6];
DropSplash splashes[300];
int splashCount = 0;
int wind = 0;
int rainDensity = 8;
int frameCount = 0;
int lastLightning = 0;

// 随机数
int random(int min, int max) {
    return rand() % (max - min + 1) + min;
}
float randomf(float min, float max) {
    return min + (rand() / (float)RAND_MAX) * (max - min);
}

// 创建雨点并插入链表
void InsertRainDrop() {
    RainDrop* p = (RainDrop*)malloc(sizeof(RainDrop));
    p->x = (float)random(20, WIDTH - 20);
    p->y = (float)random(-60, -10);
    p->startX = p->x;
    p->speedY = randomf(2.0f, 5.0f);
    p->color = RGB(random(100, 255), random(100, 255), random(100, 255));
    p->rippleRadius = 0;
    p->maxRipple = randomf(20.0f, 60.0f);
    p->rippleAlpha = 1.0f;
    p->state = FALLING;
    p->onLeaf = 0;
    p->next = rainList;
    rainList = p;
}

// 删除节点
void RemoveRainDrop(RainDrop* prev, RainDrop* p) {
    if (prev == NULL) rainList = p->next;
    else prev->next = p->next;
    free(p);
}

// 初始化荷叶
void InitLeaves() {
    for (int i = 0; i < 6; i++) {
        leaves[i].x = random(80, WIDTH - 80);
        leaves[i].y = WATER_Y - random(0, 8);
        leaves[i].radius = random(25, 50);
    }
}

// 更新场景
void Update() {
    frameCount++;

    // 密度周期变化
    int phase = (frameCount / 200) % 3;
    switch (phase) {
    case 0: rainDensity = 4;  wind = random(-2, 2); break;
    case 1: rainDensity = 15; wind = 0; break;
    case 2: rainDensity = 8;  wind = random(-1, 1); break;
    }

    // 生成新雨点
    if (frameCount % rainDensity == 0) {
        int cnt = 0;
        RainDrop* t = rainList;
        while (t) { cnt++; t = t->next; }
        if (cnt < MAX_RAIN) InsertRainDrop();
    }

    // 更新雨点
    RainDrop* p = rainList;
    RainDrop* prev = NULL;
    while (p) {
        RainDrop* next = p->next;
        if (p->state == FALLING) {
            p->x += wind * 0.02f;
            p->y += p->speedY;
            if (p->y >= WATER_Y) {
                // 荷叶检测
                int hit = 0;
                for (int i = 0; i < 6; i++) {
                    int dx = (int)p->x - leaves[i].x;
                    int dy = (int)p->y - leaves[i].y;
                    if (dx*dx + dy*dy <= leaves[i].radius * leaves[i].radius) {
                        hit = 1; break;
                    }
                }
                if (hit) {
                    p->onLeaf = 1;
                    Beep(400, 50); // 荷叶声
                    // 溅开水珠
                    for (int s = 0; s < 12; s++) {
                        if (splashCount < 300) {
                            float ang = randomf(0, 6.28f);
                            float sp = randomf(1.0f, 3.5f);
                            splashes[splashCount].x = p->x;
                            splashes[splashCount].y = p->y;
                            splashes[splashCount].vx = cos(ang) * sp;
                            splashes[splashCount].vy = sin(ang) * sp - 2.0f;
                            splashes[splashCount].life = random(10, 25);
                            splashes[splashCount].color = p->color;
                            splashCount++;
                        }
                    }
                    RemoveRainDrop(prev, p);
                    p = next;
                    continue;
                }
                // 入水
                Beep(800, 30);
                p->state = HIT;
                p->y = WATER_Y;
            }
        }
        else if (p->state == HIT) {
            p->state = RIPPLE;
            p->rippleRadius = 2.0f;
        }
        else if (p->state == RIPPLE) {
            p->rippleRadius += 1.0f;
            p->rippleAlpha = 1.0f - (p->rippleRadius / p->maxRipple);
            if (p->rippleAlpha < 0) p->rippleAlpha = 0;
            if (p->rippleRadius >= p->maxRipple) {
                RemoveRainDrop(prev, p);
                p = next;
                continue;
            }
        }
        prev = p;
        p = next;
    }

    // 更新水珠
    for (int i = 0; i < splashCount; i++) {
        if (splashes[i].life > 0) {
            splashes[i].x += splashes[i].vx;
            splashes[i].y += splashes[i].vy;
            splashes[i].vy += 0.2f;
            if (splashes[i].y >= WATER_Y || --splashes[i].life <= 0) {
                // 移除
                splashes[i] = splashes[--splashCount];
                i--;
            }
        }
    }

    // 闪电
    if (frameCount - lastLightning > random(60, 180) && random(0, 100) < 10) {
        lastLightning = frameCount;
        Beep(50, 200); // 雷声
    }
}

// 绘制全部内容
void Draw(HDC hdc, HDC memDC, HBITMAP memBmp, RECT rect) {
    // 背景
    HBRUSH bgBrush = CreateSolidBrush(RGB(15, 15, 40));
    FillRect(memDC, &rect, bgBrush);
    DeleteObject(bgBrush);

    // 水面
    RECT water = {0, WATER_Y, WIDTH, HEIGHT};
    HBRUSH waterBrush = CreateSolidBrush(RGB(5, 25, 50));
    FillRect(memDC, &water, waterBrush);
    DeleteObject(waterBrush);

    // 荷叶
    HBRUSH leafBrush = CreateSolidBrush(RGB(20, 100, 20));
    HPEN leafPen = CreatePen(PS_SOLID, 2, RGB(10, 60, 10));
    SelectObject(memDC, leafBrush);
    SelectObject(memDC, leafPen);
    for (int i = 0; i < 6; i++) {
        Ellipse(memDC,
                leaves[i].x - leaves[i].radius,
                leaves[i].y - leaves[i].radius/2,
                leaves[i].x + leaves[i].radius,
                leaves[i].y + leaves[i].radius/2);
    }
    DeleteObject(leafBrush);
    DeleteObject(leafPen);

    // 闪电（本帧有雷声则画）
    if (lastLightning == frameCount) {
        int x1 = random(100, WIDTH-100);
        int x2 = x1 + random(-60, 60);
        HPEN lPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 200));
        SelectObject(memDC, lPen);
        MoveToEx(memDC, x1, 0, NULL);
        LineTo(memDC, x2, WATER_Y);
        DeleteObject(lPen);
    }

    // 雨点
    RainDrop* p = rainList;
    while (p) {
        if (p->state == FALLING) {
            // 断续轨迹
            float len = 15.0f;
            for (float dy = 0; dy < len; dy += 4.0f) {
                float ty = p->y - dy;
                if (ty < 0) break;
                SetPixel(memDC, (int)(p->x + wind * dy/10.0f), (int)ty, p->color);
            }
            // 雨滴圆点
            HBRUSH rb = CreateSolidBrush(p->color);
            SelectObject(memDC, rb);
            Ellipse(memDC, (int)p->x-2, (int)p->y-2, (int)p->x+2, (int)p->y+2);
            DeleteObject(rb);
        }
        else if (p->state == RIPPLE) {
            int r = (int)p->rippleRadius;
            if (r > 0) {
                HPEN rPen = CreatePen(PS_DOT, 1, RGB(100, 200, 255));
                SelectObject(memDC, rPen);
                Arc(memDC, (int)p->x - r, (int)p->y - r,
                    (int)p->x + r, (int)p->y + r,
                    (int)p->x + r, (int)p->y,
                    (int)p->x + r, (int)p->y);
                DeleteObject(rPen);
            }
        }
        p = p->next;
    }

    // 溅开水珠
    for (int i = 0; i < splashCount; i++) {
        if (splashes[i].life > 0) {
            HBRUSH sb = CreateSolidBrush(splashes[i].color);
            SelectObject(memDC, sb);
            Ellipse(memDC,
                    (int)splashes[i].x - 1, (int)splashes[i].y - 1,
                    (int)splashes[i].x + 1, (int)splashes[i].y + 1);
            DeleteObject(sb);
        }
    }

    // 输出到屏幕
    BitBlt(hdc, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, SRCCOPY);
}

// 窗口过程
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HDC memDC;
    static HBITMAP memBmp;
    static RECT client;

    switch (msg) {
    case WM_CREATE: {
        HDC hdc = GetDC(hwnd);
        GetClientRect(hwnd, &client);
        memDC = CreateCompatibleDC(hdc);
        memBmp = CreateCompatibleBitmap(hdc, client.right, client.bottom);
        SelectObject(memDC, memBmp);
        ReleaseDC(hwnd, hdc);
        SetTimer(hwnd, TIMER_ID, FRAME_MS, NULL);
        srand((unsigned)time(NULL));
        InitLeaves();
        break;
    }
    case WM_TIMER:
        if (wParam == TIMER_ID) {
            Update();
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Draw(hdc, memDC, memBmp, client);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        DeleteDC(memDC);
        DeleteObject(memBmp);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// 程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "PondNightRain";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "PondNightRain", "池塘夜雨",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        100, 50, WIDTH, HEIGHT,
        NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
