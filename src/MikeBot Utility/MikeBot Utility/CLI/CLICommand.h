//
//  CLICommand.h
//  MikeBot Setup
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CLICommand : NSObject

@property (nonatomic, retain) NSData* data;

+ (CLICommand*) command: (NSString*) cmd;

- (void) handleResponse: (NSData*) response;

@end
