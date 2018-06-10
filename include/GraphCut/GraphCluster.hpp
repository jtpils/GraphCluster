#ifndef GRAPH_CLUSTER_HPP
#define GRAPH_CLUSTER_HPP

#include <string>
#include <fstream>
#include <cstring>
// #include <vector>
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
  size_t graphUpper;
  float completeRatio;

public:

GraphCluster(size_t upper = 100, float cr = 0.7);
/**
 * @brief
 * @param imageList:
 * @param vocFile:
 * @return
 */
ImageGraph BuildGraph(string imageList, string vocFile);

/**
 * @brief
 * @param imageGraph:
 * @param dir: directory that store the normalized-cut files
 * @return
 */
string GenerateNCGraph(ImageGraph imageGraph, string dir);

/**
 * @brief
 * @param filename:
 * @param clusterNum:
 * @return
 */
vector<size_t> NormalizedCut(string filename, size_t clusterNum);  

/**
 * @brief Naive graph cluster
 * @param imageGraph:
 * @param clusters:
 * @param clusterNum:
 */
void MoveImages(queue<shared_ptr<ImageGraph>> imageGraphs, string dir);  

/**
 * @brief Naive graph cluster
 * @param imageGraph:
 * @param clusters:
 * @param clusterNum:
 */
void MoveImages(vector<shared_ptr<ImageGraph>> imageGraphs, string dir);  

/**
 * @brief
 * @param imageGraph
 * @return
 */
pair<shared_ptr<ImageGraph>, shared_ptr<ImageGraph>> BiPartition(ImageGraph imageGraph, string dir); 

/**
 * @brief
 * @param igraphs
 * @param edge:
 * @return 
 */
bool HasEdge(vector<shared_ptr<ImageGraph>> graphs, LinkEdge edge);  

/**
 * @brief
 * @param igraphs
 * @param edge:
 * @return 
 */
bool HasEdge(queue<shared_ptr<ImageGraph>> graphs, LinkEdge edge);

/**
 * @brief Collect discarded edges in graph division
 * @param imageGraph:
 * @param insizeGraphs:
 * @param candidateGraphs:
 * @return
 */
priority_queue<LinkEdge> DiscardedEdges(ImageGraph imageGraph, 
                                        vector<shared_ptr<ImageGraph>> insizeGraphs, 
                                        queue<shared_ptr<ImageGraph>> candidateGraphs);

/**
 * @brief Select a image graph randomly which satisfy the completeness ratio
 * @param graphs:
 * @param edge:
 * @return
 */
pair<ImageNode, shared_ptr<ImageGraph>> SelectCRGraph(ImageGraph imageGraph, vector<shared_ptr<ImageGraph>> graphs, LinkEdge edge); 

/**
 * 
 */
int RepeatedNodeNum(const ImageGraph imageGraphL, const ImageGraph imageGraphR); 

/**
 * @brief 
 * @param imageGraph:
 * @param ncFile:
 * @param clusterNum:
 */
vector<shared_ptr<ImageGraph>> ExpanGraphCluster(ImageGraph imageGraph, queue<shared_ptr<ImageGraph>> imageGraphs, string dir, size_t clusterNum); 

/**
 * 
 * 
 * 
 */
vector<shared_ptr<ImageGraph>> NaiveGraphCluster(queue<shared_ptr<ImageGraph>> imageGraphs, string dir, size_t clusterNum); 

/**
 * 
 * 
 * 
 */
// void GraphCluster(const ImageGraph imageGraph, string dir, size_t clusterNum); 

/**
 * @brief
 * @param imageGraph
 * @param clusters
 * @param clusterNum
 * @return
 */
queue<shared_ptr<ImageGraph>> ConstructSubGraphs(ImageGraph imageGraph, vector<size_t> clusters, size_t clusterNum); 
};

}   // namespace bluefish

#endif