//
//  CLISession.m
//  MikeBot Setup
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "CLISession.h"

#import "CLICommand.h"
#import "SerialPort.h"
#import "USBDevice.h"
#import "ConfigCommand.h"
#import "MikeBot.h"

@interface CLISession ()

- (void) runNext;

@end

@implementation CLISession

- (id) initWithMikeBot: (MikeBot*) aMikeBot usbDevice: (USBDevice*) device {
    self = [super init];
    if (self != nil) {
        commandQueue = [[NSMutableArray alloc] init];
        mikeBot = aMikeBot;
        serialPort = [[SerialPort alloc] initWithPath: device.ttyDeviceFilename baudrate: 57600 dataBits: 8 parity:ParityNone stopBits: 1];
        serialPort.delegate = self;

        [mikeBot addObserver:self forKeyPath:@"midiChannel" options:NSKeyValueObservingOptionNew context:nil];
        
        [self queueCommand: [CLICommand command: @"C"]];
        [self queueCommand: [[ConfigCommand alloc] initWithMikeBot: mikeBot]];
    }
    return self;
}

- (void) queueCommand: (id) command {
    [commandQueue addObject: command];
    if (currentCommand == nil) {
        [self runNext];
    }
}

- (void) runNext {
    if ([commandQueue count] > 0) {
        currentCommand = (CLICommand*)[commandQueue objectAtIndex: 0];

        [commandQueue removeObjectAtIndex: 0];
        response = [[NSMutableData alloc] init];
        NSData * c = currentCommand.data;
        serialPort.fileHandle.writeabilityHandler =
        ^(NSFileHandle* fileHandle)
        {
            [fileHandle writeData: c];
            fileHandle.writeabilityHandler = nil;
        };
    } else {
        currentCommand = nil;
    }
}

- (void) didClose {
    serialPort.fileHandle.writeabilityHandler = nil;
    serialPort.delegate = nil;
}

- (void) didReceiveMessage:(NSData *)message {
    [response appendData: message];
    if ([response length] >= 4 && strncmp([[response subdataWithRange: NSMakeRange([response length] - 4, 4)] bytes], "\r\n> ", 4) == 0) {
        [currentCommand handleResponse: [response subdataWithRange: NSMakeRange(0, [response length] - 4)]];
        [self runNext];
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context
{
    if (object == mikeBot && [keyPath isEqualToString: @"midiChannel"]) {
        NSString * cmd = [NSString stringWithFormat: @"set %@ %@\r\n", keyPath, [change objectForKey: @"new"]];
        [self queueCommand: [CLICommand command: cmd]];
    }
}

- (void) dealloc {
    [mikeBot removeObserver: self forKeyPath: @"midiChannel"];
}

@end
