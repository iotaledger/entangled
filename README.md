# IOTA Entangled Monorepo
[![Build status](https://badge.buildkite.com/42ef0512276a270bd34cb0010fd641558b344dab4aa2225fba.svg)](https://buildkite.com/iota-foundation/entangled)

## Building

* Please use [Bazel](https://www.bazel.build/):
```shell
bazel test //...
```

## Developing Entangled
- Be sure to run `./hooks/autohook.sh install` after initial checkout!
- Pass `-c dbg` for building with debug symbols.

## Building a docker image and running a container with BAZEL (cc_image target)

* [Rules for building and handling Docker images with Bazel](https://github.com/bazelbuild/rules_docker)
* build image:
```shell
bazel run //path/tanglescope_img -- --norun
```
* create container and copy configuration file to container environment:
```shell
docker create --name ##the_name \
              --expose 8080 --expose 8081 -p 8080:8080 -p 8081:8081 \
              bazel/utils/ tanglescope:tanglescope_img
docker cp ##path_to_configuration_file ##the_name:/app/default_configuration.yaml
```
* start container
```shell
docker start ##the_name
```

## iOS

#### Compiling
To build with bitcode (using a workaround for [this issue](https://github.com/bazelbuild/rules_apple/issues/163)): `bazel build --ios_multi_cpus=i386,x86_64,armv7,arm64 --copt=-fembed-bitcode --copt=-Ofast //mobile/ios:ios_bindings`

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
1.  Drag and drop your compiled EntangledKit.framework into your Xcode project. It should appear under "Linked Frameworks and Libraries" in the General tab of your target and in the "Link Binary With Libraries" step on the Build Phases tab of your target.

2. If you do not already have a bridging header, create an empty Objective-C file. When asked if you'd like to configure your project with a bridging header, select Yes. Once the bridging header is created, you can delete your empty Objective-C file if you choose to do so.

3. Add this line to your bridging header:

`#import <EntangledKit/EntangledKit.h>`

Since you added this line to your bridging header, it is already available to all of your Swift files. You do not need to add `import EntangledKit` to your code.

4. Check out the demo app to see how to use EntangledKit's methods
