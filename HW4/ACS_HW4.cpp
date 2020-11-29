#include <iostream>
#include <thread>
#include <map>

using std::cout;
using std::cin;
using std::thread;

/*
	Попов Виталий БПИ192

	UPD16.11.20: После семинара добавляю ID книг в каталог

	Структура:
		КАТАЛОГ
		Класс Catalog - представляет собой каталог. По нему можно искать книги.
		Структура coordBook - обеспечивает координаты книги для каталога.

		БИБЛИОТЕКА
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

#pragma region CATALOG
struct coordBook {
	size_t x, y, z;

	/// <summary>
	/// конструктор по умолчанию
	/// </summary>
	coordBook() : coordBook(0, 0, 0) {}
	coordBook(size_t row, size_t bookshelf, size_t bookPlace) {
		x = row;
		y = bookshelf;
		z = bookPlace;
	}
};

static class Catalog {
public:
	/// <summary>
	/// Вставляет указанную книгу в каталог
	/// </summary>
	/// <param name="bookId">Идентификатор книги. Он уникален</param>
	/// <param name="bookPlace">Данные о местоположении книги</param>
	void setBookToCatalog(const int bookId, const coordBook bookPlace) {
		if (catalog.count(bookId))
			throw std::exception("Book id must be unic");

		catalog.emplace(bookId, bookPlace);
	}

	/// <summary>
	/// Позволяет получить данные о местоположении книги в библиотеке.
	/// ИСПОЛЬЗУЕТ КАТАЛОГ.
	/// </summary>
	/// <param name="bookId"></param>
	/// <returns></returns>
	const coordBook& getBookcoordBookByID(const int& bookId) {
		if (!catalog.count(bookId))
			throw std::exception("No such book id");

		return catalog.at(bookId);
	}

	/// <summary>
	/// Проверяет, можно ли с помощью каталога обратиться к КАЖДОЙ книге в библиотеке.
	/// </summary>
	/// <returns>Статус проверки</returns>
	bool checkCatalog(const int& bookshelfsRows, const int& bookshelfs, const int& maxBooks) {

		cout << "\nCheck catalog";
		cout << "\nCheck===================START\n";

		int currId = 1;
		coordBook coords;
		for (size_t i = 0; i < bookshelfsRows; i++) {
			for (size_t j = 0; j < bookshelfs; j++) {
				for (size_t k = 0; k < maxBooks; k++, currId++) {
					try {
						cout << "Check " << currId;
						coords = getBookcoordBookByID(currId);
						cout << "\tcoords: [" << coords.x << ", " << coords.y << ", " << coords.z << "]\n";
					}
					catch (const std::exception& e) {
						cout << "\tUNACCEPTABLE!!\n" << e.what() << std::endl;
						return false;
					}
				}
			}
		}
		cout << "Check===================END" << std::endl;
		return true;
	}

private:
	/// <summary>
	/// ЭТО КАТАЛОГ.
	/// map - отсортированная коллекция, элементами которой выступают ключ-значение
	/// ID книги - её координаты в библиотеке.
	/// "Поэтому нужно отстортировать по этому признаку (ключу), а далее уже указать местоположение"
	/// </summary>
	std::map<int, coordBook> catalog;
};
#pragma endregion

#pragma region library
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
	void addBook(int row, int bookshelf, int bookPlace, int bookID, Catalog* catalog);

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
	Library();
	~Library();

	/// <summary>
	/// Заполняем всю библиотеку книгами
	/// </summary>
	void FillLibrary(Catalog* catalog);

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
#pragma endregion

/// <summary>
/// Метод для считывания положительного числа
/// </summary>
/// <returns>Корректность числа</returns>
static bool readPositive(int& num, const std::string& text) {
	cout << text; cin >> num;
	return num > 0;
}

int main() {

	cout << "Global values:\n";
	int m = 0, n = 0, k = 0;
	if (!readPositive(m, "Input m\t") ||
		!readPositive(n, "Input n\t") ||
		!readPositive(k, "Input k\t")) {
		cout << "[ERROR] You must have positive num of books in library.\n";
		return 1;
	}
	// Устанавливаем значения
	Library::bookshelfsRows = m;
	Library::bookshelfs = n;
	Bookshelf::maxBooks = k;

	// Создаём КАТАЛОГ
	auto catalog = new Catalog();
	Library university;

	cout << "\nLets fill the library by threads\n[coords]: bookID\n";
	university.FillLibrary(catalog);

	cout << "\nCheck librarAll filled:\t"
		<< (university.CheckEveryBookshelfs() ? "yes" : "no")
		<< "\n\n";

	// Проверяем, можно ли с помощью каталога достучаться до значений
	cout << "\nCheck CATALOG filled:\t"
		<< (catalog->checkCatalog(m, n, k) ? "yes" : "no")
		<< "\n\n";

	delete catalog;
	return 0;
}

#pragma region Library
Library::Library() {
	library = new Bookshelf * [bookshelfsRows];

	for (size_t i = 0; i < bookshelfsRows; i++)
		library[i] = new Bookshelf[bookshelfs];
}
Library::~Library() {
	for (size_t i = 0; i < bookshelfsRows; i++)
		delete[] library[i];
	delete[] library;
}

void Library::FillLibrary(Catalog* catalog) {

	// Проверка на существование книг. Хотя какой это каталог без книг?
	if (!bookshelfs) {
		cout << "[ERROR] You must have books in library." << std::endl;
		return;
	}

	const size_t maxBooks = library[0][0].maxBooks;
	const size_t max = bookshelfsRows * bookshelfs * maxBooks;

	int	bookID = 1; // Уникальный ID книги.
	thread* th = new thread[max];

	// Раскладываем книги по полкам
	for (size_t i = 0; i < bookshelfsRows; i++)
		for (size_t j = 0; j < bookshelfs; j++)
			for (size_t k = 0; k < maxBooks; k++, bookID++)
				th[bookID - 1] = thread(&Bookshelf::addBook, library[i][j], i, j, k, bookID, catalog);

	// Завершаем
	for (size_t i = 0; i < max; i++) {
		if (th[i].joinable())
			th[i].join();
		else cout << i << " is not joinable\n";
	}
}

bool Library::CheckEveryBookshelfs() {

	cout << "\nLets check that all books in bookshelfs are in their places:";
	cout << "\nCheck===================START\n";

	for (size_t i = 0; i < bookshelfsRows; i++) {
		for (size_t j = 0; j < bookshelfs; j++) {
			cout << "Check " << i << " " << j << ": ";
			if (!library[i][j].isFull()) {
				cout << "\tUNACCEPTABLE!!" << std::endl;
				return false;
			}

			cout << "\tcorrect\n";
		}
	}
	cout << "Check===================END" << std::endl;
	return true;
}
#pragma endregion

#pragma region Bookshelf
void Bookshelf::addBook(int row, int bookshelf, int bookPlace, int bookID, Catalog* catalog) {

	cout << "[" << row << "; " << bookshelf << "; " << bookPlace << "]: " << bookID << '\n';
	this->books[bookPlace] = (size_t)bookID;
	catalog->setBookToCatalog(bookID, coordBook(row, bookshelf, bookPlace));
}

bool Bookshelf::isFull() {
	for (size_t i = 0; i < maxBooks; i++)
		if (this->books[i] == 0)
			return false;

	return true;
}
#pragma endregion