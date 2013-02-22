//
//  MikeBotDevice.m
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "USBDevice.h"

#import <IOKit/IOCFPlugIn.h>
#import <IOKit/IOMessage.h>
#import <IOKit/serial/IOSerialKeys.h>

#import "USBScanner.h"

@implementation USBDevice

- (id) initWithService: (io_service_t) service andScanner: (USBScanner*) scanner {
    self = [super init];
    if (self != nil) {
        self.scanner = scanner;
        
        kern_return_t kr;
        IOCFPlugInInterface 	**plugInInterface=NULL;
        SInt32 			score;
        HRESULT 			res;

        kr = IOCreatePlugInInterfaceForService(service, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);

        if ((kIOReturnSuccess != kr) || !plugInInterface) {
            NSLog(@"unable to create a plugin (%08x)", kr);
        }

        res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID)& deviceInterface);
        (*plugInInterface)->Release(plugInInterface);			// done with this
        if (res || ! deviceInterface) {
            NSLog(@"couldn't create a device interface (%08x)", (int) res);
        }

        kr = IOServiceAddInterestNotification(scanner.notificationPort,			// notifyPort
                                              service,			// service
                                              kIOGeneralInterest,		// interestType
                                              device_removed,		// callback
                                              (void*)CFBridgingRetain(self),			// refCon
                                              &notification	// notification
                                              );

        if (KERN_SUCCESS != kr)
        {
            NSLog(@"IOServiceAddInterestNotification returned 0x%08x", kr);
        }

        CFTypeRef serialNumber = IORegistryEntryCreateCFProperty(service, CFSTR(kUSBSerialNumberString),kCFAllocatorDefault,0);
        if (serialNumber) {
            self.serialNumber = CFBridgingRelease(serialNumber);
        }

    }
    return self;
}

- (void) dealloc {
    kern_return_t kr;
    if ( deviceInterface ) {
        kr = (*deviceInterface)->Release(deviceInterface);
        if (kr != KERN_SUCCESS) {
            NSLog(@"failed to release device interface");
        }
    }
    kr = IOObjectRelease(notification);
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

    kr = (*deviceInterface)->CreateInterfaceIterator(deviceInterface,
                                            &request, &iterator);
    while ((usbInterface = IOIteratorNext(iterator))) {
        io_registry_entry_t child;
        IORegistryEntryGetChildEntry(usbInterface, kIOServicePlane, & child );
        IORegistryEntryGetChildEntry(child, kIOServicePlane, & child );
        IORegistryEntryGetChildEntry(child, kIOServicePlane, & child );

        CFTypeRef ttyFilename = IORegistryEntryCreateCFProperty(child, CFSTR(kIODialinDeviceKey),kCFAllocatorDefault,0);
        if (ttyFilename) {
            self.ttyDeviceFilename = CFBridgingRelease(ttyFilename);
        }
        kr = IOObjectRelease(usbInterface);
        break;
    }
}

#pragma mark - Core Foundation C Callbacks

void device_removed(void * refcon, io_service_t service, natural_t messageType, void *	messageArgument) {
    if (messageType == kIOMessageServiceIsTerminated) {
        USBDevice * device = (__bridge USBDevice *)(refcon);

        [device.scanner didRemoveDevice: device];

        CFBridgingRelease(refcon);
    }
}


@end
