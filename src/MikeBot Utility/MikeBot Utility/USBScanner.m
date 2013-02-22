//
//  MikeBotDiscovery.m
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "USBScanner.h"

#import <IOKit/IOCFPlugIn.h>
#import <IOKit/usb/IOUSBLib.h>


#import "usb_product_and_vendor.h"
#import "USBDevice.h"

@implementation USBScanner

- (void) startScanning {
    CFMutableDictionaryRef      matchingDict;
    matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
    if (matchingDict == NULL) {
        //[NSException raise:<#(NSString *)#> format:<#(NSString *), ...#>];
        NSLog(@"IOServiceMatching returned NULL.");
        abort();
    }
    int vendorId = USB_VENDOR;
    int productId = USB_PRODUCT;
    
    CFNumberRef numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vendorId);
    CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorID), numberRef);
    CFRelease(numberRef);

    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productId);
    CFDictionarySetValue(matchingDict, CFSTR(kUSBProductID), numberRef);
    CFRelease(numberRef);
    numberRef = NULL;

    self.notificationPort = IONotificationPortCreate(kIOMasterPortDefault);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), IONotificationPortGetRunLoopSource(self.notificationPort), kCFRunLoopCommonModes);
    IOServiceAddMatchingNotification(self.notificationPort, kIOFirstMatchNotification, matchingDict, (IOServiceMatchingCallback)device_added, (__bridge void *)(self), &deviceIterator);

    device_added( self, deviceIterator);
    isScanning = YES;
}

- (void) stopScanning {
    if (isScanning) {
        CFRunLoopRemoveSource(CFRunLoopGetMain(), IONotificationPortGetRunLoopSource(self.notificationPort), kCFRunLoopDefaultMode);
        IONotificationPortDestroy(self.notificationPort);
        IOObjectRelease(deviceIterator);
        isScanning = NO;
    }
}

- (void) didAddDevice: (io_iterator_t) iterator {

    kern_return_t		kr;
    io_service_t		usbDevice;

    while ( (usbDevice = IOIteratorNext(iterator)) ) {
        USBDevice * device = [[USBDevice alloc] initWithService: usbDevice andScanner: self];

        kr = IOObjectRelease(usbDevice);

        [self.delegate didAddDevice: device];
    }
}

- (void) didRemoveDevice: (USBDevice*) device {
    [self.delegate didRemoveDevice: device];
}

- (void) dealloc {
    [self stopScanning];
}

#pragma mark - Core Foundation C Callbacks

void device_added(USBScanner * self, io_iterator_t iter) {
    [self didAddDevice: iter];
}


@end
