//
//  LitLabel.m
//  MikeBot Utility
//
//  Created by David Siegel on 22.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import "LitLabel.h"

@implementation LitLabel

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
    }
    
    return self;
}

- (void)drawRect:(NSRect)dirtyRect {
    NSRect rect = [self bounds];
    float radius = rect.size.height / 2;
    NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius: radius yRadius: radius];
    [path addClip];

    if (self.isEnabled ) {
        [[NSColor greenColor] set];
    } else {
        [[NSColor lightGrayColor] set];
    }
    NSRectFill(dirtyRect);

    [super drawRect: dirtyRect];
}

@end
