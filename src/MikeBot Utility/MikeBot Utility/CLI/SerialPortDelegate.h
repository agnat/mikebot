//
//  SerialPortDelegate.h
//  MikeBot Setup
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol SerialPortDelegate <NSObject>

- (void) didReceiveMessage: (NSData*) message;
- (void) didClose;

@end
