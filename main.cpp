#include <iostream>	// cout cin
#include <vector> //vectors
#include <string>	//string variables used
#include <fstream>	// download and uploading files
#include <ctime>		// ctime used for clock and time functions
#include "book.h"			//importing all the header files
#include "librarian.h"
#include "teacher.h"
#include "student.h"
#include "copies.h"
using namespace std;

void loadUsers();	
void loadBooks();
void loadCopies();
void login();
void readerInterface();
void adminInterface();
int getTime();
int adjustTime(int currentTime);


int startTime = getTime();//startTime is the time the program started at   (time is kept in seconds since january 1, 2010)

vector <book> books;	// This vector holds all the books loaded from book.txt
vector <copies> copiez;

vector <student> students; // students holds all the student accounts loaded from student.txt
vector <teacher> teachers; // holds all the teachers
vector <librarian> admins; // Holds all the librarians

int userIndex = -1;  //This is the index in the vector of the logged in user
					  // It doesn't need to be -1 but it made user authentication easier
int userType = -1;  // This will differentiate which type of user is using the system


int main() {
	loadBooks();
	loadCopies();
	loadUsers();
	login();
	if (userType == 3) {
		adminInterface();
	}
	else if (userType == 2 || userType == 1) {
		readerInterface();
	}
	else{
		cerr << "Error in user type." << endl;
		exit(1);
	}

	cout << "Thank you for visiting the library!" << endl;



}

void loadBooks() {    //Reads the data from book.txt
	int count, favor;
	long long ISBN;							// these are the holder variables to transfer file contents to vector books
	string title, author, category;
	ifstream data_in;
	data_in.open("book.txt");				//opens book.txt as data_in and if fails returns an error
	if (data_in.fail()) {
		cerr << "Error: Failed to open 'book.txt'.";
		exit(1);
	}
	while (!data_in.eof()) {			 // loops until end of file
		vector <int> index;
		int holder;
		data_in >> ISBN >> title >> author >> category >> favor >> count;// the book.txt file holds information in this order
		for (int i = 0; i < count; i++) {
			data_in >> holder;
			index.push_back(holder);
		}
		books.push_back(book(ISBN, title, author, category, favor, count, index));		//creates a book in the vector with the information
	}
	data_in.close();
	cout << "Successfully loaded book data" << endl;	// closes book.txt and gives a msg
}

void loadCopies() {
	// the copies.txt file holds information in this format:
	//ID ISBN Availability reader borrowDate ExpireDate #ofReservers reserverName reserveDate
	ifstream data_in;
	data_in.open("copies.txt");			//opens users.txt as data_in
	if (data_in.fail()) {
		cerr << "Error: Failed to open 'copies.txt'.";	//gives an error if it cant open user.txt
		exit(1);
	}

	while (!data_in.eof()) {			//loop until at end of file for copies.txt 
		long long ISBN;
		int ID, borrowDate, expireDate, NumofReservers, reserverDate;
		bool availability;
		string reader, reserver;
		vector <string> reservers;
		vector <int> reserverDates;
		data_in >> ID >> ISBN >> availability;
		if (!availability)
			data_in >> reader >> borrowDate >> expireDate;
		data_in >> NumofReservers;
		for (int i = 0; i < NumofReservers; i++) {
			data_in >> reserver >> reserverDate;
			reservers.push_back(reserver);
			reserverDates.push_back(reserverDate);
		}
		if (!availability) {
			copiez.push_back(copies(ID, ISBN, availability, reader, borrowDate, expireDate, reservers, reserverDates));
		}
		else {
			copiez.push_back(copies(ID, ISBN, availability, reservers, reserverDates));
		}
	}
	data_in.close();	//closes user.txt
	cout << "Successfully loaded book copies" << endl;	//gives a success message
}

void loadUsers() { //reads the data from users.txt
	string username, password;
	int type;

	ifstream data_in;
	data_in.open("users.txt");			//opens users.txt as data_in
	if (data_in.fail()) {
		cerr << "Error: Failed to open 'users.txt'.";	//gives an error if it cant open user.txt
		exit(1);
	}

	while (!data_in.eof()) {			//loop until at end of file for user.txt 
		int maxAllowed, maxTime, listSize1, listSize2, ID, penalties, ISBN;		//creates the corrosponding variables to the information in user.txt	
		vector <int> borrowed;	// This vector is going to hold the borrowed copy ID's for the user before construction for that user
		vector <int> reserved;	// This vector will hold the reserved book ISBN's for the user

		data_in >> type >> username >> password;

		switch (type) {
		case 1:
		case 2:
			data_in >> penalties >> maxAllowed >> maxTime >> listSize1;	//takes in more information from user.txt
			for (listSize1; listSize1 > 0; listSize1--) {		//listSize1 will indicate how many copy ID's the user has taken out
				data_in >> ID;								//So, it will tell the code how many more integers to scan
				borrowed.push_back(ID);
			}
			data_in >> listSize2;
			for (listSize2; listSize2 > 0; listSize2--) {		//listSize2 will indicate how many books the user has reserved
				data_in >> ISBN;								//So, it will tell the code how many more integers to scan
				reserved.push_back(ISBN);
			}
			//These if statments checks the 'type' of user to make the correct object: student or teacher
			if (type == 1) {
				students.push_back(student(username, password, maxAllowed, maxTime, penalties, borrowed, reserved));
			}
			else if (type == 2) {
				teachers.push_back(teacher(username, password, maxAllowed, maxTime, penalties, borrowed, reserved));
			}
			break;
		case 3:
			admins.push_back(librarian(username, password));
			break;
		}
	}
	data_in.close();	//closes user.txt
	cout << "Successfully loaded user data" << endl << endl;	//gives a success message
}

void login() {	// user authentication function
	while (1) {
		string user = "", pass = "";	// strings holding the username and password
		int ACCtype;
		bool found = false;			// bool if a matching account is found
		cout <<"Login: " << endl << endl;
		cout << "(1) Student Login" << endl;
		cout << "(2) Teacher Login" << endl;
		cout << "(3) Admin Login" << endl;
		cin >> ACCtype;
		cout << endl << "Username:" << endl;
		cin >> user;
		cout << "Password:" << endl;		// takes in username and password
		cin >> pass;
		if (ACCtype == 3) {
			for (int i = 0; i <= admins.size() - 1 || found; i++) {			// This searches every student in the vector students
				if (admins[i].getName() == user && admins[i].getPass() == pass) {
					found = true;						//If the username and password match then the index of the student is remembered
					userIndex = i;					//and found is set to true
					userType = 3;
					break;
				}
			}
		}
		else if (ACCtype == 2) {
			for (int i = 0; i <= teachers.size() - 1 || found; i++) {			// This searches every student in the vector students
				if (teachers[i].getName() == user && teachers[i].getPass() == pass) {
					found = true;						//If the username and password match then the index of the student is remembered
					userIndex = i;					//and found is set to true
					userType = 2;
					break;
				}
			}
		}
		else if (ACCtype == 1) {
			for (int i = 0; i <= students.size() - 1 || found; i++) {			// This searches every student in the vector students
				if (students[i].getName() == user && students[i].getPass() == pass) {
					found = true;						//If the username and password match then the index of the student is remembered
					userIndex = i;					//and found is set to true
					userType = 1;
					break;
				}
			}
		}
		else {
			cout << "Please enter a valid account type." << endl;
		}

		if (!found) {
			cout << "No user with this password found" << endl << endl;	// if not found
		}
		else {
			break;		//breaks the infinite while loop 
		}
	}
}

void readerInterface() {
	char input;
	bool logout = false;
	if (userType == 1) {
		cout << "Welcome " << students[userIndex].getName() << endl << endl;
	}else if (userType == 2) {
		cout << "Welcome " << teachers[userIndex].getName() << endl << endl;
	}
	while (1) {
		cout << "(1) Borrow Book" << endl;
		cout << "(2) Return Book" << endl;
		cout << "(3) Search Book" << endl;
		cout << "(4) Reserve Book" << endl;
		cout << "(5) Cancel Reservation" << endl;
		cout << "(6) Renew Book" << endl;
		cout << "(7) Receive a book recomendation!" << endl;
		cout << "(0) Logout" << endl;

		cin >> input;
		switch (input) {
		case '0':
			logout = true;
			break;
		case '1':
			borrowBook();
			break;
		case '2':
			returnBook();
			break;
		case '3':
			searchBook();
			break;
		case '4':
			reserveBook();
			break;
		case '5':
			cancelReservation();
			break;
		case '6':
			renewBook();
			break;
		case '7':
			recommendBook();
			break;
		default:
			cout << "Not a valid option. Please try again.";
			break;
		}

	}
}

void adminInterface() {
	char input;
	bool logout = false;
	while (logout) {

		cout << "Welcome " << admins[userIndex].getName() << endl << endl;
		cout << "(1) Search Book" << endl;
		cout << "(2) Add New Book" << endl;
		cout << "(3) Delete Book" << endl;
		cout << "(4) Add New User" << endl;
		cout << "(5) Delete User" << endl;
		cout << "(0) Logout" << endl;
		cin >> input;
		switch (input) {
		case '0': 
			logout = true;
			break;
		case '1':
			searchBook();
			break;
		case '2':
			addBook();
			break;
		case '3':
			deleteBook();
			break;
		case '4':
			addUser();
			break;
		case '5':
			deleteUser();
			break;
		default:
			cout << "Not a valid option. Please try again.";
			break;
		}
	}
}

int getTime() {	// This function gets the current time
	time_t timer;			// Time is counted in seconds since January 1, 2010
	struct tm y2k = { 0 };
	double seconds;
	// ********I did not write this
	y2k.tm_hour = -1;   y2k.tm_min = 0; y2k.tm_sec = 0;		// This code is from the <ctime> library reference : time
	y2k.tm_year = 110; y2k.tm_mon = 0; y2k.tm_mday = 2;		// The link to the page is:
																					// http://www.cplusplus.com/reference/ctime/time/
	time(&timer);  // get current time; same as: timer = time(NULL)  

	seconds = difftime(timer, mktime(&y2k));
	return seconds;
}

int adjustTime(int currentTime) { // This takes in the current time which will be gotton with the function getTime()
								  //It then adjusts the time so every X amount of seconds in real life while using this program
									// will have a day pass inside the program
	int time_difference = currentTime - startTime;
	int time = currentTime + time_difference * (864 * 10); // <--- The number being multipled with 864 is how many seconds while the program is openned will count a day	
	return time;											// At the time of writing this it is currently 10: So every 10 seconds the program is open 1 day will pass

	// This function takes the difference from the start time of the program and the current time
	// and skews it so every 10 seconds the program was open a day passed
}