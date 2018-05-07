//
//  Interface.m
//  
//
//  Created by Rajiv Shah on 4/13/18.
//

#import <Foundation/Foundation.h>
#import "Interface.h"


char* doPOW(const char* trytes, int mwm) {
    char* nonce_trytes = iota_pow(trytes, mwm);
    return nonce_trytes;
}

char* generateAddress(const char* seed, const int index, const int security){
    char* address = iota_sign_address_gen(seed, index, security);
    return address;
}

char* generateSignature(const char* seed, const int index, const int security, const char* bundleHash){
    char* signature = iota_sign_signature_gen(seed, index, security, bundleHash);
    return signature;
}
