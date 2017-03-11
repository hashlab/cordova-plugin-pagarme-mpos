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
