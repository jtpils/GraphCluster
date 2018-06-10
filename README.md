# Graph Cluster Software Package

This software package is used to partition numerous of images into different clusters under the background of large scale image-based 3D reconstruction. The software can partition the images in less than 5 minutes except the feature extraction process for about 1,000 images. The number of clusters can be set by your need.

## 1. Dependencies
- Eigen3. A fast linear algebra library implemented in C++.
- STLplus. STLplus is a collection of reusable C++ components for developers already familiar with the STL. It contains template containers to extend the STL, reusable subsystems, data persistence (serialisation) and portability components.
- Graclus. A library used for fast normalized cut, implemented in C. I make some modifications to meet my requirements. 
- libvot. A multi-thread vocabulary tree library.
- cmdLine.

## 2. How to Build
For the use of libvot, additional dependencies are needed
### (1) Required: gflags, glog and gtest (if test is enabled)
*gflags* is a handy library to parse command-line options. *glog* is a logging utility. Together with *gtest*, a unit test suite, they are made public by google to facilitate the development of modern C++ software.  

To install them on Mac:
```bash
brew update
brew glog gflags gtest
```

To install them on Linux (e.g. ubuntu):

```bash
sudo apt-get install libgoogle-glog-dev libgflags-dev libgtest-dev
```

Note: However, `apt-get` mysteriously doesn't install gtest lib even if you run `sudo apt-get install libgtest-dev`. To make things work, you have to manually build gtest from source (apt-get does ship source code in `/usr/src/gtest`). See this [post](https://askubuntu.com/questions/145887/why-no-library-files-installed-for-google-test?newreg=b9a4644b541d4d99aac52be1822d9b2b) for details.

### (2) Build the software package
```
 $ cd GraphCluster
 $ mkdir build && cd build
 $ cmake -D CUDA_USE_STATIC_CUDA_RUNTIME=OFF ..
 $ make [-j threads]
 ```

## 3. How to use
All the execution files are included in the *build/bin* subdirectory. I will give an example on how to use it on Linux operating system (For the use of Windows, you may try other commands).
### (1) Extract Feature Points
```
$ ls -d $PWD/*.jpg > image_list
$ build/bin/libvot_feature image_list && mkdir sift && mv *.sift sift
```

### (2) Image Similarity Search
```
$ ls -d $PWD/sift/*.sift > sift_list
$ mkdir voc && build/bin/image_search sift_list voc
$ mv voc/match.out .
```
The match.out file contains the search results.

### (3) Graph Cluster
```
$ build/bin/GraphCluster absolute_image_list_path absolute_matchout_path cluster_num
```
*absolute_image_list_path* is the absolute path of the image_list file, *absolute_matchout_file* is the absolute path of the match.out file, *cluster_num* is the number of clusters you want to partitiioned. 
