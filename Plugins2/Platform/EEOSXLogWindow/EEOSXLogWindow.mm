//
//  EEOSXLogWindow.m
//  EEOSXLogWindow
//
//  Created by Kenneth Hurley on 7/20/13.
//  Copyright (c) 2013 Social Systems Technology. All rights reserved.
//

#import "EEOSXLogWindow.h"

@interface EEOSXLogWindow : NSObject
{
    NSWindowController *mLogWindowController;
    NSWindow *logWindow;
    NSTextView *textLogView;
}
@property (assign) NSWindowController *mLogWindowController;
@property (assign) NSWindow *logWindow;
@property (assign) NSTextView *textLogView;

@end

@implementation EEOSXLogWindow

@synthesize mLogWindowController;
@synthesize logWindow;
@synthesize textLogView;

NSWindowController *mLogWindowController;
NSWindow *logWindow;
NSTextView *textLogView;

- (void)CreateWindow
{
    NSRect frame = NSMakeRect(200, 200, 800, 600);
    logWindow = [[[NSWindow alloc] initWithContentRect:frame
                                        styleMask:(NSTitledWindowMask | NSResizableWindowMask | NSMiniaturizableWindowMask)
                                        backing:NSBackingStoreBuffered
                                        defer:NO] autorelease];
    
    [logWindow setTitle:@"Elemental Engine 3 OSX Log Window"];
    
    NSScrollView *scrollview = [[NSScrollView alloc] initWithFrame:[[logWindow contentView] frame]];
    
    NSSize contentSize = [scrollview contentSize];
    
    [scrollview setBorderType:NSNoBorder];
    [scrollview setHasVerticalScroller:YES];
    [scrollview setHasHorizontalScroller:NO];
    [scrollview setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    
    textLogView = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, contentSize.width, contentSize.height)];
    [textLogView setMinSize:NSMakeSize(0.0, contentSize.height)];
    [textLogView setMaxSize:NSMakeSize(FLT_MAX, FLT_MAX)];
    [textLogView setVerticallyResizable:YES];
    [textLogView setHorizontallyResizable:NO];
    [textLogView setAutoresizingMask:NSViewWidthSizable];
    [[textLogView textContainer] setContainerSize:NSMakeSize(contentSize.width, FLT_MAX)];
    [[textLogView textContainer] setWidthTracksTextView:YES];
    
    [scrollview setDocumentView:textLogView];

    [logWindow setContentView:scrollview];
        
    [logWindow makeFirstResponder:textLogView];
    mLogWindowController = [[NSWindowController alloc] initWithWindow:logWindow];
    [logWindow makeKeyAndOrderFront:nil];
    
}

- (void)DestroyWindow
{
    [textLogView release];
    [logWindow release];
    [mLogWindowController release];
}

- (void)AddText:(NSString *)outString
{
    [[[textLogView textStorage] mutableString] appendString:outString];
}

@end

static EEOSXLogWindow *EEOSXWin = [EEOSXLogWindow alloc];

void OSXCreateLogWindow()
{
    [EEOSXWin CreateWindow];
}

void OSXDestroyLogWindow()
{
    [EEOSXWin DestroyWindow];
}

void OSXLogWindowMessage(char *msg)
{
    size_t sLen = strlen(msg);
    NSString *outString = [[NSString alloc] initWithBytes:msg length:sLen encoding:NSASCIIStringEncoding];
    [EEOSXWin AddText:outString];
}
