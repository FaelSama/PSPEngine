#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <malloc.h>
#include <pspctrl.h>

// PSP_MODULE_INFO is required
PSP_MODULE_INFO("game", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int exit_callback(int arg1, int arg2, void *common) {
	sceKernelExitGame();
	return 0;
}

int callback_thread(SceSize args, void *argp) {
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

int setup_callbacks(void) {
	int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	sceKernelStartThread(thid, 0, 0);
	return thid;
}

static unsigned int __attribute__((aligned(16))) list[262144];

typedef struct
{
	unsigned int color;
	float x, y, z;
}vertex;

int main(void)  {
/*	struct vertex __attribute__((aligned(16))) triangle[3] = {
    {0xFF00FF00, 0.35f, 0.0, -1.0f},
    {0xFF0000FF, -0.35f, 0.0, -1.0f},
    {0xFFFF0000, 0.0, 0.5f, -1.0f},
	};
*/
	SceCtrlData pad;

	//Triangulo
	vertex* triangle;
	triangle = (vertex*)memalign(16, 3 * sizeof(vertex));

	//Vertice 1
	triangle[0].color = 0xFF0000FF;
	triangle[0].x = -0.5f;
	triangle[0].y = -0.5f;
	triangle[0].z = -3.0f;

	//Vertice 2
	triangle[1].color = 0xFF00FF00;
	triangle[1].x =  0.5f;
	triangle[1].y = -0.5f;
	triangle[1].z =	-3.0f;

	//Vertice 3
	triangle[2].color = 0xFFFF0000;
	triangle[2].x =  0.0f;
	triangle[2].y =  0.5f;
	triangle[2].z = -3.0f;
	//

	sceGuInit();
	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,(void*)0,512);
	sceGuDispBuffer(480,272,(void*)0x88000,512);
	sceGuDepthBuffer((void*)0x110000,512);
	sceGuOffset(2048 - (480/2),2048 - (272/2));
	sceGuViewport(2048,2048,480,272);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,480,272);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
//	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	
	sceGuFinish();
    sceGuSync(0,0);
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	// Callback da XMB
	setup_callbacks();

	//Matrizes
	
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
//	sceGumOrtho(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f, -10.0f, 10.0f);
	sceGumPerspective(45.0f,16.0f/9.0f,2.0f,1000.0f);

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();

	float x, y, z, rot;
	while(1) {
		sceCtrlReadBufferPositive(&pad, 1);


		//StartFrame
		sceGuStart(GU_DIRECT,list);

		sceGuDisable(GU_DEPTH_TEST);
		sceGuDisable(GU_TEXTURE_2D);

		sceGuClearColor(0xFF000000);
		sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);

		if(pad.Buttons != 0)
		{
			if(pad.Buttons & PSP_CTRL_LTRIGGER)
			{
				rot-=0.05f;
			}
			if(pad.Buttons & PSP_CTRL_RTRIGGER)
			{
				rot+=0.05f;
			}
			if(pad.Buttons & PSP_CTRL_LEFT)
			{
				x-=0.05f;
			}
			if(pad.Buttons & PSP_CTRL_RIGHT)
			{
				x+=0.05f;
			}
			if(pad.Buttons & PSP_CTRL_UP)
			{
				y+=0.05f;
			}
			if(pad.Buttons & PSP_CTRL_DOWN)
			{
				y-=0.05f;
			}
			if(pad.Buttons & PSP_CTRL_CROSS)
			{
				z-=0.05f;
			}
			if(pad.Buttons & PSP_CTRL_TRIANGLE)
			{
				z+=0.05f;
			}
		}

		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		ScePspFVector3 rotate = {0.0f, rot, 0.0f};
		sceGumRotateXYZ(&rotate);
		
		ScePspFVector3 trans = {x, y, z};
		sceGumTranslate(&trans);

		sceGumDrawArray(GU_TRIANGLES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D, 3, 0, triangle);

		//EndFrame
		sceGuFinish();
		sceGuSync(0, 0);
		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
	}

	sceGuTerm();
	return 0;
}


