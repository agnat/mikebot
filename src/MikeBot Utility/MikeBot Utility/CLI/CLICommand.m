//
//  CLICommand.m
//  MikeBot Setup
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "CLICommand.h"

@implementation CLICommand

+ (CLICommand*) command:(NSString *)cmd {
    CLICommand * c = [[CLICommand alloc] init];
    c.data = [cmd dataUsingEncoding:NSUTF8StringEncoding];
    return c;
}

- (void) handleResponse: (NSData*) response {
}

@end
