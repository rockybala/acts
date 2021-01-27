// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

namespace Acts {
struct LayerLink {

  int m_index;
  unsigned int m_src, m_dst;
  float m_prob, m_flow;

  LayerLink(int idx, unsigned int d, unsigned int s, float p, float f) : m_index(idx), m_src(s), m_dst(d), m_prob(p), m_flow(f) {};
  ~LayerLink() {};

  struct CompareFlow {
    bool operator()(const struct LayerLink& l1, const struct LayerLink& l2) {
      return l1.m_flow > l2.m_flow;
    }
  };

  struct CompareProb {
    bool operator()(const struct LayerLink& l1, const struct LayerLink& l2) {
      return l1.m_prob > l2.m_prob;
    }
  };
  
};

struct LinkStat {
  float m_fSeg;
  int m_index;

  LinkStat(float f, int i) : m_fSeg(f), m_index(i) {};

  struct CompareStats {
    bool operator()(const struct LinkStat& l1, const struct LinkStat& l2) {
      return l1.m_fSeg > l2.m_fSeg;
    }
  };
   
};


class LayerLinker {
 public: 
  std::vector<LayerLink> m_links;

  LayerLinker(std::ifstream&);
  ~LayerLinker() {};

};

LayerLinker::LayerLinker(std::ifstream& linkFile) {
  
  unsigned int nSources;

  linkFile.read((char*)&nSources, sizeof(nSources));

  int linkIndex=0;
  
  for(unsigned int idx=0;idx<nSources;idx++) {

    unsigned int src;
    float totalFlow;
    unsigned int nLinks; 
    
    linkFile.read((char*)&src, sizeof(src));
    linkFile.read((char*)&totalFlow, sizeof(totalFlow));
    linkFile.read((char*)&nLinks, sizeof(nLinks));
    
    for(unsigned int k=0;k<nLinks;k++) {

      unsigned int dst;
      float prob, flow;
      
      linkFile.read((char*)&dst, sizeof(dst));
      linkFile.read((char*)&prob, sizeof(prob));
      linkFile.read((char*)&flow, sizeof(flow));

      m_links.push_back(Acts::LayerLink(linkIndex++,src, dst, prob, flow)); 
    }
  }

  linkFile.close();
  
  std::sort(m_links.begin(), m_links.end(), Acts::LayerLink::CompareFlow());
  /*
  std::ofstream lf("layer_links.csv");
  
  lf<<"to,from,score_flow"<<std::endl;
  
  for(std::vector<LAYER_LINK>::iterator it=m_links.begin();it!=m_links.end();++it) {

    if((*it).m_src >= 16000 || (*it).m_dst >= 16000) continue;//skipping the long strips
    
    lf<<(*it).m_src<<","<<(*it).m_dst<<","<<(*it).m_flow<<std::endl;
  }

  lf.close();
  */
}

} //namespace Acts
