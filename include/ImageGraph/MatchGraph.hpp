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

#ifndef MATCH_GRAPH_H
#define MATCH_GRAPH_H

#include <unordered_map>

#include "Eigen/Core"
#include "ImageGraph.hpp"


namespace bluefish{
    
typedef std::unordered_map<int, Eigen::Matrix3d> MotionMap;

class MatchGraph : public ImageGraph
{
private:
    std::vector<MotionMap> _adj_motion_map;

public:
    MatchGraph(int size) : ImageGraph(size) {};
    MatchGraph(const std::vector<std::string> &image_filenames, const std::vector<std::string> &sift_filenames)
                : ImageGraph(image_filenames, sift_filenames){};
    /**
     * @brief Get motion map
     */ 
    std::vector<MotionMap> GetMotionMap() const;
    /**
     * @breif Compute relative motion between image(i, j)
     * @param
     */ 
    void ComputeMotionMap();
};

}   //namespace bluefish

#endif