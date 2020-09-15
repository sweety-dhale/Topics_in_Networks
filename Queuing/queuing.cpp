//*************************************************ASSIGNMENT 2****************************************************************************//
#include<iostream>
#include <stdlib.h>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>
#include <bits/stdc++.h> 

using namespace std;

// Global Variables
int N,B,T; // N = Input output ports, B = buffer size , T = Time of simulation
float K; // K = knockout
float pack_prob; // packet generation probability
string scheduling=""; // scheduling algorithm
string filename="output.txt"; // text file to store output
int total_drops = 0; // count total drops in case of KOUQ
vector<double> delay; // to calculate average delay in queue
vector<int> lastUsedPort; // to check if output port is free or busy
map<int,int> link_Utilization; //to calculate link Uilization
vector<vector<int>> request;
double link_util=0,avg_delay=0,std_dev=0;

//Packet structure to store arrical time and destination port of each packet
struct Packet
{
	int destination_port;
	double arrival_time;
};

//******** Default values in case user didn't provide**********//
void set_Default_Val()
{
	N=8;
	B=4;
	T=10000;
	K=0.6;        
	pack_prob=0.5;           
	scheduling="INQ";
	filename="output.txt";   
}
 
//*********Check if packet will be generated or not**********//
bool check_pack_prob()
{
    if(pack_prob==1)         // if prob is 1 then packet is 
        return true;         // generated in every time slot
    
    // randomly generated value is used to determine if
    // packet is generated for given pack_prob value
	double prob = ((double) rand() / (RAND_MAX));  
	if(prob>pack_prob)
		return false;
	return true;
}

//**********Check if there is space in buffer to accomodate the packet**********//
bool check_buffer(vector<vector<Packet>>& queue,int port)
{	
    // check if buffer is not full
    // for given input port 'port'
	if(queue[port].size()>=B) 
		return false;
	return true;
}

// function to select free input port among all 
// other with highest priority
int grantInPort(int out_port,bool *port_used)
{
    // checks free input port which is next to
    // recently used one
    for(int i=lastUsedPort[out_port]+1;i<N;i++)
    {
        // if input port i has requested for output port 
        // out_port and i is free
        if(request[out_port][i]==1 && port_used[i]==false)
        {
            // changes state of input port i to used
            port_used[i]=true;
            // returns index of selected input port
            return i;   
        }
    }
    for(int i=0;i<=lastUsedPort[out_port];i++)
    {
        if(request[out_port][i]==1 && port_used[i]==false)
        {
            port_used[i]=true;
            return i;
        }
    }
    // if no input port is selected returns -1
    return -1;
}

//************** Find the output port for the packet***************//
int findPortIndex(vector<vector<Packet>>& queue,int in_port,int out_port)
{
    int n=queue[in_port].size();
    for(int i=0;i<n;i++)
    {
        if(queue[in_port][i].destination_port==out_port)
            return i;
    }
    return -1;
}


//********************** Algorithm to schedule packets based of the scheduling type**********//
void schedule_packets(vector<vector<Packet>>& queue,int curr_time)
{
	bool port_used[N]={false}; //checks if the output port is used on free

//**********scheduling algorithm for INQ***********//
    if(scheduling=="INQ")
    {
        for(int port=0;port<N;port++)
        {
            // for each input port if its queue contains any packet
            // to schedule and required output port is not used 
            if(queue[port].size()>0 && port_used[queue[port][0].destination_port]==false)
            {
                // delay calculation
                delay.push_back(curr_time-queue[port][0].arrival_time+1);
                // packet removed from queue
                queue[port].erase(queue[port].begin());
                // to keep track of link utilization
                link_Utilization[port]++;
                // to update used ports
                port_used[queue[port][0].destination_port]=true;
            }
        }
    }
    else if(scheduling=="KOUQ") //*************scheduling algorithm for KOUQ****************//
    {
        for(int port=0;port<N;port++)
        {
            // for each input port if its queue contains 
            // any packet to schedule 
            if(queue[port].size()>0)
            {
                // calculate delay
                delay.push_back(curr_time-queue[port][0].arrival_time+1);
                // packet removed from queue
                queue[port].erase(queue[port].begin());
                // to update used ports
                link_Utilization[port]++;
            }
        }
    }
    else //**********************scheduling algorithm for ISLIP***********************//
    {
        // grant phase where input ports gets mapped with
        // required output ports given that last used port
        // have lowest priority and mapping is done based 
        // on round-robin fashion
        for(int out_port=0;out_port<N;out_port++)
        {
            lastUsedPort[out_port]=grantInPort(out_port,port_used);
        }

        // accept phase where input and output ports
        // are mapped to each other resolving contention 
        // if any, also calculates delay, link utilization, etc.
        for(int out_port=0;out_port<N;out_port++)
        {
            int in_port=lastUsedPort[out_port];
            if(in_port!=(-1))
            {
                int p_idx=findPortIndex(queue,in_port,out_port);
                if(p_idx==-1)
                    continue;
                request[out_port][in_port]=0;
		// calculate delay
                delay.push_back(curr_time-queue[in_port][p_idx].arrival_time+1);
		// packet removed from queue
                queue[in_port].erase(queue[in_port].begin()+p_idx);
		// to update used ports
                link_Utilization[in_port]++;
            }
        }
    }
}

//************************Function to generate traffic or packets********************************//
void generate_traffic(vector<vector<Packet>>& queue,int t)
{	
    int out_port;
	//*************** for INQ scheduling*************//
	if(scheduling == "INQ")
    {
        for(int in_port=0;in_port<N;in_port++) //generate packets at each input port
        {
            if(check_pack_prob() && check_buffer(queue,in_port))//check if the packet generation probability and buffer capacity condition satisfies
            {
                out_port = rand()%N; //generate the output port for the packet randomly
                struct Packet p;
                p.destination_port = out_port; //set the output port for the packet
                p.arrival_time = t; //set the arrival time for the packet
                queue[in_port].push_back(p); // puch packet in the queue of that input port
            }
        }
    }
    else if(scheduling == "KOUQ")//******************for KOUQ scheduling**************************//
    {
	    int count[N]={0}; //count number of packets in each port
        for(int in_port=0;in_port<N;in_port++)
        {
            if(check_pack_prob())//check the packet probability condition
            {
                out_port = rand()%N;//generate the output port for the packet randomly
                count[out_port]++; //increases the packet count for that output port
                if(count[out_port]>K) //if packets for the output port is higher that K, increases the drop count
                {
                    total_drops+= count[out_port]== (K+1) ? 1 : 0;
                    continue;
                }
                else if(check_buffer(queue,in_port)) // if there is space in buffer push tha packet inside queue
                {
                    struct Packet p;
                    p.destination_port = out_port; //set the output port for the packet
                    double time=t +((double)in_port/10000);
                    p.arrival_time = time;//set the arrival time for the packet
                    queue[out_port].push_back(p);// puch packet in the queue of that input port
                }
            }
        }	
    }
    else if(scheduling=="ISLIP")//******************for ISLIP scheduling**************************//
    {
        for(int in_port=0;in_port<N;in_port++)
        {
            if( check_pack_prob() && check_buffer(queue,in_port))
            {
                out_port = rand()%N;         // destination port is selected randomly  
                struct Packet p;             // with uniform probability
                p.destination_port = out_port;
                p.arrival_time = t;          // current time as arrival time
                queue[in_port].push_back(p); // packet appended to queue of in_port
                request[out_port][in_port]=1;
            }
        }
    }
}

// function to calculate standard deviation of delay
float findSD_delay(int avg_delay)
{
    float std_dev=0.0;
    for(auto d:delay)
        std_dev+=pow(d-avg_delay,2);
    return sqrt(std_dev/delay.size());
}

// function to calculate performance metrics 
void performance_metrics()
{
	link_util=0,avg_delay=0;
    // to calculate avg link utilization 
	for(auto itr=link_Utilization.begin();itr!=link_Utilization.end();itr++)
		link_util+=(itr->second);
	link_util/=(link_Utilization.size()*T);
	cout<<"Link utilization : "<<link_util<<endl;

    // to calculate avg delay
	for(int i=0;i<delay.size();i++)
		avg_delay+=delay[i];
	avg_delay/=(delay.size());
	cout<<"Average delay : "<<avg_delay<<endl;

    // to calculate standard deviation of delay
    std_dev=findSD_delay(avg_delay);
    cout<<"Standard deviation of delay : "<<std_dev<<endl;

    // packet drop probability in case of KOUQ
	if(scheduling=="KOUQ")
	{
		long double K_drop=(long double)total_drops/(N*T) ; 
		cout<<"K_drop_prob : "<<K_drop<<endl;
	}
}

//***********Function to check if the outfile exists or not *********************//
inline bool exists(const std::string& name) 
{
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

//**************Function to append results to output file********************//
void write_to_file(string filename)
{
	ofstream outfile;
	if(!exists(filename)) //if file doesn't exist
	{	outfile.open(filename,std::ios_base::app);
		//write header for the first time
		outfile<<"[For default value of maxtimeslot = "<<T<<" ,pktgenprob = "<<pack_prob<<" ]"<<endl; 
		outfile << "N\t\tp\t\tBuffer Size\t\tQueue type\t\tAvg PD\t\tStd Dev PD\t\tAvg link utilization"<<endl;
		outfile.close();
	}
	//open file in append mode
	outfile.open(filename,std::ios_base::app);
	//write results to the output file
	outfile << N <<"\t\t" <<pack_prob <<"\t\t"<<B<<"\t\t\t" <<scheduling<<"\t\t\t"<<setprecision(4)<<avg_delay<<"\t\t\t"<<setprecision(4)<<std_dev<<"\t\t"<<setprecision(4)<<link_util<< endl;
}

//*******************DRIVER FUNCTION******************************//
int main(int argc,char ** argv)
{
	//Set default values to the variables
	set_Default_Val();
	int flag=1,curr=1;
	 //Reading parameters from command line and keep default values if not provided from user
     while(curr<argc)
    {
        if(curr+2>=argc+1)
        {
            flag=0;
            break;
        }
        if(strcmp(argv[curr],"-N")==0)
            N=atoi(argv[curr+1]);
        else if(strcmp(argv[curr],"-B")==0)
            B=atoi(argv[curr+1]);
        else if(strcmp(argv[curr],"-p")==0)
            pack_prob=atof(argv[curr+1]);
        else if(strcmp(argv[curr],"-queue")==0)
            scheduling=argv[curr+1];
        else if(strcmp(argv[curr],"-K")==0)
            K=atof(argv[curr+1]);
        else if(strcmp(argv[curr],"-out")==0)
            filename=argv[curr+1];
        else if(strcmp(argv[curr],"-T")==0)
            T=atol(argv[curr+1]);
        else {flag=0;break;}
            curr=curr+2;
    }
	K = K*N;

	srand(time(NULL));
	
	//maintain queue for each port to store packets
	vector<vector<Packet>> queue(N);
    if(scheduling=="ISLIP")
    {
        lastUsedPort=vector<int>(N,-1);
        request=vector<vector<int>>(N,vector<int>(N,0));
    }

	for(int t=0;t<T;t++) // run for the given time of simulation
	{
		generate_traffic(queue,t); //traffic generation
		schedule_packets(queue,t); //scheduling of the traffic
	}
	performance_metrics(); // transmission phase and evaluate average delay, link utilization and performance
	write_to_file(filename);// write the content to the files

	return 0;
}
