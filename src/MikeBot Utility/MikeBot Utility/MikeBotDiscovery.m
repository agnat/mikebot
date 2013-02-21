//
//  MikeBotDiscovery.m
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "MikeBotDiscovery.h"

#import <IOKit/IOMessage.h>
#import <IOKit/IOCFPlugIn.h>
#import <IOKit/usb/IOUSBLib.h>
#import <IOKit/serial/IOSerialKeys.h>


#import "usb_product_and_vendor.h"
#import "MikeBotDevice.h"

@implementation MikeBotDiscovery

- (void) discover {
    CFMutableDictionaryRef      matchingDict;
    matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
    if (matchingDict == NULL) {
        //[NSException raise:<#(NSString *)#> format:<#(NSString *), ...#>];
        fprintf(stderr, "IOServiceMatching returned NULL.\n");
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

- (void) stop {
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

    while ( (usbDevice = IOIteratorNext(iterator)) )
    {
        printf("Device 0x%08x added.\n", usbDevice);

        kr = IOCreatePlugInInterfaceForService(usbDevice, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &score);

        if ((kIOReturnSuccess != kr) || !plugInInterface)
        {
            printf("unable to create a plugin (%08x)\n", kr);
            continue;
        }

        MikeBotDevice * device = [[MikeBotDevice alloc] init];
        device.scanner = self;
        
        // I have the device plugin, I need the device interface
        //
        res = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID)[device deviceInterfacePtr]);
        (*plugInInterface)->Release(plugInInterface);			// done with this
        if (res || ! device.deviceInterface)
        {
            printf("couldn't create a device interface (%08x)\n", (int) res);
            continue;
        }

        FindDeviceFilename(device.deviceInterface);

        kr = IOServiceAddInterestNotification(notificationPort,			// notifyPort
                                              usbDevice,			// service
                                              kIOGeneralInterest,		// interestType
                                              device_removed,		// callback
                                              (void*)CFBridgingRetain(device),			// refCon
                                              [device notificationPtr]	// notification
                                              );

        if (KERN_SUCCESS != kr)
        {
            printf("IOServiceAddInterestNotification returned 0x%08x\n", kr);
        }
        
        // Done with this io_service_t
        //
        kr = IOObjectRelease(usbDevice);

        [self.delegate didAddDevice: device];
    }
}

- (void) didRemoveDevice: (MikeBotDevice*) device {
    [self.delegate didRemoveDevice: device];
}

- (void) dealloc {
    [self stop];
}

void device_added(MikeBotDiscovery * self, io_iterator_t iter) {
    [self didAddDevice: iter];
}

void device_removed(void * refcon, io_service_t service, natural_t messageType, void *	messageArgument) {
    kern_return_t	kr;
    if (messageType == kIOMessageServiceIsTerminated)
    {
        printf("Device 0x%08x removed.\n", service);

        // Dump our private data to stdout just to see what it looks like.

        //
        MikeBotDevice * device = (__bridge MikeBotDevice *)(refcon);
        [device.scanner didRemoveDevice: device];
        
        if ( device.deviceInterface )
            kr = (*device.deviceInterface)->Release(device.deviceInterface);

        kr = IOObjectRelease(device.notification);
        
        CFBridgingRelease(refcon);

    }
}

IOReturn FindDeviceFilename(IOUSBDeviceInterface **device)
{
    IOReturn                    kr;
    IOUSBFindInterfaceRequest   request;
    io_iterator_t               iterator;
    io_service_t                usbInterface;

    request.bInterfaceClass = kUSBCommunicationDataInterfaceClass;
    request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
    request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
    request.bAlternateSetting = kIOUSBFindInterfaceDontCare;

    kr = (*device)->CreateInterfaceIterator(device,
                                            &request, &iterator);
    while ((usbInterface = IOIteratorNext(iterator)))
    {
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
            CFShow(deviceNameAsCFString);
        }
        kr = IOObjectRelease(usbInterface);
        break;
    }
    return kr;
}


@end
