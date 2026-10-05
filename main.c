/* Stage1 MarioKartPSP main.c */
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdebug.h>
#include <psprtc.h>
#include <math.h>
#include <string.h>

PSP_MODULE_INFO("MarioKartPSP",0,1,0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);

#define BUF_WIDTH 512
#define SCR_WIDTH 480
#define SCR_HEIGHT 272
#define NUM_KARTS 6
#define NUM_WAYPOINTS 24
#define MAX_LAPS 3

static unsigned int __attribute__((aligned(16))) guList[262144];

typedef struct{float x,y,z;} Vec3;
typedef struct{unsigned int color;float x,y,z;} Vertex;
typedef struct{
 Vec3 pos; float rotY,speed; int lap,currentWaypoint,rank,isAI;
} Kart;

static Kart g_karts[NUM_KARTS];
static Vec3 g_waypoints[NUM_WAYPOINTS];

static Vertex kartVerts[]={
 {0xffffffff,-0.5f,-0.2f,0.5f},
 {0xffffffff, 0.5f,-0.2f,0.5f},
 {0xffffffff, 0.0f, 0.2f,0.5f}
};

static void initWaypoints(){for(int i=0;i<NUM_WAYPOINTS;i++){float t=((float)i/NUM_WAYPOINTS)*6.283185f;g_waypoints[i].x=cosf(t)*40.0f;g_waypoints[i].z=sinf(t)*40.0f;g_waypoints[i].y=0;}}

static void initRace(){initWaypoints();for(int i=0;i<NUM_KARTS;i++){memset(&g_karts[i],0,sizeof(Kart));g_karts[i].pos.x=i*2.0f;g_karts[i].lap=1;g_karts[i].currentWaypoint=1;g_karts[i].rank=i+1;g_karts[i].isAI=(i==0)?0:1;}}

static void updateAI(Kart*k,float dt){Vec3 t=g_waypoints[k->currentWaypoint];float dx=t.x-k->pos.x,dz=t.z-k->pos.z;float d=sqrtf(dx*dx+dz*dz);if(d<3.0f)k->currentWaypoint=(k->currentWaypoint+1)%NUM_WAYPOINTS;float ta=atan2f(dx,dz);float diff=ta-k->rotY;k->rotY+=diff*2.0f*dt;k->speed=10; k->pos.x+=sinf(k->rotY)*k->speed*dt; k->pos.z+=cosf(k->rotY)*k->speed*dt;}

static void updatePlayer(SceCtrlData*pad,float dt){Kart*k=&g_karts[0];if(pad->Buttons&PSP_CTRL_CROSS)k->speed+=10*dt;if(pad->Lx<100)k->rotY+=2*dt;if(pad->Lx>155)k->rotY-=2*dt;k->speed*=0.985f;k->pos.x+=sinf(k->rotY)*k->speed*dt;k->pos.z+=cosf(k->rotY)*k->speed*dt;}

int main(){pspDebugScreenInit();sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);sceGuInit();initRace();u64 last=sceKernelGetSystemTimeWide();SceCtrlData pad;while(1){sceCtrlReadBufferPositive(&pad,1);if(pad.Buttons&PSP_CTRL_START)break;u64 now=sceKernelGetSystemTimeWide();float dt=(now-last)/1000000.0f;last=now;updatePlayer(&pad,dt);for(int i=1;i<NUM_KARTS;i++)updateAI(&g_karts[i],dt);pspDebugScreenSetXY(0,0);pspDebugScreenPrintf("Lap:%d/%d Rank:%d
",g_karts[0].lap,MAX_LAPS,g_karts[0].rank);}sceKernelExitGame();return 0;}
