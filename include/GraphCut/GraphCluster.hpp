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
 * @brief Build a graph according to image list and vocabulary file(*.out)
 * @param imageList: a file that stores the paths of images
 * @param vocFile: vocabulary tree search file that store the similarity scores
 * @return ImageGraph
 */
ImageGraph BuildGraph(string imageList, string vocFile);

/**
 * @brief Generate a file that stores the result of Normalized-Cut
 * @param imageGraph: ImageGraph that represents the similarity information of images
 * @param dir: directory that store the normalized-cut files
 * @return The path of the file that stores the result of Normalized-Cut
 */
string GenerateNCGraph(ImageGraph imageGraph, string dir);

/**
 * @brief Normalized-Cut interface that encapusulates the original algorithm of 
 * Graclus library
 * @param filename: normalized-cut file (produced by GenerateNCGraph function)
 * @param clusterNum: the number of clusters that we want to divide into.
 * @return Cluster results that represents the cluster ID (For example, return[0] = 1 
 * suggests that image 0 belongs to 1-st cluster)
 */
vector<size_t> NormalizedCut(string filename, size_t clusterNum);  

/**
 * @brief Move images into different clusters
 * @param imageGraphs: a queue that stores all the ImageGraph after GraphCluster algorithm 
 * @param dir: root directory that you want to store the cluster result
 */
void MoveImages(queue<shared_ptr<ImageGraph>> imageGraphs, string dir);  

/**
 * @brief Move images into different clusters
 * @param imageGraphs: a queue that stores all the ImageGraph after GraphCluster algorithm 
 * @param dir: root directory that stores the cluster result
 */
void MoveImages(vector<shared_ptr<ImageGraph>> imageGraphs, string dir);  

/**
 * @brief Bi-Partition the original image graph
 * @param imageGraph: image graph that be partioned
 * @param dir: directory that stores the normalized-cut file
 * @return
 */
pair<shared_ptr<ImageGraph>, shared_ptr<ImageGraph>> BiPartition(ImageGraph imageGraph, string dir); 

/**
 * @brief Judge if graphs has "edge"
 * @param graphs: a list of image graphs
 * @param edge: edge between nodes
 * @return True if graphs has "edge"
 */
bool HasEdge(vector<shared_ptr<ImageGraph>> graphs, LinkEdge edge);  

/**
 * @brief Judge if graphs has "edge"
 * @param graphs: a list of image graphs
 * @param edge: edge between nodes
 * @return True if graphs has "edge"
 */
bool HasEdge(queue<shared_ptr<ImageGraph>> graphs, LinkEdge edge);

/**
 * @brief Collect discarded edges in graph division
 * @param imageGraph: the complete image graph before graph cluster
 * @param insizeGraphs: graphs that node size less than graphUpper
 * @param candidateGraphs: candidate graphs that node size may less than graphUpper
 * @return An priority_queue that store all the discarded edges in graph division
 */
priority_queue<LinkEdge> DiscardedEdges(ImageGraph imageGraph, 
                                        vector<shared_ptr<ImageGraph>> insizeGraphs, 
                                        queue<shared_ptr<ImageGraph>> candidateGraphs);

/**
 * @brief Select a image graph randomly which satisfy the completeness ratio
 * param imageGraph: image graph
 * @param graphs: a list of image graphs
 * @param edge: edge between nodes
 * @return Unselected node and the image graph that the selected node belongs to
 */
pair<ImageNode, shared_ptr<ImageGraph>> SelectCRGraph(ImageGraph imageGraph, 
                                                      vector<shared_ptr<ImageGraph>> graphs, 
                                                      LinkEdge edge); 

/**
 * @brief Count the number of common images between two clusters
 * @param imageGraphL: left image graph
 * @param imageGraphR: right image graph
 * @return The number of common images between two clusters
 */
int RepeatedNodeNum(const ImageGraph imageGraphL, const ImageGraph imageGraphR); 

/**
 * @brief Graph Cluster that uses the graph expansion algorithm
 * @param imageGraph: original image graph
 * @param imageGraphs: initial image graphs that divided with no common images
 * @param clusterNum: the number of clusters that divided into
 * @return A list of image graphs after expansion-graph-cluster algorithm
 */
vector<shared_ptr<ImageGraph>> ExpanGraphCluster(ImageGraph imageGraph, 
                                                queue<shared_ptr<ImageGraph>> imageGraphs, 
                                                string dir, size_t clusterNum); 

/**
 * @brief Naive graph cluster algorithm (all the clusters have no common images)
 * @param imageGraphs: initial image graphs that divided with no common images
 * @param dir: directory that store the images of clusters
 * @param clusterNum: the number of clusters that divided into
 * @return A list of image graphs after expansion-graph-cluster algorithm
 */
vector<shared_ptr<ImageGraph>> NaiveGraphCluster(queue<shared_ptr<ImageGraph>> imageGraphs, 
                                                string dir, size_t clusterNum);

/**
 * @brief Construct sub-imageGraphs according to the normalized-cut results
 * @param imageGraph: image graph
 * @param clusters: normalized-cut results that represents the cluster ID 
 * (For example, return[0] = 1 suggests that image 0 belongs to 1-st cluster)
 * @param clusterNum: the number of clusters that divided into
 * @return A list of sub-imageGraphs
 */
queue<shared_ptr<ImageGraph>> ConstructSubGraphs(ImageGraph imageGraph, 
                                                vector<size_t> clusters, 
                                                size_t clusterNum); 
};

}   // namespace bluefish

#endif