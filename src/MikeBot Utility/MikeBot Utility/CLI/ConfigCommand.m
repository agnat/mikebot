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
    //NSLog(@"%@", [[NSString alloc] initWithData: response encoding: NSASCIIStringEncoding]);
    NSError * error = nil;
    NSDictionary* config = [NSJSONSerialization JSONObjectWithData: response options: 0 error: & error];
    if (config == nil) {
        NSLog(@"json error: %@", error);
    } else {
        if ( ! [mikeBot.name isEqualToString: [config objectForKey: @"name"]]) {
            mikeBot.name = [config objectForKey: @"name"];
        }
        if ( ! [mikeBot.subtitle isEqualToString: [config objectForKey: @"subtitle"]]) {
            mikeBot.subtitle = [config objectForKey: @"subtitle"];
        }
        if ( ! [mikeBot.midiChannel isEqualToNumber: [config objectForKey: @"midiChannel"]]) {
            mikeBot.midiChannel = [config objectForKey: @"midiChannel"];
        }
    }
}

@end
