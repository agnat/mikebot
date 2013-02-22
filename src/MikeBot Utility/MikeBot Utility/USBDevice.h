//
//  MikeBotDevice.h
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <IOKit/usb/IOUSBLib.h>

@class USBScanner;

@interface USBDevice : NSObject
{
    IOUSBDeviceInterface **deviceInterface;
    io_object_t notification;
}

@property (nonatomic, retain) NSString*       serialNumber;
@property (nonatomic, retain) NSString*       ttyDeviceFilename;
@property (nonatomic, retain) USBScanner* scanner;

- (id) initWithService: (io_service_t) service andScanner: (USBScanner*) scanner;
- (void) dealloc;

@end
