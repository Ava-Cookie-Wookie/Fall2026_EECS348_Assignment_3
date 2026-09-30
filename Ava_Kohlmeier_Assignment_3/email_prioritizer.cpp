/* ============================================================================
 * Program Name : EECS 348 Assignment 3 - CEO Email Priority Queue
 * ----------------------------------------------------------------------------
 * Description  :
 *   C++ program that prioritizes emails for a busy company CEO.
 *   Emails are stored in a MaxHeap that is built from scratch on top of a
 *   list (std::vector). No pre-existing heap modules are used.
 *   The MaxHeap acts as a priority queue that decides which email the CEO
 *   should read next.
 *
 *   Priority order by sender category:
 *       Boss > Subordinate > Peer > ImportantPerson > OtherPerson
 *   Within the same sender category, the newest email is read first.
 *
 * Inputs       :
 *   A test file, given as a command-line argument (./email_prioritizer
 *   test.txt) or redirected into standard input (./email_prioritizer <
 *   test.txt). Each line holds one command:
 *       EMAIL <sender category>,<subject line>,<date MM-DD-YYYY>
 *       NEXT
 *       READ
 *       COUNT
 *
 * Output       :
 *   Terminal output in the format shown in the assignment:
 *       - COUNT prints "There are N emails to read."
 *       - NEXT prints "Next email:" followed by the indented Sender,
 *         Subject, and Date lines.
 *       - Each COUNT and NEXT output is followed by a blank line.
 *       - READ prints nothing.
 *
 * Collaborators           : None
 * Other sources for code  : Claude (Anthropic) - generated the original
 *                           program from the assignment prompt, and assisted
 *                           with the revisions listed below, debugging, and
 *                           explanations.
 * Author's full name      : Ava Kohlmeier
 * Creation date           : September 30, 2026
 * Revision date           : September 30, 2026
 * Revisions               :
 *   1. Changed output to match the assignment's expected format: a blank
 *      line after every COUNT and NEXT output, and indented Sender,
 *      Subject, and Date lines.
 *   2. NEXT on an empty inbox now prints nothing, instead of an extra
 *      message that the assignment does not ask for.
 *   3. Removed the extra blank-line tracking flag, since every output now
 *      follows the same simple spacing rule.
 *   4. Added the prologue, block comments, and line-by-line comments.
 * ==========================================================================*/

#include <fstream>     // Needed for std::ifstream to open the test file
#include <iostream>    // Needed for std::cout, std::cin, and std::cerr
#include <sstream>     // Needed for std::stringstream to split EMAIL fields
#include <stdexcept>   // Needed for std::exception and std::out_of_range
#include <string>      // Needed for std::string
#include <vector>      // Needed for std::vector, the list behind the MaxHeap

/* ----------------------------------------------------------------------------
 * Class: Email
 * Purpose: Holds one email and the rules for comparing two emails'
 *          priority. The category rank and date number are worked out once,
 *          when the email is created, so comparisons only compare numbers.
 * Author: Claude (Anthropic) - GenAI generated. Display format modified by
 *         Ava Kohlmeier with assistance from Claude.
 * --------------------------------------------------------------------------*/
class Email {                                                   // Start of the Email class
public:                                                         // Members below can be used outside the class
    Email(const std::string& sender, const std::string& subject, // Constructor takes the sender and subject,
          const std::string& date, long arrivalOrder)           // the date, and the order the email arrived in
        : sender_(sender),                                      // Store the sender category text
          subject_(subject),                                    // Store the subject line text
          date_(date),                                          // Store the date text exactly as given
          categoryRank_(rankForCategory(sender)),               // Convert the sender category to a number once
          dateKey_(keyForDate(date)),                           // Convert the date to a YYYYMMDD number once
          arrivalOrder_(arrivalOrder) {}                        // Store the arrival position; body is empty

    // Returns true if this email should be read before the other email.
    bool hasHigherPriorityThan(const Email& other) const {      // Compare this email against another one
        if (categoryRank_ != other.categoryRank_) {             // If the sender categories are different
            return categoryRank_ > other.categoryRank_;         // the higher-ranked category wins
        }                                                       // End of category check
        if (dateKey_ != other.dateKey_) {                       // If the categories match but the dates differ
            return dateKey_ > other.dateKey_;                   // the newer date wins
        }                                                       // End of date check
        return arrivalOrder_ > other.arrivalOrder_;             // Exact tie: the email later in the file wins
    }                                                           // End of hasHigherPriorityThan

    // Prints the email's details indented, in the assignment's format.
    void display(std::ostream& out) const {                     // Print this email to the given output stream
        out << "\tSender: " << sender_ << "\n"                  // Print the indented sender line
            << "\tSubject: " << subject_ << "\n"                // Print the indented subject line
            << "\tDate: " << date_ << "\n";                     // Print the indented date line
    }                                                           // End of display

private:                                                        // Members below are only used inside the class
    std::string sender_;                                        // Sender category, e.g. "Boss"
    std::string subject_;                                       // Subject line text
    std::string date_;                                          // Date text in MM-DD-YYYY format
    int categoryRank_;                                          // Sender category as a number (higher = more important)
    long dateKey_;                                              // Date as a YYYYMMDD number for easy comparison
    long arrivalOrder_;                                         // Position of this email in the test file

    // Turns a sender category into a number. Higher number = read sooner.
    static int rankForCategory(const std::string& category) {   // Look up the rank for a category name
        if (category == "Boss")            return 5;            // Boss is read first
        if (category == "Subordinate")     return 4;            // Subordinate is read next
        if (category == "Peer")            return 3;            // Peer is read next
        if (category == "ImportantPerson") return 2;            // ImportantPerson is read next
        if (category == "OtherPerson")     return 1;            // OtherPerson is read last
        return 0;                                               // Any unknown category goes after all others
    }                                                           // End of rankForCategory

    // Converts MM-DD-YYYY into YYYYMMDD so dates compare as plain numbers.
    static long keyForDate(const std::string& date) {           // Build a comparable number from the date text
        if (date.size() < 10) {                                 // If the date is too short to be MM-DD-YYYY
            return 0;                                           // treat it as the oldest possible date
        }                                                       // End of length check
        try {                                                   // Try the conversion; bad text throws an error
            long month = std::stol(date.substr(0, 2));          // Characters 0-1 are the month
            long day   = std::stol(date.substr(3, 2));          // Characters 3-4 are the day
            long year  = std::stol(date.substr(6, 4));          // Characters 6-9 are the year
            return year * 10000 + month * 100 + day;            // Combine into YYYYMMDD, e.g. 20250103
        } catch (const std::exception&) {                       // If any part was not a number
            return 0;                                           // treat it as the oldest date instead of crashing
        }                                                       // End of try/catch
    }                                                           // End of keyForDate
};                                                              // End of the Email class

/* ----------------------------------------------------------------------------
 * Class: MaxHeap
 * Purpose: A list-based binary MaxHeap of Emails, built from scratch.
 *          The highest-priority email is always at index 0.
 *          For any index i: parent = (i-1)/2, left child = 2i+1,
 *          right child = 2i+2.
 * Author: Claude (Anthropic) - GenAI generated. Unchanged except for comments.
 * --------------------------------------------------------------------------*/
class MaxHeap {                                                 // Start of the MaxHeap class
public:                                                         // Members below can be used outside the class
    // Adds an email to the heap and moves it up to its correct spot.
    void insert(const Email& email) {                           // Insert a new email
        items_.push_back(email);                                // Put the new email at the end of the list
        siftUp(items_.size() - 1);                              // Move it up until the heap order is correct
    }                                                           // End of insert

    // Returns the highest-priority email without removing it.
    const Email& peekMax() const {                              // Look at the top email
        if (isEmpty()) {                                        // If there are no emails
            throw std::out_of_range("peekMax called on an empty heap"); // report the error instead of reading bad memory
        }                                                       // End of empty check
        return items_[0];                                       // The top email is always at index 0
    }                                                           // End of peekMax

    // Removes the highest-priority email. Does nothing if the heap is empty.
    void removeMax() {                                          // Remove the top email
        if (isEmpty()) {                                        // If there are no emails
            return;                                             // there is nothing to remove
        }                                                       // End of empty check
        items_[0] = items_.back();                              // Move the last email into the top spot
        items_.pop_back();                                      // Remove the now-duplicate last slot
        if (!isEmpty()) {                                       // If any emails are left
            siftDown(0);                                        // move the new top down to its correct spot
        }                                                       // End of remaining-emails check
    }                                                           // End of removeMax

    std::size_t size() const { return items_.size(); }          // Returns how many emails are in the heap
    bool isEmpty() const { return items_.empty(); }             // Returns true if the heap has no emails

private:                                                        // Members below are only used inside the class
    std::vector<Email> items_;                                  // The list that stores the heap's emails

    static std::size_t parentOf(std::size_t i) { return (i - 1) / 2; } // Index of a node's parent
    static std::size_t leftOf(std::size_t i)   { return 2 * i + 1; }   // Index of a node's left child
    static std::size_t rightOf(std::size_t i)  { return 2 * i + 2; }   // Index of a node's right child

    // Moves an email up while it has higher priority than its parent.
    void siftUp(std::size_t index) {                            // Start at the given index
        while (index > 0) {                                     // Keep going until the email reaches the top
            std::size_t parent = parentOf(index);               // Find this email's parent
            if (!items_[index].hasHigherPriorityThan(items_[parent])) { // If it does not beat its parent
                break;                                          // it is already in the right spot
            }                                                   // End of priority check
            swapItems(index, parent);                           // Otherwise swap it with its parent
            index = parent;                                     // and continue from the parent's position
        }                                                       // End of while loop
    }                                                           // End of siftUp

    // Moves an email down while one of its children has higher priority.
    void siftDown(std::size_t index) {                          // Start at the given index
        const std::size_t count = items_.size();                // Number of emails in the heap
        while (true) {                                          // Repeat until the email settles
            std::size_t left = leftOf(index);                   // Index of the left child
            std::size_t right = rightOf(index);                 // Index of the right child
            std::size_t best = index;                           // Assume the current email is the highest

            if (left < count && items_[left].hasHigherPriorityThan(items_[best])) { // If the left child exists and beats it
                best = left;                                    // the left child is now the highest
            }                                                   // End of left-child check
            if (right < count && items_[right].hasHigherPriorityThan(items_[best])) { // If the right child exists and beats it
                best = right;                                   // the right child is now the highest
            }                                                   // End of right-child check
            if (best == index) {                                // If neither child beat the current email
                break;                                          // it is in the right spot
            }                                                   // End of settled check
            swapItems(index, best);                             // Otherwise swap with the higher child
            index = best;                                       // and continue from that child's position
        }                                                       // End of while loop
    }                                                           // End of siftDown

    // Swaps two emails in the list.
    void swapItems(std::size_t a, std::size_t b) {              // Swap the emails at positions a and b
        Email temp = items_[a];                                 // Save a copy of the first email
        items_[a] = items_[b];                                  // Put the second email in the first spot
        items_[b] = temp;                                       // Put the saved first email in the second spot
    }                                                           // End of swapItems
};                                                              // End of the MaxHeap class

/* ----------------------------------------------------------------------------
 * Class: CEOInbox
 * Purpose: The CEO's priority queue of unread emails, built on the MaxHeap.
 *          Each method carries out one command.
 * Author: Claude (Anthropic) - GenAI generated. Output format and empty-NEXT
 *         behavior modified by Ava Kohlmeier with assistance from Claude.
 * --------------------------------------------------------------------------*/
class CEOInbox {                                                // Start of the CEOInbox class
public:                                                         // Members below can be used outside the class
    // EMAIL: adds a new email to the inbox.
    void receive(const Email& email) { heap_.insert(email); }   // Insert the email into the heap

    // NEXT: shows the top email without removing it, so two NEXTs in a
    // row display the same email. Prints nothing if the inbox is empty.
    void showNext(std::ostream& out) const {                    // Display the next email to read
        if (heap_.isEmpty()) {                                  // If there are no emails
            return;                                             // print nothing
        }                                                       // End of empty check
        out << "Next email:\n";                                 // Print the heading line
        heap_.peekMax().display(out);                           // Print the top email's indented details
        out << "\n";                                            // Print the blank line after the output
    }                                                           // End of showNext

    // READ: removes the top email, whether or not NEXT displayed it.
    void markRead() { heap_.removeMax(); }                      // Remove the highest-priority email

    // COUNT: shows how many unread emails are left.
    void showCount(std::ostream& out) const {                   // Display the unread count
        out << "There are " << heap_.size() << " emails to read.\n"; // Print the count line
        out << "\n";                                            // Print the blank line after the output
    }                                                           // End of showCount

private:                                                        // Members below are only used inside the class
    MaxHeap heap_;                                              // The MaxHeap that holds the unread emails
};                                                              // End of the CEOInbox class

/* ----------------------------------------------------------------------------
 * Class: CommandProcessor
 * Purpose: Reads the test file line by line and sends each command to the
 *          CEOInbox.
 * Author: Claude (Anthropic) - GenAI generated. Blank-line tracking removed
 *         by Ava Kohlmeier with assistance from Claude.
 * --------------------------------------------------------------------------*/
class CommandProcessor {                                        // Start of the CommandProcessor class
public:                                                         // Members below can be used outside the class
    CommandProcessor(CEOInbox& inbox, std::ostream& out)        // Constructor takes the inbox and output stream
        : inbox_(inbox), out_(out), arrivalCounter_(0) {}       // Store them and start the arrival count at 0

    // Reads every line from the input and processes each command.
    void run(std::istream& in) {                                // Process a whole input stream
        std::string line;                                       // Holds one line of input at a time
        while (std::getline(in, line)) {                        // Read lines until the end of the input
            line = trim(line);                                  // Remove extra spaces and hidden characters like \r
            if (!line.empty()) {                                // Skip blank lines
                process(line);                                  // Handle the command on this line
            }                                                   // End of blank-line check
        }                                                       // End of while loop
    }                                                           // End of run

private:                                                        // Members below are only used inside the class
    CEOInbox& inbox_;                                           // The inbox the commands act on
    std::ostream& out_;                                         // Where output is printed (the terminal)
    long arrivalCounter_;                                       // Counts emails in the order they arrive

    // Decides which command a line holds and carries it out.
    void process(const std::string& line) {                     // Handle one command line
        const std::string emailPrefix = "EMAIL ";               // Every EMAIL command starts with "EMAIL "
        if (line.compare(0, emailPrefix.size(), emailPrefix) == 0) { // If the line starts with "EMAIL "
            handleEmail(line.substr(emailPrefix.size()));       // parse the fields after the prefix
        } else if (line == "NEXT") {                            // If the command is NEXT
            inbox_.showNext(out_);                              // show the next email
        } else if (line == "READ") {                            // If the command is READ
            inbox_.markRead();                                  // remove the top email
        } else if (line == "COUNT") {                           // If the command is COUNT
            inbox_.showCount(out_);                             // show the unread count
        } else {                                                // Any other text is not a valid command
            std::cerr << "Unrecognized command: " << line << "\n"; // Warn on the error stream, not the normal output
        }                                                       // End of command checks
    }                                                           // End of process

    // Splits "<sender category>,<subject line>,<date>" into three fields.
    void handleEmail(const std::string& fields) {               // Parse and add one email
        std::vector<std::string> parts;                         // Holds the separated fields
        std::stringstream stream(fields);                       // Lets the fields be read piece by piece
        std::string piece;                                      // Holds one field at a time
        while (std::getline(stream, piece, ',')) {              // Read up to each comma
            parts.push_back(trim(piece));                       // Save the field with extra spaces removed
        }                                                       // End of while loop
        if (parts.size() != 3) {                                // A valid EMAIL line has exactly three fields
            std::cerr << "Skipping malformed EMAIL line: " << fields << "\n"; // Warn and skip bad lines
            return;                                             // Do not add a broken email
        }                                                       // End of field-count check
        inbox_.receive(Email(parts[0], parts[1], parts[2], arrivalCounter_++)); // Create the email and add it to the inbox
    }                                                           // End of handleEmail

    // Removes spaces, tabs, and line-ending characters from both ends.
    static std::string trim(const std::string& text) {          // Return a cleaned-up copy of the text
        const std::string whitespace = " \t\r\n";               // Characters to strip
        std::size_t start = text.find_first_not_of(whitespace); // Find the first real character
        if (start == std::string::npos) {                       // If the text is only whitespace
            return "";                                          // return an empty string
        }                                                       // End of all-whitespace check
        std::size_t end = text.find_last_not_of(whitespace);    // Find the last real character
        return text.substr(start, end - start + 1);             // Return everything between them
    }                                                           // End of trim
};                                                              // End of the CommandProcessor class

/* ----------------------------------------------------------------------------
 * Function: main
 * Purpose: Creates the objects and gives them the input. Uses the file named
 *          on the command line if there is one, or standard input if not.
 * Author: Claude (Anthropic) - GenAI generated. Unchanged except for comments.
 * --------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {                              // Program starts here
    CEOInbox inbox;                                             // Create the CEO's empty inbox
    CommandProcessor processor(inbox, std::cout);               // Create the processor that prints to the terminal

    if (argc > 1) {                                             // If a file name was given on the command line
        std::ifstream file(argv[1]);                            // try to open that file
        if (!file) {                                            // If the file could not be opened
            std::cerr << "Could not open file: " << argv[1] << "\n"; // report the problem
            return 1;                                           // and end the program with an error code
        }                                                       // End of file-open check
        processor.run(file);                                    // Process every command in the file
    } else {                                                    // If no file name was given
        processor.run(std::cin);                                // read the commands from standard input
    }                                                           // End of input-source check
    return 0;                                                   // End the program successfully
}                                                               // End of main