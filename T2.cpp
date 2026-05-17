#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

/* ============================================================
 * 颜色
 * ============================================================ */
#define C_BG        RGB(18,  26, 46)
#define C_PANEL     RGB(28,  36, 60)
#define C_PANEL2    RGB(38,  50, 80)
#define C_EDGE      RGB(55,  80,120)
#define C_PATH      RGB(255,200, 50)
#define C_NODE      RGB(50, 120,200)
#define C_SRC       RGB(40, 180, 70)
#define C_DST       RGB(210, 50, 50)
#define C_WP        RGB(170, 70,220)
#define C_TEXT      RGB(220,230,255)
#define C_TEXTLT    RGB(130,150,200)
#define C_ACCENT    RGB(255,200, 50)
#define C_BTN_CALC  RGB(40, 160, 60)
#define C_BTN_SRC   RGB(40, 160, 60)
#define C_BTN_DST   RGB(200, 50, 50)
#define C_BTN_WP    RGB(160, 60,200)
#define C_BTN_GEN   RGB(60,  80,130)
#define C_WHITE     RGB(255,255,255)
#define C_STATUS    RGB(10,  18, 35)

/* ============================================================
 * 尺寸
 * ============================================================ */
#define WIN_W    1060
#define WIN_H    660
#define PANEL_W  240
#define MAP_W    (WIN_W-PANEL_W)
#define MAP_H    WIN_H
#define MAP_PAD  45
#define STATUS_H 26

/* ============================================================
 * 数据结构
 * ============================================================ */
#define MAX_N  60
#define MAX_WP  8
#define INF    INT_MAX

typedef struct {
    int n;
    int dist[MAX_N][MAX_N];
    float x[MAX_N], y[MAX_N];
} Graph;

Graph g;
int src_nd=-1, dst_nd=-1;
int waypoints[MAX_WP], wp_cnt=0;
int full_path[MAX_N*MAX_WP+4], path_len=0, total_dist=0;
int has_result=0;

/* UI */
int select_mode=0;   /* 0=src,1=dst,-1=none */
int add_wp_mode=0;
int hover_nd=-1;
char status_msg[160]="点击「设置起点」后，点击地图节点选择起点";

HWND g_hwnd;
HFONT fnt_n, fnt_b, fnt_s, fnt_t;

/* ============================================================
 * 路网生成
 * ============================================================ */
void gen_graph(int n){
    srand((unsigned)time(NULL)^(unsigned)clock());
    g.n=n;
    for(int i=0;i<n;i++){
        g.x[i]=(float)(rand()%880+60)/1000.0f;
        g.y[i]=(float)(rand()%580+60)/1000.0f;
    }
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) g.dist[i][j]=(i==j)?0:INF;
    /* 随机生成树 */
    int ord[MAX_N]; for(int i=0;i<n;i++) ord[i]=i;
    for(int i=n-1;i>0;i--){ int j=rand()%(i+1),t=ord[i]; ord[i]=ord[j]; ord[j]=t; }
    for(int i=1;i<n;i++){
        int u=ord[i], v=ord[rand()%i], w=3+rand()%47;
        g.dist[u][v]=g.dist[v][u]=w;
    }
    for(int k=0;k<(int)(1.5*n);k++){
        int u=rand()%n,v=rand()%n; if(u==v||g.dist[u][v]!=INF) continue;
        int w=3+rand()%47; g.dist[u][v]=g.dist[v][u]=w;
    }
    src_nd=dst_nd=-1; wp_cnt=0; path_len=0; has_result=0; select_mode=0;
    strcpy(status_msg,"路网已生成，点击「设置起点」后选择节点");
}

/* ============================================================
 * Dijkstra
 * ============================================================ */
void dijkstra(int src,int*d,int*prev){
    int vis[MAX_N]={0};
    for(int i=0;i<g.n;i++){d[i]=INF;prev[i]=-1;} d[src]=0;
    for(int it=0;it<g.n;it++){
        int u=-1;
        for(int i=0;i<g.n;i++) if(!vis[i]&&(u==-1||d[i]<d[u])) u=i;
        if(u==-1||d[u]==INF) break; vis[u]=1;
        for(int v=0;v<g.n;v++){
            if(g.dist[u][v]==INF||vis[v]) continue;
            long long nd=(long long)d[u]+g.dist[u][v];
            if(nd<d[v]){d[v]=(int)nd;prev[v]=u;}
        }
    }
}
int recon(int*prev,int from,int to,int*path){
    if(to==from){path[0]=from;return 1;}
    if(prev[to]==-1) return 0;
    int len=recon(prev,from,prev[to],path);
    if(!len) return 0; path[len]=to; return len+1;
}
void compute_path(void){
    if(src_nd<0||dst_nd<0){strcpy(status_msg,"请先设置起点和终点！");return;}
    int stops[MAX_WP+2],sc=0;
    stops[sc++]=src_nd;
    for(int i=0;i<wp_cnt;i++) stops[sc++]=waypoints[i];
    stops[sc++]=dst_nd;
    path_len=0; total_dist=0;
    int d[MAX_N],prev[MAX_N],seg[MAX_N*2];
    for(int s=0;s<sc-1;s++){
        int from=stops[s],to=stops[s+1];
        dijkstra(from,d,prev);
        if(d[to]==INF){sprintf(status_msg,"路径不可达！"); has_result=0; return;}
        int len=recon(prev,from,to,seg);
        int st=(path_len>0)?1:0;
        for(int i=st;i<len;i++) full_path[path_len++]=seg[i];
        total_dist+=d[to];
    }
    has_result=1;
    sprintf(status_msg,"路径已找到！总距离: %d  节点数: %d",total_dist,path_len);
}

/* ============================================================
 * 坐标转换
 * ============================================================ */
int nx(int i){ return (int)(g.x[i]*(MAP_W-2*MAP_PAD)+MAP_PAD); }
int ny(int i){ return (int)(g.y[i]*(MAP_H-STATUS_H-2*MAP_PAD)+MAP_PAD); }

int node_at(int mx,int my){
    for(int i=0;i<g.n;i++){
        int dx=mx-nx(i),dy=my-ny(i);
        if(dx*dx+dy*dy<=144) return i;
    }
    return -1;
}
int on_path(int u,int v){
    for(int i=0;i<path_len-1;i++)
        if((full_path[i]==u&&full_path[i+1]==v)||(full_path[i]==v&&full_path[i+1]==u))
            return 1;
    return 0;
}

/* ============================================================
 * 绘图工具
 * ============================================================ */
static void fill_rc(HDC dc,int x,int y,int w,int h,COLORREF c){
    HBRUSH b=CreateSolidBrush(c); RECT r={x,y,x+w,y+h};
    FillRect(dc,&r,b); DeleteObject(b);
}
static void fill_rr(HDC dc,int x,int y,int w,int h,int r,COLORREF c){
    HBRUSH b=CreateSolidBrush(c); HPEN p=CreatePen(PS_SOLID,1,c);
    SelectObject(dc,b); SelectObject(dc,p);
    RoundRect(dc,x,y,x+w,y+h,r,r);
    DeleteObject(b); DeleteObject(p);
}
static void dtext(HDC dc,int x,int y,const char*t,COLORREF c,HFONT f){
    SelectObject(dc,f); SetTextColor(dc,c); SetBkMode(dc,TRANSPARENT);
    TextOut(dc,x,y,t,(int)strlen(t));
}
static void draw_circle(HDC dc,int cx,int cy,int r,COLORREF fill,COLORREF border,int bw){
    HBRUSH b=CreateSolidBrush(fill);
    HPEN   p=CreatePen(PS_SOLID,bw,border);
    SelectObject(dc,b); SelectObject(dc,p);
    Ellipse(dc,cx-r,cy-r,cx+r,cy+r);
    DeleteObject(b); DeleteObject(p);
}
static void draw_line_c(HDC dc,int x1,int y1,int x2,int y2,COLORREF c,int w){
    HPEN p=CreatePen(PS_SOLID,w,c);
    HPEN op=(HPEN)SelectObject(dc,p);
    MoveToEx(dc,x1,y1,NULL); LineTo(dc,x2,y2);
    SelectObject(dc,op); DeleteObject(p);
}

/* ============================================================
 * 绘制图
 * ============================================================ */
void draw_graph(HDC dc){
    fill_rc(dc,0,0,MAP_W,MAP_H,C_BG);

    /* 普通边 */
    for(int i=0;i<g.n;i++)
        for(int j=i+1;j<g.n;j++){
            if(g.dist[i][j]==INF) continue;
            if(on_path(i,j)) continue;
            draw_line_c(dc,nx(i),ny(i),nx(j),ny(j),C_EDGE,1);
        }

    /* 路径边（高亮） */
    if(has_result){
        for(int i=0;i<path_len-1;i++){
            int u=full_path[i],v=full_path[i+1];
            draw_line_c(dc,nx(u),ny(u),nx(v),ny(v),C_PATH,3);
            /* 边权标注 */
            int mx2=(nx(u)+nx(v))/2, my2=(ny(u)+ny(v))/2;
            char wt[8]; sprintf(wt,"%d",g.dist[u][v]);
            dtext(dc,mx2-6,my2-9,wt,C_ACCENT,fnt_s);
        }
    }

    /* 节点 */
    for(int i=0;i<g.n;i++){
        COLORREF nc=C_NODE;
        if(i==src_nd) nc=C_SRC;
        else if(i==dst_nd) nc=C_DST;
        else{ for(int k=0;k<wp_cnt;k++) if(waypoints[k]==i){nc=C_WP;break;} }
        if(has_result&&nc==C_NODE)
            for(int k=0;k<path_len;k++) if(full_path[k]==i){nc=C_PATH;break;}

        int r=(i==src_nd||i==dst_nd)?12:(i==hover_nd?10:7);
        /* 光晕 */
        draw_circle(dc,nx(i),ny(i),r+4,RGB(GetRValue(nc)/4,GetGValue(nc)/4,GetBValue(nc)/4),C_BG,0);
        /* 节点 */
        draw_circle(dc,nx(i),ny(i),r,nc,C_WHITE,i==hover_nd?2:1);
        /* 标号 */
        char lab[8]; sprintf(lab,"%d",i);
        dtext(dc,nx(i)-5*(int)strlen(lab),ny(i)-6,lab,C_WHITE,fnt_s);
    }
    /* 途径点序号 */
    for(int k=0;k<wp_cnt;k++){
        int i=waypoints[k];
        char wl[8]; sprintf(wl,"P%d",k+1);
        dtext(dc,nx(i)+13,ny(i)-8,wl,C_WP,fnt_b);
    }

    /* 状态栏 */
    fill_rc(dc,0,MAP_H-STATUS_H,MAP_W,STATUS_H,C_STATUS);
    dtext(dc,10,MAP_H-STATUS_H+5,status_msg,C_ACCENT,fnt_s);
}

/* ============================================================
 * 绘制控制面板
 * ============================================================ */
void draw_panel(HDC dc){
    int px=MAP_W;
    fill_rc(dc,px,0,PANEL_W,WIN_H,C_PANEL);
    /* 标题 */
    fill_rc(dc,px,0,PANEL_W,44,RGB(18,26,50));
    dtext(dc,px+12,10,"路网最短路径",C_TEXT,fnt_t);

    int y=54;
    char tmp[80];
    sprintf(tmp,"节点: %d",g.n); dtext(dc,px+12,y,tmp,C_TEXTLT,fnt_s); y+=18;
    int ec=0; for(int i=0;i<g.n;i++) for(int j=i+1;j<g.n;j++) if(g.dist[i][j]!=INF) ec++;
    sprintf(tmp,"边数: %d",ec); dtext(dc,px+12,y,tmp,C_TEXTLT,fnt_s); y+=22;

    /* 选点状态 */
    fill_rc(dc,px,y,PANEL_W,1,C_PANEL2); y+=8;
    dtext(dc,px+12,y,"选点状态",C_TEXT,fnt_b); y+=22;

    /* 起点 */
    COLORREF sc2=has_result||src_nd>=0?C_SRC:(select_mode==0?C_ACCENT:C_PANEL2);
    fill_rc(dc,px+8,y,4,36,sc2);
    fill_rc(dc,px+16,y,PANEL_W-24,36,C_PANEL2);
    dtext(dc,px+24,y+2,"起点",C_TEXTLT,fnt_s);
    if(src_nd>=0){sprintf(tmp,"节点 V%d",src_nd); dtext(dc,px+24,y+16,tmp,C_SRC,fnt_b);}
    else dtext(dc,px+24,y+16,"未选择",C_TEXTLT,fnt_b);
    y+=44;

    /* 终点 */
    COLORREF dc2=has_result||dst_nd>=0?C_DST:(select_mode==1?C_ACCENT:C_PANEL2);
    fill_rc(dc,px+8,y,4,36,dc2);
    fill_rc(dc,px+16,y,PANEL_W-24,36,C_PANEL2);
    dtext(dc,px+24,y+2,"终点",C_TEXTLT,fnt_s);
    if(dst_nd>=0){sprintf(tmp,"节点 V%d",dst_nd); dtext(dc,px+24,y+16,tmp,C_DST,fnt_b);}
    else dtext(dc,px+24,y+16,"未选择",C_TEXTLT,fnt_b);
    y+=44;

    /* 途径点列表 */
    fill_rc(dc,px,y,PANEL_W,1,C_PANEL2); y+=8;
    dtext(dc,px+12,y,"途径点",C_TEXT,fnt_b); y+=20;
    if(wp_cnt==0){ dtext(dc,px+16,y,"（无途径点）",C_TEXTLT,fnt_s); y+=18; }
    for(int k=0;k<wp_cnt;k++){
        sprintf(tmp,"P%d: 节点V%d",k+1,waypoints[k]);
        dtext(dc,px+16,y,tmp,C_WP,fnt_s); y+=18;
    }
    y+=6;

    /* 结果 */
    fill_rc(dc,px,y,PANEL_W,1,C_PANEL2); y+=8;
    if(has_result){
        dtext(dc,px+12,y,"路径结果",C_TEXT,fnt_b); y+=20;
        int lx=px+12;
        for(int i=0;i<path_len;i++){
            char seg[8]; sprintf(seg,"V%d",full_path[i]);
            dtext(dc,lx,y,seg,C_PATH,fnt_s);
            lx+=8*(int)strlen(seg)+2;
            if(i<path_len-1){ dtext(dc,lx,y,">",C_TEXTLT,fnt_s); lx+=10; }
            if(lx>px+PANEL_W-20){lx=px+12;y+=16;}
        }
        y+=22;
        sprintf(tmp,"总距离: %d",total_dist);
        dtext(dc,px+12,y,tmp,C_ACCENT,fnt_b); y+=26;
    }

    /* 按钮 */
    y=WIN_H-5*44-28;   /* 向上移动了24像素 */
    fill_rr(dc,px+8,y,PANEL_W-16,34,6,select_mode==0?C_ACCENT:C_SRC);
    dtext(dc,px+60,y+8,"设置起点",select_mode==0?C_BG:C_WHITE,fnt_b); y+=42;

    fill_rr(dc,px+8,y,PANEL_W-16,34,6,select_mode==1?C_ACCENT:C_DST);
    dtext(dc,px+60,y+8,"设置终点",select_mode==1?C_BG:C_WHITE,fnt_b); y+=42;

    fill_rr(dc,px+8,y,PANEL_W-16,34,6,add_wp_mode?C_ACCENT:C_BTN_WP);
    dtext(dc,px+40,y+8,add_wp_mode?"点图中节点...":"添加途径点",add_wp_mode?C_BG:C_WHITE,fnt_b); y+=42;

    fill_rr(dc,px+8,y,PANEL_W-16,34,6,C_BTN_CALC);
    dtext(dc,px+46,y+8,"计算最短路径",C_WHITE,fnt_b); y+=42;

    fill_rr(dc,px+8,y,PANEL_W-16,34,6,C_BTN_GEN);
    dtext(dc,px+46,y+8,"重新生成路网",C_TEXT,fnt_b);

    /* 右键提示 */
    dtext(dc,px+8,WIN_H-STATUS_H-22,"右键: 删除最后途径点/清除",C_TEXTLT,fnt_s);
    dtext(dc,px+8,WIN_H-STATUS_H-8,"ESC 退出",C_TEXTLT,fnt_s);
}

/* ============================================================
 * WndProc
 * ============================================================ */
LRESULT CALLBACK WndProc(HWND hw,UINT msg,WPARAM wp,LPARAM lp){
    static HDC memDC=NULL;
    static HBITMAP memBmp=NULL;

    switch(msg){
    case WM_CREATE:{
        HDC dc=GetDC(hw);
        memDC=CreateCompatibleDC(dc);
        memBmp=CreateCompatibleBitmap(dc,WIN_W,WIN_H);
        SelectObject(memDC,memBmp);
        ReleaseDC(hw,dc);
        fnt_n=CreateFont(16,0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,0,0,DEFAULT_QUALITY,0,"微软雅黑");
        fnt_b=CreateFont(16,0,0,0,FW_BOLD,  0,0,0,GB2312_CHARSET,0,0,DEFAULT_QUALITY,0,"微软雅黑");
        fnt_s=CreateFont(13,0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,0,0,DEFAULT_QUALITY,0,"微软雅黑");
        fnt_t=CreateFont(20,0,0,0,FW_BOLD,  0,0,0,GB2312_CHARSET,0,0,DEFAULT_QUALITY,0,"微软雅黑");
        SetTimer(hw,1,50,NULL);
        break;
    }
    case WM_TIMER:
        InvalidateRect(hw,NULL,FALSE); break;
    case WM_PAINT:{
        draw_graph(memDC);
        draw_panel(memDC);
        PAINTSTRUCT ps; HDC dc=BeginPaint(hw,&ps);
        BitBlt(dc,0,0,WIN_W,WIN_H,memDC,0,0,SRCCOPY);
        EndPaint(hw,&ps); break;
    }
    case WM_MOUSEMOVE:{
        int mx=LOWORD(lp),my=HIWORD(lp);
        if(mx<MAP_W) hover_nd=node_at(mx,my);
        else hover_nd=-1;
        break;
    }
    case WM_LBUTTONDOWN:{
        int mx=LOWORD(lp),my=HIWORD(lp);
        int px2=MAP_W;
        if(mx>=px2){
            /* 面板按钮 */
            int by=WIN_H-5*44-28;   /* 与绘图时Y坐标保持一致 */
            if(my>=by&&my<by+34){ select_mode=0; add_wp_mode=0; strcpy(status_msg,"点击地图节点设置起点"); } by+=42;
            if(my>=by&&my<by+34){ select_mode=1; add_wp_mode=0; strcpy(status_msg,"点击地图节点设置终点"); } by+=42;
            if(my>=by&&my<by+34){
                add_wp_mode=!add_wp_mode; select_mode=-1;
                strcpy(status_msg,add_wp_mode?"点击地图节点添加途径点（右键删除）":"途径点模式关闭");
            } by+=42;
            if(my>=by&&my<by+34) compute_path(); by+=42;
            if(my>=by&&my<by+34){ gen_graph(50+rand()%11); }
        } else {
            int clicked=node_at(mx,my);
            if(clicked>=0){
                if(add_wp_mode){
                    if(wp_cnt<MAX_WP&&clicked!=src_nd&&clicked!=dst_nd){
                        int dup=0; for(int k=0;k<wp_cnt;k++) if(waypoints[k]==clicked){dup=1;break;}
                        if(!dup){waypoints[wp_cnt++]=clicked; sprintf(status_msg,"已添加途径点P%d: V%d",wp_cnt,clicked);}
                        else sprintf(status_msg,"V%d已在途径点中",clicked);
                    }
                } else if(select_mode==0){
                    src_nd=clicked; has_result=0; path_len=0;
                    sprintf(status_msg,"起点: V%d，请设置终点",clicked); select_mode=1;
                } else if(select_mode==1){
                    if(clicked==src_nd) strcpy(status_msg,"终点不能与起点相同！");
                    else{ dst_nd=clicked; has_result=0; path_len=0;
                          sprintf(status_msg,"终点: V%d，可添加途径点或点「计算」",clicked); select_mode=-1; }
                }
            }
        }
        InvalidateRect(hw,NULL,FALSE); break;
    }
    case WM_RBUTTONDOWN:{
        if(wp_cnt>0){ wp_cnt--; sprintf(status_msg,"已删除途径点P%d",wp_cnt+1); }
        else{ src_nd=dst_nd=-1; path_len=0; has_result=0; select_mode=0; strcpy(status_msg,"已清除，请重新选择起点"); }
        InvalidateRect(hw,NULL,FALSE); break;
    }
    case WM_KEYDOWN:
        if(wp==VK_ESCAPE) DestroyWindow(hw);
        break;
    case WM_DESTROY:
        DeleteObject(fnt_n); DeleteObject(fnt_b);
        DeleteObject(fnt_s); DeleteObject(fnt_t);
        DeleteDC(memDC); DeleteObject(memBmp);
        PostQuitMessage(0); break;
    default: return DefWindowProc(hw,msg,wp,lp);
    }
    return 0;
}

/* ============================================================
 * WinMain
 * ============================================================ */
int WINAPI WinMain(HINSTANCE hi,HINSTANCE hp,LPSTR cmd,int show){
    gen_graph(50);
    WNDCLASS wc={0};
    wc.lpfnWndProc=WndProc; wc.hInstance=hi;
    wc.lpszClassName="PathSys";
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    RegisterClass(&wc);
    g_hwnd=CreateWindow("PathSys","路网最短路径 - 数据结构实验",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        CW_USEDEFAULT,CW_USEDEFAULT,WIN_W,WIN_H,NULL,NULL,hi,NULL);
    ShowWindow(g_hwnd,show); UpdateWindow(g_hwnd);
    MSG m; while(GetMessage(&m,NULL,0,0)){ TranslateMessage(&m); DispatchMessage(&m); }
    return 0;
}
