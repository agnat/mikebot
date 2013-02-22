//
//  ConfigCommand.h
//  MikeBot Setup
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "CLICommand.h"

@class MikeBot;

@interface ConfigCommand : CLICommand
{
    MikeBot * mikeBot;
}

- (id) initWithMikeBot: (MikeBot*) mikeBot;

@end
