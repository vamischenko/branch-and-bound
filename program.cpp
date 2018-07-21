// program.cpp: определяет точку входа для консольного приложения.
//

// для visual studio надо раскомментировать
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

#include <stdio.h>  // ввод-вывод

#include <ctime>    // определение времени работы программы

#include <list>     // подключаем заголовок списка
#include <iterator> // заголовок итераторов

#ifdef WINDOWS
#include  "Windows.h"// специфика для win
#endif

using namespace std;

#define  NMAX 1000 // максимальное кол-во работ

// Структура описывающая работу 
// Работы нумеруются в стиле с++ с нуля
struct TJob{
	int num; // исходный номер работы
	int p;   // processing time
	int r;   // release date
	int d;   // deadline
};

// вспомогательный класс для удобства работы со списками
class TJ{
public:
	int num;
	int param;

	TJ(){num=0;param=0;}            // пустой конструктор
	TJ(int a,int b){num=a;param=b;} // конструктор с параметрами
	TJ(const TJ  &copyin){num=copyin.num;param=copyin.param;}// конструктор копирующий
	~TJ(){};                         // пустой деструктор
	void fill(int a,int b){num=a;param=b;} // заполняем 
	int operator<(const TJ &rhs) const{ // перегрузка оператора < для list::sort

		if( this->param < rhs.param ) return 1;
		return 0;
	}
	TJ& operator=(const TJ &rhs){ // присваивание
	   this->num= rhs.num;
	   this->param = rhs.param;
	   return *this;
	}

	int operator==(const TJ &rhs) const // сравнение
	{
	   if( this->num != rhs.num) return 0;
	   return 1;
	}
	
};

// добавление элемента в список по уникальному параметру
void AddNewItemInSortedList(list<TJ> &m_list,int num, int param, bool isUnique = false){
  int i;

  // если список пуст 
  if (m_list.empty()){
	  m_list.push_front( TJ(num, param) );
      return; // и выходим
  }

  //если новый элемент >= последнего, добавляем в хвост
  list<TJ>::iterator ir = m_list.end();ir--; 
   if( (*ir).param <= param ) {
	  if(!(isUnique && ((*ir).param == param)))
		  m_list.push_back( TJ(num, param) );
	  return; // и выходим
  }

  // если новый элемент <= первого, добавляем в голову
  if ( ( (*(m_list.begin())).param >= param ) ) {
	  if(!(isUnique && ((*(m_list.begin())).param == param)))
		  m_list.push_front( TJ(num, param) );
	  return; // и выходим
  }

  // указатели на текущий и следующий элементы
  ir = (m_list.begin()); 
  list<TJ>::iterator ir1=ir; ir1++;

  // идем по списку, чтобы найти место вставки
  while( ir1 != m_list.end()){

	  // нашли место вставки
	  if( (*ir).param <= param && param <=  (*ir1).param ) {
		  
		  // если вставляем элементы только с уникальными param и параметр неникален
		  if (  isUnique && ( (*ir).param == param || param ==  (*ir1).param))
			return; //вставка не требуется, выходим
	  
		  // в любом другом случае вставляем новое значение после текущего
		  m_list.insert(ir1,TJ(num, param));
		  return; // и выходим
	  }

	  ir++;
	  ir1++;
  }

}

// функция readFromFile читает из файла строки и создает массив работ
// каждая строка файла описывает параметры работы
// Первая колонка - p (processing time), вторая - r (release date), третья - d (deadline) 
// Строки разделены запятыми
//
// возвращает массив работ и количество элементов массива через параметры
bool readFromFile(const char *fname, TJob* Jobs, int &n){
	
	int num = 0; // номер работы
	
	FILE* f = fopen(fname,"r"); // открываем файл

	if (!f){                    // если указатель пуст - возникла проблема с открытием файла
		printf("Error opening file %s\n", fname);  // выводим ошибку
		return false;            // возвращаем признак ошибки
	}

	while(!feof(f)){ // считываем все строки из файла
		Jobs[num].num = num; // присваиваем работе очередной номер
		fscanf(f,"%d,%d,%d\n",&Jobs[num].p, &Jobs[num].r, &Jobs[num].d); // читаем строку из файла
		printf("%3d %3d %3d\n", Jobs[num].p, Jobs[num].r, Jobs[num].d);
		num++; // увеличиваем номер следующей работы на 1
	}

	fclose(f); // закрываем файл

	n = num; // запоминаем количество работ

	if ( n == 0 ){
		printf("File %s is empty\n", fname);  // выводим ошибку
		return false; // возвращаем признак ошибки
	}

	return true;
} 

// функция printToFile записывает в файл результат работы
// каждая строка файла описывает параметры работы
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



	FILE* f = fopen(fnameOUT, "w"); // открываем файл

	if (!f){                    // если указатель пуст - возникла проблема с открытием файла
		printf("Error creating file %s\n", fnameOUT);  // выводим ошибку
		return false;           // возвращаем признак ошибки
	}


	for (list<TJ>::iterator it = J.begin(); it != J.end(); ++it)
		fprintf(f,"%d ", (*it).num + 1);

	fprintf(f,"\n%d",Lmax); //выводим Lmax

	fclose(f); // закрываем файл

	return true;
}

// ищет для расписания J максимальное значение опоздания Lmax 
int calcLmax(list<TJ> J, TJob* JobsParamReadOnly){

	// нам нужно выяснить максимальное опоздание
	// для этого пройдем по списку
	int Lmax = -9999999; // пусть начальное значение будет наименьшим
	int t = 0; // время начала работ = 0
	for (list<TJ>::iterator it = J.begin(); it != J.end(); ++it){
		int p = JobsParamReadOnly[(*it).num].p; // продолжительность работы
		int r = JobsParamReadOnly[(*it).num].r; // время начала работы
		int d = JobsParamReadOnly[(*it).num].d; // дедлайн
		t = max(t,r) + p; // время завершения работы
		int L = t - d;    // задержка работы

		if ( L > Lmax ) Lmax = L; // если задержка больше максимальной, запоминаем 

	}

	return Lmax;
}

// функция EDD упорядочивает список работ J 
// по возрастанию дедлайна JobsParam[j].d
// на входе 
//    J -список задач, для которых выполняется поиск решения
// возвращает список отсортированных работ
// и через параметр Lmax - максимальное значение опоздания
list<TJ> EDD(list<TJ> J, TJob* JobsParamReadOnly, int &Lmax){

	// список J уже отсортирован по d
	// поэтому нам нужно выяснить только максимальное опоздание
	Lmax = calcLmax(J,JobsParamReadOnly);

	return J;
}

// функция находит оптимальное решение задачи 1|rj,prmp|Lmax
// минимизации максимального опоздания работы single machine with relese date
// с возможностью прерывать работы и начинать новые
// 
// возвращает список отсортированных работ
// и передает в Lmax - максимальное значение опоздания
list<TJ> one_rj_prmp_Lmax(list<TJ> Jnofinish, TJob* JobsParamReadOnly,int n, int &Lmax, bool &preemptionNoExist,int t=0){
	
	Lmax = -99999999;
	preemptionNoExist = true;

	//создаем новый список, чтобы вернуть результат
	list<TJ> Jrez; 

	// если список пуст, возвращаем пустое значение
	if (Jnofinish.empty())
		return Jrez;

	bool f = false; // часть работы вернули в невыполненные, нужен, чтобы определить разрыв работ

	// копируем данные из J в Jnofinish - это список еще не законченных работ
	//list<TJ> Jnofinish = J; 


	// копируем параметры работ, так как будем портить значения p
	TJob JobsParam[NMAX];
	memcpy(JobsParam, JobsParamReadOnly, sizeof(TJob)*n);

	// выделяем во вспомогательный список значения r для работ из J
	list<TJ> Jr;
	Jr.push_front(TJ(-1,t));
	for (list<TJ>::iterator it = Jnofinish.begin(); it != Jnofinish.end(); ++it){		
		// если очередное значение r еще не находится в списке (отбираем только уникальные r) 
		if (JobsParam[(*it).num].r >= t)
			AddNewItemInSortedList(Jr, (*it).num, JobsParam[(*it).num].r, true);
	}
		
	// цикл по r1<r2<..<rn
	list<TJ>::iterator ir = (Jr.begin()); 
	while( ir != Jr.end()){

		// если Jnofinish пуст - расписание составлено
		if( Jnofinish.empty())
			return Jrez;

		// начальное время равно r_min
		t = max(t, (*(ir)).param); 

		list<TJ>::iterator ir1 = ir; ir1++; // в ir1 храним r[i+1]
		if ( ir1 == Jr.end()) // выходим из цикла слоения
			break;
		// ищем среди невыполненных работ из Jnofinish
		// удовлетворяющие условию r[j] <= t
		// так как Jnofinish отсортированна по дедлайнам, поиск  по правилу EDD
		// можно производить проходом от головы списка к хвосту
		list<TJ>::iterator id = Jnofinish.begin();      // первый элемент списка
		list<TJ>::iterator iend; // последний элемент списка
		while( id!=Jnofinish.end()){
			 
			//если нашли такую работу
			if (JobsParam[((*id).num)].r <= t) {

				if ( !Jrez.empty() ) {iend = Jrez.end(); iend--;}
				// если список пуст и работа еще не была добавлена в хвост списка
				if(Jrez.empty() || (*iend).num != (*id).num){
					Jrez.push_back((*id));
					// если на предыдущей итерации работа была не окончена
					// то выполнен разрыв работы т.к. последняя и текущая работа не совпали
					if (f)
						preemptionNoExist= false;
				}
				else // а если работа та же что в хвосте, сбрасываем флаг разрыва
					f = false;


				// если t+p меньше r[j+1] 
				if (t + JobsParam[((*id).num)].p <= (*(ir1)).param){
					// увеличиваем время 
					t = max(t,JobsParam[((*id).num)].r)+JobsParam[((*id).num)].p;
					//считаем задержку и сохраняем если она больше максмальной
					int L = t - JobsParam[((*id).num)].d;
					if (L > Lmax) Lmax = L;
					// удаляем работу из списка, так-как закончили её
					list<TJ>::iterator tmp_id = id; tmp_id++;
					Jnofinish.erase(id);
					id = tmp_id;//

				}
				else {// иначе разрываем работу т.к. дошли до очередной границы r[i]
					f = true; // установили флаг разрыва работы 
					JobsParam[((*id).num)].p -= (*ir1).param - t; // уменьшаем работу на время выполненного куска
					t = (*ir).param; // t=r[i] время 
					break; // прерываем цикл while( id!=Jnofinish.end())
				}

			}
			else // текущая работа не подходит, переходим к следующей
				id++;

			// если нашли
		}

		ir++;
	}

	// копируем остаток списка 
	if( !Jnofinish.empty() )

		for (list<TJ>::iterator it = Jnofinish.begin(); it != Jnofinish.end(); ++it){
			list<TJ>::iterator iend;
			if ( !Jrez.empty() ) {iend = Jrez.end(); iend--;}
			// если список пуст и работа еще не была добавлена в хвост списка
			if(Jrez.empty() || (*iend).num != (*it).num){
				Jrez.push_back((*it));
				// если на предыдущей итерации работа была не окончена
				// то выполнен разрыв работы т.к. последняя и текущая работа не совпали
				if (f)
					preemptionNoExist= false;
			}
			else // а если работа та же что в хвосте, сбрасываем флаг разрыва
				f = false;

			// увеличиваем время 
			t = max(t,JobsParam[((*it).num)].r)+ JobsParam[((*it).num)].p;
			//считаем задержку и сохраняем если она больше максмальной
			int L = t - JobsParam[((*it).num)].d;
			if (L > Lmax) Lmax = L;
		}

	return Jrez;
}

// рекурсивный расчет 1|rj|Lmax 
// методом ветвей и границ
// входные параметры
// Jnofinish - работы, которые еще не попали в расписание (не выполнены на момент t)
// JobsParamReadOnly - массив, хранящий p,r,d для Jj работы в j столбце
// n - количество работ, всего
// Jshed - в этом списке формируется текущее расписание
// Jrecord - в этом списке хранится ранее найденный рекорд (расписание с лучшим Lmax = Lrecord)
// Lbmin - минимально возможное значение Lmax, если нашли такое непрерывное расписание - решение найдено
// Lrecord - рекорд Lmax найденный ранее
// t момент времени, для которого формируется расписание

// результат работы возвращается в Jrecord, Lrecord
void one_rj_Lmax_recurs (list<TJ> Jnofinish, TJob* JobsParamReadOnly, int n,
	list<TJ> Jshed, list<TJ> &Jrecord, int LbMin, int &Lbrecord,  int t = 0, int lvl=0){
		
		/*
		// тестовый блок
		printf("\n\n\Lvl = %d" , lvl);
		printf("\nt = %d" , t);
		printf("\nLb = %d \n", Lbrecord);
		for (list<TJ>::iterator it = Jshed.begin(); it != Jshed.end(); ++it)
			printf("%d ",(*it).num + 1);// прибавляем к номерам работ 1, чтобы сдвинуть нумерацию с 0 на 1
		//getch();
		*/

		int Lbcur; // текущее значение минимальной границы для расписания которое будет построено дальше

		if (Jnofinish.empty()){ // список нераспределенных работ пуст
			
			Lbcur = calcLmax(Jshed,JobsParamReadOnly);
			if (Lbcur < Lbrecord){
				Lbrecord = Lbcur; // сохраняем текущее значение Lmax
				Jrecord = Jshed;
			}
			return; // работа завершена
		}
		
		//----------- отсечение текущей ветви
		if (t > 0){ // корень не отсекаем
			
			
			bool preemptionNoExist; // дало ли построение расписания с возможностью разрывов непрерывное расписание
			// ищем Lbcur и строим расписание с возможными разрывами
			
			list<TJ> Jcurend = one_rj_prmp_Lmax(Jnofinish,JobsParamReadOnly,n,Lbcur,preemptionNoExist, t); 
			int Lbcur2 = calcLmax(Jshed, JobsParamReadOnly);

			//printf("\n\nL1 = %d\n",Lbcur);
			//printf("L2 = %d",Lbcur2);

			Lbcur = max(Lbcur,Lbcur2);
			list<TJ> Jcur = Jshed;
			Jcur.splice(Jcur.end(),Jcurend); // сливаем начало и хвост расписания

			
			printf("\nL = %d\n",Lbcur);
			for (list<TJ>::iterator it = Jcur.begin(); it != Jcur.end(); ++it){
				printf("%d ",(*it).num + 1);// прибавляем к номерам работ 1, чтобы сдвинуть нумерацию с 0 на 1
			}

			// если получилось расписание с неразорванными работами
			// и минимально возможная нижняя граница совпала с текущей, результат найден
 			if( Lbcur == LbMin && preemptionNoExist){
				Lbrecord = Lbcur; // сохраняем текущее значение Lmax
				Jrecord = Jcur;   // сохраняем полученный список
				return; // и прекращаем поиск
			}

			// если текущиая нижняя граница больше рекорда, отсекаем эту ветвь
			if( Lbcur >= Lbrecord)
				return;// и прекращаем ветвление

			// если текущиая нижняя граница меньше рекорда и работы в расписании не разорваны 
			// сохраняем новый рекорд и переходим к ветвлению
			if( Lbcur < Lbrecord && preemptionNoExist )

				Lbrecord = Lbcur; // сохраняем текущее значение Lmax
				Jrecord = Jcur;   // сохраняем полученный список

		}


		//-----------  ветвление 

	    // цикл по Jnofinish
		// ищем min(max(t,r) + p)
		list<TJ>::iterator ir = (Jnofinish.begin()); 
		// инициализируем первую min
		int num = (*ir).num;
		int min = max(t,JobsParamReadOnly[num].r) + JobsParamReadOnly[num].p;
		ir++;
		while( ir != Jnofinish.end()){
			num = (*ir).num;
			int min1 = max( t, JobsParamReadOnly[num].r ) + JobsParamReadOnly[num].p;
			if (min1 < min) min = min1;
			ir++;     // переходим к следующей работе
		}

		// если для работы Jj rj >= min, отсекаем ветвь
		ir = (Jnofinish.begin());
		while( ir != Jnofinish.end()){
			
			int num = (*ir).num; // номер работы
			int param = (*ir).param;

			if ( JobsParamReadOnly[num].r >= min ){ // если условие отсечения верно
				ir++;     // переходим к следующей работе
				continue; // пропускаем дальнейшие действия для текущей работы
			}

			// иначе производим ветвление
			
			int tnew = max(t,JobsParamReadOnly[num].r) + JobsParamReadOnly[num].p; // считаем время для новой ветви
			Jshed.push_back(*ir);                  // добавляем текущую работу в хвост расписания
			list<TJ>::iterator irtmp = ir;irtmp++; // подготавливаем указатель для удаления
			Jnofinish.erase(ir);				   // удаляем текущую работу из нераспределенных
			
			// рекурсия
			one_rj_Lmax_recurs (Jnofinish,JobsParamReadOnly, n,
												Jshed, Jrecord,LbMin,Lbrecord,tnew,lvl+1);

			if ( Lbrecord == LbMin ) 
				return; // найдено оптимальное решение, прекращаем поиск
			
			Jshed.pop_back();                    // удаляем работу из расписания
			Jnofinish.insert(irtmp,TJ(num, param)); // возвращаем на место работу в список нераспределенных
			ir = irtmp;
		}
}

// эта функция подготавливает данные для 
// рекурсивного расчета 1|rj|Lmax 
//
// возвращает список отсортированных работ
// и передает через параметр Lmax - максимальное значение опоздания
list<TJ> one_rj_Lmax(list<TJ> Jnofinish, TJob* JobsParamReadOnly,int n, int &Lmax){
	
	int Lb;      // это нижняя граница - значение из one_rj_prmp_Lmax
	int Lbrecord; // начальный рекорд рассчитаем в EDD
	bool preemptionNoExist; // в этой переменной one_rj_prmp_Lmax вернет признак неразрывного расписания
	list<TJ> Jrecord;       // это расписание - рекорд, у него лучший Lmax >= Lb
	list<TJ> Jshed;         // в этом списке будет накапливаться расписание
	
	Lbrecord = -999999999; // забиваем мин. значениями
	Lmax     = -999999999; // забиваем мин. значениями
	Lb       = -999999999; // забиваем мин. значениями

	// считаем Lb - это минимальное значение Lmax, которое может быть в нашей задаче
	Jrecord = one_rj_prmp_Lmax(Jnofinish,JobsParamReadOnly,n,Lb,preemptionNoExist);
	printf("\nL_1_rj_prmp_Lmax = %d", Lb);

	// если one_rj_pmp_Lmax вернула неразрывное расписание -решение найдено
	if (preemptionNoExist) {
		Lmax = Lb;      // присваиваем максимальное значение опоздания
		return Jrecord; // и возвращаем найденное расписание
	}

	// иначе формируем расписание-рекорд с помощью EDD
	Jrecord = EDD(Jnofinish, JobsParamReadOnly, Lbrecord);

	printf("\nLrecord = %d\n", Lbrecord);
	for (list<TJ>::iterator it = Jrecord.begin(); it != Jrecord.end(); ++it)
		printf("%d ", (*it).num + 1);// прибавляем к номерам работ 1, чтобы сдвинуть нумерацию с 0 на 1

	if ( Lbrecord > Lb)

		one_rj_Lmax_recurs (Jnofinish, JobsParamReadOnly, n,
		                          Jshed, Jrecord, Lb, Lbrecord);

	Lmax= Lbrecord;
	return Jrecord;
}

// функция предназначена для тестирования.
// решает задачу полным перебором
// рекурсия
void fullRecurs(list<TJ> Jnofinish, TJob* JobsParamReadOnly,int n, int &Lbrecord,
	            list<TJ> Jshed,list<TJ> &Jrecord,int t=0){

		int Lbcur; // текущее значение минимальной границы для расписания которое будет построено дальше

		if (Jnofinish.empty()){ // список нераспределенных работ пуст
			
			Lbcur = calcLmax(Jshed,JobsParamReadOnly);
			if (Lbcur <= Lbrecord){
				Lbrecord = Lbcur; // сохраняем текущее значение Lmax
				Jrecord = Jshed;

				printf("\n\nLb = %d \n", Lbrecord);
				for (list<TJ>::iterator it = Jshed.begin(); it != Jshed.end(); ++it)
					printf("%d ",(*it).num + 1);// прибавляем к номерам работ 1, чтобы сдвинуть нумерацию с 0 на 1

			}

			return; // работа завершена
		}

		list<TJ>::iterator ir = (Jnofinish.begin());
		while( ir != Jnofinish.end()){
			
			// производим ветвление
			int num = (*ir).num; // номер работы
			int param = (*ir).param;

			Jshed.push_back(*ir);                  // добавляем текущую работу в хвост расписания
			list<TJ>::iterator irtmp = ir;irtmp++; // подготавливаем указатель для удаления
			Jnofinish.erase(ir);				   // удаляем текущую работу из нераспределенных
			
			//if (t == 0){
			//	printf("%d ",num);
			//}

			// рекурсия
			fullRecurs(Jnofinish,JobsParamReadOnly,n, Lbrecord,
	            Jshed,Jrecord,t+1);

			
			Jshed.pop_back();                    // удаляем работу из расписания
			Jnofinish.insert(irtmp,TJ(num, param)); // возвращаем на место работу в список нераспределенных
			ir = irtmp;
		}

}

// функция предназначена для тестирования.
// решает задачу полным перебором
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

	// описание переменных
	//
	TJob JobsParam[NMAX]; // Массив работ. Не изменяется в процессе работы. Хранит исходные данные 
	int N;      // Количество работ
	int t;      // Момент времени t
	int Lmax;   // Максимальное значение опоздания в расписании

	list<TJ> J; // В этом списке будут храниться еще не выполненные работы
	list<TJ> shedule; // В этом списке будет храниться оптимальное расписание (последовательность номеров работ)


	// считывание имени файла через параметры
	// если параметров нет будет считаться, что имя файла - input.txt
	char *fname = "sample.csv";

	if (argc > 1){ // если количество параметров в командной строке > 0

		// преобразование для win, для linux просто присваиваем 
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

	// считываем данные о работах из файла
	if (!readFromFile(fname, JobsParam, N)){//если вернулся NULL
		char s[10];
		printf("\nPress any key then press Enter for exit.");
		scanf("%s", s);
		return 0;           // выходим с ошибкой
	}

	// заполняем список работ номерами работ 
	// сортируя по дедлайнам
	// при работе с J следим, чтобы сортировка не нарушалась
	for (int i = 0; i < N; i++)
		AddNewItemInSortedList(J, i, JobsParam[i].d);


	//********** вызов тестовой функции поиска Lmax, она решает задачу полным перебором

	//если в строке вызова есть второй параметр(помимо имени программы), запускаем полный перебор
	if (argc > 2){
		shedule = full(J, JobsParam, N, Lmax);

		printf("\nTest result: ");//Выводим результат
		printf("\nLmax = %d\n", Lmax);
		for (list<TJ>::iterator it = shedule.begin(); it != shedule.end(); ++it)
			printf("%d ", (*it).num + 1);// прибавляем к номерам работ 1, чтобы сдвинуть нумерацию с 0 на 1
	}

	//********** ищем оптимальное расписание shedule и максимальное отставание Lmax
	unsigned int start_time = clock(); // начальное время

	shedule = one_rj_Lmax(J, JobsParam, N, Lmax);

	unsigned int end_time = clock(); // конечное время
	unsigned int search_time = end_time - start_time; // искомое время

	printf("\nResult: ");//Выводим результат
	printf("\n\nLmax = %d\n\n", Lmax);
	for (list<TJ>::iterator it = shedule.begin(); it != shedule.end(); ++it)
		printf("%d ", (*it).num + 1);// прибавляем к номерам работ 1, чтобы сдвинуть нумерацию с 0 на 1

	printf("\ntime = %d\n", search_time);

	// записываем данные о работах в файл
	if (!printToFile(fname, shedule, Lmax)){//если вернулся NULL
		char s[10];
		printf("\nPress any key then press Enter for exit..");
		scanf("%\rs",s);
		return 0;           // выходим с ошибкой
	}

#ifdef WINDOWS
	printf("\nPress any key for exit.");
	getch();
#endif
	return 0;
}

