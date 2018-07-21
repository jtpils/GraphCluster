#!/bin/bash


img_dir=$1
cluster_option=$2
max_img_num=$3
completeness_ratio=$4
img_format=$5

# extract features
echo "[Extract Feature Points]: "
ls -d $img_dir/*.$img_format > $img_dir/image_list
../build/bin/libvot_feature $img_dir/image_list
mkdir $img_dir/sift && mv $img_dir/*.sift $img_dir/sift

# image similarity search
echo "[Image Similarity Search]: "
ls -d $img_dir/sift/*.sift > $img_dir/sift_list
mkdir $img_dir/voc
../build/bin/image_search $img_dir/sift_list $img_dir/voc
mv $img_dir/voc/match.out $img_dir

#graph cluster
echo "[Graph Cluster]: " 
../build/bin/GraphCluster $img_dir/image_list $img_dir/match.out $cluster_option $max_img_num $completeness_ratio
