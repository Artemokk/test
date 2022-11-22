#include <iostream>
#include <fstream>
#include <omp.h>
#include <string>
#include <vector>

using namespace std;

// КОММЕНТАРИИ ДЕВОПСА
// 1) Необходимо подсчитывать время выполнения рассчетов и выводить это время в консоль
// 2) Исходя из времени выполенния рассчетов выбрать оптимальный вариант распараллеливания вложенных циклов на строчках 36 - 49
// 3) Постараться распараллелить циклы на строках 53-58 и 60-62 и 114-128

const double eps = 0.001;//Точность
string base = __FILE__;

//Функция решения СЛАУ методом простых итераций, где присутствует диагональное преобладание
vector<double> simpleIter(vector<vector<double>> a, vector<double> b, int n)
{
	vector<double> res(n); // Хранит результат выполнения функции и начальное приближение
	int i, j = 0;
	int count = 0;

	//Распараллеленный цикл начального приближения
#pragma omp parallel 
	{
#pragma omp for
		for (i = 0; i < n; i++)
		{
			res[i] = b[i] / a[i][i];//Начальное приближение: свободный член делится на диагональный элемент
		}
	}

	vector<double> Xn(n);	// Хранение последующего приближения

	do {
		//Распараллелевание итерационного подсчета (будет достигаться барьерная синхронизация каждого из потоков
		//и алгоритм не пострадает, поскольку каждый из потоков имеет в своем распоряжении соответствующее приближение)
		count++;
#pragma omp parallel 
		{
#pragma omp for 
			for (i = 0; i < n; i++) {
				Xn[i] = b[i] / a[i][i];//Следующее приближение
				for (j = 0; j < n; j++) {
					if (i == j)//диагональный элемент 0
						continue;//переходим к след. члену
					else {
						Xn[i] -= a[i][j] / a[i][i] * res[j];//делаем итерационный подсчет, умножая на res - предыдущее приближение
					}
				}
			}
		}
		//Проверка: является ли текущее приближение конечным ответом
		bool flag = true; //Флажок найденного решения
		//cout << " \nШаг итерации: " << count << endl;
		for (i = 0; i < n; i++) {
			//cout << i << ":  " << abs(Xn[i] - res[i]) << endl;
			if (abs(Xn[i] - res[i]) > eps) {//Условие достижения  приближения
				flag = false;
				break;
			}
		}

		for (i = 0; i < n; i++) {
			res[i] = Xn[i];//Запоминаем пред. приближение
		}

		if (flag)//true flag - выходим из цикла
			break;
	} while (1);

	cout << "Количество итераций: " << count << endl;
	return res; //возвращаем вектор решений
}

int readFile(string full_name)
{
	ifstream Infile;
	Infile.open(full_name);
	if (!Infile.is_open()) { // если файл не открыт
		cout << "Ошибка чтения файла!\n"; // сообщить об этом
		cout << base << endl;
	}
	int n = 0;
	string line;
	while (getline(Infile, line))
		++n;
	Infile.close();
	return n;
}

int main()
{
	setlocale(LC_ALL, "Russian");

	double solution = 0; //Переменная проверки
	int n = 0; //Число уравнений
	int i, j = 0;

	//Меню выбора с дальнейшим открытием файла для считывания СЛАУ
	cout << "***************Решение СЛАУ методом простых итераций***************\n";
	cout << "Введите название файла с уравнениями: " << endl;
	cout << "1. data1.txt\n";
	cout << "2. data2.txt\n";
	cout << "3. data3.txt\n";
	cout << "4. data4.txt\n";
	cout << "5. data5.txt\n";

	string name, full_name;
	cin >> full_name;
	n = readFile(full_name);

	vector<vector<double>> a(n, vector<double>(n)); // матрица коэффициентов
	vector<double> b(n); // вектор свободных членов
	vector<double> x(n); // вектор решений

	ifstream Infile;
	Infile.open(full_name);
	//Цикл для считывания данных в соответствующие массивы
	for (i = 0; i < n; i++)
	{

		for (j = 0; j <= n; j++)
		{
			if (j != n)
			{
				Infile >> a[i][j]; //если не последний элемент, т.е не свободный член, то в массив а
			}
			else
			{
				Infile >> b[i]; //если последний, то в массив свободных членов - b
			}
		}
	}

	Infile.close();

	cout << endl;
	//Матрица исходных коэффициентов а при неизвестных и свободных членов b
	cout << "----------------Матрица----------------" << endl;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j <= n; j++)
		{
			if (j != n)
			{
				cout << a[i][j] << "\t";
			}
			else
			{
				cout << b[i] << "\t";
			}
		}
		cout << endl;
	}

	cout << endl;

	Infile.close();

	x = simpleIter(a, b, n);//Расчет функции для поиска вектора решений, передали массивы а, b и кол-во уравнений

	cout << endl;

	cout << "Решение" << endl;
	for (i = 0; i < n; i++)
		cout << "x[" << i + 1 << "]=" << x[i] << endl;
	cout << endl;

	cout << "Проверка" << endl;
	cout << "---------" << endl;
	for (i = 0; i < n;i++)
	{
		solution = 0;
		for (j = 0; j < n;j++)
			solution = solution + a[i][j] * x[j];
		cout << solution << " - значение " << i + 1 << "-ого уравнения" << endl;
	}

	return 0;
}
