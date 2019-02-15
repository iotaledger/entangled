//
//  Interface.h
//  EntangledKit
//
//
//  Copyright © 2019 IOTA Foundation. All rights reserved.
//

#ifndef __MOBILE__IOS_INTERFACE_H_
#define __MOBILE__IOS_INTERFACE_H_

#import <Foundation/Foundation.h>
#include <stddef.h>

@interface EntangledIOSBindings : NSObject

+ (NSString*)iota_ios_digest:(NSString*)trytes;
+ (NSString*)iota_ios_pow_trytes:(NSString*)trytes mwm:(int)mwm;
+ (char*)iota_ios_sign_address_gen_trits:(const char*)seed
                                   index:(const int)index
                                security:(const int)security;
+ (char*)iota_ios_sign_signature_gen_trits:(const char*)seed
                                     index:(const int)index
                                  security:(const int)security
                                bundleHash:(const char*)bundleHash;
+ (NSArray*)iota_ios_pow_bundle:(NSArray*)txsTrytes
                          trunk:(NSString*)trunk
                         branch:(NSString*)branch
                            mwm:(int)mwm;

@end

#endif  // __MOBILE__IOS_INTERFACE_H_
