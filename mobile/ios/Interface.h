//
//  Interface.h
//  EntangledKit
//
//  Created by Rajiv Shah on 4/13/18.
//  Copyright © 2018 IOTA Foundation. All rights reserved.
//


#ifndef Interface_h
#define Interface_h

char* doPOW(const char* trytes, int mwm);
char* generateAddress(const char* seed, const int index, const int security);
char* generateSignature(const char* seed, const int index, const int security, const char* bundleHash);

#endif /* Interface_h */
