//
//  EEOSXLogWindow.h
//  EEOSXLogWindow
//
//  Created by Kenneth Hurley on 7/20/13.
//  Copyright (c) 2013 Social Systems Technology. All rights reserved.
//

#ifdef __OBJC__
#import <Foundation/Foundation.h>

#endif

void OSXCreateLogWindow();
void OSXDestroyLogWindow();
void OSXLogWindowMessage(char *msg);

