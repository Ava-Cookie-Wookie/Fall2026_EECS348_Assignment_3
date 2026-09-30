// email_prioritizer.cpp
// Prioritizes a CEO's inbox using a hand-built, list-based MaxHeap.
//
// Build: g++ -std=c++17 -Wall -o email_prioritizer email_prioritizer.cpp
// Run:   ./email_prioritizer test.txt      (or pipe the file into stdin)

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Email: one message in the inbox, and the rules for comparing priority.
// ---------------------------------------------------------------------------
class Email {
public:
    Email(const std::string& sender, const std::string& subject,
          const std::string& date, long arrivalOrder)
        : sender_(sender),
          subject_(subject),
          date_(date),
          categoryRank_(rankForCategory(sender)),
          dateKey_(keyForDate(date)),
          arrivalOrder_(arrivalOrder) {}

    // Higher sender category wins; within a category, the newer date wins.
    // If both match, the email that arrived later in the file wins.
    bool hasHigherPriorityThan(const Email& other) const {
        if (categoryRank_ != other.categoryRank_) {
            return categoryRank_ > other.categoryRank_;
        }
        if (dateKey_ != other.dateKey_) {
            return dateKey_ > other.dateKey_;
        }
        return arrivalOrder_ > other.arrivalOrder_;
    }

    void display(std::ostream& out) const {
        out << "Sender: " << sender_ << "\n"
            << "Subject: " << subject_ << "\n"
            << "Date: " << date_ << "\n";
    }

private:
    std::string sender_;
    std::string subject_;
    std::string date_;
    int categoryRank_;
    long dateKey_;
    long arrivalOrder_;

    static int rankForCategory(const std::string& category) {
        if (category == "Boss")            return 5;
        if (category == "Subordinate")     return 4;
        if (category == "Peer")            return 3;
        if (category == "ImportantPerson") return 2;
        if (category == "OtherPerson")     return 1;
        return 0;  // unknown categories sort to the very end
    }

    // Converts MM-DD-YYYY into YYYYMMDD so dates compare as plain numbers.
    static long keyForDate(const std::string& date) {
        if (date.size() < 10) {
            return 0;
        }
        try {
            long month = std::stol(date.substr(0, 2));
            long day   = std::stol(date.substr(3, 2));
            long year  = std::stol(date.substr(6, 4));
            return year * 10000 + month * 100 + day;
        } catch (const std::exception&) {
            return 0;
        }
    }
};

// ---------------------------------------------------------------------------
// MaxHeap: list-based (array-backed) binary max-heap of Emails.
// For index i: parent = (i-1)/2, left child = 2i+1, right child = 2i+2.
// ---------------------------------------------------------------------------
class MaxHeap {
public:
    void insert(const Email& email) {
        items_.push_back(email);
        siftUp(items_.size() - 1);
    }

    const Email& peekMax() const {
        if (isEmpty()) {
            throw std::out_of_range("peekMax called on an empty heap");
        }
        return items_[0];
    }

    void removeMax() {
        if (isEmpty()) {
            return;
        }
        items_[0] = items_.back();
        items_.pop_back();
        if (!isEmpty()) {
            siftDown(0);
        }
    }

    std::size_t size() const { return items_.size(); }
    bool isEmpty() const { return items_.empty(); }

private:
    std::vector<Email> items_;

    static std::size_t parentOf(std::size_t i) { return (i - 1) / 2; }
    static std::size_t leftOf(std::size_t i)   { return 2 * i + 1; }
    static std::size_t rightOf(std::size_t i)  { return 2 * i + 2; }

    void siftUp(std::size_t index) {
        while (index > 0) {
            std::size_t parent = parentOf(index);
            if (!items_[index].hasHigherPriorityThan(items_[parent])) {
                break;
            }
            swapItems(index, parent);
            index = parent;
        }
    }

    void siftDown(std::size_t index) {
        const std::size_t count = items_.size();
        while (true) {
            std::size_t left = leftOf(index);
            std::size_t right = rightOf(index);
            std::size_t best = index;

            if (left < count && items_[left].hasHigherPriorityThan(items_[best])) {
                best = left;
            }
            if (right < count && items_[right].hasHigherPriorityThan(items_[best])) {
                best = right;
            }
            if (best == index) {
                break;
            }
            swapItems(index, best);
            index = best;
        }
    }

    void swapItems(std::size_t a, std::size_t b) {
        Email temp = items_[a];
        items_[a] = items_[b];
        items_[b] = temp;
    }
};

// ---------------------------------------------------------------------------
// CEOInbox: the priority queue of unread emails, built on the MaxHeap.
// ---------------------------------------------------------------------------
class CEOInbox {
public:
    void receive(const Email& email) { heap_.insert(email); }

    // Shows the top email without removing it, so repeated NEXTs match.
    void showNext(std::ostream& out) const {
        if (heap_.isEmpty()) {
            out << "No emails to read.\n";
            return;
        }
        out << "Next email:\n";
        heap_.peekMax().display(out);
    }

    // Removes the top email whether or not it was displayed first.
    void markRead() { heap_.removeMax(); }

    void showCount(std::ostream& out) const {
        out << "There are " << heap_.size() << " emails to read.\n";
    }

private:
    MaxHeap heap_;
};

// ---------------------------------------------------------------------------
// CommandProcessor: reads the test file and drives the inbox.
// ---------------------------------------------------------------------------
class CommandProcessor {
public:
    CommandProcessor(CEOInbox& inbox, std::ostream& out)
        : inbox_(inbox), out_(out), arrivalCounter_(0), lastOutputWasNext_(false) {}

    void run(std::istream& in) {
        std::string line;
        while (std::getline(in, line)) {
            line = trim(line);
            if (!line.empty()) {
                process(line);
            }
        }
    }

private:
    CEOInbox& inbox_;
    std::ostream& out_;
    long arrivalCounter_;
    bool lastOutputWasNext_;  // keeps blank-line spacing matching the sample

    void process(const std::string& line) {
        const std::string emailPrefix = "EMAIL ";
        if (line.compare(0, emailPrefix.size(), emailPrefix) == 0) {
            handleEmail(line.substr(emailPrefix.size()));
        } else if (line == "NEXT") {
            inbox_.showNext(out_);
            lastOutputWasNext_ = true;
        } else if (line == "READ") {
            inbox_.markRead();
        } else if (line == "COUNT") {
            if (lastOutputWasNext_) {
                out_ << "\n";
            }
            inbox_.showCount(out_);
            out_ << "\n";
            lastOutputWasNext_ = false;
        } else {
            std::cerr << "Unrecognized command: " << line << "\n";
        }
    }

    // Parses "<sender category>, <subject line>, <date>".
    void handleEmail(const std::string& fields) {
        std::vector<std::string> parts;
        std::stringstream stream(fields);
        std::string piece;
        while (std::getline(stream, piece, ',')) {
            parts.push_back(trim(piece));
        }
        if (parts.size() != 3) {
            std::cerr << "Skipping malformed EMAIL line: " << fields << "\n";
            return;
        }
        inbox_.receive(Email(parts[0], parts[1], parts[2], arrivalCounter_++));
    }

    static std::string trim(const std::string& text) {
        const std::string whitespace = " \t\r\n";
        std::size_t start = text.find_first_not_of(whitespace);
        if (start == std::string::npos) {
            return "";
        }
        std::size_t end = text.find_last_not_of(whitespace);
        return text.substr(start, end - start + 1);
    }
};

// ---------------------------------------------------------------------------
// Entry point: wire up the objects and hand them the input stream.
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    CEOInbox inbox;
    CommandProcessor processor(inbox, std::cout);

    if (argc > 1) {
        std::ifstream file(argv[1]);
        if (!file) {
            std::cerr << "Could not open file: " << argv[1] << "\n";
            return 1;
        }
        processor.run(file);
    } else {
        processor.run(std::cin);
    }
    return 0;
}