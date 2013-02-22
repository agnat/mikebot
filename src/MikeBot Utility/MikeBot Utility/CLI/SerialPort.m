//
//  SerialPort.m
//  MikeBot Setup
//
//  Created by David Siegel on 17.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "SerialPort.h"

#import <sys/ioctl.h>
#import <IOKit/serial/ioss.h>

@implementation SerialPort

- (id) initWithPath: (NSString*) path baudrate: (int) baudrate dataBits: (int) bits parity: (ParityMode) parity stopBits: (int) stopBits
{
    self = [super init];
    if (self != nil) {
        int fd = open([path cStringUsingEncoding: NSASCIIStringEncoding], O_RDWR | O_NOCTTY | O_NONBLOCK);
        /*
        if (fcntl(fd, F_SETFL, 0) == -1)
        {
            NSLog(@"ERROR - failed to clear O_NONBLOCK: %s (%d)", strerror(errno), errno);
        }*/

        self.fileHandle = [[NSFileHandle alloc] initWithFileDescriptor: fd];

        if (fd == -1) {
            NSLog(@"ERROR - open() failed: %s (%d)", strerror(errno), errno);
        }
        if (tcgetattr(fd, &originalAttributes) == -1) {
            NSLog(@"ERROR - tcgetattr() failed: %s (%d)", strerror(errno), errno);
        }

        attributes = originalAttributes;

        attributes.c_cflag &= ~CSIZE;
        switch (bits) {
            case 5: attributes.c_cflag |= CS5; break;
            case 6: attributes.c_cflag |= CS6; break;
            case 7: attributes.c_cflag |= CS7; break;
            case 8: attributes.c_cflag |= CS8; break;
            default:
                NSLog(@"Unsupported char size %d", bits);
        }

        switch (parity) {
            case ParityNone:
                attributes.c_cflag &= ~PARENB;
                break;
            case ParityOdd:
                attributes.c_cflag |= PARENB;
                attributes.c_cflag |= PARODD;
                break;
            case ParityEven:
                attributes.c_cflag |= PARENB;
                attributes.c_cflag &= ~PARODD;
                break;
        }

        switch (stopBits) {
            case 1: attributes.c_cflag &= ~CSTOPB; break;
            case 2: attributes.c_cflag |=  CSTOPB; break;
            default:
                NSLog(@"Unhandled stop bit count %d", stopBits);
        }

        //attributes.c_lflag |= ICANON;
        attributes.c_cflag |= CLOCAL;

        if (tcsetattr(fd, TCSANOW, &attributes) == -1) {
            NSLog(@"ERROR - tcsetattr() failed: %s (%d)", strerror(errno), errno);
        }

        if (ioctl(fd, IOSSIOSPEED, &baudrate) == -1) {
            NSLog(@"ERROR - failed to set baudrate");
        }
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(readMessage:)
                                                     name: NSFileHandleReadCompletionNotification
                                                   object: self.fileHandle];
        if (ioctl(fd, TIOCSDTR) == -1) {
            NSLog(@"ERROR - failed to set DTR: %s (%d)", strerror(errno), errno);
        }

        [self.fileHandle readInBackgroundAndNotify];

    }
    return self;
}

- (void) closeFile {
    if (tcdrain(self.fileHandle.fileDescriptor) == -1) {
        NSLog(@"Error waiting for drain - %s(%d)", strerror(errno), errno);
    }
    if (ioctl(self.fileHandle.fileDescriptor, TIOCCDTR) == -1) {
        NSLog(@"ERROR - failed to clear DTR: %s (%d)", strerror(errno), errno);
    }
    if (tcsetattr(self.fileHandle.fileDescriptor, TCSANOW, &originalAttributes) == -1) {
        NSLog(@"Error resetting tty attributes - %s(%d)", strerror(errno), errno);
    }

    [self.fileHandle closeFile];
}

- (void) readMessage: (NSNotification*) notification {
    NSData * data = [[notification userInfo]
                     objectForKey:NSFileHandleNotificationDataItem];
    if ([data length]){
        [self.delegate didReceiveMessage: data];
        [[notification object] readInBackgroundAndNotify];
    } else {
        [self.delegate didClose];
    }
}

@end
