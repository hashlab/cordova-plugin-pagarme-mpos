/* Copyright (C) 2017 HashLab - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT License.
 *
 * You should have received a copy of the MIT license in
 * the COPYING file.
 */

#import <Cordova/CDVPlugin.h>
#import "mpos-ios.h"

@interface Mpos : CDVPlugin
- (void)searchAccessories:(CDVInvokedUrlCommand *)command;
- (void)listenDevices:(CDVInvokedUrlCommand *)command;
- (void)stopListeningDevices:(CDVInvokedUrlCommand *)command;
- (void)initialize:(CDVInvokedUrlCommand *)command;
- (void)openConnection:(CDVInvokedUrlCommand *)command;
- (void)closeConnection:(CDVInvokedUrlCommand *)command;
- (void)openSession:(CDVInvokedUrlCommand *)command;
- (void)closeSession:(CDVInvokedUrlCommand *)command;
- (void)downloadTables:(CDVInvokedUrlCommand *)command;
- (void)display:(CDVInvokedUrlCommand *)command;
- (void)pay:(CDVInvokedUrlCommand *)command;
- (void)finish:(CDVInvokedUrlCommand *)command;
- (void)cancel:(CDVInvokedUrlCommand *)command;
@end
