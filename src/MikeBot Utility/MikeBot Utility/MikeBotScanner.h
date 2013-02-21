//
//  MikeBotDiscovery.h
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "MikeBotScannerDelegate.h"

@class MikeBotDevice;

@interface MikeBotScanner : NSObject
{
    IONotificationPortRef notificationPort;
    BOOL                  isScanning;
    io_iterator_t         deviceIterator;
}

@property (nonatomic, retain) id<MikeBotScannerDelegate> delegate;

- (void) startScanning;
- (void) stopScanning;

- (void) didAddDevice: (io_iterator_t) iter;
- (void) didRemoveDevice: (MikeBotDevice*) device;

- (void) dealloc;

@end
