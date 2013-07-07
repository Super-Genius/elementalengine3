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

int main( int argc, char * argv[] )
{
    
    size_t DisplayWidth = 800;
    size_t DisplayHeight = 600;
    
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

    gRenderer = GetRendererInterface();
    
    gRenderer->Initialize(mainWindow, false, DisplayWidth, DisplayHeight, 24, GetDisplayBPP());
    
    /* Make the window's context current */
    glfwMakeContextCurrent(mainWindow);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(mainWindow))
    {
        /* Render here */
        
        /* Swap front and back buffers */
        glfwSwapBuffers(mainWindow);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}