//
//  MikeBotDiscovery.h
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "DiscoveryDelegate.h"

@class MikeBotDevice;

@interface MikeBotDiscovery : NSObject
{
    IONotificationPortRef notificationPort;
    BOOL                  isScanning;
    io_iterator_t         deviceIterator;
}

@property (nonatomic, retain) id<DiscoveryDelegate> delegate;

- (void) discover;
- (void) stop;

- (void) didAddDevice: (io_iterator_t) iter;
- (void) didRemoveDevice: (MikeBotDevice*) device;

- (void) dealloc;

@end
