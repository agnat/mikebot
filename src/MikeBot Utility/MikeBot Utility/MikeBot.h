//
//  MikeBot.h
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface MikeBot : NSManagedObject

@property (nonatomic, retain) NSString * serialNumber;
@property (nonatomic, retain) NSString * name;
@property (nonatomic, retain) NSString * subtitle;

@end
