#include "info_struct.h"
#include "stat_cal.h"
#include <map>
#include <list>
#include <vector>

#ifndef PKT_HANDLER
#define PKT_HANDLER

using namespace std;

class pkt_list{ 
	//Packet List.
	//Raw packets information.
public:
	vector<pkt> head_to_pkt;

public:
	int append(const info_head & head, const pkt_info & info){
		pkt tmp;
		tmp.head = head;
		tmp.info = info;
	
		head_to_pkt.push_back(tmp);

		return 1;
	}
};

class pkt_info_map{ 
	//Packet information map.
	//Read information from packet list, transfer the packets into flows.
public:
	map<info_head, vector<pkt_info> > head_to_pkts;

public:
	int handler(const pkt_list & p){
		typedef vector<pkt>::const_iterator pkt_it;
		typedef map<info_head, vector<pkt_info> >::iterator map_it;

		pkt_it pi = p.head_to_pkt.begin();

		for(; pi != p.head_to_pkt.end(); ++ pi){
			map_it mi = head_to_pkts.find(pi->head);
			if (mi == head_to_pkts.end() ){
				vector<pkt_info> temp_vec;
				temp_vec.push_back(pi->info);

				head_to_pkts.insert(make_pair(pi->head, temp_vec) );
			}
			else
				mi->second.push_back(pi->info);
		}

		return 1;
	}
};

class flow_info_map{ 
	//Flow information map
	//Read information from packet information map, extract the statistics from each flow.
public:
	map <info_head, flow_info> head_to_flow;

public:
	int handler(const pkt_info_map & p){
		typedef map<info_head, vector<pkt_info> >::const_iterator pkt_map_it;
		typedef map<info_head, flow_info>::iterator flow_map_it;
		
		pkt_map_it pmi = p.head_to_pkts.begin();

		for(; pmi != p.head_to_pkts.end(); ++ pmi){
			flow_info fi = stat_cal(pmi->second);
			head_to_flow.insert(make_pair(pmi->first, fi) );
		}

		return 1;
	}
};


class uni_dir_flow_list{ 
	//Unidirectional flow information list. 
	//Not in use.
public:
	map<info_head, uni_dir_flow> head_to_uni_dir_flow;

public:
	int handler(const flow_info_map & f){
		typedef map<info_head, flow_info>::const_iterator flow_map_it;
		typedef map<info_head, uni_dir_flow>::iterator uni_dir_flow_it;

		flow_map_it fmi = f.head_to_flow.begin();

		for(; fmi != f.head_to_flow.end(); ++fmi){
			uni_dir_flow_it udfi_1 = head_to_uni_dir_flow.find(fmi->first);
			uni_dir_flow_it udfi_2 = head_to_uni_dir_flow.find(reversed_info_head(fmi->first));

			if( udfi_1 == head_to_uni_dir_flow.end() &&
				udfi_1 == head_to_uni_dir_flow.end() ){
				uni_dir_flow temp;
				temp.A = fmi->second;
				head_to_uni_dir_flow.insert(make_pair(fmi->first, temp));
			}
		}

		return 1;
	}
};

#endif