#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

/*
	Попов Виталий БПИ192 
	вариант 17

	Структура:
		В main создаётся потока, исполняющие функцию gardener, 
		и сад с шириной и длиной из консоли.

		Функция gardener начинает из startPlace и перемешается по step
		в двумерном массиве сада с целью заполнить его по плану.

		Garden - класс, содержащий сад и план сада. 
		Может вывести в консоль содержимое, 
		а также организовать проверку сада на соответствие плану
*/

mutex mtx;

/// <summary>Отвечает за работу с садом</summary>
class Garden
{
public:
	/// <summary>
	/// Создаёт сад, план сада и устанавливает их длину и ширину
	/// </summary>
	/// <param name="width">ширина</param>
	/// <param name="height">длина</param>
	Garden(const unsigned int& width, const unsigned int& height);
	~Garden();

	/// <summary>
	/// Выводит сад в консоль
	/// </summary>
	/// <param name="garden">Сад</param>
	static void showGarden(const Garden& garden);
	/// <summary>
	/// Выводит план сада в консоль
	/// </summary>
	/// <param name="garden">Сад</param>
	static void showGardenPlan(const Garden& garden);

	/// <summary>
	/// Проверяет, соответствует ли сад своему плану
	/// </summary>
	/// <returns>Результат проверки</returns>
	bool checkGarden();

	const unsigned int getWidth() { return width; }
	const unsigned int getHeight() { return height; }

	bool** getGarden() { return garden; }
	bool** getGardenPlan() { return plan; }

private:
	/// <summary>
	/// Выводит в консоль содержимое двумерного массива
	/// </summary>
	/// <param name="garden">Сад</param>
	/// <param name="width">Ширина</param>
	/// <param name="height">Длина</param>
	static void showField(const bool** garden, const int& width, const int& height);

	unsigned int width, height;
	bool** garden;
	bool** plan;
};

/// <summary>
/// Структура для работы с координатами
/// </summary>
struct coord {
	int x, y;

	coord(const int& x, const int& y) {
		this->x = x;
		this->y = y;
	}
};

/// <summaryРабота садовника</summary>
/// <param name="gardenInit">Сад, который нужно обработать</param>
/// <param name="name">Садовник</param>
/// <param name="place">Начальные координаты садовника</param>
/// <param name="step">Направление садовника</param>
void gardener(Garden& gardenInit, const string& name, coord& startPlace, coord& step);

/// <summary>
/// Метод для считывания положительного числа
/// </summary>
/// <returns>Корректность числа</returns>
bool readPositive(int& num, const string& text) {
	cout << text; cin >> num;
	return num > 0;
}

int main() {
	srand(time(NULL));
	//-----------------------Создание сада
	int n = 0, m = 0;
	if (!readPositive(n, "Input width\t") ||
		!readPositive(m, "Input height\t")) {
		cout << "[ERROR] You must have positive num for width and height.\n";
		return 0;
	}

	auto garden = Garden(n, m);
	Garden::showGardenPlan(garden);
	
	//-----------------------Работа садовников
	coord coordStart1(0, 0);
	coord coordStep1(1, 0);

	coord coordStart2(n - 1, m - 1);
	coord coordStep2(0, -1);

	cout << "\n|====| Gardeners STARTS working\t|====|====|====|====|====|====|====|====|\n";
	thread gardener1(&gardener, ref(garden), "#1", ref(coordStart1), ref(coordStep1));
	thread gardener2(&gardener, ref(garden), "#2", ref(coordStart2), ref(coordStep2));
	gardener1.join();
	gardener2.join();
	cout << "|====| Gardeners ENDS work\t|====|====|====|====|====|====|====|====|\n";

	//-----------------------Проверка
	cout << "\nThe work is over. Time to check it.\n Let's start with visual analysis\n";
	Garden::showGardenPlan(garden);
	Garden::showGarden(garden);
	cout << "\nAnd now check it by computer\n STATUS:\t" 
		<< (garden.checkGarden() ? "All CORRECT" : "Smth went WRONG...") << endl;
	
	return 0;
}

#pragma region Garden
Garden::Garden(const unsigned int& width, const unsigned int& height) {
	this->width = width;
	this->height = height;

	this->garden = new bool* [width];
	this->plan = new bool* [width];

	for (size_t i = 0; i < width; i++) {
		this->garden[i] = new bool[height] {0};
		this->plan[i] = new bool[height];
		for (size_t j = 0; j < height; j++)
			this->plan[i][j] = rand() % 2 == 0;

	}
}

Garden::~Garden() {
	// Удаляем сад
	for (int i = 0; i < (int)width; i++)
		delete[] garden[i];
	delete[] garden;

	// Удаляем план сада
	for (int i = 0; i < (int)width; i++)
		delete[] plan[i];
	delete[] plan;
}

void Garden::showGarden(const Garden& garden) {
	cout << "\nCurrent GARDEN" << "\n";
	garden.showField((const bool**)garden.garden, garden.width, garden.height);
}

void Garden::showGardenPlan(const Garden& garden) {
	cout << "\nGarden PLAN" << "\n";
	garden.showField((const bool**)garden.plan, garden.width, garden.height);
}

void Garden::showField(const bool** garden, const int& width, const int& height) {
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++)
			cout << garden[j][i] << " ";
		cout << "\n";
	}
}

bool Garden::checkGarden() {
	for (size_t x = 0; x < width; x++)
		for (size_t y = 0; y < height; y++)
			if (garden[x][y] != plan[x][y])
				return false;
	return true;
}
#pragma endregion

void gardener(Garden& gardenInit, const string& name, coord& startPlace, coord& step) {
	auto garden = gardenInit.getGarden();
	auto plan = gardenInit.getGardenPlan();
	auto width = gardenInit.getWidth();
	auto height = gardenInit.getHeight();
	int x = startPlace.x, y = startPlace.y, workTime;

	int currentSteps = 0;
	do
	{
		mtx.lock();
		cout << "Thread:" << this_thread::get_id() << '\t'
			<< "Gardener " << name << '\t'
			<< "position (" << x << ", " << y << ")\t";
		workTime = 10;

		if (!plan[x][y]) cout << "no garden in this cell\n";
		else if (garden[x][y]) cout << "Done by another gardener\n";
		else {
			garden[x][y] = true;
			cout << "working on cell\n";
			workTime = 50;
		}
		mtx.unlock();
		this_thread::sleep_for(chrono::milliseconds(workTime));

		x += step.x;
		y += step.y;
		currentSteps++;

		/* Недоработка задания:
			"спускаемся вниз" -- спустившись вниз
			мы идём в другую сторону
			или телепортируемся в начало строки/столбца и продолжаем в том же направлении?
			На всякий случай я написал оба варианта.

			Учитывая, что мы не управляем дроном / не стремимся искать оптимальный путь,
			то пойти вначало и продолжить работу не кажется чем-то невозможным.
		*/
		// Вариант, где мы разворачиваемся
		//if (step.x != 0 && (x < 0 || x >= width)) {// Садовник по горизонтали
		//	x -= step.x;
		//	step.x *= -1;
		//	y++;
		//}
		//else if (step.y != 0 && (y < 0 || y >= height)) {// Садовник по вертикали
		//	x--;
		//	y -= step.y;
		//	step.y *= -1;
		//}

		// Вариант с телепортацией и продолжаем идти по направлению
		if (step.x != 0 && x >= width) {// Садовник по горизонтали
			x = 0;
			y++;
		}
		else if (step.y != 0 && y < 0) {// Садовник по вертикали
			x--;
			y = height - 1;
		}
	} while (currentSteps != width * height);
}