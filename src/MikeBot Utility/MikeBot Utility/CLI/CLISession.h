//
//  CLISession.h
//  MikeBot Setup
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "SerialPortDelegate.h"

@class CLICommand;
@class USBDevice;
@class MikeBot;
@class SerialPort;

@interface CLISession : NSObject <SerialPortDelegate>
{
    NSMutableArray * commandQueue;
    CLICommand *     currentCommand;
    NSMutableData * response;
    SerialPort    * serialPort;
    MikeBot       * mikeBot;
}

- (id) initWithMikeBot: (MikeBot*) mikeBot usbDevice: (USBDevice*) device;

- (void) queueCommand: (id) command;

@end
