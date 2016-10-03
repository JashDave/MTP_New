
#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>

#ifndef __TEST_UTILS__
#define __TEST_UTILS__

#define ll long long
#define ull unsigned long long

using namespace std;
using namespace std::chrono;


const std::string currentDateTime() {
  //Ref: http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}



class Measure {
//private:
public:
  double min=numeric_limits<double>::max(),max=-1,avg,sum=0,dur;
  uint64_t st,ed;
  int count=0;
  high_resolution_clock::time_point t1,t2;
  vector<double> diff_entries;
  vector<uint64_t> start_entries;
  vector<uint64_t> end_entries;
public:
  inline void start(){
    t1=high_resolution_clock::now();
  }
  inline void end(){
  //record end time and calc min,max,sum
    t2=high_resolution_clock::now();
    dur = duration_cast<microseconds>(t2 -t1).count();
    st = duration_cast<microseconds>(t1.time_since_epoch()).count();
    ed = duration_cast<microseconds>(t2.time_since_epoch()).count();
    diff_entries.push_back(dur);
    start_entries.push_back(st);
    end_entries.push_back(ed);
    count++;// diff_entries.size()
    sum+=dur;
    if(min>dur){
      min=dur;
    } else if(max<dur){
      max=dur;
    }
  }

  double getAvg(){
    return double(sum/(double)count);
  }

  void print(string desc){
    cout<<desc<<endl;
    avg=sum/count;
    cout<<"Min\t"<<min<<"us"<<endl;
    cout<<"Max\t"<<max<<"us"<<endl;
    cout<<"Avg\t"<<avg<<"us"<<endl;
    cout<<"Count\t"<<count<<endl;
  }

  void saveToFile(string desc,string filename,bool overridetime=false){
  ofstream file;
  if(overridetime){
    file.open(filename);
  } else {
    int sep = filename.find_last_of("/\\");
    string dir="";
    string fn="";
    if(sep<0){
      fn=filename;
    } else {
      dir = filename.substr(0,sep+1);
      fn = filename.substr(sep+1);
    }
    file.open(dir+currentDateTime()+"_"+fn);
  }
  file << desc << "\n";
  avg=sum/count;
  file << "Min,Max,Avg,Count\n";
  file << min << "," << max << "," << avg << "," << count << "\n";
  file << "\n";
  file << "Duration,Start Time,End Time\n";
  for(ll i=0;i<diff_entries.size();i++){
    file << diff_entries[i] << "," << start_entries[i] << "," << end_entries[i] << "\n";
  }
  file.close();
  }
};

#endif
