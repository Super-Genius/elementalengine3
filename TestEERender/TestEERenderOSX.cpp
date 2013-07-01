//
//  TestEERenderOSX.c
//  TestEERender
//
//  Created by Kenneth Hurley on 6/30/13.
//
//

#include <stdio.h>
#include <OpenGL/OpenGL.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <AGL/agl.h>

#include "ElementalEngine.hpp"

WindowRef       mainWindow;
IToolBox        *gToolBox;
DLLPRIORITYMAP gDLLPMap;

// renderer and context
IRenderer       *gRenderer;
IRenderContext  *gRenderContext;

// test texture
IBaseTextureObject  *gTestTexture;

unsigned int GetDisplayWidth()
{
	return (unsigned int)CGDisplayPixelsWide( kCGDirectMainDisplay );
}

unsigned int GetDisplayHeight()
{
	return (unsigned int)CGDisplayPixelsHigh( kCGDirectMainDisplay );
}

void HideMouseCursor()
{
	CGDisplayHideCursor( kCGNullDirectDisplay );
	CGAssociateMouseAndMouseCursorPosition( false );
	CGDisplayMoveCursorToPoint( kCGDirectMainDisplay, CGPointZero );
}

void ShowMouseCursor()
{
	CGAssociateMouseAndMouseCursorPosition( true );
	CGDisplayShowCursor( kCGNullDirectDisplay );
}

WindowRef CreateWindow(unsigned int width, unsigned int height)
{
    
    WindowRef window;
    
    unsigned int screenWidth = GetDisplayWidth();
    unsigned int screenHeight = GetDisplayHeight();

    Rect rect;
    rect.left = ( screenWidth - width ) / 2;
    rect.top = ( screenHeight - height ) / 2;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;

    OSErr result = CreateNewWindow( kDocumentWindowClass,
                                   ( kWindowStandardDocumentAttributes |
                                    //									  kWindowNoTitleBarAttribute |
                                    kWindowStandardHandlerAttribute ) &
                                   ~kWindowResizableAttribute,
                                   &rect, &window );

    if ( result != noErr )
        return NULL;

    SetWindowTitleWithCFString( window, CFStringCreateWithCString( 0, "TestEERender", CFStringGetSystemEncoding() ) );
    
    return window;

}

pascal OSStatus WindowEvents (EventHandlerCallRef  nextHandler,
                                   EventRef             theEvent,
                                   void*                userData)
{

    UInt32    eventClass;
    UInt32    eventKind;
    eventClass = GetEventClass (theEvent);
    eventKind  = GetEventKind (theEvent);
    
    switch (eventKind)
    {
        case kEventWindowClosed:
            QuitApplicationEventLoop();
            return noErr;
    }
    
    return eventNotHandledErr;
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
    
    EventTypeSpec myEventSpecification = {kEventClassWindow, kEventWindowClosed};
    
    EventHandlerUPP handlerUPP;
    handlerUPP = NewEventHandlerUPP(WindowEvents);
    
    InstallWindowEventHandler(mainWindow, handlerUPP, 1, &myEventSpecification, NULL, NULL);
#endif
    
    // call Init Elemental Engine here.
    
    gToolBox = EngineGetToolBox();
    
    if (gToolBox->LoadPlugins(_T("./Plugins/*.dlo"), gDLLPMap) != noErr)
    {
        DialogRef alertDialog;
        CreateStandardAlert(kAlertStopAlert, CFSTR("Loading Error!"), CFSTR("Unable to load plugins, check log file."), NULL, &alertDialog);
        RunStandardAlert (alertDialog, NULL, NULL);
        
        return 2;
    }
    
    if (gToolBox->InitPlugins(gDLLPMap) != noErr)
    {
        DialogRef alertDialog;
        CreateStandardAlert(kAlertStopAlert, CFSTR("Loading Error!"), CFSTR("Unable to initialize plugins, check log file."), NULL, &alertDialog);
        RunStandardAlert (alertDialog, NULL, NULL);
        
        return 2;
    }
    
    if (mainWindow != NULL)
    {
        ShowWindow( mainWindow );
        SelectWindow( mainWindow );
    }

    RunApplicationEventLoop();
    
    DisposeWindow( (WindowPtr) mainWindow );
    return 0;

}