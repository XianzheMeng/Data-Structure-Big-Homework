#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ========== 颜色与尺寸常量 ========== */
#define C_BG         RGB(245,247,250)
#define C_SIDEBAR    RGB(28, 54, 100)
#define C_DISABLED   RGB(120,140,170)
#define C_WHITE      RGB(255,255,255)
#define C_ACCENT     RGB(46, 117,182)
#define C_TEXT       RGB(33,  37, 41)
#define C_TEXTLT     RGB(120,130,150)
#define C_GREEN      RGB(40, 167, 69)
#define C_RED        RGB(210, 50, 50)
#define C_TITLEBG    RGB(28,  54,100)
#define C_YELLOW     RGB(255,200, 50)

#define WIN_W    960
#define WIN_H    640
#define TOPBAR_H  50
#define SIDEBAR_W 210
#define ITEM_H    40
#define PANEL_X   (SIDEBAR_W+10)
#define PANEL_Y   (TOPBAR_H+10)
#define PANEL_W   (WIN_W-SIDEBAR_W-20)
#define PANEL_H   (WIN_H-TOPBAR_H-20)

/* ========== 数据结构 ========== */
#define MAX_NAME     64
#define MAX_MENUS    50
#define MAX_CH       10
#define MAX_ROLES    10
#define MAX_USERS    20

typedef struct MenuItem {
    int id;
    char name[MAX_NAME];
    char action[32];
    int  level;
    struct MenuItem *ch[MAX_CH];
    int  ch_cnt;
} MenuItem;

typedef struct { int id; char name[MAX_NAME]; int allowed[MAX_MENUS]; int a_cnt; } Role;
typedef struct { int id; char uname[MAX_NAME]; char pwd[MAX_NAME]; int role_id; } User;

/* ========== 全局数据 ========== */
MenuItem  menus[MAX_MENUS]; int menu_cnt=0;
MenuItem *roots[MAX_CH];    int root_cnt=0;
Role      roles[MAX_ROLES]; int role_cnt=0;
User      users[MAX_USERS]; int user_cnt=0;

User *cur_user=NULL;
Role *cur_role=NULL;

int  cur_page=0;          // 0=登录, 1=主界面
int  sel_root=-1;
int  cur_panel=-1;        // 0=权限配置,1=用户管理,2=修改密码,3=系统日志,4=备份恢复,5=基础查询,6=高级查询

char ln_user[MAX_NAME]="", ln_pass[MAX_NAME]="", ln_err[64]="";
int  ln_focus=0;          // 0=用户名, 1=密码

/* ---------- 权限配置面板 ---------- */
int  perm_role_sel=0;                // 当前选择角色索引
int  perm_temp[MAX_MENUS]={0};       // 临时勾选状态，1=有权限，0=无

/* ---------- 用户管理面板 ---------- */
char um_new_uname[MAX_NAME]="", um_new_pwd[MAX_NAME]="";
int  um_new_role=0;
int  um_sel_user=-1;
int  um_input_focus=0;   // 0=用户名, 1=密码

/* ---------- 修改密码面板 ---------- */
char cp_old_pwd[MAX_NAME]="", cp_new_pwd[MAX_NAME]="", cp_msg[128]="";
int  cp_input_focus=0;   // 0=旧密码, 1=新密码

HWND g_hwnd;
HFONT fnt_normal, fnt_bold, fnt_small, fnt_title;

/* ========== 初始化数据 ========== */
static void add_menu(int id, const char* nm, const char* act, int lv, MenuItem** out) {
    MenuItem* m = &menus[menu_cnt++];
    m->id = id; strcpy(m->name, nm); strcpy(m->action, act);
    m->level = lv; m->ch_cnt = 0;
    if (out) *out = m;
}
void init_data() {
    MenuItem *m1,*m2,*m3,*tmp;
    add_menu(1,"系统管理","sys",1,&m1);
    add_menu(2,"用户管理","user_mgr",1,&m2);
    add_menu(3,"数据查询","query",1,&m3);
    add_menu(11,"权限配置","perm_config",2,&tmp); m1->ch[m1->ch_cnt++]=tmp;
    add_menu(12,"系统日志","sys_log",2,&tmp);    m1->ch[m1->ch_cnt++]=tmp;
    add_menu(13,"备份恢复","backup",2,&tmp);     m1->ch[m1->ch_cnt++]=tmp;
    add_menu(21,"新增用户","user_add",2,&tmp);   m2->ch[m2->ch_cnt++]=tmp;
    add_menu(22,"删除用户","user_del",2,&tmp);   m2->ch[m2->ch_cnt++]=tmp;
    add_menu(23,"修改密码","chg_pwd",2,&tmp);    m2->ch[m2->ch_cnt++]=tmp;
    add_menu(31,"基础查询","basic_query",2,&tmp); m3->ch[m3->ch_cnt++]=tmp;
    add_menu(32,"高级查询","adv_query",2,&tmp);  m3->ch[m3->ch_cnt++]=tmp;
    roots[root_cnt++]=m1; roots[root_cnt++]=m2; roots[root_cnt++]=m3;

    Role *r;
    r=&roles[role_cnt++]; r->id=1; strcpy(r->name,"系统管理员"); r->a_cnt=0;
    int a1[]={1,2,3,11,12,13,21,22,23,31,32};
    for(int i=0;i<11;i++) r->allowed[r->a_cnt++]=a1[i];
    r=&roles[role_cnt++]; r->id=2; strcpy(r->name,"普通用户"); r->a_cnt=0;
    int a2[]={3,31}; for(int i=0;i<2;i++) r->allowed[r->a_cnt++]=a2[i];
    r=&roles[role_cnt++]; r->id=3; strcpy(r->name,"操作员"); r->a_cnt=0;
    int a3[]={2,3,21,23,31,32}; for(int i=0;i<6;i++) r->allowed[r->a_cnt++]=a3[i];

    User *u;
    u=&users[user_cnt++]; u->id=1; strcpy(u->uname,"admin"); strcpy(u->pwd,"admin123"); u->role_id=1;
    u=&users[user_cnt++]; u->id=2; strcpy(u->uname,"zhangsan"); strcpy(u->pwd,"zs123"); u->role_id=2;
    u=&users[user_cnt++]; u->id=3; strcpy(u->uname,"lisi"); strcpy(u->pwd,"ls456"); u->role_id=3;
}

/* ========== 辅助函数 ========== */
Role *find_role(int id) { for(int i=0;i<role_cnt;i++) if(roles[i].id==id) return &roles[i]; return NULL; }
int has_perm(Role *r, int id) { if(!r)return 0; for(int i=0;i<r->a_cnt;i++) if(r->allowed[i]==id)return 1; return 0; }
MenuItem *find_menu(int id) { for(int i=0;i<menu_cnt;i++) if(menus[i].id==id) return &menus[i]; return NULL; }
User *find_user(const char *uname) { for(int i=0;i<user_cnt;i++) if(strcmp(users[i].uname,uname)==0) return &users[i]; return NULL; }

/* ========== 绘图基础工具 ========== */
static void fill_rect(HDC dc, int x,int y,int w,int h, COLORREF c) {
    HBRUSH b=CreateSolidBrush(c); RECT r={x,y,x+w,y+h}; FillRect(dc,&r,b); DeleteObject(b);
}
static void draw_text(HDC dc, int x,int y, const char* t, COLORREF c, HFONT f) {
    SelectObject(dc,f); SetTextColor(dc,c); SetBkMode(dc,TRANSPARENT);
    TextOut(dc,x,y,t,(int)strlen(t));
}
static void draw_border(HDC dc, int x,int y,int w,int h, COLORREF c, int thick) {
    HPEN p=CreatePen(PS_SOLID,thick,c); HPEN old=(HPEN)SelectObject(dc,p);
    HBRUSH nb=(HBRUSH)GetStockObject(NULL_BRUSH); HBRUSH ob=(HBRUSH)SelectObject(dc,nb);
    Rectangle(dc,x,y,x+w,y+h); SelectObject(dc,old); SelectObject(dc,ob); DeleteObject(p);
}
static void fill_rrect(HDC dc, int x,int y,int w,int h, int r, COLORREF c) {
    HBRUSH b=CreateSolidBrush(c); HPEN p=CreatePen(PS_SOLID,1,c);
    HBRUSH ob=(HBRUSH)SelectObject(dc,b); HPEN op=(HPEN)SelectObject(dc,p);
    RoundRect(dc,x,y,x+w,y+h,r,r); SelectObject(dc,ob); SelectObject(dc,op); DeleteObject(b); DeleteObject(p);
}
static int in_rect(int mx,int my, int x,int y,int w,int h) { return mx>=x&&mx<=x+w&&my>=y&&my<=y+h; }

/* ========== 权限配置面板：初始化 perm_temp ========== */
void load_perm_temp() {
    Role *r = &roles[perm_role_sel];
    for(int i=0;i<menu_cnt;i++) {
        perm_temp[i] = 0;
        if(menus[i].level==2 && has_perm(r, menus[i].id))
            perm_temp[i] = 1;
    }
}

/* 保存权限：用 perm_temp 更新角色 */
void save_permissions() {
    Role *r = &roles[perm_role_sel];
    r->a_cnt = 0;
    for(int i=0;i<menu_cnt;i++) {
        if(menus[i].level==2 && perm_temp[i]) {
            r->allowed[r->a_cnt++] = menus[i].id;
        }
    }
}

/* ========== 用户管理操作 ========== */
void add_user() {
    if(strlen(um_new_uname)==0||strlen(um_new_pwd)==0) return;
    if(find_user(um_new_uname)) {
        MessageBox(g_hwnd,"用户名已存在","错误",MB_OK|MB_ICONERROR);
        return;
    }
    if(user_cnt>=MAX_USERS) return;
    User *u=&users[user_cnt++];
    u->id=user_cnt+100; // 简单避免ID重复
    strcpy(u->uname,um_new_uname); strcpy(u->pwd,um_new_pwd);
    u->role_id=roles[um_new_role].id;
    strcpy(um_new_uname,""); strcpy(um_new_pwd,"");
}

void delete_user(int idx) {
    if(idx<0||idx>=user_cnt) return;
    if(strcmp(users[idx].uname,cur_user->uname)==0) {
        MessageBox(g_hwnd,"不能删除当前登录用户","错误",MB_OK|MB_ICONERROR);
        return;
    }
    for(int i=idx;i<user_cnt-1;i++) users[i]=users[i+1];
    user_cnt--;
}

void change_password() {
    if(strcmp(cp_old_pwd,cur_user->pwd)!=0) {
        strcpy(cp_msg,"旧密码错误"); return;
    }
    if(strlen(cp_new_pwd)==0) {
        strcpy(cp_msg,"新密码不能为空"); return;
    }
    strcpy(cur_user->pwd,cp_new_pwd);
    strcpy(cp_msg,"密码修改成功！"); strcpy(cp_old_pwd,""); strcpy(cp_new_pwd,"");
}

/* ========== 绘制登录界面 ========== */
void draw_login(HDC dc) {
    fill_rect(dc,0,0,WIN_W,WIN_H,C_TITLEBG);
    int cx=WIN_W/2-150, cy=WIN_H/2-190;
    fill_rrect(dc,cx,cy,300,380,10,C_WHITE);
    draw_text(dc,cx+50,cy+20,"菜单权限管理系统",C_TITLEBG,fnt_title);
    draw_text(dc,cx+70,cy+52,"数据结构实验",C_TEXTLT,fnt_small);
    draw_border(dc,cx+15,cy+78,270,1,RGB(220,225,235),1);
    draw_text(dc,cx+18,cy+88,"用户名",C_TEXT,fnt_normal);
    COLORREF ub = (ln_focus==0)?C_ACCENT:RGB(200,205,215);
    draw_border(dc,cx+15,cy+108,270,30,ub, ln_focus==0?2:1);
    draw_text(dc,cx+22,cy+112, ln_user[0]?ln_user:"请输入用户名", ln_user[0]?C_TEXT:C_TEXTLT,fnt_normal);
    draw_text(dc,cx+18,cy+148,"密  码",C_TEXT,fnt_normal);
    COLORREF pb = (ln_focus==1)?C_ACCENT:RGB(200,205,215);
    draw_border(dc,cx+15,cy+168,270,30,pb, ln_focus==1?2:1);
    char stars[MAX_NAME]=""; int pl=(int)strlen(ln_pass);
    for(int i=0;i<pl&&i<32;i++) strcat(stars,"*");
    draw_text(dc,cx+22,cy+172, stars[0]?stars:"请输入密码", stars[0]?C_TEXT:C_TEXTLT,fnt_normal);
    if(ln_err[0]) draw_text(dc,cx+18,cy+208,ln_err,C_RED,fnt_small);
    fill_rrect(dc,cx+15,cy+234,270,38,6,C_ACCENT);
    draw_text(dc,cx+112,cy+246,"登  录",C_WHITE,fnt_bold);
    draw_text(dc,cx+10,cy+288,"测试账号:",C_TEXTLT,fnt_small);
    draw_text(dc,cx+10,cy+308,"admin / admin123  (管理员)",C_TEXTLT,fnt_small);
    draw_text(dc,cx+10,cy+326,"zhangsan / zs123  (普通用户)",C_TEXTLT,fnt_small);
    draw_text(dc,cx+10,cy+344,"lisi / ls456       (操作员)",C_TEXTLT,fnt_small);
}

/* ========== 绘制主界面 ========== */
void draw_main(HDC dc) {
    fill_rect(dc,0,0,WIN_W,WIN_H,C_BG);
    fill_rect(dc,0,0,WIN_W,TOPBAR_H,C_TITLEBG);
    draw_text(dc,16,12,"菜单权限管理系统",C_WHITE,fnt_bold);
    char info[128]; sprintf(info,"当前用户: %s  |  角色: %s", cur_user->uname, cur_role->name);
    draw_text(dc,WIN_W-420,16,info,RGB(180,200,230),fnt_small);
    fill_rrect(dc,WIN_W-76,10,64,30,5,C_RED);
    draw_text(dc,WIN_W-60,16,"退出",C_WHITE,fnt_normal);

    fill_rect(dc,0,TOPBAR_H,SIDEBAR_W,WIN_H-TOPBAR_H,C_SIDEBAR);
    int y=TOPBAR_H+8;
    for(int i=0;i<root_cnt;i++){
        MenuItem *root=roots[i];
        int perm=has_perm(cur_role,root->id);
        int open=(sel_root==i);
        if(open) fill_rect(dc,0,y,SIDEBAR_W,ITEM_H,C_ACCENT);
        if(open) fill_rect(dc,0,y+4,4,ITEM_H-8,C_YELLOW);
        fill_rect(dc,14,y+12,16,16, perm?(open?C_WHITE:C_ACCENT):C_DISABLED);
        draw_text(dc,38,y+10,root->name, perm?C_WHITE:C_DISABLED,fnt_normal);
        if(perm) { char s[2]={open?'-':'+',0}; draw_text(dc,SIDEBAR_W-20,y+10,s,C_WHITE,fnt_bold); }
        y+=ITEM_H;
        if(open){
            for(int j=0;j<root->ch_cnt;j++){
                MenuItem *sub=root->ch[j];
                int sp=has_perm(cur_role,sub->id);
                fill_rect(dc,0,y,SIDEBAR_W,ITEM_H,RGB(22,46,88));
                fill_rect(dc,24,y+18,8,8, sp?C_ACCENT:C_DISABLED);
                draw_text(dc,40,y+10,sub->name, sp?C_WHITE:C_DISABLED,fnt_normal);
                if(!sp) draw_text(dc,SIDEBAR_W-50,y+10,"无权限",C_DISABLED,fnt_small);
                y+=ITEM_H;
            }
        }
    }

    fill_rrect(dc,PANEL_X,PANEL_Y,PANEL_W,PANEL_H,8,C_WHITE);

    switch(cur_panel) {
        case 0: {
            draw_text(dc,PANEL_X+20,PANEL_Y+20,"角色权限配置",C_TITLEBG,fnt_bold);
            draw_text(dc,PANEL_X+20,PANEL_Y+55,"选择角色:",C_TEXT,fnt_normal);
            RECT r1={PANEL_X+120,PANEL_Y+52,PANEL_X+220,PANEL_Y+78};
            FillRect(dc,&r1,CreateSolidBrush(RGB(240,244,252)));
            draw_border(dc,PANEL_X+120,PANEL_Y+52,100,26,C_ACCENT,1);
            draw_text(dc,PANEL_X+128,PANEL_Y+55,roles[perm_role_sel].name,C_TEXT,fnt_normal);
            draw_text(dc,PANEL_X+232,PANEL_Y+55,"??",C_ACCENT,fnt_small);

            int px=PANEL_X+20, py=PANEL_Y+100;
            for(int i=0;i<menu_cnt;i++){
                if(menus[i].level==2){
                    int mid=menus[i].id;
                    int allowed = perm_temp[i];  // 使用临时状态
                    RECT box={px,py,px+20,py+20};
                    FillRect(dc,&box,CreateSolidBrush(allowed?C_GREEN:C_RED));
                    draw_text(dc,px+26,py+2,menus[i].name,C_TEXT,fnt_small);
                    py+=28;
                }
            }
            fill_rrect(dc,PANEL_X+20,PANEL_Y+PANEL_H-60,100,30,4,C_ACCENT);
            draw_text(dc,PANEL_X+48,PANEL_Y+PANEL_H-52,"保存权限",C_WHITE,fnt_normal);
            break;
        }
        case 1: {
            draw_text(dc,PANEL_X+20,PANEL_Y+20,"用户管理",C_TITLEBG,fnt_bold);
            draw_text(dc,PANEL_X+20,PANEL_Y+55,"用户名:",C_TEXT,fnt_normal);
            draw_border(dc,PANEL_X+100,PANEL_Y+52,140,26, um_input_focus==0?C_ACCENT:RGB(200,205,215), um_input_focus==0?2:1);
            draw_text(dc,PANEL_X+106,PANEL_Y+55,um_new_uname[0]?um_new_uname:"输入用户名",um_new_uname[0]?C_TEXT:C_TEXTLT,fnt_small);
            draw_text(dc,PANEL_X+260,PANEL_Y+55,"密码:",C_TEXT,fnt_normal);
            draw_border(dc,PANEL_X+320,PANEL_Y+52,140,26, um_input_focus==1?C_ACCENT:RGB(200,205,215), um_input_focus==1?2:1);
            draw_text(dc,PANEL_X+326,PANEL_Y+55,um_new_pwd[0]?um_new_pwd:"输入密码",um_new_pwd[0]?C_TEXT:C_TEXTLT,fnt_small);
            draw_text(dc,PANEL_X+480,PANEL_Y+55,"角色:",C_TEXT,fnt_normal);
            RECT r2={PANEL_X+540,PANEL_Y+52,PANEL_X+660,PANEL_Y+78};
            FillRect(dc,&r2,CreateSolidBrush(RGB(240,244,252)));
            draw_border(dc,PANEL_X+540,PANEL_Y+52,120,26,C_ACCENT,1);
            draw_text(dc,PANEL_X+546,PANEL_Y+55,roles[um_new_role].name,C_TEXT,fnt_small);
            fill_rrect(dc,PANEL_X+680,PANEL_Y+52,60,26,3,C_GREEN);
            draw_text(dc,PANEL_X+694,PANEL_Y+55,"新增",C_WHITE,fnt_small);

            draw_text(dc,PANEL_X+20,PANEL_Y+100,"现有用户:",C_TEXTLT,fnt_small);
            int uy=PANEL_Y+130;
            for(int i=0;i<user_cnt;i++){
                RECT bar={PANEL_X+20,uy,PANEL_X+PANEL_W-20,uy+30};
                FillRect(dc,&bar,CreateSolidBrush(um_sel_user==i ? RGB(200,220,255) : C_BG));
                char buf[256]; sprintf(buf,"%s  (角色: %s)",users[i].uname, find_role(users[i].role_id)->name);
                draw_text(dc,PANEL_X+30,uy+5,buf,C_TEXT,fnt_small);
                if(um_sel_user==i) {
                    fill_rrect(dc,PANEL_X+PANEL_W-100,uy+2,70,26,3,C_RED);
                    draw_text(dc,PANEL_X+PANEL_W-90,uy+5,"删除",C_WHITE,fnt_small);
                }
                uy+=35;
            }
            break;
        }
        case 2: {
            draw_text(dc,PANEL_X+20,PANEL_Y+20,"修改密码",C_TITLEBG,fnt_bold);
            draw_text(dc,PANEL_X+20,PANEL_Y+60,"旧密码:",C_TEXT,fnt_normal);
            draw_border(dc,PANEL_X+120,PANEL_Y+58,200,28, cp_input_focus==0?C_ACCENT:RGB(200,205,215), cp_input_focus==0?2:1);
            draw_text(dc,PANEL_X+126,PANEL_Y+61,cp_old_pwd[0]?cp_old_pwd:"输入旧密码", cp_old_pwd[0]?C_TEXT:C_TEXTLT,fnt_normal);
            draw_text(dc,PANEL_X+20,PANEL_Y+100,"新密码:",C_TEXT,fnt_normal);
            draw_border(dc,PANEL_X+120,PANEL_Y+98,200,28, cp_input_focus==1?C_ACCENT:RGB(200,205,215), cp_input_focus==1?2:1);
            draw_text(dc,PANEL_X+126,PANEL_Y+101,cp_new_pwd[0]?cp_new_pwd:"输入新密码", cp_new_pwd[0]?C_TEXT:C_TEXTLT,fnt_normal);
            fill_rrect(dc,PANEL_X+120,PANEL_Y+150,100,30,4,C_ACCENT);
            draw_text(dc,PANEL_X+148,PANEL_Y+158,"确认修改",C_WHITE,fnt_normal);
            if(cp_msg[0]) draw_text(dc,PANEL_X+20,PANEL_Y+200,cp_msg,C_RED,fnt_small);
            break;
        }
        case 3: draw_text(dc,PANEL_X+20,PANEL_Y+30,"系统日志：暂无日志记录",C_TEXTLT,fnt_normal); break;
        case 4: draw_text(dc,PANEL_X+20,PANEL_Y+30,"备份恢复：点击备份按钮执行模拟备份",C_TEXTLT,fnt_normal); break;
        case 5: draw_text(dc,PANEL_X+20,PANEL_Y+30,"基础查询：请输入查询条件...",C_TEXTLT,fnt_normal); break;
        case 6: draw_text(dc,PANEL_X+20,PANEL_Y+30,"高级查询：请输入SQL语句（模拟）",C_TEXTLT,fnt_normal); break;
        default: draw_text(dc,PANEL_X+20,PANEL_Y+30,"请点击左侧菜单项进行操作",C_TEXTLT,fnt_normal); break;
    }
}

/* ========== 菜单点击 → 面板索引 ========== */
int menu_to_panel(const char* action) {
    if(strcmp(action,"perm_config")==0) return 0;
    if(strcmp(action,"user_add")==0||strcmp(action,"user_del")==0) return 1;
    if(strcmp(action,"chg_pwd")==0) return 2;
    if(strcmp(action,"sys_log")==0) return 3;
    if(strcmp(action,"backup")==0) return 4;
    if(strcmp(action,"basic_query")==0) return 5;
    if(strcmp(action,"adv_query")==0) return 6;
    return -1;
}

/* ========== 登录验证 ========== */
void try_login() {
    ln_err[0]=0;
    for(int i=0;i<user_cnt;i++){
        if(strcmp(users[i].uname,ln_user)==0 && strcmp(users[i].pwd,ln_pass)==0){
            cur_user=&users[i]; cur_role=find_role(cur_user->role_id);
            cur_page=1; sel_root=-1; cur_panel=-1; return;
        }
    }
    strcpy(ln_err,"用户名或密码错误");
}

/* ========== 窗口过程 ========== */
LRESULT CALLBACK WndProc(HWND hw,UINT msg,WPARAM wp,LPARAM lp) {
    static HDC memDC=NULL;
    static HBITMAP memBmp=NULL;
    switch(msg) {
    case WM_CREATE: {
        HDC dc=GetDC(hw);
        memDC=CreateCompatibleDC(dc);
        memBmp=CreateCompatibleBitmap(dc,WIN_W,WIN_H);
        SelectObject(memDC,memBmp);
        ReleaseDC(hw,dc);
        fnt_normal=CreateFont(18,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,DEFAULT_QUALITY,0,"宋体");
        fnt_bold  =CreateFont(18,0,0,0,FW_BOLD,  0,0,0,DEFAULT_CHARSET,0,0,DEFAULT_QUALITY,0,"宋体");
        fnt_small =CreateFont(14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,DEFAULT_QUALITY,0,"宋体");
        fnt_title =CreateFont(22,0,0,0,FW_BOLD,  0,0,0,DEFAULT_CHARSET,0,0,DEFAULT_QUALITY,0,"宋体");
        SetTimer(hw,1,50,NULL);
        break;
    }
    case WM_TIMER:
    case WM_PAINT: {
        if(cur_page==0) draw_login(memDC);
        else draw_main(memDC);
        PAINTSTRUCT ps; HDC dc=BeginPaint(hw,&ps);
        BitBlt(dc,0,0,WIN_W,WIN_H,memDC,0,0,SRCCOPY);
        EndPaint(hw,&ps);
        break;
    }
    case WM_LBUTTONDOWN: {
        int mx=LOWORD(lp), my=HIWORD(lp);
        if(cur_page==0) {
            int cx=WIN_W/2-150, cy=WIN_H/2-190;
            if(in_rect(mx,my,cx+15,cy+108,270,30)) ln_focus=0;
            else if(in_rect(mx,my,cx+15,cy+168,270,30)) ln_focus=1;
            else if(in_rect(mx,my,cx+15,cy+234,270,38)) try_login();
            InvalidateRect(hw,NULL,FALSE);
        } else {
            if(in_rect(mx,my,WIN_W-76,10,64,30)) {
                cur_page=0; ln_user[0]=ln_pass[0]=ln_err[0]=0; sel_root=-1; cur_panel=-1;
                InvalidateRect(hw,NULL,FALSE); break;
            }
            // 左侧菜单点击
            int hit_id=0;
            int y=TOPBAR_H+8;
            if(mx>=0&&mx<SIDEBAR_W) {
                for(int i=0;i<root_cnt;i++){
                    if(my>=y&&my<y+ITEM_H) { hit_id=-(i+1); break; }
                    y+=ITEM_H;
                    if(sel_root==i){
                        for(int j=0;j<roots[i]->ch_cnt;j++){
                            if(my>=y&&my<y+ITEM_H) { hit_id=roots[i]->ch[j]->id; break; }
                            y+=ITEM_H;
                        }
                        if(hit_id) break;
                    }
                }
            }
            if(hit_id<0) {
                int idx=(-hit_id)-1;
                if(has_perm(cur_role,roots[idx]->id))
                    sel_root=(sel_root==idx)?-1:idx;
                else MessageBox(hw,"您没有访问此菜单的权限","权限不足",MB_OK|MB_ICONWARNING);
                cur_panel=-1;
            } else if(hit_id>0) {
                MenuItem *item=find_menu(hit_id);
                if(!has_perm(cur_role,hit_id)) {
                    MessageBox(hw,"您没有执行此操作的权限","权限不足",MB_OK|MB_ICONWARNING);
                } else {
                    int panel=menu_to_panel(item->action);
                    if(panel==0) {
                        perm_role_sel=0;
                        load_perm_temp();  // 初始化临时权限
                    } else if(panel==1) {
                        um_new_role=0; um_input_focus=0;
                    } else if(panel==2) {
                        cp_old_pwd[0]=cp_new_pwd[0]=cp_msg[0]=0; cp_input_focus=0;
                    }
                    cur_panel=panel;
                }
            }
            // 右侧面板交互
            if(cur_panel==0) {
                if(in_rect(mx,my,PANEL_X+232,PANEL_Y+55,40,26)) {
                    perm_role_sel=(perm_role_sel+1)%role_cnt;
                    load_perm_temp();
                }
                // 点击权限方块切换状态
                int px=PANEL_X+20, py=PANEL_Y+100;
                for(int i=0;i<menu_cnt;i++){
                    if(menus[i].level==2){
                        if(in_rect(mx,my,px,py,20,20)) {
                            perm_temp[i] = !perm_temp[i];
                            break;
                        }
                        py+=28;
                    }
                }
                if(in_rect(mx,my,PANEL_X+20,PANEL_Y+PANEL_H-60,100,30)) {
                    save_permissions();
                    MessageBox(hw,"权限已保存","提示",MB_OK|MB_ICONINFORMATION);
                }
            }
            else if(cur_panel==1) {
                if(in_rect(mx,my,PANEL_X+540,PANEL_Y+52,120,26)) {
                    um_new_role=(um_new_role+1)%role_cnt;
                }
                if(in_rect(mx,my,PANEL_X+680,PANEL_Y+52,60,26)) {
                    add_user();
                }
                // 切换输入焦点
                if(in_rect(mx,my,PANEL_X+100,PANEL_Y+52,140,26)) um_input_focus=0;
                if(in_rect(mx,my,PANEL_X+320,PANEL_Y+52,140,26)) um_input_focus=1;
                // 用户列表选择
                int uy=PANEL_Y+130;
                for(int i=0;i<user_cnt;i++){
                    if(in_rect(mx,my,PANEL_X+20,uy,PANEL_W-20,30)) {
                        um_sel_user=(um_sel_user==i)?-1:i;
                        break;
                    }
                    uy+=35;
                }
                // 删除按钮 —— 修复：动态计算当前选中用户的 Y 坐标
                if(um_sel_user!=-1) {
                    int del_y = PANEL_Y + 130 + um_sel_user * 35 + 2;  // +2 与绘制时 uy+2 对应
                    if(in_rect(mx,my,PANEL_X+PANEL_W-100, del_y, 70, 26)) {
                        delete_user(um_sel_user);
                        um_sel_user=-1;
                    }
                }
            }
            else if(cur_panel==2) {
                if(in_rect(mx,my,PANEL_X+120,PANEL_Y+58,200,28)) cp_input_focus=0;
                if(in_rect(mx,my,PANEL_X+120,PANEL_Y+98,200,28)) cp_input_focus=1;
                if(in_rect(mx,my,PANEL_X+120,PANEL_Y+150,100,30)) {
                    change_password();
                }
            }
            InvalidateRect(hw,NULL,FALSE);
        }
        break;
    }
    case WM_CHAR: {
        if(cur_page==0) {
            char ch=(char)wp;
            char *buf=(ln_focus==0)?ln_user:ln_pass;
            int len=(int)strlen(buf);
            if(ch=='\b') { if(len>0) buf[len-1]=0; }
            else if(ch=='\t') ln_focus=1-ln_focus;
            else if(ch=='\r') { if(ln_focus==0) ln_focus=1; else try_login(); }
            else if(len<MAX_NAME-1) { buf[len]=ch; buf[len+1]=0; }
            InvalidateRect(hw,NULL,FALSE);
        } else {
            char ch=(char)wp;
            if(cur_panel==1) {
                char *buf = (um_input_focus==0) ? um_new_uname : um_new_pwd;
                int len=(int)strlen(buf);
                if(ch=='\b') { if(len>0) buf[len-1]=0; }
                else if(len<MAX_NAME-1) { buf[len]=ch; buf[len+1]=0; }
                InvalidateRect(hw,NULL,FALSE);
            } else if(cur_panel==2) {
                char *buf = (cp_input_focus==0) ? cp_old_pwd : cp_new_pwd;
                int len=(int)strlen(buf);
                if(ch=='\b') { if(len>0) buf[len-1]=0; }
                else if(len<MAX_NAME-1) { buf[len]=ch; buf[len+1]=0; }
                InvalidateRect(hw,NULL,FALSE);
            }
        }
        break;
    }
    case WM_DESTROY:
        DeleteObject(fnt_normal); DeleteObject(fnt_bold); DeleteObject(fnt_small); DeleteObject(fnt_title);
        DeleteDC(memDC); DeleteObject(memBmp);
        PostQuitMessage(0); break;
    default: return DefWindowProc(hw,msg,wp,lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hi,HINSTANCE hp,LPSTR cmd,int show) {
    init_data();
    WNDCLASS wc={0};
    wc.lpfnWndProc=WndProc; wc.hInstance=hi; wc.lpszClassName="MenuSys";
    wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1); wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    RegisterClass(&wc);
    g_hwnd=CreateWindow("MenuSys","菜单权限管理系统 - 数据结构实验",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        CW_USEDEFAULT,CW_USEDEFAULT,WIN_W,WIN_H,NULL,NULL,hi,NULL);
    ShowWindow(g_hwnd,show); UpdateWindow(g_hwnd);
    MSG m; while(GetMessage(&m,NULL,0,0)){ TranslateMessage(&m); DispatchMessage(&m); }
    return 0;
}
