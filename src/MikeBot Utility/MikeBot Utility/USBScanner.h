//
//  MikeBotDiscovery.h
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "USBScannerDelegate.h"

@class USBDevice;

@interface USBScanner : NSObject
{
    BOOL                  isScanning;
    io_iterator_t         deviceIterator;
}

@property (nonatomic, retain) id<USBScannerDelegate> delegate;
@property (assign)            IONotificationPortRef  notificationPort;

- (void) startScanning;
- (void) stopScanning;

- (void) didAddDevice: (io_iterator_t) iter;
- (void) didRemoveDevice: (USBDevice*) device;

- (void) dealloc;

@end
