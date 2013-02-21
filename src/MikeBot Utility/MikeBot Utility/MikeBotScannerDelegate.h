//
//  DiscoveryDelegate.h
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

@class MikeBotDevice;

@protocol MikeBotScannerDelegate <NSObject>

- (void) didAddDevice: (MikeBotDevice*) device;
- (void) didRemoveDevice: (MikeBotDevice*) device;

@end
