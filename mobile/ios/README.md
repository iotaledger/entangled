## Entangled for iOS

#### Compiling
To build with bitcode (using a workaround for [this issue](https://github.com/bazelbuild/rules_apple/issues/163)): `bazel build --ios_multi_cpus=i386,x86_64,armv7,arm64 --copt=-fembed-bitcode --copt=-O3 //mobile/ios:ios_bindings`

To build without bitcode:
`bazel build --ios_multi_cpus=i386,x86_64,armv7,arm64 -c opt //mobile/ios:ios_bindings`

The framework is located inside `bazel-bin/mobile/ios/ios_bindings.zip`. Double click on it to unzip the archive and show the framework.

#### Objective-C
1. If you do not have a group for your frameworks, create one in Xcode by going to File > New Group. You can name this group "Frameworks".

2. Right-click on the "Frameworks" group and select "Add Files to <your project name>". Navigate to `bazel-bin/mobile/ios/EntangledKit.framework`. Ensure you have selected "Copy items if needed" and click "Add".

3. Add this line to the header file of your code:

`#import <EntangledKit/EntangledKit.h>`

4. Check out the demo app to see how to use EntangledKit's methods

#### Swift
1. If you do not have a group for your frameworks, create one in Xcode by going to File > New Group. You can name this group "Frameworks".

2. Right-click on the "Frameworks" group and select "Add Files to <your project name>". Navigate to `bazel-bin/mobile/ios/EntangledKit.framework`. Ensure you have selected "Copy items if needed" and click "Add".

3. If you do not already have a bridging header, create an empty Objective-C file. When asked if you'd like to configure your project with a bridging header, select Yes. Once the bridging header is created, you can delete your empty Objective-C file if you choose to do so.

4. Add this line to your bridging header:

`#import <EntangledKit/EntangledKit.h>`

Since you added this line to your bridging header, it is already available to all of your Swift files. You do not need to add `import EntangledKit` to your code.

5. Check out the demo app to see how to use EntangledKit's methods

#### Troubleshooting
- If you see an error that includes something similar to `Undefined symbols for architecture x86_64:
  "___gxx_personality_v0", referenced from:`, add `-lc++` to `Other Linker Flags` in the Build Settings of your target
