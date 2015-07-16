#include <iostream>
#include <string>
#include <functional>	// hash
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "stupidgraph.h"

enum class Suit : char {
	HEARTS, SPADES, DIAMONDS, CLUBS
};

class Card {
	int val;
	Suit suit;
public:
	Card(int v, Suit s) : val{v}, suit{s} {}
	virtual int get_val() {return val;}
	Suit get_suit() {return suit;}
};

class Blackjack_card : public Card {
public:
	Blackjack_card(int v, Suit s) : Card{v, s} {}
	int get_val() override {
		int val = Card::get_val();
		// ace is 11, facecards are 13
		if (val == 1) return 11;
		if (val >= 10) return 10;
		return val;
	}
};

// call center
class Response {}; class Call {};
class Call_handler {
public:
	Response virtual get_call(Call) = 0; // not const because finish call will be called
	void virtual finish_call() = 0;
};

class Employee : public Call_handler {
protected:
	std::string name;
	size_t id;
	bool busy;
	Employee() {}	// default constructor in case derived classes need to be put into container
	Employee(const std::string& n, size_t i) : name{n}, id{i}, busy{false} {}
public:
	size_t get_id() const {return id;}
	bool is_busy()  const {return busy;}
	void set_busy() 	{busy = true;}
	void clear_busy() 	{busy = false;}
	bool operator==(const Employee& other) const {return other.id == id;} // identity and hash
};
// hash specialization for Employees
template <typename E>
struct Id_hash {
	size_t operator()(const E& e) const {
		return std::hash<size_t>()(e.get_id());
	}
};


class Callcenter;	// forward declaration
class Fresher : public Employee {
	Callcenter* workplace;	// need to be added back onto available queue after finishing call
public:
	Fresher() : Employee{}, workplace{nullptr} {}
	Fresher(const std::string& n, size_t i, Callcenter* w) : Employee{n,i}, workplace{w} {}
	Response get_call(Call c) override {
		// handle call ...
		std::cout << "Hello this is " << name << " speaking\n";
		Response res;
		finish_call();
		return res;
	}
	void finish_call() override;
};

class Tech_lead : public Employee {
public:
	Tech_lead(const std::string& n, size_t i) : Employee{n,i} {}
	Response get_call(Call c) override {
		// handle call ...
		std::cout << "This is technical lead " << name << ", what's your issue?\n";
		Response res;
		finish_call();
		return res;
	}
	void finish_call() override {
		clear_busy();
		std::cout << "Thank you for calling\n";
	}
};

class Product_manager : public Employee {
	int num_calls;
public:
	Product_manager(const std::string& n, size_t i) : Employee{n,i}, num_calls{0} {}
	Response get_call(Call c) override {
		// handle call ...
		std::cout << "You've reached the product manager " << name << "; leave your message\n";
		Response res;
		// finished all calls
		finish_call();
		return res;
	}
	// put another call on the product manager
	int take_call() {
		if (!num_calls) set_busy();
		++num_calls;
		return num_calls;
	}
	void finish_call() {
		--num_calls;
		if (!num_calls) clear_busy();
		std::cout << "I still have " << num_calls << " on the line\n";
	}
};

class Callcenter {
	std::queue<Fresher> available_freshers;
	std::unordered_map<size_t,Fresher> busy_freshers;	// storage necessary since reference is returned
	Tech_lead tech_lead;
	Product_manager product_manager;
public:
	Callcenter(Tech_lead&& tl, Product_manager&& pm) : tech_lead{tl}, product_manager{pm} {}
	void add_fresher(const std::string& name, size_t id) {
		Fresher fresher {name, id, this};
		available_freshers.push(fresher);
	}
	void fresher_finished(Fresher& fresher) {
		available_freshers.push(fresher);
		busy_freshers.erase(fresher.get_id());
	}
	Call_handler& get_call_handler() {
		if (!available_freshers.empty()) {
			Fresher fresher {std::move(available_freshers.front())};
			available_freshers.pop();
			fresher.set_busy();
			busy_freshers[fresher.get_id()] = fresher;
			return busy_freshers[fresher.get_id()];
		}
		if (!tech_lead.is_busy()) {
			tech_lead.set_busy();
			return tech_lead;
		}
		else {
			product_manager.take_call();
			return product_manager;
		}
	}
	size_t available_handlers() const {
		return available_freshers.size() + !tech_lead.is_busy() + !product_manager.is_busy();
	}
};

void Fresher::finish_call() {
	// put back onto avaiable queue of its workplace
	std::cout << name << ": have a nice day\n";
	clear_busy();
	workplace->fresher_finished(*this);
}
// after finished handling call should get readded onto Callcenter's queue


// online book reader system
constexpr long long invalid = -1;
class User;
class Book;
class Library;	// manager for books and users

// example of convoluted design from trying to make index of books and users static
// better design would be to have the Library hold the Book_map and User_map
// these classes are just for demonstration purposes (particularly the use of function-static variables
// as substitute for class static variables and variadic templates for emplace)
class Book {
	std::string content;
	double price;
	long long id;

	using Book_map = std::unordered_map<size_t, Book>;
	static Book_map& books() {
		static Book_map books_;
		return books_;
	}
	// allow manager to adjust price
	friend class Library;
	void adjust_price(double new_price) {price = new_price;}

public:
	Book() : id{invalid} {}
	explicit operator bool() const {return id != invalid;}

	// require moving of content into book (caller must call with literal or std::move)
	Book(long long i, double p, std::string&& c) :
		content{c}, price{p}, id{i} {}

	static Book find(size_t id) {
		auto book = books().find(id);
		if (book != books().end()) return book->second;
		else return Book{};
	}
	static void add_book(const Book& book) {books()[book.get_id()] = book;}
	static void add_book(Book&& book) {books()[book.get_id()] = std::move(book);}
	static void add_book(long long i, double p, std::string&& c) {
		books()[i] = Book{i, p, std::move(c)};
	}
	static void remove_book(size_t book_id) {
		books().erase(book_id);
	}
	static size_t count_books() {
		return books().size();
	}

	std::pair<std::string, size_t> read(size_t start_index, size_t chars_to_read) {
		// second value is the index to be continued on
		return {content.substr(start_index, chars_to_read), start_index + chars_to_read};
	}
	double get_price() const {return price;}
	long long get_id() const {return id;}
};

enum class User_type : char {
	REGULAR, PREMIUM, ADMIN, SUPERADMIN
};

class User {
	std::string name;
	std::unordered_set<size_t> owned_books;
	double balance;
	long long id;	
	User_type type;
	// privately contained index of all users
	using User_map = std::unordered_map<size_t, User>;
	// having static variable in function ensures only 1 definition
	// overcomes need for user to define in a cpp file
	static User_map& users() {
		static User_map users_;
		return users_;
	}
public:
	// invalid user marked by id == -1
	User() : id{invalid} {};
	explicit operator bool() const {return id != invalid;}

	User(const std::string& n, long long i, User_type t = User_type::REGULAR) :
		name{n}, balance{0}, id{i}, type{t} {}


	static User find(size_t user_id) {
		auto user = users().find(user_id);
		if (user != users().end()) return user->second;
		else return User{};
	}
	static void add_user(const User& user) {users()[user.get_id()] = user;}
	static void add_user(User&& user) {users()[user.get_id()] = std::move(user);}	
	static void add_user(const std::string& n, long long i, User_type t = User_type::REGULAR) {
		users()[i] = User{n,i,t};
	}
	static void remove_user(size_t user_id) {
		users().erase(user_id);
	}
	static size_t count_users() {
		return users().size();
	}

	long long get_id() const 	{return id;}
	double get_balance() const 	{return balance;}
	void add_balance(double addition) {balance += addition;}
	bool buy_book(size_t book_id) {
		// already own book
		if (owned_books.find(book_id) != owned_books.end()) return false;
		const Book& book {Book::find(book_id)};
		// book doesn't exist or too pricy
		if (!book || book.get_price() > balance) return false;
		balance -= book.get_price();
		owned_books.insert(book_id);
		return true;
	}

	Book get_book(size_t book_id) const {
		if (owned_books.find(book_id) != owned_books.end()) return Book::find(book_id);
		return Book{};	// invalid book
	}
};

class Library {
	Book b;
	User u;
public:
	Library() = default;
	Book find_book(size_t id) const {return Book::find(id);}
	User find_user(size_t id) const {return User::find(id);}

	void add_book(const Book& book) {Book::add_book(book);}
	void add_user(const User& user) {User::add_user(user);}
	// transfers ownership to library
	template <typename... Args>
	void emplace_book(Args... args) {Book::add_book(Book{args...});}
	template <typename... Args>
	void emplace_user(Args... args) {User::add_user(User{args...});}

	size_t num_books() const {return Book::count_books();}
	size_t num_users() const {return User::count_users();}

	void listen_request() {}	// listen to user requests (web server)
};



// 1-1 chat server using static graph
enum class Chat_status : char { 
	ONLINE, OFFLINE, SUSPENDED, BLACKLISTED, BANNED
};
class Chat_server;
class Chat_user {
	long long id;
	std::string handle;
	std::string password;
	Chat_status status;

	friend class Chat_server;
	void change_password(const std::string& new_password) {password = new_password;}

public:
	Chat_user() : id{invalid} {}
	Chat_user(long long i, const std::string& h, const std::string& p) :
		id{i}, handle{h}, password{p}, status{Chat_status::OFFLINE} {}

	long long get_id() 				const {return id;}
	const std::string& get_handle() const {return handle;}
	bool is_online()				const {return status == Chat_status::ONLINE;}
};
class Chat_server {
	// holds all connection information
	Static_graph users;
	// holds all accessory information
	std::vector<Chat_user> user_info;
public:
	long long add_user(const std::string& handle, const std::string& password) {
		size_t user_id {users.add_node()};
		// user_info's index should correspond to user_id
		user_info.emplace_back(user_id, handle, password);
		return user_id;
	}
	void add_friend(size_t user_id, size_t friend_id) {
		// no way to say no :(
		users.add_edge(user_id, friend_id);
		users.add_edge(friend_id, user_id);
	}
};

void test_cards() {
	Blackjack_card c1 {11, Suit::HEARTS};

	std::cout << c1.get_val() << ' ' << (int)c1.get_suit() << std::endl;
}

void test_callcenter() {
	Callcenter callcenter {Tech_lead{"Bill Gates", 1234}, Product_manager{"Steven Jobs", 6666}};
	// callcenter.add_fresher("F1", 1);
	// callcenter.add_fresher("F2", 2);
	// callcenter.add_fresher("F3", 3);
	// callcenter.add_fresher("F4", 4);
	// callcenter.add_fresher("F5", 5);

	std::cout << "before: " << callcenter.available_handlers() << std::endl;
	Call_handler& handler = callcenter.get_call_handler();
	auto& another_handler = callcenter.get_call_handler();

	std::cout << "during: " << callcenter.available_handlers() << std::endl;
	handler.get_call(Call{});
	another_handler.get_call(Call{});

	std::cout << "after:  " << callcenter.available_handlers() << std::endl;
}

void test_library() {
	Library mylib;
	// User admin {"Robo admin", 1, User_type::ADMIN};
	// User joe {"Joe Schmoe", 2};
	mylib.emplace_user("Robot admin", 1, User_type::ADMIN);
	mylib.emplace_user("Joe Schmoe", 2);

	Book b1 {1, 0.99, "Pride and Prejudice"};
	Book b2 {2, 2.59, "Anna Karenina"};
	mylib.add_book(b1);
	mylib.add_book(b2);
}