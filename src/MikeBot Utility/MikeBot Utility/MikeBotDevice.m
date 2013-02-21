//
//  MikeBotDevice.m
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "MikeBotDevice.h"

#import <IOKit/serial/IOSerialKeys.h>

@implementation MikeBotDevice

- (IOUSBDeviceInterface***) deviceInterfacePtr {
    return & _deviceInterface;
}

- (io_object_t*) notificationPtr {
    return & _notification;
}

- (void) dealloc {
    NSLog(@"MikeBotDevice dealloc");
    kern_return_t kr;
    if ( _deviceInterface ) {
        kr = (*_deviceInterface)->Release(_deviceInterface);
        if (kr != KERN_SUCCESS) {
            NSLog(@"failed to release device interface");
        }
    }
    kr = IOObjectRelease(_notification);
    if (kr != KERN_SUCCESS) {
        NSLog(@"failed to release notification");
    }
}

- (NSString*) ttyDeviceFilename {
    if (_ttyDeviceFilename == nil) {
        [self findTTYDeviceFilename];
    }
    return _ttyDeviceFilename;
}

- (void) findTTYDeviceFilename {
    IOReturn                    kr;
    IOUSBFindInterfaceRequest   request;
    io_iterator_t               iterator;
    io_service_t                usbInterface;

    request.bInterfaceClass = kUSBCommunicationDataInterfaceClass;
    request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
    request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
    request.bAlternateSetting = kIOUSBFindInterfaceDontCare;

    kr = (*_deviceInterface)->CreateInterfaceIterator(_deviceInterface,
                                            &request, &iterator);
    while ((usbInterface = IOIteratorNext(iterator))) {
        io_registry_entry_t child;
        IORegistryEntryGetChildEntry(usbInterface, kIOServicePlane, & child );
        IORegistryEntryGetChildEntry(child, kIOServicePlane, & child );
        IORegistryEntryGetChildEntry(child, kIOServicePlane, & child );

        CFTypeRef deviceNameAsCFString;
        deviceNameAsCFString = IORegistryEntryCreateCFProperty (
                                                                child,
                                                                CFSTR(kIODialinDeviceKey),
                                                                kCFAllocatorDefault,0);
        if (deviceNameAsCFString) {
            self.ttyDeviceFilename = CFBridgingRelease(deviceNameAsCFString);
        }
        kr = IOObjectRelease(usbInterface);
        break;
    }
}

@end
