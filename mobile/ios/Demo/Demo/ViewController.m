//
//  ViewController.m
//  EntangledDemo
//
//
//  Copyright © 2018 IOTA Foundation. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()
    
    @end

@implementation ViewController
    
- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    NSString * empty = @"";
    NSLog(@"===Entangled iOS Demo===");
    
    // PoW
    NSLog(@"Starting PoW");
    NSDate* startPoW = [NSDate date];
    char * trytesChars = [TX_TRYTES cStringUsingEncoding:NSUTF8StringEncoding];
    char * nonce = doPOW(trytesChars, 14);
    NSString * nonceString = [NSString stringWithFormat:@"%s", nonce];
    NSLog(@"Received nonce: %@ ", nonceString);
    NSDate* endPoW = [NSDate date];
    NSTimeInterval durationPoW = [endPoW timeIntervalSinceDate:startPoW] * 1000;
    NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationPoW, @"ms"]);
    
    // Address
    NSLog(@"Starting Address");
    NSDate* startAddress = [NSDate date];
    char * seedChars = [SEED cStringUsingEncoding:NSUTF8StringEncoding];
    char * address = generateAddress(seedChars, 2, 2);
    NSString * addressString = [NSString stringWithFormat:@"%s", address];
    NSLog(@"Calculated address: %@ ", addressString);
    NSDate* endAddress = [NSDate date];
    NSTimeInterval durationAddress = [endAddress timeIntervalSinceDate:startAddress] * 1000;
    NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationAddress, @"ms"]);
    
    // Signature
    NSLog(@"Starting Signature");
    NSDate* startSignature = [NSDate date];
    char * signature = generateSignature(seedChars, 2, 2, seedChars);
    NSString * signatureString = [NSString stringWithFormat:@"%s", signature];
    NSLog(@"Caluclated signature: %@ ", signatureString);
    NSDate* endSignature = [NSDate date];
    NSTimeInterval durationSignature = [endSignature timeIntervalSinceDate:startSignature] * 1000;
    NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationSignature, @"ms"]);
}
    
    
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
    
    
    @end

