//
//  ConfigCommand.m
//  MikeBot Setup
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "ConfigCommand.h"

#import "MikeBot.h"

@implementation ConfigCommand

- (id) initWithMikeBot: (MikeBot*) bot {
    self = [super init];
    if (self != nil) {
        self.data = [@"config\n" dataUsingEncoding:NSUTF8StringEncoding];
        mikeBot = bot;
    }
    return self;
}

- (void) handleResponse:(NSData *)response {
    NSError * error = nil;
    NSDictionary* json = [NSJSONSerialization JSONObjectWithData: response options: 0 error: & error];
    if (json == nil) {
        NSLog(@"json error: %@", error);
    } else {
        mikeBot.name = [json objectForKey: @"name"];
        mikeBot.subtitle = [json objectForKey: @"subtitle"];
        mikeBot.midiChannel = [json objectForKey: @"midiChannel"];
    }
}
@end
