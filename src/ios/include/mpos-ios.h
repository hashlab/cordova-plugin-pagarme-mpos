//
//  mpos-ios.h
//  mpos-ios
//
//  Created by Daniel Ferreira on 10/07/15.
//  Copyright (c) 2015 Pagar.me. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <ExternalAccessory/ExternalAccessory.h>
#import "abecs.h"
#import "packet.h"
#import "mpos.h"

FOUNDATION_EXPORT double mpos_iosVersionNumber;
FOUNDATION_EXPORT const unsigned char mpos_iosVersionString[];

#define PMMposControllerErrorDomain @"me.pagar.mposios"

@interface PMEmvApplication : NSObject
+ (PMEmvApplication *)applicationWithCardBrand:(NSString *)brand paymentMethod:(mpos_payment_method_t)paymentMethod;
@property (nonatomic, retain) NSString *cardBrand;
@property (nonatomic, assign) mpos_payment_method_t paymentMethod;
@end

@interface PMMposController : NSObject
- (id)initWithAccessory:(EAAccessory *)accessory encryptionKey:(NSString *)key;

- (void)openConnection;
- (void)closeConnection;

- (void)openSessionWithCallback:(void(^)(NSError *))block;
- (void)closeSessionWithMessage:(NSString *)message callback:(void(^)(NSError *))block;

- (void)downloadEMVTablesToDeviceWithCallback:(void(^)(BOOL, NSError *))cb forceUpdate:(BOOL)force;
- (void)displayText:(NSString *)text;
- (void)payAmount:(NSInteger)amount withApplications:(NSArray *)applications magstripePaymentMethod:(mpos_payment_method_t)magstripePaymentMethod withCallback:(void(^)(NSString *, NSDictionary *, NSError *))block;
- (void)finishTransactionWithSuccessfulConnection:(BOOL)connected responseCode:(int)responseCode emvData:(NSString *)emvData withCallback:(void(^)(NSError *))block;
- (void)cancelOperationWithCallback:(void(^)())block;
@end
