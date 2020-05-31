Creating Docker Base Images
=================================

In order to run some of the applications in this repository, we need access to
system packages. This folder contains the relevant dockerfiles for building
the base docker containers. To build, run from the project base:

  `docker build containers/ -f containers/Dockerfile.<platform> -t ewfuentes/washbox:<platform>`

You can then push this to Dockerhub by running:
  
   `docker push ewfuentes/washbox:<platform>`
   
Creating Docker Base Images For Other Architectures
--------------------------------------------------------------

Before running the instructions above, we need to install qemu and register
it with docker. Install qemu by running:

```
  sudo apt-get install qemu binfmt-support qemu-user-static
  docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
```

You should now be able to build docker images for other architectures.
