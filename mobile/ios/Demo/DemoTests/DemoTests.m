//
//  EntangledDemoTests.m
//  EntangledDemoTests
//
//
//  Copyright © 2018 IOTA Foundation. All rights reserved.
//

#import <EntangledKit/EntangledKit.h>
#import <XCTest/XCTest.h>
#import "Constants.h"
#import "TestConstants.h"

@interface EntangledDemoTests : XCTestCase

@end

@implementation EntangledDemoTests

- (void)setUp {
  [super setUp];
}

- (void)tearDown {
  [super tearDown];
}

- (void)testDoPOW {
  char* trytesChars = [TX_TRYTES cStringUsingEncoding:NSUTF8StringEncoding];
  char* nonce = doPOW(trytesChars, 14);
  NSString* nonceString = [NSString stringWithFormat:@"%s", nonce];
  XCTAssertEqualObjects(nonceString, EXPECTED_NONCE);
}

- (void)testGenerateAddress {
  char* seedChars = [SEED cStringUsingEncoding:NSUTF8StringEncoding];
  char* address = generateAddress(seedChars, 2, 2);
  NSString* addressString = [NSString stringWithFormat:@"%s", address];
  XCTAssertEqualObjects(addressString, EXPECTED_ADDRESS);
}

- (void)testGenerateSignature {
  char* seedChars = [SEED cStringUsingEncoding:NSUTF8StringEncoding];
  char* signature = generateSignature(seedChars, 2, 2, seedChars);
  NSString* signatureString = [NSString stringWithFormat:@"%s", signature];
  XCTAssertEqualObjects(signatureString, EXPECTED_SIGNATURE);
}

@end
