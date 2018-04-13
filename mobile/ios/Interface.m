//
//  Interface.m
//  
//
//  Created by Rajiv Shah on 4/13/18.
//

#import <Foundation/Foundation.h>
#import "Interface.h"
#import "mobile/interface/bindings.h"

char* doPOW(const char* trytes, int mwm) {
    char* nonce_trytes = do_pow(trytes, mwm);
    return nonce_trytes;
}

char* generateAddress(const char* seed, const int index, const int security){
    char* address = generate_address(seed, index, security);
    return address;
}

char* generateSignature(const char* seed, const int index, const int security, const char* bundleHash){
    char* signature = generate_signature(seed, index, security, bundleHash);
    return signature;
}
