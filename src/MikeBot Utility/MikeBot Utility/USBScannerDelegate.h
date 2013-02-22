//
//  DiscoveryDelegate.h
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

@class USBDevice;

@protocol USBScannerDelegate <NSObject>

- (void) didAddDevice: (USBDevice*) usbDevice;
- (void) didRemoveDevice: (USBDevice*) usbDevice;

@end
