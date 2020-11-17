#include <iostream>
#include <thread>

using namespace std;

/*
	Попов Виталий БПИ192

	UPD16.11.20: После семинара добавляю ID книг в каталог

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
	static size_t maxBooks;

	Bookshelf() {
		books = new size_t[maxBooks]{ 0 };
	}

	/// <summary>
	///	Ложит книгу bookID на полку bookPlace.
	/// Выводит информацию о местоположении книги и её ID
	/// </summary>
	/// <param name="row">Для вывода: ряд книг</param>
	/// <param name="bookshelf">Для вывода: номер книжной полки</param>
	void addBook(int row, int bookshelf, int bookPlace, int bookID);

	/// <returns>Проверяет, заполнена ли книжная полка</returns>
	bool isFull();

private:
	/// <summary>
	/// ID книг в библиотеке
	/// </summary>
	size_t* books;

};
size_t Bookshelf::maxBooks = 0;

class Library
{
public:
	/// <summary>
	/// Количество шкафов
	/// </summary>
	static size_t bookshelfs;

	/// <summary>
	/// Количество рядов
	/// </summary>
	static size_t bookshelfsRows;

	/// <summary>
	/// Создаём ряды шкафов
	/// </summary>
	Library() {
		library = new Bookshelf * [bookshelfsRows];

		for (size_t i = 0; i < bookshelfsRows; i++)
		{
			library[i] = new Bookshelf[bookshelfs];
			//for (size_t j = 0; j < bookshelfs; j++)
			//	library[i][j] = Bookshelf();
		}
	}
	~Library() {
		for (size_t i = 0; i < bookshelfsRows; i++)
			delete[] library[i];
		delete[] library;
	}

	/// <summary>
	/// Заполняем всю библиотеку книгами
	/// </summary>
	void FillLibrary();

	/// <summary>
	/// Проверяет, отработала ли программа
	/// </summary>
	/// <returns>Заполнены ли все шкафы с книгами</returns>
	bool CheckEveryBookshelfs();

private:
	size_t lastBookshelf = 0;
	Bookshelf** library;
};
size_t Library::bookshelfs = 0;
size_t Library::bookshelfsRows = 0;

/// <summary>
/// Метод для считывания положительного числа 
/// </summary>
/// <returns>Корректность числа</returns>
bool readPositive(int& num, string text) {
	cout << text; cin >> num;
	return num > 0;
}

int main()
{
	int m, n, k;
	if (!readPositive(m, "Input m\t") ||
		!readPositive(n, "Input n\t") ||
		!readPositive(k, "Input k\t")) {
		cout << "[ERROR] You must have positive num of books in library.\n";
		return 1;
	}
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
	return 0;
}

#pragma region Library
void Library::FillLibrary() {

	// Проверка на существование книг. Хотя какой это каталог без книг?
	if (bookshelfs == 0) {
		cout << "[ERROR] You must have books in library." << endl;
		return;
	}

	const int maxBooks = library[0][0].maxBooks;
	const int max = bookshelfsRows * bookshelfs * maxBooks;

	int	bookID = 1; // Уникальный ID книги.
	thread* th = new thread[max];

	// Раскладываем книги по полкам
	for (size_t i = 0; i < bookshelfsRows; i++)
		for (size_t j = 0; j < bookshelfs; j++)
			for (size_t k = 0; k < maxBooks; k++, bookID++)
				th[bookID - 1] = thread(&Bookshelf::addBook, library[i][j], i, j, k, bookID);

	// Завершаем
	for (size_t i = 0; i < max; i++) {
		if (th[i].joinable())
			th[i].join();
		else cout << i << " is not joinable\n";
	}
}

bool Library::CheckEveryBookshelfs() {

	cout << "\n\nCheck===================START\n";

	for (size_t i = 0; i < bookshelfsRows; i++) {
		for (size_t j = 0; j < bookshelfs; j++) {
			cout <<  "Check "<< i << " " << j << ": ";
			if (!library[i][j].isFull()) {
				cout << "\tUNACCEPTABLE!!" << endl;
				return false;
			}

			cout << "\tcorrect\n";
		}
	}
	cout << "Check===================END" << endl;
	return true;
}
#pragma endregion

#pragma region Bookshelf
void Bookshelf::addBook(int row, int bookshelf, int bookPlace, int bookID) {

	cout << "[" << row << "; " << bookshelf << "; " << bookPlace << "]: " << bookID << '\n';
	this->books[bookPlace] = (size_t)bookID;
}

bool Bookshelf::isFull() {
	for (size_t i = 0; i < maxBooks; i++)
		if (this->books[i] == 0)
			return false;
	
	return true;
}
#pragma endregion