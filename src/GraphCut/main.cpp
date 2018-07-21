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

#include "stlplus3/filesystemSimplified/file_system.hpp"

// #define __DEBUG__

int main(int argc, char ** argv)
{
    if(argc < 6) {
        cout << "Please input file path of the vocabulary tree\n" << endl;
        cout << "Usage: \n" << 
            "i23dSFM_GraphCluster absolut_img_path absolut_voc_path " << 
            "cluster_option max_img_size completeness_ratio\n";
        cout << "Notice: cluster_option must be 'naive' or 'expansion'\n";
        return 0;
    }

    // It's best to put them on image root path
    string img_list(argv[1]);
    string voc_file(argv[2]); 
    string cluster_option(argv[3]);
    int max_image_num = atoi(argv[4]);
    float completeness_ratio = atof(argv[5]);

    GraphCluster graph_cluster(max_image_num, completeness_ratio);
    
    string dir = stlplus::folder_part(voc_file);
    ImageGraph img_graph = graph_cluster.BuildGraph(img_list, voc_file);

#ifdef __DEBUG__
    img_graph.ShowInfo();
#endif

    size_t clustNum = 1;
    cout << "nodes: " << img_graph.GetNodeSize() << endl;
    cout << "graphUpper: " << graph_cluster.graphUpper << endl;
    if(img_graph.GetNodeSize() < graph_cluster.graphUpper) {
        cout << "size of graphs less than cluster size, camera cluster is the origin one\n";
        return 0;
    }
    else {
        clustNum = img_graph.GetNodeSize() / graph_cluster.graphUpper;
    }

    string nc_graph = graph_cluster.GenerateNCGraph(img_graph, dir);
    vector<size_t> clusters = graph_cluster.NormalizedCut(nc_graph, clustNum);
    queue<shared_ptr<ImageGraph>> sub_image_graphs = 
        graph_cluster.ConstructSubGraphs(img_graph, clusters, clustNum);

    if(cluster_option == "naive") {
        graph_cluster.NaiveGraphCluster(sub_image_graphs, dir, clustNum);
    }
    else if(cluster_option == "expansion") {
        vector<shared_ptr<ImageGraph>> insize_graphs = 
            graph_cluster.ExpanGraphCluster(img_graph, sub_image_graphs, dir, clustNum);
        graph_cluster.MoveImages(insize_graphs, dir);
    }
    else {
        cout << "cluster_option must be 'naive' or 'expansion'\n";
        return 0;
    }

}