// 17.8 on testsamples using 2 digit precision

#include <bits/stdc++.h>
using namespace std;

#define g(n) scanf("%d",&n)
#define gl(n) scanf("%lld", &n)
#define f(i,n) for(int i=0; i<n; i++)
#define pb push_back
#define mp make_pair
#define fab(i,a,b) for(int i=a; i<=b; i++)
#define test(t) while(t--)
#define getcx getchar//_unlocked
#define PLAYER_MAX true
#define PLAYER_MIN false

typedef long long int lli;
typedef pair<int,int> pii;
typedef vector<int> vi;
typedef vector< vi > vvi;

const int TOTAL_TIME = 15;
float SMOOTHING_FACTOR = 0.001;
class Graph
{
public:
    map<string, int> dictionary;
    int totalVars;
    vector<vector<string> > possibleValues;
    vector<vector<int> > parents;
    vector<vector<int> > child;
    vector<vector<float> > CPT;
    vector<string> names;
    vector<vector<int> > realData; //original data with blanks
    vector<vector<int> > data;//expanded data for updating CPT
    vector<float> weights;
    vector<int> questionMarks;
    string filename;
    time_t initTime;
    Graph(string fnm, time_t init)
    {
        realData.resize(0);
        weights.resize(0);
        initTime = init;
        filename = fnm;
        names.resize(37);
        dictionary.clear();
        totalVars = 0;
        possibleValues.resize(37);
        parents.resize(37);
        child.resize(37);
        CPT.resize(37);
        data.resize(0);
        questionMarks.resize(0);
        readBayesNet();
        readDataFile();
    }

    void readBayesNet()
    {
        string line;
        ifstream myfile(filename); 
        string temp;
        string name;
        
        if (myfile.is_open())
        {
            while (! myfile.eof() )
            {
                stringstream ss;
                getline (myfile,line);
                ss.str(line);
                ss>>temp;
                if(temp.compare("variable")==0)
                {
                    ss>>name;
                    dictionary[name] = totalVars++;
                    names[totalVars-1] = name;
                    getline (myfile,line);
                    stringstream ss2;
                    ss2.str(line);
                    for(int i=0;i<4;i++)
                        ss2>>temp;
                    possibleValues[totalVars-1].clear();
                    while(temp.compare("};")!=0) //adding all the possible values
                    {
                        possibleValues[totalVars-1].pb(temp);
                        ss2>>temp;
                    }
                }

                else if(temp.compare("probability")==0)
                {                        
                    ss>>temp;
                    ss>>temp;
                    int currentID = dictionary.find(temp)->second;
                    ss>>temp;
                    parents[currentID].clear();
                    while(temp.compare(")")!=0)
                    {
                        int parIndex = dictionary.find(temp)->second;
                        child[parIndex].pb(currentID);
                        parents[currentID].pb(parIndex);
                        ss>>temp;
                    }
                    getline (myfile,line);
                    stringstream ss2;
                    ss2.str(line);
                    ss2>> temp;
                    ss2>> temp;
                    CPT[currentID].clear();
                    while(temp.compare(";")!=0)
                    {
                        CPT[currentID].pb(atof(temp.c_str()));
                        ss2>>temp;
                    }                
                }   
            }
            
            myfile.close();
        }
    }

    void readDataFile()
    {
        string line, temp;
        while(getline(cin, line))
        {
            stringstream ss;
            ss.str(line);
            vector<int> values;
            int question_index = -1;
            for(int i=0; i<totalVars; i++)
            {
                ss >> temp;
                if(temp.compare("\"?\"") == 0)
             	{
             		question_index = i;
             		values.pb(-1);
             		continue;
             	}
                int data_index = 0;
                while(data_index < possibleValues[i].size())
                {
                	if(possibleValues[i][data_index].compare(temp) == 0)
                		break;
                	data_index++;
                }
                values.pb(data_index);
            }
            questionMarks.pb(question_index);
            realData.pb(values);
            if(question_index==-1)
            {
                data.pb(values);
                weights.pb(1);
            }
            else
            {
                int node_id = question_index;
                int s = possibleValues[node_id].size();
                for(int i=0; i<s; i++)
                {
                    vector<int> temp(values.begin(), values.end());
                    temp[node_id] = i;
                    data.pb(temp);
                    weights.pb(1.0/s);
                }
            }
        }
    }

    void rewrite() //assuming correct CPT
    {
        string line;
        ifstream myfile(filename); 
        string temp;
        string name;
        if (myfile.is_open())
        {
            while (! myfile.eof() )
            {
                stringstream ss;
                getline (myfile,line);
                ss.str(line);
                ss>>temp;
                if(temp.compare("probability")==0)
                {                        
                    ss>>temp;
                    ss>>temp;
                    int currentID = dictionary.find(temp)->second;
                    cout << line << endl;
                    getline (myfile,line);
                    cout << "table ";
                    for(int i=0; i < CPT[currentID].size(); i++)
                        printf(" %.4f ",CPT[currentID][i]);
                    cout << ";" << endl;
                }
                else if(line.compare("")!=0)
                	cout << line << endl;
                else 
                    cout << line;
            }
            
            myfile.close();
        }
    }

    void rewrite(string outfile)
    {
        string line;
        ifstream myfile(filename); 
        ofstream out;
        out.open(outfile);
        string temp;
        string name;
        if (myfile.is_open())
        {
            while (! myfile.eof() )
            {
                stringstream ss;
                getline (myfile,line);
                ss.str(line);
                ss>>temp;
                if(temp.compare("probability")==0)
                {                        
                    ss>>temp;
                    ss>>temp;
                    int currentID = dictionary.find(temp)->second;
                    out << line << endl;
                    getline (myfile,line);
                    out << "table ";
                    for(int i=0; i < CPT[currentID].size(); i++)
                        out << " " << CPT[currentID][i] << " ";
                    out << ";" << endl;
                }
                else if(line.compare("")!=0)
                    out << line << endl;
                else 
                    out << line;
            }
            
            myfile.close();
        }
    }

    int computeIndex(vi& values, vi& allSize)
    {
    	int ans = 0, x=1;
    	for(int j=allSize.size()-1; j>=0; j--)
    	{
    		ans += x*values[j];
    		x*=allSize[j];
    	}
    	return ans;
    }	

    // computes P(var=val/parents(var)) using data[lno]
    float computeProbability(int lno, int var, int val)
    {
    	vi values(0);
    	vi allSize(0);
    	values.pb(val);
    	allSize.pb(possibleValues[var].size());

    	for(int j=0; j<parents[var].size(); j++)
		{
			allSize.pb(possibleValues[parents[var][j]].size());
			values.pb(realData[lno][parents[var][j]]);
		}

		return CPT[var][computeIndex(values, allSize)];
    }


    // Make CPT from the data set (numvars * datasize * numvars + numVars * 108 is max)
    void updateCPT()
    {
        for(int i=0; i<CPT.size(); i++)
        {
            CPT[i] = vector<float>(CPT[i].size(), SMOOTHING_FACTOR); //init with 1 for smoothing
            vi ids(0); //ids of current node and its parents
            vi allSize(0); //allsize[i] is the number of possible values that ids[i] can take
            ids.pb(i);
            allSize.pb(possibleValues[i].size());
            for(int j=0; j<parents[i].size(); j++)
            {
                allSize.pb(possibleValues[parents[i][j]].size());
                ids.pb(parents[i][j]);
            }
            
            int jump = CPT[i].size()/possibleValues[i].size();
            vector<float> sumParts(jump, possibleValues[i].size() * SMOOTHING_FACTOR);

            for(int j=0; j<data.size(); j++)
            {
                vi values(0);
                for(int k=0; k<ids.size(); k++)
                {
                    values.pb(data[j][ids[k]]); 
                }
                int data_index = computeIndex(values, allSize);
                CPT[i][data_index] += weights[j]; //
                sumParts[data_index%jump]+=weights[j];
            }

            for(int j=0; j<CPT[i].size(); j++)
            {
                if(sumParts[j%jump] != 0)
                    CPT[i][j] /= sumParts[j%jump];
                else
                    CPT[i][j] = 0.0;
            }
        }
    }

    void initCPT()
    {
        for(int i=0; i<CPT.size(); i++)
        {
            int len = CPT[i].size()/possibleValues[i].size();
            int slots = possibleValues[i].size();
            vector<float> sums(len,1.0);
            vector<int> nonzerocount(len,0);
            for(int j=0; j<CPT[i].size(); j++)
            {
                if(CPT[i][j] < 0)
                    continue;
                sums[j%len] -= CPT[i][j];
                nonzerocount[j%len]++;
            }

            for(int j=0; j<CPT[i].size(); j++)
            {
                if(CPT[i][j] < 0)
                    CPT[i][j] = sums[j%len]/(slots - nonzerocount[j%len]);
            }
        }
    }

    void updateDataset()
    {
        weights.clear();
        for(int i=0; i<realData.size(); i++)
        {
            if(questionMarks[i] == -1)//no empty slot in the ith line
            {
                weights.pb(1);
                continue;
            }
            int var = questionMarks[i];
            vector<float> probs(0);
            float probs_sum = 0.0;
            for(int j=0; j<possibleValues[var].size();j++)
            {
                float prob = computeProbability(i, var, j);
                for(int k = 0; k<child[var].size(); k++)
                {
                    realData[i][var] = j;
                    prob *= computeProbability(i,child[var][k], realData[i][child[var][k]]);
                }
                probs_sum += prob;
                probs.pb(prob);
            }
            int max_index = max_element(probs.begin(), probs.end()) - probs.begin();
            realData[i][var] = max_index;

            float alpha = 1.0/probs_sum;
            for(int j=0; j<probs.size(); j++)
                weights.pb(alpha*probs[j]);
        }
    }

    void runEM()
    {
        int iter_count = 0;
        initCPT();
        updateDataset();
        while(time(NULL) - initTime < TOTAL_TIME-5)
        {
            iter_count++;
        	updateCPT();
            updateDataset();
        }
        rewrite();
    }
};

Graph* G;

int main(int argc, char** argv)
{
	srand(time(NULL));
    G = new Graph(argv[1], time(NULL));
    G->runEM();
    return 0;
}
