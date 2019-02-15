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
    
    
    // Bundle PoW
    NSLog(@"Starting Bundle PoW");
    NSDate* startBundlePoW = [NSDate date];
    NSArray* bundle = @[TX_0_TRYTES, TX_1_TRYTES, TX_2_TRYTES];
    NSArray* attachedBundle = [EntangledIOSBindings iota_ios_pow_bundle:bundle trunk:@"HYDAVPDGIIFDZVDYRRKGKFEOYOMGE9AQMKTN9XFYWOHGUWLAVFONTBAFFAXFMACGBSLDHPVKIC9WZ9999" branch:@"RLACKHIZIJPGPK9JRXOGJNPGVIXHDJWQSDZDNMVPZVXKHAFVVWJRHHVMPKQHKSMNVQBNANTDOEBY99999" mwm:14];
    NSLog(@"Attached bundle: %@", attachedBundle);
    NSDate* endBundlePoW = [NSDate date];
    NSTimeInterval durationBundlePoW = [endBundlePoW timeIntervalSinceDate:startBundlePoW] * 1000;
    NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationBundlePoW, @"ms"]);
    
    /*
    // PoW
    NSLog(@"Starting PoW");
    NSDate* startPoW = [NSDate date];
    char * trytesChars = [TX_TRYTES cStringUsingEncoding:NSUTF8StringEncoding];
    char * nonce = iota_ios_pow(trytesChars, 14);
    NSString * nonceString = [NSString stringWithFormat:@"%s", nonce];
    NSLog(@"Received nonce: %@ ", nonceString);
    NSDate* endPoW = [NSDate date];
    NSTimeInterval durationPoW = [endPoW timeIntervalSinceDate:startPoW] * 1000;
    NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationPoW, @"ms"]);
    
    // Address
    NSLog(@"Starting Address");
    NSDate* startAddress = [NSDate date];
    char * seedChars = [SEED cStringUsingEncoding:NSUTF8StringEncoding];
    char * address = iota_ios_sign_address_gen(seedChars, 2, 2);
    NSString * addressString = [NSString stringWithFormat:@"%s", address];
    NSLog(@"Calculated address: %@ ", addressString);
    NSDate* endAddress = [NSDate date];
    NSTimeInterval durationAddress = [endAddress timeIntervalSinceDate:startAddress] * 1000;
    NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationAddress, @"ms"]);
    
    // Signature
    NSLog(@"Starting Signature");
    NSDate* startSignature = [NSDate date];
    char * signature = iota_ios_sign_signature_gen(seedChars, 2, 2, seedChars);
    NSString * signatureString = [NSString stringWithFormat:@"%s", signature];
    NSLog(@"Caluclated signature: %@ ", signatureString);
    NSDate* endSignature = [NSDate date];
    NSTimeInterval durationSignature = [endSignature timeIntervalSinceDate:startSignature] * 1000;
    NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationSignature, @"ms"]);
    */
    
    // Digest
    NSLog(@"Starting Digest");
    NSDate* startDigest = [NSDate date];
    NSString* digest = [EntangledIOSBindings iota_ios_digest:TX_TRYTES];
    NSLog(@"Calculated digest: %@ ", digest);
    NSDate* endDigest = [NSDate date];
    NSTimeInterval durationDigest = [endDigest timeIntervalSinceDate:startDigest] * 1000;
    NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationDigest, @"ms"]);
}
    
    
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
    
    
    @end

