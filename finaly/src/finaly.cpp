#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iomanip>
#include <sstream>
#include <exception>

using namespace std;

struct Year {
	int value;
	Year(int year)
	: value(year) {}
};

struct Month {
	int value;
	Month(int month)
	: value(month){}
};

struct Day {
	int value;
	Day(int day)
	: value(day) {}
};

bool operator<(const Day& lhs, const Day& rhs) {
	return lhs.value < rhs.value;
}
bool operator<(const Month& lhs, const Month& rhs) {
	return lhs.value < rhs.value;
}
bool operator<(const Year& lhs, const Year& rhs) {
	return lhs.value < rhs.value;
}
bool operator==(const Day& lhs, const Day& rhs) {
	return lhs.value == rhs.value;
}
bool operator==(const Month& lhs, const Month& rhs) {
	return lhs.value == rhs.value;
}
bool operator==(const Year& lhs, const Year& rhs) {
	return lhs.value == rhs.value;
}
ostream& operator<<(ostream& stream, const Year& a) {
	stream << a.value;
    return stream;
}
ostream& operator<<(ostream& stream, const Month& a) {
	stream << a.value;
    return stream;
}
ostream& operator<<(ostream& stream, const Day& a) {
	stream << a.value;
    return stream;
}

class Date {
public:
	Date () : year(1), month(1), day(1) {}
	Date (Year new_year, Month new_month, Day new_day)
	: year(new_year.value),
	  month(new_month.value),
	  day(new_day.value) {
		if (new_month.value >= 13 || new_month.value <= 0) {
			string error = "Month value is invalid: " + to_string(new_month.value);
			throw invalid_argument(error);
		}
		if (new_day.value >= 32 || new_day.value <= 0) {
			string error = "Day value is invalid: " + to_string(new_day.value);
			throw invalid_argument(error);
		}
	}
	Year GetYear() const {
		return year;
	}
	Month GetMonth() const {
		return month;
	}
	Day GetDay() const {
		return day;
	}
private:
	Year year;
	Month month;
	Day day;
};

bool operator<(const Date& lhs, const Date& rhs) {
	if(lhs.GetYear() == rhs.GetYear()) {
		if(lhs.GetMonth() == rhs.GetMonth()) {
			return lhs.GetDay() < rhs.GetDay();
		} else {
			return lhs.GetMonth() < rhs.GetMonth();
		}
	}
	return lhs.GetYear() < rhs.GetYear();
}

bool operator==(const Date& lhs, const Date& rhs) {
	return (
		(lhs.GetYear() == rhs.GetYear()) &&
		(lhs.GetMonth() == rhs.GetMonth()) &&
		(lhs.GetDay() == rhs.GetDay())
	);
}

ostream& operator<<(ostream& stream, const Date& a) {
	cout << setfill('0');
	stream << setw(4) << a.GetYear() << '-' << setw(2) << a.GetMonth() << '-'<< setw(2)  << a.GetDay();
    return stream;
}

void ParseStr(const string& s) {
	if(
			s != "Add" &&
			s != "Del" &&
			s != "Print" &&
			s != "Find"
					) {
		string error = "Unknown command: " + s;
		throw invalid_argument(error);
	}
}

istream& operator>>(istream& stream, Date& a) {
	int d = -1;
	int m = -1;
	int y = -1;
	stream >> y;
	stream.ignore(1);
	stream >> m;
	stream.ignore(1);
	stream >> d;
	a = Date(y,m,d);
    return stream;
}

void EnsureNextSymbolAndSkip(stringstream& stream, const string& s) {
	if (stream.peek() != '-') {
		string error = "Wrong date format: " + s;
		throw invalid_argument(error);
	}
	stream.ignore(1);
}

Date ParseDate(const string& s) {
	stringstream stream(s);
	Date date;
	int y = -1,m = -1,d = -1;
	stream >> y;
	EnsureNextSymbolAndSkip(stream, s);
	stream >> m;
	EnsureNextSymbolAndSkip(stream, s);
	stream >> d;
	if(!stream.eof()) {
		string error = "Wrong date format: "+ s;
		throw invalid_argument(error);
	}
	date = Date(y,m,d);
	return date;
}


class Database {
public:
	void AddEvent(const Date& date, const string& event) {
		m[date].insert(event) ;
	}

	bool DeleteEvent(const Date& date, const string& event) {
		int old_size = m[date].size();
		m[date].erase(event) ;
		int new_size = m[date].size();
		if (m[date].size() == 0) {
			m.erase(date);
		}
		return new_size < old_size;
	}

	int DeleteDate(const Date& date){
		int N = m[date].size();
		m.erase(date);
		return N;
	}

	string Find(const Date& date) const {
		string str;
		if (m.count(date)) {
			unsigned int j = 0;
			for(const auto& i : m.at(date)) {
				str += i;
				if (j != m.at(date).size()-1){
					str += '\n';
				}
				j++;
			}
		}
		return str;
	}

	void Print() const {
		for (const auto& i : m) {
			for(const auto& s : i.second){
				cout << i.first << ' ' << s << endl;
			}
		}
	}
private:
	map<Date,set<string>> m;
};

void CheckGetLine(const	string& command, int& count,string& str,string& event, Date& date) {
	string w = "";
	string temp_date = "";
	count = 0;
	istringstream iss(command);
	while (iss >> w) {
		if (count == 0) {
			ParseStr(w);
			str = w;
		} else if (count == 1) {
			ParseDate(w);
			temp_date = w;
			stringstream d;
			d << w;
			d >> date;
		} else if (count == 2) {
			event = w;
		}
		count++;
	}
	if(
			(str == "Add" ||
			str == "Del" ||
			str == "Find") &&
			count == 1
					) {
		string error = "Wrong date format: ";
		throw invalid_argument(error);
	}
	if(
			str == "Add" &&
			count == 2
					) {
		string error = "Wrong date format: " + temp_date;
		throw invalid_argument(error);
	}
}

int main() {
	try {
	Database db;
	string command;
	while (getline(cin, command)) {
		int count = 0;
		string str="";
		string event="";
		Date date{1,1,1};
		CheckGetLine(command, count, str, event, date);
		if ("Add" == str && event != "" && count == 3) {
			db.AddEvent(date, event);
		} else if ("Find" == str && count == 2) {
			if (db.Find(date) != "") {
				cout << db.Find(date) << endl;
			}
		} else if ("Del" == str && (count == 2 || count == 3)) {
			if (count == 3) {
							if(db.DeleteEvent(date, event)){
								cout << "Deleted successfully" << endl;
							} else {
								cout << "Event not found" << endl;
							}
			} else {
				cout << "Deleted "<< db.DeleteDate(date) << " events" << endl;
			}
 		} else if ("Print" == str && count == 1) {
			db.Print();
		} else if ("" == str) {
		}
	}

	} catch (exception& e) {
		cout << e.what() << endl;
	}

	return 0;
}
