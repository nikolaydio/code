#include "environment.h"

#include <cstdio>

class CAgent : public IAgent {
	AgentStatus Update(EnvirInfo info) {
		AgentStatus out;
		out.forward = 1;
		out.left = 1;
		out.right = 1;
		out.fire = 0;
		for(int i = 0; i < RAY_COUNT; ++i) {
			if(info.ray_detail[i] == ET_MISSLE) {
				info.ray_length[i] = MAX_DIST;
			}
		}
		if(info.ray_length[0] < info.ray_length[2])
		{
			out.left = 0;
		}else if(info.ray_length[0] > info.ray_length[2]){
			out.right = 0;
		}
		if(info.ray_length[2] < MAX_DIST / 5) {
			out.forward = 0;
			out.right = 0;
		}
		if(info.ray_length[2] == MAX_DIST) {
			out.fire = 1;
		}
		return out;
	}
};

IAgent* CreateCAgent() {
	return new CAgent;
}