# Storage System in Modern C++

Imitate Minio at first pahse. Thanks and appreicated on the opensource project Minio.

In project GoinCPP, it implements those convenient foundation inside Golang, e.g. channel.

## Build CPPIO
Start Container
```
$ ./startdebug.sh 
STEP 1/5: FROM docker.io/gcc:latest
STEP 2/5: WORKDIR /workspace
--> Using cache f362f947c7d6f9224856a2d2ddea71d6d1afc84eab3750e73652a99c35e570b8
--> f362f947c7d6
STEP 3/5: COPY buildtools/cmake-3.31.4-linux-x86_64.sh /workspace
--> Using cache bb8b7c9c1deb45397d6bda077fba6475bffe6bd14c6fa5b73e55bfb167aede37
--> bb8b7c9c1deb
STEP 4/5: RUN sh /workspace/cmake-3.31.4-linux-x86_64.sh --skip-license --prefix=/usr
--> Using cache 2a65f77a841cdd66623094bf7d4f9c8f40f21a26bbc262379b99df2930134547
--> 2a65f77a841c
STEP 5/5: CMD ["bash"]
--> Using cache b4f74b806791de1c8a2881b967ff9fc52759537e7d73b545f3f572fa6128b345
COMMIT cppio
--> b4f74b806791
Successfully tagged localhost/cppio:latest
b4f74b806791de1c8a2881b967ff9fc52759537e7d73b545f3f572fa6128b345
root@1bb21add4871:/workspace#
```
Build Dependencies
```
oot@1bb21add4871:/workspace# cd cppio/
root@1bb21add4871:/workspace/cppio# ./bootstrap.sh
```
Build CPPIO
```
root@1bb21add4871:/workspace/cppio# ./makecppio.sh 
```

## Run CPPIO
Currently, it is developing Command Sub-system.
```
root@1bb21add4871:/workspace/cppio/build# ./cppio --help
Shows a list of commands or help for one command
```
```
root@1bb21add4871:/workspace/cppio/build# ./cppio server
Command Server Options:
  -C [ --config ] arg   specify server configuration via YAML configuration
  -A [ --address ] arg  bind to a specific ADDRESS:PORT, ADDRESS can be an IP 
                        or hostname
  -f [ --flag ]         Boolean flag option
  --positional arg

Pls. provide storage pools
```

## Contacts
zegang.luo@qq.com