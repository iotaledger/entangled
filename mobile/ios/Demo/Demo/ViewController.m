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
  NSString* empty = @"";
  NSLog(@"===Entangled iOS Demo===");

  // Bundle PoW
  NSLog(@"Starting Bundle PoW");
  NSDate* startBundlePoW = [NSDate date];
  NSArray* bundle = @[ TX_0_TRYTES, TX_1_TRYTES, TX_2_TRYTES ];
  NSArray* attachedBundle = [EntangledIOSBindings
      iota_ios_pow_bundle:bundle
                    trunk:@"HYDAVPDGIIFDZVDYRRKGKFEOYOMGE9AQMKTN9XFYWOHGUWLAVFONTBAFFAXFMACGBSLDHPVKIC9WZ9999"
                   branch:@"RLACKHIZIJPGPK9JRXOGJNPGVIXHDJWQSDZDNMVPZVXKHAFVVWJRHHVMPKQHKSMNVQBNANTDOEBY99999"
                      mwm:@14];
  NSLog(@"Attached bundle: %@", attachedBundle);
  NSDate* endBundlePoW = [NSDate date];
  NSTimeInterval durationBundlePoW = [endBundlePoW timeIntervalSinceDate:startBundlePoW] * 1000;
  NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationBundlePoW, @"ms"]);

  // PoW
  NSLog(@"Starting PoW");
  NSDate* startPoW = [NSDate date];
  NSString* nonce = [EntangledIOSBindings iota_ios_pow_trytes:TX_TRYTES mwm:@14];
  NSLog(@"Received nonce: %@ ", nonce);
  NSDate* endPoW = [NSDate date];
  NSTimeInterval durationPoW = [endPoW timeIntervalSinceDate:startPoW] * 1000;
  NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationPoW, @"ms"]);

  // Address
  NSLog(@"Starting Address");
  NSDate* startAddress = [NSDate date];
  int8_t* address = NULL;
  address = [EntangledIOSBindings iota_ios_sign_address_gen_trits:SEED_TRITS index:2 security:2];
  NSDate* endAddress = [NSDate date];
  free(address);
  NSTimeInterval durationAddress = [endAddress timeIntervalSinceDate:startAddress] * 1000;
  NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationAddress, @"ms"]);

  // Signature
  NSLog(@"Starting Signature");
  NSDate* startSignature = [NSDate date];
  int8_t* signature = NULL;
  signature = [EntangledIOSBindings iota_ios_sign_signature_gen_trits:SEED_TRITS
                                                                index:2
                                                             security:2
                                                           bundleHash:SEED_TRITS];
  NSDate* endSignature = [NSDate date];
  free(signature);
  NSTimeInterval durationSignature = [endSignature timeIntervalSinceDate:startSignature] * 1000;
  NSLog(@"%@", [empty stringByAppendingFormat:@"%@ %f %@", @"Completed in", durationSignature, @"ms"]);

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
