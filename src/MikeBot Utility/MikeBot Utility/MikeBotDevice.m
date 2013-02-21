//
//  MikeBotDevice.m
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "MikeBotDevice.h"

@implementation MikeBotDevice

- (IOUSBDeviceInterface***) deviceInterfacePtr {
    return & _deviceInterface;
}

- (io_object_t*) notificationPtr {
    return & _notification;
}

- (void) dealloc {
    NSLog(@"MikeBotDevice dealloc");
}

@end
