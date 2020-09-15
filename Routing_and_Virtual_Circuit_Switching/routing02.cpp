/*                                 Topics in Networks
*                                     Assignment 1
*           Question 1:- Given a topology of network find Routing and Forwarding table
*           Group:- Shivangi Garg(194101046) and Sweety Dhale(194101052)
*/
#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<fstream> 
#include<iostream>
#include<iomanip>
#include<sstream>
#include<vector> 
#include<dirent.h>
#include<sys/stat.h> 
#include<sys/types.h>
#include<bits/stdc++.h>

#define inf INT_MAX/4

using namespace std;

vector<vector<int>> hop;                                        // to keep track of hop count
vector<vector<int>> delay;                                      // to store delay
vector<vector<float>> capacity;                                   // to store capacity
vector<vector<int>> dist1;                                      // to store first shortest path distance
vector<vector<int>> dist2;                                      // to temporarily store distance
vector<vector<int>> path1;                                      // to store first shortest path
vector<vector<int>> path2;                                      // to store second shortest path

int nodes,edges,p=-1;                                           // to store nodes, edges and p value
int connReq,connAccepted,VCID=1000;                             // to store connection request,VCID value,etc
string flag;                                                    // to store flag value


void topInitialize()                                            // function to allocate space and initialize 
{                                                               // default values to the structures 
    hop=vector<vector<int>> (nodes,vector<int> (nodes,inf));    // needed to store Topology file info
    delay=vector<vector<int>> (nodes,vector<int> (nodes,inf));  // hop and delay is initialize to infinity
    capacity=vector<vector<float>> (nodes,vector<float> (nodes,0.0)); // value if there is no edge between two nodes
    dist1=vector<vector<int>> (nodes,vector<int> (nodes,0));
    dist2=vector<vector<int>> (nodes,vector<int> (nodes,0));
    path1=vector<vector<int>> (nodes,vector<int> (nodes,0));
    path2=vector<vector<int>> (nodes*nodes);

    for(int i=0;i<nodes;i++)
    {
        delay[i][i]=0;                                           // hop and delay value for same node is zero
        hop[i][i]=0;   
    }   
}


int readTopFile(string topFile)                                 // function to read Topology file contents
{
    ifstream fin;
    fin.open(topFile);                                          // to open file to in read mode

    if(!fin)                                                    // if unable to open required file
    {
        cout<<"\n Unable to open Topology file"<<endl;
        return -1;
    }

    fin>> nodes>> edges;                                        // read nodes and edges value
    topInitialize();                                            // to initialize required structures
    int src,dest,del,cap;                                       // to store values read from file
    float bw;
    while(fin>>src && fin>>dest && fin>>del && fin>>cap && fin>>bw)
    {                                                           // to check proper file format and
        if(src>=nodes || dest>=nodes)                           // correctness of data
        {
            cout<<"\n Topology file contains wrong data";
            cout<<"\n Source or destination node value is invalid"<<endl;
            return -1;
        }
        hop[src][dest]=hop[dest][src]=1;                        // as given network or topology is 
        delay[src][dest]=delay[dest][src]=del;                  // undirected
        capacity[src][dest]=capacity[dest][src]=cap;
    }
    fin.close();
    return 0;
}


int getFirstPath(int src,int dest,vector<int>& p)                 // to traverse first shortest path between
{                                                                 // src and dest and store it to p
    int temp_d=dest;
    vector<int> p1;
    p1.push_back(dest);
    while(path1[src][temp_d]!=src)
    {
        temp_d=path1[src][temp_d];
        p1.push_back(temp_d);
    }
    p1.push_back(src);
    for(auto it=p1.rbegin();it<p1.rend();it++)
        p.push_back(*it);
    return p.size()-1;
}


int getSecondPath(int src,int dest,vector<int>& p)                      // to traverse second shortest path
{                                                                       // between src and dest and store it to p
    for(int i=0;i<path2[src*nodes+dest].size();i++)
        p.push_back(path2[src*nodes+dest][i]);
    return p.size()-1;
}


int getDelay(int src,vector<int>& p)                                   // to calculate delay between src and
{                                                                      // dest over path p
    int t_delay=0;
    for(int i=1;i<p.size();i++)
    {
        t_delay+=delay[src][p[i]];
        src=p[i];
    }
    return t_delay;
}


int updateCapacity(int src,float equi_cap,vector<int>& p)               // to update capacity matrix for some 
{                                                                     // connection over path p by substracting
    for(int i=1;i<p.size();i++)                                       // equivalent capacity value
    {
        capacity[src][p[i]]-=equi_cap;
        capacity[p[i]][src]-=equi_cap;
        src=p[i];
    }
    return 0;
}


bool checkLinkCap(int src,float e_cap,vector<int>& p)                   // to check if connection can be established
{                                                                     // over path p for given equivalent capacity
    bool f=true;
    for(int i=1;i<p.size();i++)
    {   
        if(capacity[src][p[i]]<e_cap)
        {
            f=false;
            break;
        }
        src=p[i];
    }
    return f;
}


void findFirstPath()                                                 // function to find first shortest path
{                                                                    // for all pairs using Floyd-Warshall
    if(flag=="hop")                                                  // if flag value is "hop" then path is calculated
    {                                                                // according to hop value otherwise according
        for(int i=0;i<nodes;i++)                                     // to delay value
            for(int j=0;j<nodes;j++)
                {
                    dist1[i][j]=hop[i][j];
                    if (i == j)
			        	path1[i][j] = i;                             
			        else if (dist1[i][j] != inf)
				        path1[i][j] = i;
			        else
				        path1[i][j] = -1;
                }
    }
    else if(flag=="dist")
    {
        for(int i=0;i<nodes;i++)
            for(int j=0;j<nodes;j++)
                {
                    dist1[i][j]=delay[i][j];
                    if (i == j)
			        	path1[i][j] = 0;
			        else if (dist1[i][j] != inf)
				        path1[i][j] = i;
			        else
				        path1[i][j] = -1;
                }
    }
    
    for(int k=0;k<nodes;k++)
    {
        for(int i=0;i<nodes;i++)
            for(int j=0;j<nodes;j++)
                if(dist1[i][j]>dist1[i][k]+dist1[k][j])
                {
                    dist1[i][j]=dist1[i][k]+dist1[k][j];            // if there is shortest path between
                    path1[i][j]=path1[k][j];                        // i and j via k
                }
    }
}


void findSecondPath()                                               // function to calculate second shortest
{                                                                   // for every pair of nodes using Dijkstra's
    for(int src=0;src<nodes;src++)                                  // shortest path algorithm
    {
       for(int dest=src;dest<nodes;dest++)
        {
           if(src==dest || dist1[src][dest]==inf)
           {
                if(src==dest)
                    path2[src*nodes+dest].push_back(src);          // path2 matrix stores second shortest path 
                else                                               // between src and dest at (src*nodes+dest)th
                {                                                  // position
                    path2[src*nodes+dest].push_back(-1);           // if there is no first shortest path for given 
                    path2[dest*nodes+src].push_back(-1);           // pair then store '-1' value as indicator
                }                      
                continue;
           }

            vector<int> path(nodes);
            if(flag=="hop")                                        // dist2 matrix is initialized according
                for(int i=0;i<nodes;i++)                           // to flag value
                    for(int j=0;j<nodes;j++)
                        dist2[i][j]=dist2[j][i]=hop[i][j];
            else
                for(int i=0;i<nodes;i++)
                    for(int j=0;j<nodes;j++)
                        dist2[i][j]=dist2[j][i]=delay[i][j];
            
            int temp_s=src,temp_d=dest;                            // for evary src and dest pair, first 
            dist2[src][dest]=dist2[dest][src]=inf;                 // shortest path is eliminated by assigning
            while(path1[src][temp_d]!=src)                         // infinity value
            {
                temp_s=temp_d;
                temp_d=path1[src][temp_s];
                dist2[temp_s][temp_d]=dist2[temp_d][temp_s]=inf;
            }
            dist2[src][temp_d]=dist2[temp_d][src]=inf;

            vector<int> dist(nodes);
            vector<int> vis(nodes,0);
            for(int i=0;i<nodes;i++)
            {
                dist[i]=dist2[src][i];
                path[i]=src;
            }
            dist[src]=0;

            for(int i=0;i<nodes;i++)                                    // for every src value the closest node 
            {                                                           // need to find and check if other nodes 
                int min_d=INT_MAX;                                      // can be traversed through it and it's 
                int cnt=0,idx;                                          // not already visited
                for(auto it:dist)
                {
                    if(min_d>it && !vis[cnt])
                    {
                        min_d=it;
                        idx=cnt;                                        // index is recorded
                    }
                    cnt++;
                }
                
                vis[idx]=1;                                             // make it visited

                for(int j=0;j<nodes;j++)
                {
                    if(!vis[j] && dist[j]>min_d+dist2[idx][j])         // for those nodes value is visited
                    {                                                  // which are nearer when go via idx
                        dist[j]=min_d+dist2[idx][j];
                        path[j]=idx;
                    }
                }
            }

            vector<int> p;
            p.push_back(dest);
            temp_d=dest;
            while (path[temp_d]!=temp_d)
            {
                temp_d=path[temp_d];                                  // second shortest path has been found 
                p.push_back(temp_d);                                  // and stored in path2 matrix
            }     
            
            if(dist[dest]!=inf)
            {
                for(auto i=p.rbegin();i<p.rend();i++)
                    path2[src*nodes+dest].push_back(*i);

                for(auto i=p.begin();i<p.end();i++)
                    path2[dest*nodes+src].push_back(*i);
            }
        }
    }
}


int writeRoutingTable(string RTfile)                                  // function to write Routing table information
{                                                                     // to file
    fstream rt_file;
    rt_file.open(RTfile, ios::trunc | ios::out );                     // to open file in write mode
    if(!rt_file)
    {
        cout<<"\n Unable to create Routing table file"<<endl;
        return -1;
    }

    int p_cost,s,d;
    for(int src=0;src<nodes;src++)
        for(int dest=0;dest<nodes;dest++)
            if(src!=dest && dist1[src][dest]!=inf)
            {
                vector<int> p1;
                rt_file<<right<<setfill(' ')<<setw(5)<<src;          // to print source
                rt_file<<right<<setfill(' ')<<setw(5)<<dest;         // to print destination
                rt_file<<right<<setfill(' ')<<setw(5);
                p_cost=getFirstPath(src,dest,p1);                    // to calculate cost and path
                for (auto it=p1.begin();it<p1.end();it++)
                    rt_file<<*it<<" ";                               // to print path
                rt_file<<right<<setfill(' ')<<setw(15)<<getDelay(src,p1);   // to print delay
                rt_file<<right<<setfill(' ')<<setw(5)<<p_cost<<"\n"; // to print cost
                
                if(path2[src*nodes+dest].size()>1);
                else
                    continue;
                vector<int> p2;
                rt_file<<right<<setfill(' ')<<setw(5)<<src;
                rt_file<<right<<setfill(' ')<<setw(5)<<dest;
                rt_file<<right<<setfill(' ')<<setw(5);
                p_cost=getSecondPath(src,dest,p2);
                for (auto it=p2.begin();it<p2.end();it++)
                    rt_file<<*it<<" ";
                rt_file<<right<<setfill(' ')<<setw(15)<<getDelay(src,p2);
                rt_file<<right<<setfill(' ')<<setw(5)<<p_cost<<"\n";              
            }
    rt_file.close();
    return 0;
}

int writeForwardTable(string FW_file,int src,int dest,vector<int>& p)
{                                                                      // function to write Forward Table 
    fstream fw_file;                                                   // information in file
    fw_file.open(FW_file, ios::app | ios::out );                       // to open file in write and append mode
    if(!fw_file)
    {
        cout<<"\n Unable to write in Forward table file"<<endl;
        return -1;
    }

    int temp_s=src;
    fw_file<<right<<setfill(' ')<<setw(10)<<src;
    fw_file<<right<<setfill(' ')<<setw(10)<<"--";
    fw_file<<right<<setfill(' ')<<setw(10)<<VCID;
    fw_file<<right<<setfill(' ')<<setw(10)<<p[1];
    fw_file<<right<<setfill(' ')<<setw(10)<<VCID<<"\n";

    for(auto i=1;i<p.size()-1;i++)
    {
        fw_file<<right<<setfill(' ')<<setw(10)<<p[i];                       // Router ID
        fw_file<<right<<setfill(' ')<<setw(10)<<temp_s;                     // ID of incoming port
        fw_file<<right<<setfill(' ')<<setw(10)<<VCID;                       // VCID
        fw_file<<right<<setfill(' ')<<setw(10)<<p[i+1];                     // ID of outgoing port
        fw_file<<right<<setfill(' ')<<setw(10)<<VCID<<"\n";                 // VCID
        temp_s=p[i];
    }

    fw_file<<right<<setfill(' ')<<setw(10)<<dest;
    fw_file<<right<<setfill(' ')<<setw(10)<<temp_s;
    fw_file<<right<<setfill(' ')<<setw(10)<<VCID;
    fw_file<<right<<setfill(' ')<<setw(10)<<"--";
    fw_file<<right<<setfill(' ')<<setw(10)<<VCID<<"\n";
    fw_file.close();
    return 0;
}

int setConnection(string FW_file,int src,int dest,float equi_cap)                           // function to check if connection request can 
{                                                           // be accepted and to keep track of accepted 
    vector<int> p;
    int dist=getFirstPath(src,dest,p);
    bool set=checkLinkCap(src,equi_cap,p);                            // if given connection request have 
    if(set)                                                           // equivalent capacity value less than
    {                                                                 // every intermediate link of path then
        if(!updateCapacity(src,equi_cap,p))                           // request is accepted by updating 
            writeForwardTable(FW_file,src,dest,p);                    // capacity matrix and writing into file
        connAccepted++;
        VCID+=dist*10;
    }
    else                                                              // if it fails for first shortest path, then
    {                                                                 // it checks for second shortest path
        vector<int> p1;
        dist=getSecondPath(src,dest,p1);
        set=checkLinkCap(src,equi_cap,p1);
        if(set)
        {
            if(!updateCapacity(src,equi_cap,p1))
                writeForwardTable(FW_file,src,dest,p1);
            connAccepted++;
            VCID+=dist*10;
        }
    }    
    return 0;
}


int readConnFile(string connFile, string FW_file)                              // function to read Connection requests
{
    ifstream fin;                                              // to open file in read mode
    fin.open(connFile);
    if(!fin)                                                   // if unable to open a file
    {
        cout<<"\n Unable to open Connection file"<<endl;
        return -1;
    }

    fstream fw_file;                                        // connections, link capacity, VCID, etc
    fw_file.open(FW_file, ios::trunc | ios::out);
    if(!fw_file)
    {
        cout<<"\n Unable to create Forward table file"<<endl;
        return -1;
    }
    fw_file.close();

    fin>> connReq;                                             // to read total connection requests and
    int src,dest,max_c,avg_c,min_c,cnt=0;                      // to store data read from Connection request file
    float equi_cap;                     
    if(p==0)
        while(fin>>src && fin>>dest && fin>>min_c && fin>>avg_c && fin>>max_c)   
        {                                                      // to check proper file format 
            if(src>=nodes || dest>=nodes)                      // and correctness of data
            {
                cout<<"\n Connection file contains wrong data";
                cout<<"\n Source or destination node value exceeds"<<endl;
                return -1;
            }
            float v1=max_c,v2=(avg_c+(max_c-min_c)*0.25);      // if p=0 then equivalent capacity is 
            if(v1<v2)                                          // calculated accordingly
                equi_cap=v1;
            else
                equi_cap=v2;

            setConnection(FW_file,src,dest,equi_cap);
            cnt++;
        }
    else
    {
        while(fin>>src && fin>>dest && fin>>min_c && fin>>avg_c && fin>>max_c)
        {
            if(src>=nodes || dest>=nodes)
            {
                cout<<"\n Connection file contains wrong data";
                cout<<"\n Source or destination node value exceeds"<<endl;
                return -1;
            }
            setConnection(FW_file,src,dest,max_c);
            cnt++;
        }
    }
    connReq=cnt;
    fin.close();
    return 0;
}


int writePathsFile(string P_file)                                    // function to write count of total connection 
{                                                                    // requests and total accepted connections
    fstream p_file;
    p_file.open(P_file, ios::trunc | ios::out );
    if(!p_file)
    {
        cout<<"\n Unable to create Paths file"<<endl;
        return -1;
    }
    p_file<<"\n "<<connReq<<"\t"<<connAccepted;
    p_file.close();
    return 0;
}

int main(int argc,char *argv[])                                         // main function and also driver function
{                                                                       // All required files, flag and p value is 
	if(argc!=8)                                                         // provided through command line
    {
        cout<<"\n Improper argument count"<<endl;
        return -1;
    }
    
    flag=argv[6],p=stoi(argv[7]);

    if(flag=="hop" || flag=="dist");
    else
    {
        cout<<"\n Improper argument value of \'flag\'"<<endl;
        return -1;
    }

    if(p==0 || p==1);
    else
    {
        cout<<"\n Improper argument value of \'p\'"<<endl;
        return -1;
    }
    
    if(readTopFile(argv[1]))                                         // to read topology file
        return -1;
    else
        cout<<"\n Topology file read successfully"<<endl;
    
    cout<<"\n First shortest path:"<<endl;
    findFirstPath();                                                // to find first shortest path
    cout<<"\n First shortest path calculated"<<endl;

    cout<<"\n Second shortest path: "<<endl;
    findSecondPath();                                               // to find second shortest path
    cout<<"\n Second shortest path calculated"<<endl;

    cout<<"\n Routing table file: "<<endl;
    if(writeRoutingTable(argv[3]))                                  // to write into routing table file
        return -1;
    else
        cout<<"\n Routing table prepared"<<endl;

    if(readConnFile(argv[2],argv[4]))                                // to read connection file
        return -1;
    else
        cout<<"\n Connection file read successfully"<<endl;
        cout<<"\n Forward Table prepared"<<endl;

    if(writePathsFile(argv[5]))                                    // to write into paths file
        return -1;
    else
        cout<<"\n Paths file prepared"<<endl;

    cout<<"\n Total connection requests: "<<connReq<<endl;
    cout<<"\n Total connections accepted: "<<connAccepted<<endl;  

	return 0;
}