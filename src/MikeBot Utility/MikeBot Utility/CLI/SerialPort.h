//
//  SerialPort.h
//  MikeBot Setup
//
//  Created by David Siegel on 17.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <termios.h>

#import "SerialPortDelegate.h"

typedef enum ParityMode {
    ParityNone,
    ParityOdd,
    ParityEven
} ParityMode;

@interface SerialPort : NSObject
{
    struct termios originalAttributes;
    struct termios attributes;
}

@property (nonatomic, retain) NSFileHandle * fileHandle;
@property (nonatomic, retain) id<SerialPortDelegate> delegate;

- (id) initWithPath: (NSString*) path baudrate: (int) baudrate dataBits: (int) bits parity: (ParityMode) parity stopBits: (int) stopBits;

- (void) closeFile;

- (void) readMessage: (NSNotification*) notification;

@end
