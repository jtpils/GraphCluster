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

#include "GraphCluster.hpp"

// #include "third_party/cmdLine/cmdLine.h"
#include "stlplus3/filesystemSimplified/file_system.hpp"

extern "C"
{
  #include "graclus/graclus/Graclus.h"
}

namespace bluefish {
GraphCluster::GraphCluster(size_t upper, float cr)
{
    graphUpper = upper;
    completeRatio = cr;
}

ImageGraph GraphCluster::BuildGraph(string imageList, string vocFile)
{
    ImageGraph image_graph;
    ifstream img_in(imageList);
    ifstream voc_in(vocFile);
    string img;
    int idx = 0;
    
    if(!img_in.is_open()) {
        cerr << "File of image list cannot be opened!" << endl;
        return image_graph;
    }
    while(img_in >> img) {
        ImageNode inode(idx, img);
        image_graph.AddNode(inode);
        idx++;
    }
    img_in.close();

    if(!voc_in.is_open()) {
        cerr << "File of vocabulary tree cannot be opened!" << endl;
        return image_graph;
    }
    size_t src, dst;
    float score;
    int k = 0;
    while(voc_in >> src >> dst >> score) {
        // an undirect weight graph is required
         if(src != dst) {
             image_graph.AddEdgeu(src, dst, score);
         }
    }
    voc_in.close();

    return image_graph;
}

string GraphCluster::GenerateNCGraph(ImageGraph imageGraph, string dir)
{
    int k = 0;
    vector<string> ncFiles = stlplus::folder_files(dir);
    for(auto file : ncFiles) {
        if(file.find("normalized_cut") != string::npos) {
            k++;
        }
    }
    string filename = dir + "/normalized_cut_" + to_string(k) + ".txt";
    ofstream nc_out(filename);
    if(!nc_out.is_open()) {
        cerr << "Normalized-Cut Graph cannot be opened!" << endl;
        return filename;
    }
    nc_out << imageGraph.GetNodeSize() << " " << imageGraph.GetEdgeSize() / 2 << " 1\n";

    std::vector<ImageNode> img_nodes = imageGraph.GetImageNode();
    std::vector<EdgeMap> edge_maps = imageGraph.GetEdgeMap();

    for (int i = 0; i < img_nodes.size(); i++) {
		for (EdgeMap::iterator it = edge_maps[i].begin(); it != edge_maps[i].end(); it++) {
			nc_out << it->second.dst + 1 << " " << it->second.score * 1e4 << " ";
		}
        nc_out << endl;
	}
    nc_out.close();
    return filename;
}

vector<size_t> GraphCluster::NormalizedCut(string filename, size_t clusterNum)
{
    vector<size_t> clusters;
    char path[256];
    strcpy(path, filename.c_str());

    Graclus graclus = normalizedCut(path, clusterNum);
    
    for(int i = 0; i < graclus.clusterNum; i++) {
        clusters.push_back((size_t)graclus.part[i]);
    }
    
    return clusters;
}

void GraphCluster::MoveImages(queue<shared_ptr<ImageGraph>> imageGraphs, string dir)
{
    int i = 0;
    while(!imageGraphs.empty()) {
        shared_ptr<ImageGraph> ig = imageGraphs.front();
        imageGraphs.pop();
        std::vector<ImageNode> img_nodes = ig->GetImageNode();

        if(!stlplus::folder_create(dir + "/image_part_" + std::to_string(i))) {
            cerr << "image part " << i << " cannot be created!" << endl;
        }

        for(auto inode : img_nodes) {
            string filename = stlplus::filename_part(inode.image_name);
            string new_file = dir + "/image_part_" + std::to_string(i) + "/" + filename;
            if(!stlplus::file_copy(inode.image_name, new_file)) {
                cout << "cannot copy " << inode.image_name << " to " << new_file << endl;
            }
        }
        i++;
    }
}

void GraphCluster::MoveImages(vector<shared_ptr<ImageGraph>> imageGraphs, string dir)
{
    ofstream out_graph(dir + "/graph.txt");
    if(!out_graph.is_open()) {
        cout << "graph.txt cannot be created!\n";
        return;
    }

    ofstream out_cluster(dir + "/clusters.txt");
    if(!out_cluster.is_open()) {
        cout << "clusters.txt cannot be created!\n";
        return;
    }

    for(int i = 0; i < imageGraphs.size(); i++) {
        std::vector<ImageNode> img_nodes = imageGraphs[i]->GetImageNode();
        string sub_folder = dir + "/image_part_" + std::to_string(i);
        out_cluster << sub_folder << "\n";

        if(!stlplus::folder_create(sub_folder)) {
            cerr << "image part " << i << " cannot be created!" << endl;
        }

        for(auto inode : img_nodes) {
            string filename = stlplus::filename_part(inode.image_name);
            out_graph << inode.idx << " ";
            string new_file = sub_folder + "/" + filename;
            if(!stlplus::file_copy(inode.image_name, new_file)) {
                cout << "cannot copy " << inode.image_name << " to " << new_file << endl;
            }
        }
        out_graph << "\n";
    }
    out_graph.close();
    out_cluster.close();
}

pair<shared_ptr<ImageGraph>, shared_ptr<ImageGraph>> GraphCluster::BiPartition(ImageGraph imageGraph, string dir)
{
    pair<shared_ptr<ImageGraph>, shared_ptr<ImageGraph>> graph_pair;

    string nc_file = GenerateNCGraph(imageGraph, dir);
    vector<size_t> clusters = NormalizedCut(nc_file, 2);
    queue<shared_ptr<ImageGraph>> graphs = ConstructSubGraphs(imageGraph, clusters, 2);
    if(graphs.size() != 2) {
        cout << "Error occured when bi-partition image graph\n";
        return graph_pair;
    }
    graph_pair.first = graphs.front();
    graphs.pop();
    graph_pair.second = graphs.front();
    graphs.pop();    
    return graph_pair;
}

bool GraphCluster::HasEdge(vector<shared_ptr<ImageGraph>> graphs, LinkEdge edge)
{
    for(auto graph : graphs) {
        std::vector<EdgeMap> edge_maps = graph->GetEdgeMap();
        int real_src = graph->Map2CurrentIdx(edge.src);
        int real_dst = graph->Map2CurrentIdx(edge.dst);
        if(real_src == -1 || real_dst == -1) continue;
        EdgeMap::iterator it = edge_maps[real_src].find(real_dst);
        if(it != edge_maps[real_src].end()) {
            return true;
        }
    }
    return false;
}

bool GraphCluster::HasEdge(queue<shared_ptr<ImageGraph>> graphs, LinkEdge edge)
{
    std::vector<shared_ptr<ImageGraph>> igs;
    while(!graphs.empty()) {
        igs.push_back(graphs.front());
        graphs.pop();
    }
    bool hasEdge = HasEdge(igs, edge);
    for(auto graph : igs) {
        graphs.push(graph);
    }
    return hasEdge;
}

priority_queue<LinkEdge> GraphCluster::DiscardedEdges(ImageGraph imageGraph, 
                                        vector<shared_ptr<ImageGraph>> insizeGraphs, 
                                        queue<shared_ptr<ImageGraph>> candidateGraphs)
{
    priority_queue<LinkEdge> discarded_edges;
    std::vector<EdgeMap> edge_maps = imageGraph.GetEdgeMap();
    std::vector<ImageNode> nodes = imageGraph.GetImageNode();

    for(int i = 0; i < nodes.size(); i++) {
        for(int j = i + 1; j < nodes.size(); j++) {
            EdgeMap::iterator it = edge_maps[i].find(j);
            if(it != edge_maps[i].end()) {
                LinkEdge edge = it->second;
                // if(!HasEdge(insizeGraphs, edge) && !HasEdge(candidateGraphs, edge)) {
                if(!HasEdge(insizeGraphs, edge)) {
                    discarded_edges.push(edge);
                }
            }
        }
    }
    return discarded_edges;
}

int GraphCluster::RepeatedNodeNum(const ImageGraph imageGraphL, const ImageGraph imageGraphR)
{
    int num = 0;
    std::vector<ImageNode> l_nodes = imageGraphL.GetImageNode();
    std::vector<ImageNode> r_nodes = imageGraphR.GetImageNode();
    for(auto lnode : l_nodes) {
        for(auto rnode : r_nodes) {
            if(lnode.idx == rnode.idx) num++;
        }
    }
    return num;
}

pair<ImageNode, shared_ptr<ImageGraph>> GraphCluster::SelectCRGraph(ImageGraph imageGraph, vector<shared_ptr<ImageGraph>> graphs, LinkEdge edge)
{
    ImageNode unselected_node, selected_node;
    bool find = true;
    srand((unsigned)time(NULL));
    int ran = rand() % 2;

    for(int i = 0; i < graphs.size(); i++) {
        unselected_node = (ran == 0) ? imageGraph.GetNode(edge.dst) : imageGraph.GetNode(edge.src);
        selected_node = (ran == 0) ? graphs[i]->GetNode(edge.src) : graphs[i]->GetNode(edge.dst);
        int repeatedNodeNum = 0;
        if(selected_node.idx != -1) {
            for(int j = 0; j < graphs.size(); j++) {
                if(i != j) {
                    repeatedNodeNum += RepeatedNodeNum(*graphs[i], *graphs[j]);
                    // if((float)repeatedNodeNum / (float)graphs[i].GetNodeSize() > completeRatio) {
                    //     break;
                    // }
                }
            }
            if((float)repeatedNodeNum / (float)graphs[i]->GetNodeSize() <= completeRatio) {
                return make_pair(unselected_node, graphs[i]);
            }
        }
    }
    shared_ptr<ImageGraph> rep(new ImageGraph());
    return make_pair(ImageNode(), rep);
}

vector<shared_ptr<ImageGraph>> GraphCluster::ExpanGraphCluster(ImageGraph imageGraph, queue<shared_ptr<ImageGraph>> imageGraphs, string dir, size_t clusterNum)
{
    vector<shared_ptr<ImageGraph>> insize_graphs;
    queue<shared_ptr<ImageGraph>>& candidate_graphs = imageGraphs;

    while(!candidate_graphs.empty()) {
        while(!candidate_graphs.empty()) {
            shared_ptr<ImageGraph> ig = candidate_graphs.front();
            candidate_graphs.pop();
            if(ig->GetNodeSize() <= graphUpper) {
                insize_graphs.push_back(ig);
            }
            else {
                pair<shared_ptr<ImageGraph>, shared_ptr<ImageGraph>> ig_pair = BiPartition(*ig, dir);
                candidate_graphs.push(ig_pair.first);
                candidate_graphs.push(ig_pair.second);
            }
        }

        // Graph expansion
        priority_queue<LinkEdge> discarded_edges = 
            DiscardedEdges(imageGraph, insize_graphs, candidate_graphs);
        while(!discarded_edges.empty()) {
            LinkEdge edge = discarded_edges.top();
            discarded_edges.pop();
            cout << "discarded_edges: " << edge.src << ", " << edge.dst << endl; 

            pair<ImageNode, shared_ptr<ImageGraph>> crp_graph = 
                SelectCRGraph(imageGraph, insize_graphs, edge);
            ImageNode unselected_node = crp_graph.first;
            shared_ptr<ImageGraph> selected_graph = crp_graph.second;
            if(unselected_node.idx != -1) {
                if(selected_graph->GetNode(unselected_node.idx).idx == -1) {
                    selected_graph->AddNode(unselected_node);
                }
                selected_graph->AddEdgeu(selected_graph->Map2CurrentIdx(edge.src), 
                                        selected_graph->Map2CurrentIdx(edge.dst), 
                                        edge.score);
            }
        }
        // After graph expansion, there may be some image graph that doesn't
        // satisfy the size constraint, check this condition
        std::vector<shared_ptr<ImageGraph>>::iterator igIte;
        for(igIte = insize_graphs.begin(); igIte != insize_graphs.end();) {
            // Make a little relax of original constraint condition
            if((*igIte)->GetNodeSize() > graphUpper) {
                candidate_graphs.push(*igIte);
                igIte = insize_graphs.erase(igIte);
            }
            else igIte++;
        }
    }
    cout << "end ExpanGraphCluster\n";
    return insize_graphs;
}

vector<shared_ptr<ImageGraph>> GraphCluster::NaiveGraphCluster(queue<shared_ptr<ImageGraph>> imageGraphs, string dir, size_t clusterNum)
{
    MoveImages(imageGraphs, dir);
}

queue<shared_ptr<ImageGraph>> GraphCluster::ConstructSubGraphs(ImageGraph imageGraph, vector<size_t> clusters, size_t clusterNum)
{
    queue<shared_ptr<ImageGraph>> imageGraphs;
    std::vector<ImageNode> nodes = imageGraph.GetImageNode();
    std::vector<EdgeMap> edge_maps = imageGraph.GetEdgeMap();
    EdgeMap::iterator it;
    
    for(int i = 0; i < clusterNum; i++) {
        shared_ptr<ImageGraph> ig(new ImageGraph());
        // Add nodes
        for(int j = 0; j < clusters.size(); j++) {
            if(clusters[j] == i) {
                ig->AddNode(nodes[j]);
            }
        }

        // Add edges
        std::vector<ImageNode> ig_nodes = ig->GetImageNode();
        for(int l = 0; l < ig_nodes.size(); l++) {
            for(int r = l + 1; r < ig_nodes.size(); r++) {
                it = edge_maps[imageGraph.Map2CurrentIdx(ig_nodes[l].idx)].find(
                               imageGraph.Map2CurrentIdx(ig_nodes[r].idx));
                if(it != edge_maps[imageGraph.Map2CurrentIdx(ig_nodes[l].idx)].end()) {
                    ig->AddEdgeu(l, r, it->second.score);
                }
            }
        }
        imageGraphs.push(ig);
    }
    return imageGraphs;
}

}   // namespace bluefish