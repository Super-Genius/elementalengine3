//
//  TestEERenderOSX.c
//  TestEERender
//
//  Created by Kenneth Hurley on 6/30/13.
//
//

#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>

#include "GLFW/glfw3.h"
#include "ElementalEngine.hpp"

GLFWwindow *mainWindow;
IToolBox        *gToolBox;
DLLPRIORITYMAP gDLLPMap;

// renderer and context
IRenderer       *gRenderer;
IRenderContext  *gRenderContext;

// test texture
IBaseTextureObject  *gTestTexture;

unsigned int GetDisplayWidth()
{
    const GLFWvidmode *vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return (unsigned int)vidMode->width;
}

unsigned int GetDisplayHeight()
{
    const GLFWvidmode *vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return (unsigned int)vidMode->height;
}

unsigned int GetDisplayBPP()
{
    const GLFWvidmode *vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return (unsigned int)(vidMode->redBits + vidMode->greenBits + vidMode->blueBits);
}



GLFWwindow *CreateWindow(unsigned int width, unsigned int height)
{
    
    GLFWwindow* window;
    
    // Initialize the library
    if (!glfwInit())
        return NULL;
    
    // do not show window yet.
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return NULL;
    }
    
    return window;
    

}

void RenderScene()
{
    gRenderer->SetBackgroundColor(255, 255, 255, 0);
    gRenderer->ClearScreen(true, true);
    
    gRenderer->RenderToContext(gRenderContext);
    gRenderer->BeginScene(true);
    gRenderer->Draw2DQuad(100.0f, 100.0f, 250.0f, 250.0f, NULL, 0xffff0000);
    // basic texture modulate
    gRenderer->SetMaterial(0, NULL);
    gRenderer->Draw2DQuad(350.0f, 350.0f, 250.0f, 250.0f, gTestTexture, 0xff0000ff);
    gRenderer->EndScene();
    gRenderer->Present(gRenderContext);
    
    static UINT64 iPreviousTickCount = GetTickCount();
    UINT64 iCurrentTickCount = GetTickCount();
    UINT64 iDeltaTickCount = iCurrentTickCount - iPreviousTickCount;
    iPreviousTickCount = iCurrentTickCount;
    
    GLOBALTICKPARAMS gtp;
    gtp.tickCount = (DWORD)iDeltaTickCount;
    static DWORD msgGlobalUpdateTick = CHashString(_T("GlobalUpdateTick")).GetUniqueID();
    gToolBox->SendMessage(msgGlobalUpdateTick, sizeof(GLOBALTICKPARAMS), &gtp, NULL, NULL);
    
}
int main( int argc, char * argv[] )
{
    
    size_t DisplayWidth = 1424;
    size_t DisplayHeight = 730;
    
    mainWindow = NULL;
#ifdef FULLSCREEN
	DisplayHeight = GetDisplayHeight();
	DisplayWidth = GetDisplayWidth();
#else
    mainWindow = CreateWindow(DisplayWidth, DisplayHeight);
    if (mainWindow == NULL)
    {
		fprintf( stderr, "Failed to create window!\n" );
		return 1;
    }
#endif
    
    // call Init Elemental Engine here.
    
    gToolBox = EngineGetToolBox();
    
    if (gToolBox->LoadPlugins(_T("./Plugins/*.dlo"), gDLLPMap) != noErr)
    {
        CFUserNotificationDisplayNotice(0, kCFUserNotificationNoteAlertLevel,
                                        NULL, NULL, NULL, CFSTR("Loading Error!"),
                                        CFSTR("Unable to load plugins, check log file."), NULL);
        return 2;
    }
    
    if (gToolBox->InitPlugins(gDLLPMap) != noErr)
    {
        CFUserNotificationDisplayNotice(0, kCFUserNotificationNoteAlertLevel,
                                        NULL, NULL, NULL, CFSTR("Initializing Error!"),
                                        CFSTR("Unable to initialize plugins, check log file."), NULL);
        return 2;
    }
    
    if (mainWindow != NULL)
    {
        glfwShowWindow(mainWindow );
    }

    gRenderer = EEGetRendererInterface();
    
    // Make the window's context current
    glfwMakeContextCurrent(mainWindow);
    
    unsigned int bpp = GetDisplayBPP();
    gRenderer->Initialize(mainWindow, false, DisplayWidth, DisplayHeight, 24, bpp);
    
    gRenderContext = gRenderer->CreateNewContext(0, DisplayWidth, DisplayHeight, 24, bpp);
    
    gTestTexture = EELoadTexture(_T("Textures/wall_512x512.jpg"));
    
    
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(mainWindow))
    {
        // Render here 
        
        RenderScene();
        
        // Swap front and back buffers
        glfwSwapBuffers(mainWindow);
        
        // Poll for and process events
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}