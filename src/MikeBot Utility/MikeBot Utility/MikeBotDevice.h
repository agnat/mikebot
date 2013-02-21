//
//  MikeBotDevice.h
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <IOKit/usb/IOUSBLib.h>

@class MikeBotScanner;

@interface MikeBotDevice : NSObject

@property (nonatomic, retain) NSString* ttyDeviceFilename;

@property (assign) IOUSBDeviceInterface **deviceInterface;
@property (assign) io_object_t notification;
@property (nonatomic, retain) MikeBotScanner* scanner;

- (IOUSBDeviceInterface***) deviceInterfacePtr;
- (io_object_t*) notificationPtr;

- (void) dealloc;

@end
