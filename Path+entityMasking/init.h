#ifndef INIT_H
#define INIT_H
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <cstdio>
#include <float.h>
#include <cmath>

using namespace std;


string version = "";

int output_model = 0;

int num_threads = 10;
int trainTimes = 25;
float alpha = 0.02;
float reduce = 0.98;
int tt,tt1;
int dimensionC = 230;
int dimensionWPE = 5;
int dimensionR = 40;	//dimension of relation embedding, aim to grasp the similarity between r

int window = 3;
int limit = 30;
float marginPositive = 2.5;
float marginNegative = 0.5;
float margin = 2;
float Belt = 0.001;
int num_path_max = 0;
float *matrixB1, *matrixRelation, *matrixW1, *matrixRelationDao, *matrixRelationPr, *matrixRelationPrDao;
float *matrixB1_egs, *matrixRelation_egs, *matrixW1_egs, *matrixRelationPr_egs;
float *matrixB1_exs, *matrixRelation_exs, *matrixW1_exs, *matrixRelationPr_exs;
float *wordVecDao,*wordVec_egs,*wordVec_exs;
float *positionVecE1, *positionVecE2, *matrixW1PositionE1, *matrixW1PositionE2;
float *positionVecE1_egs, *positionVecE2_egs, *matrixW1PositionE1_egs, *matrixW1PositionE2_egs, *positionVecE1_exs, *positionVecE2_exs, *matrixW1PositionE1_exs, *matrixW1PositionE2_exs;
float *matrixW1PositionE1Dao;
float *matrixW1PositionE2Dao;
float *positionVecDaoE1;
float *positionVecDaoE2;
float *matrixW1Dao;
float *matrixB1Dao;
float *vectorRelation;
float *vectorRelationDao;
double mx = 0;
int batch = 16;
int npoch;
int len;
float rate = 1;
float w = 0.5;
FILE *logg;

float *wordVec;
int wordTotal, dimension, relationTotal, testTotal;
int PositionMinE1, PositionMaxE1, PositionTotalE1,PositionMinE2, PositionMaxE2, PositionTotalE2;
map<string,int> wordMapping;
vector<string> wordList;
map<string,int> relationMapping;
vector<int *> trainLists, trainPositionE1, trainPositionE2;
vector<int> trainLength;
vector<int> headList, tailList, relationList;
vector<int *> testtrainLists, testPositionE1, testPositionE2;
vector<int> testtrainLength;
vector<int> testheadList, testtailList, testrelationList;
vector<std::string> nam;

map<string,vector<int> > bags_train, bags_test;
//For training data, use relation as the key, all the entity pair will
//be replaced by a universal vector, maybe like X and Y
map<string,vector<int> > bags_relation_train;
map<pair<string,string>, string> entity_pair_relation; 
map<string, string> entity_relation; 

map<string,int> test_ok;
map<string,int> bags_train_id,bags_test_id;
map<string,vector<int *> >train_path, test_path;
map<int, vector<int> >train_pair,test_pair;
map<pair<int,int>, vector<int> >train_pair_bags,test_pair_bags;


vector<int *> path_testtrainLists, path_testPositionE1, path_testPositionE2;
vector<int> path_testtrainLength;
vector<int> path_testheadList, path_testtailList, path_testrelationList;
map<string,vector<int> > path_bags_test;
map<string,int> path_bags_test_id;



void init() {
	FILE *f = fopen("../data/vec4.bin", "rb");
	fscanf(f, "%d", &wordTotal);
	fscanf(f, "%d", &dimension);
	cout<<"wordTotal=\t"<<wordTotal<<endl;
	cout<<"Word dimension=\t"<<dimension<<endl;
	PositionMinE1 = 0;
	PositionMaxE1 = 0;
	PositionMinE2 = 0;
	PositionMaxE2 = 0;
	wordVec = (float *)malloc((wordTotal+1) * dimension * sizeof(float));
	wordList.resize(wordTotal+1);
	wordList[0] = "UNK";
	for (int b = 1; b <= wordTotal; b++) {
		string name = "";
		while (1) {
			char ch = fgetc(f);
			if (feof(f) || ch == ' ') break;
			if (ch != '\n') name = name + ch;
		}
		int last = b * dimension;
		float smp = 0;
		for (int a = 0; a < dimension; a++) {
			fread(&wordVec[a + last], sizeof(float), 1, f);
			smp += wordVec[a + last]*wordVec[a + last];
		}
		smp = sqrt(smp);
		for (int a = 0; a< dimension; a++)
			wordVec[a+last] = wordVec[a+last] / smp;
		wordMapping[name] = b;
		wordList[b] = name;
	}
	wordTotal+=1;
	fclose(f);
	char buffer[1000];
	f = fopen("../data/relation2id.txt", "r");
	while (fscanf(f,"%s",buffer)==1) {
		int id;
		fscanf(f,"%d",&id);
		relationMapping[(string)(buffer)] = id;
		relationTotal++;
		nam.push_back((std::string)(buffer));
	}
	fclose(f);
	cout<<"relationTotal:\t"<<relationTotal<<endl;
	
	f = fopen("../data/train.txt", "r");
	while (fscanf(f,"%s",buffer)==1)  {
		string e1 = buffer;
		fscanf(f,"%s",buffer);
		string e2 = buffer;
		fscanf(f,"%s",buffer);
		string head_s = (string)(buffer);
		int head = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);
		int tail = wordMapping[(string)(buffer)];
		string tail_s = (string)(buffer);
		fscanf(f,"%s",buffer);
		// The push_back(headList.size()) just indicate the sentence id
		bags_train[e1+"\t"+e2+"\t"+(string)(buffer)].push_back(headList.size());
		bags_relation_train[(string)(buffer)].push_back(headList.size());
		// Y->cover, N->insert
		entity_pair_relation[make_pair(e1,e2)] = (string)(buffer);
	//	entity_pair_relation[make_pair(e2,e1)] = (string)(buffer);
		entity_relation[e1+"\t"+e2+"\t"+(string)(buffer)] = (string)(buffer);

		int num = relationMapping[(string)(buffer)];
		int len = 0, lefnum = 0, rignum = 0;
		std::vector<int> tmpp;
		while (fscanf(f,"%s", buffer)==1) {
			std::string con = buffer;
			if (con=="###END###") break;
			int gg = wordMapping[con];
			if (con == head_s) lefnum = len;
			if (con == tail_s) rignum = len;
			len++;
			tmpp.push_back(gg);
		}
		headList.push_back(head);
		tailList.push_back(tail);
		relationList.push_back(num);
		trainLength.push_back(len);
		int *con=(int *)calloc(len,sizeof(int));
		int *conl=(int *)calloc(len,sizeof(int));
		int *conr=(int *)calloc(len,sizeof(int));
		for (int i = 0; i < len; i++) {
			con[i] = tmpp[i];
			conl[i] = lefnum - i;
			conr[i] = rignum - i;
			if (conl[i] >= limit) conl[i] = limit;
			if (conr[i] >= limit) conr[i] = limit;
			if (conl[i] <= -limit) conl[i] = -limit;
			if (conr[i] <= -limit) conr[i] = -limit;
			if (conl[i] > PositionMaxE1) PositionMaxE1 = conl[i];
			if (conr[i] > PositionMaxE2) PositionMaxE2 = conr[i];
			if (conl[i] < PositionMinE1) PositionMinE1 = conl[i];
			if (conr[i] < PositionMinE2) PositionMinE2 = conr[i];
		}
		trainLists.push_back(con);
		trainPositionE1.push_back(conl);
		trainPositionE2.push_back(conr);
	}
	fclose(f);
	cout <<"Show e1 e2 re: " << bags_train.size() << endl;
	cout <<"Show pair: " << entity_pair_relation.size() << endl;
	cout <<"Show string: " << entity_relation.size() << endl;

	printf("initializing training path\n");

	vector<string> a_train;
	int bags_id = 0;
	for (map<string,vector<int> >:: iterator it = bags_train.begin(); it!=bags_train.end(); it++)
	{
		a_train.push_back(it->first);
		bags_train_id[it->first] = bags_id;
		bags_id++;
	}
	f = fopen("./train_path.txt","r");
	while (fscanf(f,"%s",buffer)==1) {
		string e1 = buffer;
		fscanf(f,"%s",buffer);
		string e2 = buffer;
		fscanf(f,"%s",buffer);

		int id;
		fscanf(f,"%d",&id);
		while(id != -1)
		{
			int *a = (int *)malloc((id+1)* sizeof(int));
			a[0] = id;
			fscanf(f,"%d",&id);
			a[1] = id;
			fscanf(f,"%d",&id);
			a[2] = id;
			train_path[e1+"\t"+e2+"\t"+(string)(buffer)].push_back(a);
			fscanf(f,"%d",&id);
		}
	}
	fclose(f);

	f = fopen("../data/test.txt", "r");	
	while (fscanf(f,"%s",buffer)==1)  {
		string e1 = buffer;
		fscanf(f,"%s",buffer);
		string e2 = buffer;
		bags_test[e1+"\t"+e2].push_back(testheadList.size());
		fscanf(f,"%s",buffer);
		string head_s = (string)(buffer);
		int head = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);
		string tail_s = (string)(buffer);
		int tail = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);
		int num = relationMapping[(string)(buffer)];
		int len = 0 , lefnum = 0, rignum = 0;
		std::vector<int> tmpp;
		while (fscanf(f,"%s", buffer)==1) {
			std::string con = buffer;
			if (con=="###END###") break;
			int gg = wordMapping[con];
			if (head_s == con) lefnum = len;
			if (tail_s == con) rignum = len;
			len++;
			tmpp.push_back(gg);
		}
		testheadList.push_back(head);
		testtailList.push_back(tail);
		testrelationList.push_back(num);
		testtrainLength.push_back(len);
		int *con=(int *)calloc(len,sizeof(int));
		int *conl=(int *)calloc(len,sizeof(int));
		int *conr=(int *)calloc(len,sizeof(int));
		for (int i = 0; i < len; i++) {
			con[i] = tmpp[i];
			conl[i] = lefnum - i;
			conr[i] = rignum - i;
			if (conl[i] >= limit) conl[i] = limit;
			if (conr[i] >= limit) conr[i] = limit;
			if (conl[i] <= -limit) conl[i] = -limit;
			if (conr[i] <= -limit) conr[i] = -limit;
			if (conl[i] > PositionMaxE1) PositionMaxE1 = conl[i];
			if (conr[i] > PositionMaxE2) PositionMaxE2 = conr[i];
			if (conl[i] < PositionMinE1) PositionMinE1 = conl[i];
			if (conr[i] < PositionMinE2) PositionMinE2 = conr[i];
		}
		testtrainLists.push_back(con);
		testPositionE1.push_back(conl);
		testPositionE2.push_back(conr);
	}
	fclose(f);

	printf("initializing testing path\n");

	vector<string> a_test;
	bags_id = 0;
	for (map<string,vector<int> >:: iterator it = bags_test.begin(); it!=bags_test.end(); it++)
	{
		a_test.push_back(it->first);
		bags_test_id[it->first] = bags_id;
		bags_id++;
	}
	f = fopen("./test_path.txt","r");
	while (fscanf(f,"%s",buffer)==1) {
		string e1 = buffer;
		fscanf(f,"%s",buffer);
		string e2 = buffer;

		int id;
		fscanf(f,"%d",&id);
		while(id != -1)
		{
			int *a = (int *)malloc((id+3)* sizeof(int));
			a[0] = id;
			fscanf(f,"%d",&id);
			a[1] = id;
			fscanf(f,"%d",&id);
			a[2] = id;
			fscanf(f,"%d",&id);
			a[3] = id;
			fscanf(f,"%d",&id);
			a[4] = id;
			test_path[e1+"\t"+e2].push_back(a);
			fscanf(f,"%d",&id);
		}
	}
	fclose(f);

	cout<<PositionMinE1<<' '<<PositionMaxE1<<' '<<PositionMinE2<<' '<<PositionMaxE2<<endl;

	for (int i = 0; i < trainPositionE1.size(); i++) {
		int len = trainLength[i];
		int *work1 = trainPositionE1[i];
		for (int j = 0; j < len; j++)
			work1[j] = work1[j] - PositionMinE1;
		int *work2 = trainPositionE2[i];
		for (int j = 0; j < len; j++)
			work2[j] = work2[j] - PositionMinE2;
	}

	for (int i = 0; i < testPositionE1.size(); i++) {
		int len = testtrainLength[i];
		int *work1 = testPositionE1[i];
		for (int j = 0; j < len; j++)
			work1[j] = work1[j] - PositionMinE1;
		int *work2 = testPositionE2[i];
		for (int j = 0; j < len; j++)
			work2[j] = work2[j] - PositionMinE2;
	}
	PositionTotalE1 = PositionMaxE1 - PositionMinE1 + 1;
	PositionTotalE2 = PositionMaxE2 - PositionMinE2 + 1;
}

float CalcTanh(float con) {
	if (con > 20) return 1.0;
	if (con < -20) return -1.0;
	float sinhx = exp(con) - exp(-con);
	float coshx = exp(con) + exp(-con);
	return sinhx / coshx;
}

float tanhDao(float con) {
	float res = CalcTanh(con);
	return 1 - res * res;
}

float sigmod(float con) {
	if (con > 20) return 1.0;
	if (con < -20) return 0.0;
	con = exp(con);
	return con / (1 + con);
}

int getRand(int l,int r) {
	int len = r - l;
	int res = rand()*rand() % len;
	if (res < 0)
		res+=len;
	return res + l;
}

float getRandU(float l, float r) {
	float len = r - l;
	float res = (float)(rand()) / RAND_MAX;
	return res * len + l;
}

void norm(float* a, int ll, int rr)
{
	float tmp = 0;
	for (int i=ll; i<rr; i++)
		tmp+=a[i]*a[i];
	if (tmp>1)
	{
		tmp = sqrt(tmp);
		for (int i=ll; i<rr; i++)
			a[i]/=tmp;
	}
}

double relation_loss(int r0, int r1, int r2, int flag1, int flag2, double g_bar)
{
	double relation_loss = 0;
	if(flag1 == 0)
	{
		vector<double> f_loss;
		double sum = 0.0;
		// calculate all relation with rA+rB
		for (int rr = 0; rr < relationTotal; rr++)
		{
			double loss_tmp = 0;
			for(int i = 0; i < dimensionR; i++)
			{
				loss_tmp -= abs(vectorRelationDao[rr*dimensionR+i]-vectorRelationDao[r1*dimensionR+i]-vectorRelationDao[r2*dimensionR+i]);
			}
			f_loss.push_back(exp(loss_tmp));
			sum += exp(loss_tmp);
		}
		relation_loss = f_loss[r0]/sum;
		if (flag2 == 1)
		{
			for (int i = 0; i < dimensionR; i++)
			{
				// update all relations
				for (int rr = 0; rr < relationTotal; rr++)
				{
					if (rr == r0)
					{
						double grad_r = 1-(f_loss[r0]/sum);
						// The grad for rr==r0
						grad_r = -grad_r;
						// if(ri-(rA+rB))>=0
						// abs need to be divided to differnent cases
						if ((vectorRelationDao[r0*dimensionR+i]-vectorRelationDao[r1*dimensionR+i]-vectorRelationDao[r2*dimensionR+i])>=0)
						{
							// if r0-(r1+r2)>0, then our target is min it, else max it
							grad_r = grad_r * 1;
						}
						else
						{
							// objective is max it, so the opposite way
							grad_r = -grad_r;
						}
						// Here is plus, so it is max it
						vectorRelation[r0*dimensionR+i] += grad_r*g_bar*alpha;
					}

					else if(rr == r1)
					{
						// Why????
						double grad_r = -1;
						if ((vectorRelationDao[r0*dimensionR+i]-vectorRelationDao[r1*dimensionR+i]-vectorRelationDao[r2*dimensionR+i])>=0)
						{
							grad_r = -grad_r;
						}
						else
						{
							grad_r = grad_r;
						}

						for (int id = 0; id < relationTotal; id++)
						{
							double grad_tmp = f_loss[id]/sum;
							if ((vectorRelationDao[id*dimensionR+i]-vectorRelationDao[r1*dimensionR+i]-vectorRelationDao[r2*dimensionR+i])>=0)
							{
								grad_tmp = grad_tmp;
							}
							else
							{
								grad_tmp = -grad_tmp;
							}
							if(id == r1)
								grad_tmp = 0;
							// 减去由其他relation带来的梯度,why?
							grad_r -= grad_tmp;
						}
						vectorRelation[r1*dimensionR+i] += grad_r*g_bar*alpha;
					}
					else if(rr == r2)
					{
						double grad_r = -1;
						if ((vectorRelationDao[r0*dimensionR+i]-vectorRelationDao[r1*dimensionR+i]-vectorRelationDao[r2*dimensionR+i])>=0)
						{
							grad_r = -grad_r;
						}
						else
						{
							grad_r = grad_r;
						}

						for (int id = 0; id < relationTotal; id++)
						{
							double grad_tmp = f_loss[id]/sum;
							if ((vectorRelationDao[id*dimensionR+i]-vectorRelationDao[r1*dimensionR+i]-vectorRelationDao[r2*dimensionR+i])>=0)
							{
								grad_tmp = grad_tmp;
							}
							else
							{
								grad_tmp = -grad_tmp;
							}
							if(id == r2)
								grad_tmp = 0;

							grad_r -= grad_tmp;
						}
						vectorRelation[r2*dimensionR+i] += grad_r*g_bar*alpha;
					}
					else
					{
						// d(loss)/d(p) * d(p)/d(o_i) * d(o_i)/d(r_i)
						double grad_r = f_loss[rr]/sum;
						if ((vectorRelationDao[rr*dimensionR+i]-vectorRelationDao[r1*dimensionR+i]-vectorRelationDao[r2*dimensionR+i])>=0)
						{
							grad_r = grad_r;
						}
						else
						{
							grad_r = -grad_r;
						}
						vectorRelation[rr*dimensionR+i] += grad_r*g_bar*alpha;
					}
				}
			}
		}
	}
	
	if(flag1 == 1)
	{
		vector<double> f_loss;
		double sum = 0.0;
		for (int rr = 0; rr < relationTotal; rr++)
		{
			double loss_tmp = 0;
			for(int i = 0; i < dimensionR; i++)
			{
				loss_tmp -= abs(vectorRelation[rr*dimensionR+i]-vectorRelation[r1*dimensionR+i]-vectorRelation[r2*dimensionR+i]);
			}
			f_loss.push_back(exp(loss_tmp));
			sum += exp(loss_tmp);
		}
		relation_loss = f_loss[r0]/sum;
	}

	return relation_loss;
}


#endif
