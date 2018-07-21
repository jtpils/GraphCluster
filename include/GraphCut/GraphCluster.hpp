/** 
  Copyright (c) 2018 Yu Chen

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, 
  this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer 
  in the documentation and/or other materials provided with the distribution.

  3. Neither the name of the GraphCluster nor the names of its contributors may 
  be used to endorse or promote products derived from this software without specific 
  prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GRAPH_CLUSTER_HPP
#define GRAPH_CLUSTER_HPP

#include <string>
#include <fstream>
#include <cstring>
#include <memory>
#include <queue>
#include <utility>

#include "ImageGraph.hpp"

using namespace std;
using namespace bluefish;


namespace bluefish {
class GraphCluster 
{
public:
  size_t graphUpper;    // upper bound of cluster size
  float completeRatio;  // completeness ratio

public:

GraphCluster(size_t upper = 100, float cr = 0.7);

/** 
 * @brief  Build a graph according to image list and vocabulary file(*.out)
 * @note   
 * @param  imageList: a file that stores the paths of images
 * @param  vocFile: vocabulary tree search file that store the similarity scores
 * @retval ImageGraph
 */
ImageGraph BuildGraph(string imageList, string vocFile);

/** 
 * @brief  Generate a file that stores the result of Normalized-Cut
 * @note   
 * @param  imageGraph: ImageGraph that represents the similarity information of images
 * @param  dir: directory that store the normalized-cut files
 * @retval The path of the file that stores the result of Normalized-Cut
 */
string GenerateNCGraph(ImageGraph imageGraph, string dir);

/** 
 * @brief  Normalized-Cut interface that encapusulates the original algorithm of Graclus library
 * @note   
 * @param  filename: normalized-cut file (produced by GenerateNCGraph function)
 * @param  clusterNum: the number of clusters that we want to divide into.
 * @retval Cluster results that represents the cluster ID (For example, return[0] = 1 
 *         suggests that image 0 belongs to 1-st cluster)
 */
vector<size_t> NormalizedCut(string filename, size_t clusterNum);  

/** 
 * @brief  Move images into different clusters
 * @note   
 * @param  imageGraphs: a queue that stores all the ImageGraph after GraphCluster algorithm 
 * @param  dir: root directory that you want to store the cluster result
 * @retval None
 */
void MoveImages(queue<shared_ptr<ImageGraph>> imageGraphs, string dir);  

/** 
 * @brief  Move images into different clusters
 * @note   
 * @param  imageGraphs: a queue that stores all the ImageGraph after GraphCluster algorithm 
 * @param  dir: root directory that stores the cluster result
 * @retval None
 */
void MoveImages(vector<shared_ptr<ImageGraph>> imageGraphs, string dir);  

/** 
 * @brief  Bi-Partition the original image graph
 * @note   
 * @param  imageGraph: image graph that be partioned
 * @param  dir: directory that stores the normalized-cut file
 * @retval 
 */
pair<shared_ptr<ImageGraph>, shared_ptr<ImageGraph>> BiPartition(ImageGraph imageGraph, string dir); 

/** 
 * @brief  Judge if graphs has "edge"
 * @note   
 * @param  graphs: a list of image graphs
 * @param  edge: edge between nodes
 * @retval True if graphs has "edge"
 */
bool HasEdge(vector<shared_ptr<ImageGraph>> graphs, LinkEdge edge);  

/** 
 * @brief  Judge if graphs has "edge"
 * @note   
 * @param  graphs: a list of image graphs
 * @param  edge: edge between nodes
 * @retval True if graphs has "edge"
 */
bool HasEdge(queue<shared_ptr<ImageGraph>> graphs, LinkEdge edge);

/** 
 * @brief  Collect discarded edges in graph division
 * @note   
 * @param  imageGraph: the complete image graph before graph cluster
 * @param  insizeGraphs: graphs that node size less than graphUpper
 * @param  candidateGraphs: candidate graphs that node size may less than graphUpper
 * @retval An priority_queue that store all the discarded edges in graph division
 */
priority_queue<LinkEdge> DiscardedEdges(ImageGraph imageGraph, 
                                        vector<shared_ptr<ImageGraph>> insizeGraphs, 
                                        queue<shared_ptr<ImageGraph>> candidateGraphs);

/** 
 * @brief  Select a image graph randomly which satisfy the completeness ratio
 * @note   
 * @param  imageGraph: image graph
 * @param  graphs: a list of image graphs
 * @param  edge: edge between nodes
 * @retval Unselected node and the image graph that the selected node belongs to
 */
pair<ImageNode, shared_ptr<ImageGraph>> SelectCRGraph(ImageGraph imageGraph, 
                                                      vector<shared_ptr<ImageGraph>> graphs, 
                                                      LinkEdge edge); 

/** 
 * @brief  Count the number of common images between two clusters
 * @note   
 * @param  imageGraphL: left image graph
 * @param  imageGraphR: right image graph
 * @retval The number of common images between two clusters
 */
int RepeatedNodeNum(const ImageGraph imageGraphL, const ImageGraph imageGraphR); 

/** 
 * @brief  Graph Cluster that uses the graph expansion algorithm
 * @note   
 * @param  imageGraph: original image graph
 * @param  imageGraphs: initial image graphs that divided with no common images
 * @param  dir: directory to store normalized-cut files
 * @param  clusterNum: the number of clusters that divided into
 * @retval A list of image graphs after expansion-graph-cluster algorithm
 */
vector<shared_ptr<ImageGraph>> ExpanGraphCluster(ImageGraph imageGraph, 
                                                queue<shared_ptr<ImageGraph>> imageGraphs, 
                                                string dir, size_t clusterNum); 

/** 
 * @brief  Naive graph cluster algorithm (all the clusters have no common images)
 * @note   
 * @param  imageGraphs: initial image graphs that divided with no common images
 * @param  dir: directory that store the images of clusters
 * @param  clusterNum: the number of clusters that divided into
 * @retval A list of image graphs after expansion-graph-cluster algorithm
 */
vector<shared_ptr<ImageGraph>> NaiveGraphCluster(queue<shared_ptr<ImageGraph>> imageGraphs, 
                                                string dir, size_t clusterNum);

/** 
 * @brief  Construct sub-imageGraphs according to the normalized-cut results
 * @note   
 * @param  imageGraph: image graph
 * @param  clusters: normalized-cut results that represents the cluster ID 
 *         (For example, return[0] = 1 suggests that image 0 belongs to 1-st cluster)
 * @param  clusterNum: the number of clusters that divided into
 * @retval A list of sub-imageGraphs
 */
queue<shared_ptr<ImageGraph>> ConstructSubGraphs(ImageGraph imageGraph, 
                                                vector<size_t> clusters, 
                                                size_t clusterNum); 
};

}   // namespace bluefish

#endif