#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

/*
	����� ������� ���192 
	������� 17

	���������:
		� main �������� ������, ����������� ������� gardener, 
		� ��� � ������� � ������ �� �������.

		������� gardener �������� �� startPlace � ������������ �� step
		� ��������� ������� ���� � ����� ��������� ��� �� �����.

		Garden - �����, ���������� ��� � ���� ����. 
		����� ������� � ������� ����������, 
		� ����� ������������ �������� ���� �� ������������ �����
*/

mutex mtx;

/// <summary>�������� �� ������ � �����</summary>
class Garden
{
public:
	/// <summary>
	/// ������ ���, ���� ���� � ������������� �� ����� � ������
	/// </summary>
	/// <param name="width">������</param>
	/// <param name="height">�����</param>
	Garden(const unsigned int& width, const unsigned int& height);
	~Garden();

	/// <summary>
	/// ������� ��� � �������
	/// </summary>
	/// <param name="garden">���</param>
	static void showGarden(const Garden& garden);
	/// <summary>
	/// ������� ���� ���� � �������
	/// </summary>
	/// <param name="garden">���</param>
	static void showGardenPlan(const Garden& garden);

	/// <summary>
	/// ���������, ������������� �� ��� ������ �����
	/// </summary>
	/// <returns>��������� ��������</returns>
	bool checkGarden();

	const unsigned int getWidth() { return width; }
	const unsigned int getHeight() { return height; }

	bool** getGarden() { return garden; }
	bool** getGardenPlan() { return plan; }

private:
	/// <summary>
	/// ������� � ������� ���������� ���������� �������
	/// </summary>
	/// <param name="garden">���</param>
	/// <param name="width">������</param>
	/// <param name="height">�����</param>
	static void showField(const bool** garden, const int& width, const int& height);

	unsigned int width, height;
	bool** garden;
	bool** plan;
};

/// <summary>
/// ��������� ��� ������ � ������������
/// </summary>
struct coord {
	int x, y;

	coord(const int& x, const int& y) {
		this->x = x;
		this->y = y;
	}
};

/// <summary������ ���������</summary>
/// <param name="gardenInit">���, ������� ����� ����������</param>
/// <param name="name">��������</param>
/// <param name="place">��������� ���������� ���������</param>
/// <param name="step">����������� ���������</param>
void gardener(Garden& gardenInit, const string& name, coord& startPlace, coord& step);

/// <summary>
/// ����� ��� ���������� �������������� �����
/// </summary>
/// <returns>������������ �����</returns>
bool readPositive(int& num, const string& text) {
	cout << text; cin >> num;
	return num > 0;
}

int main() {
	srand(time(NULL));
	//-----------------------�������� ����
	int n = 0, m = 0;
	if (!readPositive(n, "Input width\t") ||
		!readPositive(m, "Input height\t")) {
		cout << "[ERROR] You must have positive num for width and height.\n";
		return 0;
	}

	auto garden = Garden(n, m);
	Garden::showGardenPlan(garden);
	
	//-----------------------������ ����������
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

	//-----------------------��������
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
	// ������� ���
	for (int i = 0; i < (int)width; i++)
		delete[] garden[i];
	delete[] garden;

	// ������� ���� ����
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

		/* ����������� �������:
			"���������� ����" -- ����������� ����
			�� ��� � ������ �������
			��� ��������������� � ������ ������/������� � ���������� � ��� �� �����������?
			�� ������ ������ � ������� ��� ��������.

			��������, ��� �� �� ��������� ������ / �� ��������� ������ ����������� ����,
			�� ����� ������� � ���������� ������ �� ������� ���-�� �����������.
		*/
		// �������, ��� �� ���������������
		//if (step.x != 0 && (x < 0 || x >= width)) {// �������� �� �����������
		//	x -= step.x;
		//	step.x *= -1;
		//	y++;
		//}
		//else if (step.y != 0 && (y < 0 || y >= height)) {// �������� �� ���������
		//	x--;
		//	y -= step.y;
		//	step.y *= -1;
		//}

		// ������� � ������������� � ���������� ���� �� �����������
		if (step.x != 0 && x >= width) {// �������� �� �����������
			x = 0;
			y++;
		}
		else if (step.y != 0 && y < 0) {// �������� �� ���������
			x--;
			y = height - 1;
		}
	} while (currentSteps != width * height);
}