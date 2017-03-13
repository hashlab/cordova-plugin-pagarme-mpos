#import "MposPlugin.h"
#import "mpos-ios.h"

@implementation Mpos {
	PMMposController *controller;
	CDVInvokedUrlCommand *listenerCommand;
}

- (void)searchAccessories:(CDVInvokedUrlCommand *)command {
	[[self commandDelegate] runInBackground:^{
		NSArray *connectedAccessories = [[EAAccessoryManager sharedAccessoryManager] connectedAccessories];
		NSMutableArray *accessories = [NSMutableArray array];
		for (EAAccessory *accessory in connectedAccessories) {
			[accessories addObject:[NSNumber numberWithUnsignedInteger:[accessory connectionID]]];
		}
		
		CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsArray:accessories];
		[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
	}];
}

- (void)accessoryDidConnect:(NSNotification *)notification {
	EAAccessory *accessory = [[notification userInfo] objectForKey:EAAccessoryKey];
	
	NSDictionary *parameters = @{ @"type": @"connected", @"id": [NSNumber numberWithUnsignedInteger:[accessory connectionID]] };

	CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsDictionary:parameters];
	[[self commandDelegate] sendPluginResult:result callbackId:[listenerCommand callbackId]];		
}
- (void)accessoryDidDisconnect:(NSNotification *)notification {
	EAAccessory *accessory = [[notification userInfo] objectForKey:EAAccessoryKey];

	NSDictionary *parameters = @{ @"type": @"disconnected", @"id": [NSNumber numberWithUnsignedInteger:[accessory connectionID]] };	
	
	CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsDictionary:parameters];
	[[self commandDelegate] sendPluginResult:result callbackId:[listenerCommand callbackId]];			
}

- (void)listenDevices:(CDVInvokedUrlCommand *)command {
	// Minimize damage in case -listenDevices: is called twice.
	if (listenerCommand != nil) [listenerCommand release];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	listenerCommand = [command retain];
	
	[[EAAccessoryManager sharedAccessoryManager] registerForLocalNotifications];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(accessoryDidConnect:) name:EAAccessoryDidConnectNotification object:[EAAccessoryManager sharedAccessoryManager]];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(accessoryDidDisconnect:) name:EAAccessoryDidDisconnectNotification object:[EAAccessoryManager sharedAccessoryManager]];
}
- (void)stopListeningDevices:(CDVInvokedUrlCommand *)command {
	[[NSNotificationCenter defaultCenter] removeObserver:self];	
	[[EAAccessoryManager sharedAccessoryManager] unregisterForLocalNotifications];
	
	if (listenerCommand != nil) [listenerCommand release];
}

- (void)initialize:(CDVInvokedUrlCommand *)command {
	if (controller != nil) [controller release];

	EAAccessory *device = nil;
	NSArray *connectedAccessories = [[EAAccessoryManager sharedAccessoryManager] connectedAccessories];
	for (EAAccessory *accessory in connectedAccessories) {
		if ([accessory connectionID] == [[[command arguments] objectAtIndex:0] unsignedIntegerValue]) {
			device = accessory;
			break;
		}
	}
	if (device == nil) {
		CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:@"Device ID not found"];
		[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
	}
	
	controller = [[PMMposController alloc] initWithAccessory:device encryptionKey:[[command arguments] objectAtIndex:1]];
	
	CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
	[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
}

- (void)openConnection:(CDVInvokedUrlCommand *)command {
	[controller openConnection];

	CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
	[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
}

- (void)closeConnection:(CDVInvokedUrlCommand *)command {
	[controller closeConnection];

	CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
	[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
}

- (void)openSession:(CDVInvokedUrlCommand *)command {
	[controller openSessionWithCallback:^(NSError *error){
		CDVPluginResult *result;
		if (error != nil) {
			result = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageToErrorObject:[error code]];
		}
		else {
			result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
		}

		[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
	}];
}

- (void)closeSession:(CDVInvokedUrlCommand *)command {
	[controller closeSessionWithMessage:[[command arguments] objectAtIndex:0] callback:^(NSError *error){
		CDVPluginResult *result;
		if (error != nil) {
			result = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageToErrorObject:[error code]];
		}
		else {
			result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
		}

		[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];		
	}];
}

- (void)downloadTables:(CDVInvokedUrlCommand *)command {
	[controller downloadEMVTablesToDeviceWithCallback:^(BOOL loaded, NSError *error){
		if (error != nil) {
			CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageToErrorObject:[error code]];
			[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
		}
		else {
			CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsBool:loaded];
			[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
		}
	} forceUpdate:[[[command arguments] objectAtIndex:0] boolValue]];
}

- (void)display:(CDVInvokedUrlCommand *)command {
	[controller displayText:[[command arguments] objectAtIndex:0]];
	
	CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];	
	[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];			
}

- (void)pay:(CDVInvokedUrlCommand *)command {
	id appParam = [[command arguments] objectAtIndex:1];
	NSMutableArray *applications;

	if ([appParam isKindOfClass:[NSNull class]]) {
		applications = nil;
	}
	else {
		applications = [NSMutableArray array];
		for (NSDictionary *object in appParam) {
			if (![object objectForKey:@"payment_method"] || ![[object objectForKey:@"payment_method"] isKindOfClass:[NSString class]]
				|| ![object objectForKey:@"card_brand"] || ![[object objectForKey:@"card_brand"] isKindOfClass:[NSString class]]) {
				
				CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:@"Invalid applications passed."];
				[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
				return;
			}
			
			mpos_payment_method_t method = [[object objectForKey:@"payment_method"] isEqualToString:@"debit_card"] ? MPM_DEBIT : MPM_CREDIT;

			PMEmvApplication *emvApplication = [PMEmvApplication applicationWithCardBrand:[object objectForKey:@"card_brand"] paymentMethod:method];
			[applications addObject:emvApplication];
		}
	}
	mpos_payment_method_t magstripeMethod = [[[command arguments] objectAtIndex:2] isEqualToString:@"debit_card"] ? MPM_DEBIT : MPM_CREDIT;
	[controller payAmount:[[[command arguments] objectAtIndex:0] intValue] withApplications:applications magstripePaymentMethod:magstripeMethod withCallback:^(NSString *cardHash, NSDictionary *paymentInfo, NSError *error){
		if (error != nil) {
			CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageToErrorObject:[error code]];
			[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];
		}
		else {
			NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:paymentInfo];
			[dict setObject:cardHash forKey:@"CardHash"];

			CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsDictionary:dict];
			[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];			
		}		
	}];
}

- (void)finish:(CDVInvokedUrlCommand *)command {
	NSArray *arguments = [command arguments];
	[controller finishTransactionWithSuccessfulConnection:[[arguments objectAtIndex:0] boolValue] responseCode:[[arguments objectAtIndex:1] intValue] emvData:[arguments objectAtIndex:2] withCallback:^(NSError *error){
		CDVPluginResult *result;
		if (error != nil) {
			result = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageToErrorObject:[error code]];
		}
		else {
			result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
		}

		[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];		
	}];
}

- (void)cancel:(CDVInvokedUrlCommand *)command {
	[controller cancelOperationWithCallback:^{
		CDVPluginResult *result = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
		[[self commandDelegate] sendPluginResult:result callbackId:[command callbackId]];		
	}];
}
@end
