IOTA Entangled Monorepo
=======================================

Building
=============
* `git submodule update --init --recursive`
* Please use https://www.bazel.build/
* `bazel test //...`

Building a docker image and running a container with BAZEL (cc_image target)
=============================================================================
#https://github.com/bazelbuild/rules_docker
build image:
# bazel run //path/tanglescope_img -- --norun 

create container and copy configuration file to container environment:
# docker create --name ##the_name --expose 8080 --expose 8081  -p 8080:8080 -p 8081:8081 bazel/utils/ tanglescope:tanglescope_img

# docker cp ##path_to_configuration_file ##the_name:/app/default_configuration.yaml

start container
# docker start ##the_name

iOS
====================================
* A pre-compiled framework is included in the repo for demo use only. This framework should not be used in production.
* Bitcode is currently not supported (see [this bug](https://github.com/bazelbuild/bazel/issues/4982) for more info)

## How to Use

#### Objective-C
1. Drag and drop your compiled EntangledKit.framework into your Xcode project. It should appear under "Linked Frameworks and Libraries" in the General tab of your target and in the "Link Binary With Libraries" step on the Build Phases tab of your target.

2. Add this line to the header file of your code:
`#import <EntangledKit/EntangledKit.h>`

3. Check out the demo app to see how to use EntangledKit's methods

#### Swift
1.  Drag and drop your compiled EntangledKit.framework into your Xcode project. It should appear under "Linked Frameworks and Libraries" in the General tab of your target and in the "Link Binary With Libraries" step on the Build Phases tab of your target.

2. If you do not already have a bridging header, create an empty Objective-C file. When asked if you'd like to configure your project with a bridging header, select Yes. Once the bridging header is created, you can delete your empty Objective-C file if you choose to do so.

3. Add this line to your bridging header:
`#import <EntangledKit/EntangledKit.h>`
Since you added this line to your bridging header, it is already available to all of your Swift files. You do not need to add `import EntangledKit` to your code. 

4. Check out the demo app to see how to use EntangledKit's methods

