#include <ctime>
#include "environment.h"
#include "drawer.h"
#include "entity.h"
#include <string>
#include <cstring>
IAgent* CreateCAgent();

IDrawer* CreateSFMLDrawer();
IAgent* CreatePyAgent(wchar_t*, const char* fn);
#include <cstdio>
IEnvironment* CreateEnvironment(IDrawer* d);
wchar_t *GetWC(const char *c)
{
    const size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, c, cSize);

    return wc;
}
int main(int argc, char* argv[])
{
    if(argc < 3) {
        printf("No enough agents.\n");
        return 0;
    }
    IAgent* team1logic = CreatePyAgent(L"", argv[1]);
    IAgent* team2logic = CreatePyAgent(L"", argv[2]);
    
    IDrawer *drawer = CreateSFMLDrawer();
    drawer->Init();
    IEnvironment *env = CreateEnvironment(drawer);

    env->AddAgent(team1logic, 0, 0);
    env->AddAgent(team1logic, 0, 1);
    env->AddAgent(team1logic, 0, 2);
    env->AddAgent(team1logic, 0, 3);
    env->AddAgent(team2logic, 1, 0);
    env->AddAgent(team2logic, 1, 1);
    env->AddAgent(team2logic, 1, 2);
    env->AddAgent(team2logic, 1, 3);
    while(drawer->Alive())
    {
        env->Update();
        drawer->Prepare();
        env->RenderWorld();
        drawer->Present();
    }
    drawer->Done();

    return 0;
}