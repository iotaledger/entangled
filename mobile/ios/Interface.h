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

+ (char*)iota_ios_checksum:(const char*)input
              input_length:(const size_t)input_length
           checksum_length:(const size_t)checksum_length;
+ (char*)iota_ios_digest:(char const* const)trytes;
+ (char*)iota_ios_pow:(const char*)trytes mwm:(int)mwm;
+ (char*)iota_ios_sign_address_gen:(const char*)seed
                             index:(const int)index
                          security:(const int)security;
+ (char*)iota_ios_sign_signature_gen:(const char*)seed
                               index:(const int)index
                            security:(const int)security
                          bundleHash:(const char*)bundleHash;
+ (NSArray*)iota_ios_pow_bundle:(NSArray*)txsTrytes
                          trunk:(NSString*)trunk
                         branch:(NSString*)branch
                            mwm:(int)mwm;

@end

#endif  // __MOBILE__IOS_INTERFACE_H_
