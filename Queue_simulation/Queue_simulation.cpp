#include <chrono>
#include <thread>
#include <random>
#include<iostream>
#include<bits/stdc++.h>
#include <time.h>
using namespace std;


long double  lambda;
long double mu;
long int T;

int servers=3;
double t=0.0;
int customers=0;
double sys_cust=0.0;
double avg_wait=0.0;
double avg_res=0.0;
double n_arr=0.0;
double event=0.0;
double n_dept=INT_MAX;
queue<double> q;
int flag=-1;
double avg_serve=0.0;
int nextDepartIndex;
double custDepartures[3];
double serve_time;
int customer_served=0;

double Exponential_Distribution(double temp)
{
  int seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::exponential_distribution<double> distribution (temp);
  double number = distribution(generator);
  return number;
}

int max(double custDepartures[],int x,int y)
{
	if(custDepartures[x]<custDepartures[y])
		return x;
	return y;
}

double mod(double x)
{
	if(x<0)
	return -1*x;

	return x;
}

void takeIntputs()
{
	cout<< "Enter arrival Rate : ";
	cin>>lambda;
	cout<<endl;
	cout<<"Enter service Rate : ";
	cin>>mu;
	cout<<endl;
	cout<<"Enter Time of simulation : ";
	cin>>T;
}

int main()
{
	char choice;
	cout<<"Enter the part choice : ";
	cin>>choice;
	cout<<endl;

	if(choice == 'a')
	{
	takeIntputs();
	for (int i=0; i < 3; i++)
        custDepartures[i] = INT_MAX;
	while(t<T)
	{
		if(n_arr<n_dept)
		{	
			flag=1;
			t = n_arr;
			if(customers+1<=3)
			{
				int arrayIndex=0;
				if(custDepartures[1]==INT_MAX)
				arrayIndex = 1;
				else if(custDepartures[2]==INT_MAX)
				arrayIndex=2;

				serve_time= Exponential_Distribution(mu);
				custDepartures[arrayIndex] = t + serve_time;
                			if (customers+1 == 1)
                			{
                    				nextDepartIndex = arrayIndex;   
						n_dept = custDepartures[nextDepartIndex];
                			}
			}
			else
			q.push(n_arr);
				
		}
		else
		{
			t=n_dept;
			flag=0;
			custDepartures[nextDepartIndex] = INT_MAX;
		}
		double x=t-event;
		if(flag==1)
			{
				event=t;
				n_arr = t + Exponential_Distribution(lambda);
			}
		else if(flag==0)
			{
				customer_served++;
				event=t;
				t=n_dept;
				if (customers-1> 0)
            			{ 
					if (customers-1 >= servers)
                    			{
						double response = n_dept - q.front();
						double wait = n_dept - serve_time - q.front();
						
						q.pop();
						avg_res+=response;
						serve_time = Exponential_Distribution(mu);
						avg_serve=1/mu;
						custDepartures[nextDepartIndex] = t + serve_time;
					}
					  
					int index=max(custDepartures,max(custDepartures,0,1),2); 
                			nextDepartIndex = index;
                			n_dept = custDepartures[nextDepartIndex];
            			}
            			else
                			n_dept = INT_MAX;
			}
		sys_cust+=(customers*x);

		if(flag==1)
		customers++;
		if(flag==0)
		customers--;
	} 
	//cout<<avg_serve/T<<endl;
	double l = sys_cust/ t;
	double wait_time = mod((l*t)/customer_served - avg_serve);
	double response_time = (l*t)/customer_served;
	cout<<"Average_Waiting : "<<wait_time<<endl;
	cout<<"Average_Response : "<<response_time<<endl;
	cout<<"Average number of passengers in system  : "<<l<<endl;
	cout<<"Average number of passengers in q : "<<(wait_time*customer_served)/t<<endl;
	
	}
return 0;

}
