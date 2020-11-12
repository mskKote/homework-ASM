#include <iostream>
#include <thread>
#include <vector>

using namespace std;

/*
	Попов Виталий БПИ192

	Структура:
		Bookshelf - отвечает за хранение и заполнение 1-ой книжной полки.
		Также способно проверить, заполнена ли книжная полка.

		Library - хранит библиотеку, 
		представляющую собой ряды по несколько книжных полок.

		Параметры для этих классов не изменяются 
		--> они static и задаются с самого начала.

		Необходимо сделать "Портфель задач", 
		т.е. занесение 1-ой книги в отдельном потоке. 
		Многопоточность представлена в методе заполнения Library - FillLibrary.

*/

/// <summary>
/// Отвечает за 1 книжную полку. 
/// Библиотека - это массив книжных полок.
/// </summary>
class Bookshelf {
public:
	static unsigned int maxBooks;

	/// <summary>
	/// Метод добавляет 1 книгу и выводит самую последнюю информацию.
	/// Вывод может искажаться за счёт многопоточности. 
	/// Выводится самое последнее значение currentBooks, 
	/// а не то, которое было на момент выполнения потока
	///
	/// Также из-за использования 1 потока вывода в консоль
	/// информация в консоли может искажаться
	/// </summary>
	/// <param name="row">Для вывода: ряд книг</param>
	/// <param name="bookshelf">Для вывода: номер книжной полки</param>
	void addBook(int row, int bookshelf);

	/// <returns>Проверяет, заполнена ли книжная полка</returns>
	bool isFull() { return currentBooks == maxBooks; }

private:
	unsigned int currentBooks = 0;
};
unsigned int Bookshelf::maxBooks = 0;

class Library
{
public:
	/// <summary>
	/// Количество шкафов
	/// </summary>
	static unsigned int bookshelfs;

	/// <summary>
	/// Количество рядов
	/// </summary>
	static unsigned int bookshelfsRows;

	/// <summary>
	/// Создаём ряды шкафов
	/// </summary>
	Library() {
		library = new Bookshelf * [bookshelfsRows];

		for (size_t i = 0; i < bookshelfsRows; i++)
		{
			library[i] = new Bookshelf[bookshelfs];
			// Мб тут ошибка. Мб не нужно создавать все экземпляры
			for (size_t j = 0; j < bookshelfs; j++)
				library[i][j] = Bookshelf();
		}
	}
	~Library() {
		for (size_t i = 0; i < bookshelfsRows; i++)
			delete[] library[i];
	}

	/// <summary>
	/// Заполняем всю библиотеку книгами
	/// </summary>
	void FillLibrary();

	/// <summary>
	/// Устанавливаем 1 книгу в указанную книжную полку.
	/// </summary>
	/// <param name="row">Ряд книнг</param>
	/// <param name="indexBookshelf">Книжная полка в ряду</param>
	void AddLastBook(int row, int indexBookshelf);

	/// <summary>
	/// Проверяет, отработала ли программа
	/// </summary>
	/// <returns>Заполнены ли все шкафы с книгами</returns>
	bool CheckEveryBookshelfs();

private:
	unsigned int lastBookshelf = 0;
	Bookshelf** library;
};
unsigned int Library::bookshelfs = 0;
unsigned int Library::bookshelfsRows = 0;


int main()
{
	int m, n, k;
	cout << "Input m\t"; cin >> m;
	cout << "Input n\t"; cin >> n;
	cout << "Input k\t"; cin >> k;
	cout << "\n";
	// Устанавливаем значения
	Library::bookshelfsRows = m;
	Library::bookshelfs = n;
	Bookshelf::maxBooks = k;

	Library university;
	university.FillLibrary();

	cout << "\nAll filled:\t"
		<< (university.CheckEveryBookshelfs() ? "yes" : "no")
		<< "\n\n";
}

#pragma region Library
void Library::FillLibrary() {
	for (size_t i = 0; i < bookshelfsRows; i++) 
		for (size_t j = 0; j < bookshelfs; j++) 
			AddLastBook(i, j);
}

void Library::AddLastBook(int row, int indexBookshelf) {

	//if (library[lastBookshelf].isFull()) return;
	//library[row][indexBookshelf].addBook();
	int max = library[row][indexBookshelf].maxBooks;
	thread* th = new thread[max];

	for (size_t i = 0; i < max; i++)
		th[i] = thread([&]() { library[row][indexBookshelf].addBook(row, indexBookshelf); });

	for (size_t i = 0; i < max; i++)
		th[i].join();

}

bool Library::CheckEveryBookshelfs() {
	for (size_t i = 0; i < bookshelfsRows; i++) {
		for (size_t j = 0; j < bookshelfs; j++) {
			if (!library[i][j].isFull())
				return false;
		}
	}
	return true;
}
#pragma endregion

void Bookshelf::addBook(int row, int bookshelf) {
	
	cout << "bookshelf [" << row << "; " << bookshelf << "] is full: ";
	currentBooks++;
	
	if (isFull()) cout << "YES [" << currentBooks << '/' << maxBooks << ']' << '\n';
	else		   cout << "no [" << currentBooks << '/' << maxBooks << ']' << '\n';


	cout << "thread ID: " << this_thread::get_id() << '\n'
		<< "=================\n";
}
