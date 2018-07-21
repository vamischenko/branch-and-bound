// program.cpp: ���������� ����� ����� ��� ����������� ����������.
//

// ��� visual studio ���� �����������������
//#include "stdafx.h"

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#elif defined(linux) || defined(UNIX) || defined(__unix__)
#define UNIX
#else
#error Undefined OS
#endif

#ifdef WINDOWS
#include <conio.h>
#endif

#ifdef UNIX
#include <string.h>
#include <curses.h>
#endif

#include <stdio.h>  // ����-�����

#include <ctime>    // ����������� ������� ������ ���������

#include <list>     // ���������� ��������� ������
#include <iterator> // ��������� ����������

#ifdef WINDOWS
#include  "Windows.h"// ��������� ��� win
#endif

using namespace std;

#define  NMAX 1000 // ������������ ���-�� �����

// ��������� ����������� ������ 
// ������ ���������� � ����� �++ � ����
struct TJob{
	int num; // �������� ����� ������
	int p;   // processing time
	int r;   // release date
	int d;   // deadline
};

// ��������������� ����� ��� �������� ������ �� ��������
class TJ{
public:
	int num;
	int param;

	TJ(){num=0;param=0;}            // ������ �����������
	TJ(int a,int b){num=a;param=b;} // ����������� � �����������
	TJ(const TJ  &copyin){num=copyin.num;param=copyin.param;}// ����������� ����������
	~TJ(){};                         // ������ ����������
	void fill(int a,int b){num=a;param=b;} // ��������� 
	int operator<(const TJ &rhs) const{ // ���������� ��������� < ��� list::sort

		if( this->param < rhs.param ) return 1;
		return 0;
	}
	TJ& operator=(const TJ &rhs){ // ������������
	   this->num= rhs.num;
	   this->param = rhs.param;
	   return *this;
	}

	int operator==(const TJ &rhs) const // ���������
	{
	   if( this->num != rhs.num) return 0;
	   return 1;
	}
	
};

// ���������� �������� � ������ �� ����������� ���������
void AddNewItemInSortedList(list<TJ> &m_list,int num, int param, bool isUnique = false){
  int i;

  // ���� ������ ���� 
  if (m_list.empty()){
	  m_list.push_front( TJ(num, param) );
      return; // � �������
  }

  //���� ����� ������� >= ����������, ��������� � �����
  list<TJ>::iterator ir = m_list.end();ir--; 
   if( (*ir).param <= param ) {
	  if(!(isUnique && ((*ir).param == param)))
		  m_list.push_back( TJ(num, param) );
	  return; // � �������
  }

  // ���� ����� ������� <= �������, ��������� � ������
  if ( ( (*(m_list.begin())).param >= param ) ) {
	  if(!(isUnique && ((*(m_list.begin())).param == param)))
		  m_list.push_front( TJ(num, param) );
	  return; // � �������
  }

  // ��������� �� ������� � ��������� ��������
  ir = (m_list.begin()); 
  list<TJ>::iterator ir1=ir; ir1++;

  // ���� �� ������, ����� ����� ����� �������
  while( ir1 != m_list.end()){

	  // ����� ����� �������
	  if( (*ir).param <= param && param <=  (*ir1).param ) {
		  
		  // ���� ��������� �������� ������ � ����������� param � �������� ���������
		  if (  isUnique && ( (*ir).param == param || param ==  (*ir1).param))
			return; //������� �� ���������, �������
	  
		  // � ����� ������ ������ ��������� ����� �������� ����� ��������
		  m_list.insert(ir1,TJ(num, param));
		  return; // � �������
	  }

	  ir++;
	  ir1++;
  }

}

// ������� readFromFile ������ �� ����� ������ � ������� ������ �����
// ������ ������ ����� ��������� ��������� ������
// ������ ������� - p (processing time), ������ - r (release date), ������ - d (deadline) 
// ������ ��������� ��������
//
// ���������� ������ ����� � ���������� ��������� ������� ����� ���������
bool readFromFile(const char *fname, TJob* Jobs, int &n){
	
	int num = 0; // ����� ������
	
	FILE* f = fopen(fname,"r"); // ��������� ����

	if (!f){                    // ���� ��������� ���� - �������� �������� � ��������� �����
		printf("Error opening file %s\n", fname);  // ������� ������
		return false;            // ���������� ������� ������
	}

	while(!feof(f)){ // ��������� ��� ������ �� �����
		Jobs[num].num = num; // ����������� ������ ��������� �����
		fscanf(f,"%d,%d,%d\n",&Jobs[num].p, &Jobs[num].r, &Jobs[num].d); // ������ ������ �� �����
		printf("%3d %3d %3d\n", Jobs[num].p, Jobs[num].r, Jobs[num].d);
		num++; // ����������� ����� ��������� ������ �� 1
	}

	fclose(f); // ��������� ����

	n = num; // ���������� ���������� �����

	if ( n == 0 ){
		printf("File %s is empty\n", fname);  // ������� ������
		return false; // ���������� ������� ������
	}

	return true;
} 

// ������� printToFile ���������� � ���� ��������� ������
// ������ ������ ����� ��������� ��������� ������
bool printToFile(char *fname, list<TJ> J, int Lmax){

	char fnameOUT[255], tmp[255];
	

#ifdef WINDOWS
	char delimiter = '\\';
#else
	char delimiter = '/';
#endif

	strncpy(fnameOUT, fname, 254);
	char * first = strrchr(fnameOUT, delimiter);
	if (first == NULL){
		strncpy(fnameOUT, "result_", 254);
		strncat(fnameOUT, fname, 254);
	}
	else{
		*first = '\0';
		first++;
		strncpy(tmp, "\\result_", 254);
		strncat(tmp, first, 254);
		strncat(fnameOUT, tmp, 254);
	}



	FILE* f = fopen(fnameOUT, "w"); // ��������� ����

	if (!f){                    // ���� ��������� ���� - �������� �������� � ��������� �����
		printf("Error creating file %s\n", fnameOUT);  // ������� ������
		return false;           // ���������� ������� ������
	}


	for (list<TJ>::iterator it = J.begin(); it != J.end(); ++it)
		fprintf(f,"%d ", (*it).num + 1);

	fprintf(f,"\n%d",Lmax); //������� Lmax

	fclose(f); // ��������� ����

	return true;
}

// ���� ��� ���������� J ������������ �������� ��������� Lmax 
int calcLmax(list<TJ> J, TJob* JobsParamReadOnly){

	// ��� ����� �������� ������������ ���������
	// ��� ����� ������� �� ������
	int Lmax = -9999999; // ����� ��������� �������� ����� ����������
	int t = 0; // ����� ������ ����� = 0
	for (list<TJ>::iterator it = J.begin(); it != J.end(); ++it){
		int p = JobsParamReadOnly[(*it).num].p; // ����������������� ������
		int r = JobsParamReadOnly[(*it).num].r; // ����� ������ ������
		int d = JobsParamReadOnly[(*it).num].d; // �������
		t = max(t,r) + p; // ����� ���������� ������
		int L = t - d;    // �������� ������

		if ( L > Lmax ) Lmax = L; // ���� �������� ������ ������������, ���������� 

	}

	return Lmax;
}

// ������� EDD ������������� ������ ����� J 
// �� ����������� �������� JobsParam[j].d
// �� ����� 
//    J -������ �����, ��� ������� ����������� ����� �������
// ���������� ������ ��������������� �����
// � ����� �������� Lmax - ������������ �������� ���������
list<TJ> EDD(list<TJ> J, TJob* JobsParamReadOnly, int &Lmax){

	// ������ J ��� ������������ �� d
	// ������� ��� ����� �������� ������ ������������ ���������
	Lmax = calcLmax(J,JobsParamReadOnly);

	return J;
}

// ������� ������� ����������� ������� ������ 1|rj,prmp|Lmax
// ����������� ������������� ��������� ������ single machine with relese date
// � ������������ ��������� ������ � �������� �����
// 
// ���������� ������ ��������������� �����
// � �������� � Lmax - ������������ �������� ���������
list<TJ> one_rj_prmp_Lmax(list<TJ> Jnofinish, TJob* JobsParamReadOnly,int n, int &Lmax, bool &preemptionNoExist,int t=0){
	
	Lmax = -99999999;
	preemptionNoExist = true;

	//������� ����� ������, ����� ������� ���������
	list<TJ> Jrez; 

	// ���� ������ ����, ���������� ������ ��������
	if (Jnofinish.empty())
		return Jrez;

	bool f = false; // ����� ������ ������� � �������������, �����, ����� ���������� ������ �����

	// �������� ������ �� J � Jnofinish - ��� ������ ��� �� ����������� �����
	//list<TJ> Jnofinish = J; 


	// �������� ��������� �����, ��� ��� ����� ������� �������� p
	TJob JobsParam[NMAX];
	memcpy(JobsParam, JobsParamReadOnly, sizeof(TJob)*n);

	// �������� �� ��������������� ������ �������� r ��� ����� �� J
	list<TJ> Jr;
	Jr.push_front(TJ(-1,t));
	for (list<TJ>::iterator it = Jnofinish.begin(); it != Jnofinish.end(); ++it){		
		// ���� ��������� �������� r ��� �� ��������� � ������ (�������� ������ ���������� r) 
		if (JobsParam[(*it).num].r >= t)
			AddNewItemInSortedList(Jr, (*it).num, JobsParam[(*it).num].r, true);
	}
		
	// ���� �� r1<r2<..<rn
	list<TJ>::iterator ir = (Jr.begin()); 
	while( ir != Jr.end()){

		// ���� Jnofinish ���� - ���������� ����������
		if( Jnofinish.empty())
			return Jrez;

		// ��������� ����� ����� r_min
		t = max(t, (*(ir)).param); 

		list<TJ>::iterator ir1 = ir; ir1++; // � ir1 ������ r[i+1]
		if ( ir1 == Jr.end()) // ������� �� ����� �������
			break;
		// ���� ����� ������������� ����� �� Jnofinish
		// ��������������� ������� r[j] <= t
		// ��� ��� Jnofinish �������������� �� ���������, �����  �� ������� EDD
		// ����� ����������� �������� �� ������ ������ � ������
		list<TJ>::iterator id = Jnofinish.begin();      // ������ ������� ������
		list<TJ>::iterator iend; // ��������� ������� ������
		while( id!=Jnofinish.end()){
			 
			//���� ����� ����� ������
			if (JobsParam[((*id).num)].r <= t) {

				if ( !Jrez.empty() ) {iend = Jrez.end(); iend--;}
				// ���� ������ ���� � ������ ��� �� ���� ��������� � ����� ������
				if(Jrez.empty() || (*iend).num != (*id).num){
					Jrez.push_back((*id));
					// ���� �� ���������� �������� ������ ���� �� ��������
					// �� �������� ������ ������ �.�. ��������� � ������� ������ �� �������
					if (f)
						preemptionNoExist= false;
				}
				else // � ���� ������ �� �� ��� � ������, ���������� ���� �������
					f = false;


				// ���� t+p ������ r[j+1] 
				if (t + JobsParam[((*id).num)].p <= (*(ir1)).param){
					// ����������� ����� 
					t = max(t,JobsParam[((*id).num)].r)+JobsParam[((*id).num)].p;
					//������� �������� � ��������� ���� ��� ������ �����������
					int L = t - JobsParam[((*id).num)].d;
					if (L > Lmax) Lmax = L;
					// ������� ������ �� ������, ���-��� ��������� �
					list<TJ>::iterator tmp_id = id; tmp_id++;
					Jnofinish.erase(id);
					id = tmp_id;//

				}
				else {// ����� ��������� ������ �.�. ����� �� ��������� ������� r[i]
					f = true; // ���������� ���� ������� ������ 
					JobsParam[((*id).num)].p -= (*ir1).param - t; // ��������� ������ �� ����� ������������ �����
					t = (*ir).param; // t=r[i] ����� 
					break; // ��������� ���� while( id!=Jnofinish.end())
				}

			}
			else // ������� ������ �� ��������, ��������� � ���������
				id++;

			// ���� �����
		}

		ir++;
	}

	// �������� ������� ������ 
	if( !Jnofinish.empty() )

		for (list<TJ>::iterator it = Jnofinish.begin(); it != Jnofinish.end(); ++it){
			list<TJ>::iterator iend;
			if ( !Jrez.empty() ) {iend = Jrez.end(); iend--;}
			// ���� ������ ���� � ������ ��� �� ���� ��������� � ����� ������
			if(Jrez.empty() || (*iend).num != (*it).num){
				Jrez.push_back((*it));
				// ���� �� ���������� �������� ������ ���� �� ��������
				// �� �������� ������ ������ �.�. ��������� � ������� ������ �� �������
				if (f)
					preemptionNoExist= false;
			}
			else // � ���� ������ �� �� ��� � ������, ���������� ���� �������
				f = false;

			// ����������� ����� 
			t = max(t,JobsParam[((*it).num)].r)+ JobsParam[((*it).num)].p;
			//������� �������� � ��������� ���� ��� ������ �����������
			int L = t - JobsParam[((*it).num)].d;
			if (L > Lmax) Lmax = L;
		}

	return Jrez;
}

// ����������� ������ 1|rj|Lmax 
// ������� ������ � ������
// ������� ���������
// Jnofinish - ������, ������� ��� �� ������ � ���������� (�� ��������� �� ������ t)
// JobsParamReadOnly - ������, �������� p,r,d ��� Jj ������ � j �������
// n - ���������� �����, �����
// Jshed - � ���� ������ ����������� ������� ����������
// Jrecord - � ���� ������ �������� ����� ��������� ������ (���������� � ������ Lmax = Lrecord)
// Lbmin - ���������� ��������� �������� Lmax, ���� ����� ����� ����������� ���������� - ������� �������
// Lrecord - ������ Lmax ��������� �����
// t ������ �������, ��� �������� ����������� ����������

// ��������� ������ ������������ � Jrecord, Lrecord
void one_rj_Lmax_recurs (list<TJ> Jnofinish, TJob* JobsParamReadOnly, int n,
	list<TJ> Jshed, list<TJ> &Jrecord, int LbMin, int &Lbrecord,  int t = 0, int lvl=0){
		
		/*
		// �������� ����
		printf("\n\n\Lvl = %d" , lvl);
		printf("\nt = %d" , t);
		printf("\nLb = %d \n", Lbrecord);
		for (list<TJ>::iterator it = Jshed.begin(); it != Jshed.end(); ++it)
			printf("%d ",(*it).num + 1);// ���������� � ������� ����� 1, ����� �������� ��������� � 0 �� 1
		//getch();
		*/

		int Lbcur; // ������� �������� ����������� ������� ��� ���������� ������� ����� ��������� ������

		if (Jnofinish.empty()){ // ������ ���������������� ����� ����
			
			Lbcur = calcLmax(Jshed,JobsParamReadOnly);
			if (Lbcur < Lbrecord){
				Lbrecord = Lbcur; // ��������� ������� �������� Lmax
				Jrecord = Jshed;
			}
			return; // ������ ���������
		}
		
		//----------- ��������� ������� �����
		if (t > 0){ // ������ �� ��������
			
			
			bool preemptionNoExist; // ���� �� ���������� ���������� � ������������ �������� ����������� ����������
			// ���� Lbcur � ������ ���������� � ���������� ���������
			
			list<TJ> Jcurend = one_rj_prmp_Lmax(Jnofinish,JobsParamReadOnly,n,Lbcur,preemptionNoExist, t); 
			int Lbcur2 = calcLmax(Jshed, JobsParamReadOnly);

			//printf("\n\nL1 = %d\n",Lbcur);
			//printf("L2 = %d",Lbcur2);

			Lbcur = max(Lbcur,Lbcur2);
			list<TJ> Jcur = Jshed;
			Jcur.splice(Jcur.end(),Jcurend); // ������� ������ � ����� ����������

			
			printf("\nL = %d\n",Lbcur);
			for (list<TJ>::iterator it = Jcur.begin(); it != Jcur.end(); ++it){
				printf("%d ",(*it).num + 1);// ���������� � ������� ����� 1, ����� �������� ��������� � 0 �� 1
			}

			// ���� ���������� ���������� � �������������� ��������
			// � ���������� ��������� ������ ������� ������� � �������, ��������� ������
 			if( Lbcur == LbMin && preemptionNoExist){
				Lbrecord = Lbcur; // ��������� ������� �������� Lmax
				Jrecord = Jcur;   // ��������� ���������� ������
				return; // � ���������� �����
			}

			// ���� �������� ������ ������� ������ �������, �������� ��� �����
			if( Lbcur >= Lbrecord)
				return;// � ���������� ���������

			// ���� �������� ������ ������� ������ ������� � ������ � ���������� �� ��������� 
			// ��������� ����� ������ � ��������� � ���������
			if( Lbcur < Lbrecord && preemptionNoExist )

				Lbrecord = Lbcur; // ��������� ������� �������� Lmax
				Jrecord = Jcur;   // ��������� ���������� ������

		}


		//-----------  ��������� 

	    // ���� �� Jnofinish
		// ���� min(max(t,r) + p)
		list<TJ>::iterator ir = (Jnofinish.begin()); 
		// �������������� ������ min
		int num = (*ir).num;
		int min = max(t,JobsParamReadOnly[num].r) + JobsParamReadOnly[num].p;
		ir++;
		while( ir != Jnofinish.end()){
			num = (*ir).num;
			int min1 = max( t, JobsParamReadOnly[num].r ) + JobsParamReadOnly[num].p;
			if (min1 < min) min = min1;
			ir++;     // ��������� � ��������� ������
		}

		// ���� ��� ������ Jj rj >= min, �������� �����
		ir = (Jnofinish.begin());
		while( ir != Jnofinish.end()){
			
			int num = (*ir).num; // ����� ������
			int param = (*ir).param;

			if ( JobsParamReadOnly[num].r >= min ){ // ���� ������� ��������� �����
				ir++;     // ��������� � ��������� ������
				continue; // ���������� ���������� �������� ��� ������� ������
			}

			// ����� ���������� ���������
			
			int tnew = max(t,JobsParamReadOnly[num].r) + JobsParamReadOnly[num].p; // ������� ����� ��� ����� �����
			Jshed.push_back(*ir);                  // ��������� ������� ������ � ����� ����������
			list<TJ>::iterator irtmp = ir;irtmp++; // �������������� ��������� ��� ��������
			Jnofinish.erase(ir);				   // ������� ������� ������ �� ����������������
			
			// ��������
			one_rj_Lmax_recurs (Jnofinish,JobsParamReadOnly, n,
												Jshed, Jrecord,LbMin,Lbrecord,tnew,lvl+1);

			if ( Lbrecord == LbMin ) 
				return; // ������� ����������� �������, ���������� �����
			
			Jshed.pop_back();                    // ������� ������ �� ����������
			Jnofinish.insert(irtmp,TJ(num, param)); // ���������� �� ����� ������ � ������ ����������������
			ir = irtmp;
		}
}

// ��� ������� �������������� ������ ��� 
// ������������ ������� 1|rj|Lmax 
//
// ���������� ������ ��������������� �����
// � �������� ����� �������� Lmax - ������������ �������� ���������
list<TJ> one_rj_Lmax(list<TJ> Jnofinish, TJob* JobsParamReadOnly,int n, int &Lmax){
	
	int Lb;      // ��� ������ ������� - �������� �� one_rj_prmp_Lmax
	int Lbrecord; // ��������� ������ ���������� � EDD
	bool preemptionNoExist; // � ���� ���������� one_rj_prmp_Lmax ������ ������� ������������ ����������
	list<TJ> Jrecord;       // ��� ���������� - ������, � ���� ������ Lmax >= Lb
	list<TJ> Jshed;         // � ���� ������ ����� ������������� ����������
	
	Lbrecord = -999999999; // �������� ���. ����������
	Lmax     = -999999999; // �������� ���. ����������
	Lb       = -999999999; // �������� ���. ����������

	// ������� Lb - ��� ����������� �������� Lmax, ������� ����� ���� � ����� ������
	Jrecord = one_rj_prmp_Lmax(Jnofinish,JobsParamReadOnly,n,Lb,preemptionNoExist);
	printf("\nL_1_rj_prmp_Lmax = %d", Lb);

	// ���� one_rj_pmp_Lmax ������� ����������� ���������� -������� �������
	if (preemptionNoExist) {
		Lmax = Lb;      // ����������� ������������ �������� ���������
		return Jrecord; // � ���������� ��������� ����������
	}

	// ����� ��������� ����������-������ � ������� EDD
	Jrecord = EDD(Jnofinish, JobsParamReadOnly, Lbrecord);

	printf("\nLrecord = %d\n", Lbrecord);
	for (list<TJ>::iterator it = Jrecord.begin(); it != Jrecord.end(); ++it)
		printf("%d ", (*it).num + 1);// ���������� � ������� ����� 1, ����� �������� ��������� � 0 �� 1

	if ( Lbrecord > Lb)

		one_rj_Lmax_recurs (Jnofinish, JobsParamReadOnly, n,
		                          Jshed, Jrecord, Lb, Lbrecord);

	Lmax= Lbrecord;
	return Jrecord;
}

// ������� ������������� ��� ������������.
// ������ ������ ������ ���������
// ��������
void fullRecurs(list<TJ> Jnofinish, TJob* JobsParamReadOnly,int n, int &Lbrecord,
	            list<TJ> Jshed,list<TJ> &Jrecord,int t=0){

		int Lbcur; // ������� �������� ����������� ������� ��� ���������� ������� ����� ��������� ������

		if (Jnofinish.empty()){ // ������ ���������������� ����� ����
			
			Lbcur = calcLmax(Jshed,JobsParamReadOnly);
			if (Lbcur <= Lbrecord){
				Lbrecord = Lbcur; // ��������� ������� �������� Lmax
				Jrecord = Jshed;

				printf("\n\nLb = %d \n", Lbrecord);
				for (list<TJ>::iterator it = Jshed.begin(); it != Jshed.end(); ++it)
					printf("%d ",(*it).num + 1);// ���������� � ������� ����� 1, ����� �������� ��������� � 0 �� 1

			}

			return; // ������ ���������
		}

		list<TJ>::iterator ir = (Jnofinish.begin());
		while( ir != Jnofinish.end()){
			
			// ���������� ���������
			int num = (*ir).num; // ����� ������
			int param = (*ir).param;

			Jshed.push_back(*ir);                  // ��������� ������� ������ � ����� ����������
			list<TJ>::iterator irtmp = ir;irtmp++; // �������������� ��������� ��� ��������
			Jnofinish.erase(ir);				   // ������� ������� ������ �� ����������������
			
			//if (t == 0){
			//	printf("%d ",num);
			//}

			// ��������
			fullRecurs(Jnofinish,JobsParamReadOnly,n, Lbrecord,
	            Jshed,Jrecord,t+1);

			
			Jshed.pop_back();                    // ������� ������ �� ����������
			Jnofinish.insert(irtmp,TJ(num, param)); // ���������� �� ����� ������ � ������ ����������������
			ir = irtmp;
		}

}

// ������� ������������� ��� ������������.
// ������ ������ ������ ���������
list<TJ> full(list<TJ> Jnofinish, TJob* JobsParamReadOnly,int n, int &Lbrecord){
	
	Lbrecord = 9999999;
	list<TJ> Jshed;
	list<TJ> Jrecord;
	fullRecurs(Jnofinish,JobsParamReadOnly,n,Lbrecord,
		                                 Jshed, Jrecord);
	return Jrecord;
}



#ifdef WINDOWS
		int _tmain(int argc, _TCHAR* argv[])
#else
		int main(int argc, char* argv[])
#endif
{

	// �������� ����������
	//
	TJob JobsParam[NMAX]; // ������ �����. �� ���������� � �������� ������. ������ �������� ������ 
	int N;      // ���������� �����
	int t;      // ������ ������� t
	int Lmax;   // ������������ �������� ��������� � ����������

	list<TJ> J; // � ���� ������ ����� ��������� ��� �� ����������� ������
	list<TJ> shedule; // � ���� ������ ����� ��������� ����������� ���������� (������������������ ������� �����)


	// ���������� ����� ����� ����� ���������
	// ���� ���������� ��� ����� ���������, ��� ��� ����� - input.txt
	char *fname = "sample.csv";

	if (argc > 1){ // ���� ���������� ���������� � ��������� ������ > 0

		// �������������� ��� win, ��� linux ������ ����������� 
		char *buf = new char[100];
#ifdef WINDOWS
		CharToOem(argv[1], buf);
#else
		strncpy(buf, argv[1], 100);
#endif
		fname = buf;
		//fname = argv[1]; 

		puts(fname);
	}

	// ��������� ������ � ������� �� �����
	if (!readFromFile(fname, JobsParam, N)){//���� �������� NULL
		char s[10];
		printf("\nPress any key then press Enter for exit.");
		scanf("%s", s);
		return 0;           // ������� � �������
	}

	// ��������� ������ ����� �������� ����� 
	// �������� �� ���������
	// ��� ������ � J ������, ����� ���������� �� ����������
	for (int i = 0; i < N; i++)
		AddNewItemInSortedList(J, i, JobsParam[i].d);


	//********** ����� �������� ������� ������ Lmax, ��� ������ ������ ������ ���������

	//���� � ������ ������ ���� ������ ��������(������ ����� ���������), ��������� ������ �������
	if (argc > 2){
		shedule = full(J, JobsParam, N, Lmax);

		printf("\nTest result: ");//������� ���������
		printf("\nLmax = %d\n", Lmax);
		for (list<TJ>::iterator it = shedule.begin(); it != shedule.end(); ++it)
			printf("%d ", (*it).num + 1);// ���������� � ������� ����� 1, ����� �������� ��������� � 0 �� 1
	}

	//********** ���� ����������� ���������� shedule � ������������ ���������� Lmax
	unsigned int start_time = clock(); // ��������� �����

	shedule = one_rj_Lmax(J, JobsParam, N, Lmax);

	unsigned int end_time = clock(); // �������� �����
	unsigned int search_time = end_time - start_time; // ������� �����

	printf("\nResult: ");//������� ���������
	printf("\n\nLmax = %d\n\n", Lmax);
	for (list<TJ>::iterator it = shedule.begin(); it != shedule.end(); ++it)
		printf("%d ", (*it).num + 1);// ���������� � ������� ����� 1, ����� �������� ��������� � 0 �� 1

	printf("\ntime = %d\n", search_time);

	// ���������� ������ � ������� � ����
	if (!printToFile(fname, shedule, Lmax)){//���� �������� NULL
		char s[10];
		printf("\nPress any key then press Enter for exit..");
		scanf("%\rs",s);
		return 0;           // ������� � �������
	}

#ifdef WINDOWS
	printf("\nPress any key for exit.");
	getch();
#endif
	return 0;
}

