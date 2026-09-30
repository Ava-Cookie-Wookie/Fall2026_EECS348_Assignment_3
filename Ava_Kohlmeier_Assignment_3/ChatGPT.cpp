#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

struct Email {
    string sender;
    string subject;
    string date;
    int priority;
    int order;
};

int getPriority(const string& sender) {
    if (sender == "Boss") return 1;
    if (sender == "Subordinate") return 2;
    if (sender == "Peer") return 3;
    if (sender == "ImportantPerson") return 4;
    return 5; // OtherPerson
}

// Convert MM-DD-YYYY to YYYYMMDD so dates can be compared.
int dateValue(const string& date) {
    int month = stoi(date.substr(0, 2));
    int day = stoi(date.substr(3, 2));
    int year = stoi(date.substr(6, 4));

    return year * 10000 + month * 100 + day;
}

int main() {
    vector<Email> emails;
    string line;
    int order = 0;

    // Read all input.
    while (getline(cin, line)) {
        if (line.empty())
            continue;

        if (line.rfind("EMAIL ", 0) == 0) {
            // Remove "EMAIL ".
            string data = line.substr(6);

            // Find the first comma: separates sender from subject.
            size_t comma1 = data.find(',');

            // Find the last comma: separates subject from date.
            size_t comma2 = data.rfind(',');

            string sender = data.substr(0, comma1);
            string subject = data.substr(comma1 + 2,
                                         comma2 - comma1 - 2);
            string date = data.substr(comma2 + 1);

            emails.push_back({
                sender,
                subject,
                date,
                getPriority(sender),
                order++
            });
        }
        else if (line == "COUNT") {
            cout << "There are " << emails.size()
                 << " emails to read.\n\n";
        }
        else if (line == "NEXT") {
            if (emails.empty())
                continue;

            // Find the highest-priority email.
            // If priorities are equal, newest date comes first.
            auto best = min_element(
                emails.begin(),
                emails.end(),
                [](const Email& a, const Email& b) {
                    if (a.priority != b.priority)
                        return a.priority < b.priority;

                    int dateA = dateValue(a.date);
                    int dateB = dateValue(b.date);

                    if (dateA != dateB)
                        return dateA > dateB;

                    return a.order < b.order;
                }
            );

            cout << "Next email:\n";
            cout << "Sender: " << best->sender << '\n';
            cout << "Subject: " << best->subject << '\n';
            cout << "Date: " << best->date << '\n';
        }
        else if (line == "READ") {
            if (emails.empty())
                continue;

            // Remove the current highest-priority email.
            auto best = min_element(
                emails.begin(),
                emails.end(),
                [](const Email& a, const Email& b) {
                    if (a.priority != b.priority)
                        return a.priority < b.priority;

                    int dateA = dateValue(a.date);
                    int dateB = dateValue(b.date);

                    if (dateA != dateB)
                        return dateA > dateB;

                    return a.order < b.order;
                }
            );

            emails.erase(best);
        }
    }

    return 0;
}


