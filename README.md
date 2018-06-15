# IOTA Entangled Monorepo
[![Build status](https://badge.buildkite.com/42ef0512276a270bd34cb0010fd641558b344dab4aa2225fba.svg)](https://buildkite.com/iota-foundation/entangled)

## Building

* Please use [Bazel](https://www.bazel.build/):
```shell
bazel test //...
```

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
