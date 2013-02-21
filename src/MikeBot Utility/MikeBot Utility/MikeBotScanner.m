//
//  MikeBotDiscovery.m
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "MikeBotScanner.h"

#import <IOKit/IOMessage.h>
#import <IOKit/IOCFPlugIn.h>
#import <IOKit/usb/IOUSBLib.h>


#import "usb_product_and_vendor.h"
#import "MikeBotDevice.h"

@implementation MikeBotScanner

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

    notificationPort = IONotificationPortCreate(kIOMasterPortDefault);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), IONotificationPortGetRunLoopSource(notificationPort), kCFRunLoopCommonModes);
    IOServiceAddMatchingNotification(notificationPort, kIOFirstMatchNotification, matchingDict, (IOServiceMatchingCallback)device_added, (__bridge void *)(self), &deviceIterator);

    device_added( self, deviceIterator);
    isScanning = YES;
}

- (void) stopScanning {
    if (isScanning) {
        CFRunLoopRemoveSource(CFRunLoopGetMain(), IONotificationPortGetRunLoopSource(notificationPort), kCFRunLoopDefaultMode);
        IONotificationPortDestroy(notificationPort);
        IOObjectRelease(deviceIterator);
        isScanning = NO;
    }
}

- (void) didAddDevice: (io_iterator_t) iterator {

    kern_return_t		kr;
    io_service_t		usbDevice;
    IOCFPlugInInterface 	**plugInInterface=NULL;
    SInt32 			score;
    HRESULT 			res;

    while ( (usbDevice = IOIteratorNext(iterator)) ) {
        kr = IOCreatePlugInInterfaceForService(usbDevice, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);

        if ((kIOReturnSuccess != kr) || !plugInInterface)
        {
            NSLog(@"unable to create a plugin (%08x)", kr);
            continue;
        }

        MikeBotDevice * device = [[MikeBotDevice alloc] init];
        device.scanner = self;
        
        res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID)[device deviceInterfacePtr]);
        (*plugInInterface)->Release(plugInInterface);			// done with this
        if (res || ! device.deviceInterface)
        {
            NSLog(@"couldn't create a device interface (%08x)", (int) res);
            continue;
        }

        kr = IOServiceAddInterestNotification(notificationPort,			// notifyPort
                                              usbDevice,			// service
                                              kIOGeneralInterest,		// interestType
                                              device_removed,		// callback
                                              (void*)CFBridgingRetain(device),			// refCon
                                              [device notificationPtr]	// notification
                                              );

        if (KERN_SUCCESS != kr)
        {
            NSLog(@"IOServiceAddInterestNotification returned 0x%08x", kr);
        }

        kr = IOObjectRelease(usbDevice);

        [self.delegate didAddDevice: device];
    }
}

- (void) didRemoveDevice: (MikeBotDevice*) device {
    [self.delegate didRemoveDevice: device];
}

- (void) dealloc {
    [self stopScanning];
}

#pragma mark - Core Foundation C Callbacks

void device_added(MikeBotScanner * self, io_iterator_t iter) {
    [self didAddDevice: iter];
}

void device_removed(void * refcon, io_service_t service, natural_t messageType, void *	messageArgument) {
    if (messageType == kIOMessageServiceIsTerminated) {
        MikeBotDevice * device = (__bridge MikeBotDevice *)(refcon);
        
        [device.scanner didRemoveDevice: device];
        
        CFBridgingRelease(refcon);
    }
}

@end
