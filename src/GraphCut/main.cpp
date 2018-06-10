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