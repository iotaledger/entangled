package org.iota.mobile;

public class Interface {
    public static native String iota_pow_trytes(String trytes, int mwm);
    public static native String[] iota_pow_bundle(String[] bundle, String trunk, String branch, int mwm);
    public static native String iota_sign_address_gen(String seed, int index, int security);
    public static native String iota_sign_signature_gen(String trytes, int index, int security, String bundleHash);
}
