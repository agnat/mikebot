//
//  AppDelegate.h
//  MikeBot Utility
//
//  Created by David Siegel on 21.02.13.
//  Copyright (c) 2013 David Siegel. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "USBScanner.h"

@interface AppDelegate : NSObject <NSApplicationDelegate, USBScannerDelegate>
{
    USBScanner * scanner;
    NSMutableDictionary * sessions;
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSArrayController *mikeBotArray;
@property (assign) IBOutlet NSPopUpButton * midiChannelPopUp;
@property (assign) IBOutlet NSTextField * firmwareVersion;
@property (assign) IBOutlet NSTextField * serialNumber;

@property (readonly, strong, nonatomic) NSPersistentStoreCoordinator *persistentStoreCoordinator;
@property (readonly, strong, nonatomic) NSManagedObjectModel *managedObjectModel;
@property (readonly, strong, nonatomic) NSManagedObjectContext *managedObjectContext;

- (IBAction)saveAction:(id)sender;

@end
