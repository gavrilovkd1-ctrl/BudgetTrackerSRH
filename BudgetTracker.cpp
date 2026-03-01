#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <chrono>
#include <limits>
using namespace std;

// -----------------------------------------------------------
// Global file paths and state variables used throughout the
// application. These correspond to the CSV files where budget
// data, technical metadata, and recurring items are stored.
// -----------------------------------------------------------
string BFile = "budget.csv";      // main transaction log
string TFile = "technical.csv";   // stores balance, date, goal
string RFile = "recurring.csv";   // recurring income/expenses

double balance = 0.0;              // running account balance
int date = 0;                      // current day number
double goal = 0.0;                 // user-set goal for balance

// -----------------------------------------------------------
// Text formatting helpers
// -----------------------------------------------------------
// Return a string wrapped in ANSI escape codes to render green
// (useful for highlighting success messages in the console).
string green(string input){
    return "\033[32m" + input + "\033[0m";
}

// Similar to green(), but renders the text in red (error/warning).
string red(string input){
    return "\033[31m" + input + "\033[0m";
}

// -----------------------------------------------------------
// Technical data functions
// -----------------------------------------------------------
// StartUp() reads the metadata from technical.csv, updates the
// global balance, date, and goal variables, and applies any
// recurring transactions that are due on the new date.
void StartUp(){
    ifstream tech(TFile);
    string line, stb, std, stg; // accents for string parts
    getline(tech, line);            // skip header
    getline(tech, line);            // read second line that contains data
    stringstream ss(line);
    getline(ss, stb, ',');          // balance
    getline(ss, std, ',');          // date
    getline(ss, stg, ',');          // goal
    balance = stod(stb);
    date = stoi(std)+1; // increment the stored date each startup
    goal = stod(stg);
    tech.close();

    // Apply any recurring transactions whose date falls on the
    // current day (date == stored + 30). This simulates a monthly
    // recurrence schedule.
    ifstream recur(RFile);
    string myDate, amount;
    getline(recur, line);           // skip header
    while (getline(recur, line)){
        stringstream ss(line);
        getline(ss, myDate, ',');
        getline(ss, amount, ',');
        if (stoi(myDate)+30 == date){
            balance += stod(amount);
        }
    }
    recur.close();
}

// SaveTech() writes the current technical state back to the
// technical.csv file so it can be reloaded on the next run.
void SaveTech(){
    ofstream tech("technical.csv");
    tech << "balance,date,goal\n";
    tech << balance << ',' << date << ',' << goal;
    tech.close();
}

// -----------------------------------------------------------
// Input helper functions
// -----------------------------------------------------------

// YesNoInput prompts the user for a Y/N answer, repeating until
// valid input is received. It clears any stray input from the
// stream before reading.
string YesNoInput(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string input;
    cin >> input;
    if (!((input == "Y")||(input == "N"))){
        cout << "Please enter " << "Y" << " or " << "N" << ": \n";
        YesNoInput();
    }
    return input;
}

// ExpenseInput allows the user to type one of a fixed set of
// categories for an expense. It loops until a valid category is
// entered, case-insensitive between the two variants we allow.
string ExpenseInput(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string allowedInput[10] = {"Rent", "rent", "Clothing", "clothing", "Groceries", "groceries", "Cafes", "cafes", "Other", "other"};
    string input;
    cin >> input;
    for (string item : allowedInput){
        if (input == item) {
            return input;
            break;
        }
    }
    cout << "Please, enter one of these categories:\n";
    for (int i = 0; i<10; i += 2){
        cout << "-" << allowedInput[i] << endl;
    }
    cout << "Your input: ";
    return ExpenseInput();
}

// IncomeInput does the same as ExpenseInput but for income
// categories.
string IncomeInput(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string allowedInput[10] = {"Salary", "salary", "Loans", "loans", "Business", "business", "Deposits", "deposits", "Other", "other"};
    string input;
    cin >> input;
    for (string item : allowedInput){
        if (input == item) {
            return input;
            break;
        }
    }
    cout << "Please, enter one of these categories:\n";
    for (int i = 0; i<10; i += 2){
        cout << "-" << allowedInput[i] << endl;
    }
    cout << "Your input: ";
    return IncomeInput();
}

// ValidatedInput reads a non-negative double from the console
// and repeats until the user types a valid number.
double ValidatedInput(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    double input;
    cin >> input;
    if (cin.fail()){
        cout << "Enter a number: ";
        return ValidatedInput();
    }
    if (input<0){
        cout << "Enter a positive number: ";
        return ValidatedInput();
    }
    return input;
}

// UserChoiceInput safely reads an integer choice from the user
// (used in the main menu) and discards invalid input.
int UserChoiceInput(){
    int UserChoice;
    bool err;
    do{
        err = false;
        cin >> UserChoice;
        if (cin.fail()) {
            err = true;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (err);
    return UserChoice;
}

// DateInput collects a start and end day from the user for a
// statement and returns them as a pair. It ensures numeric input.
pair<int, int> DateInput(){
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string start, end;
    cout << "Enter the starting date of the statement period: ";
    cin >> start;
    cout << "Enter the end of the statement period: ";
    cin >> end;
    try {
        int d1 = stoi(start);
        int d2 = stoi(end);
        return {d1, d2};
    }
    catch (...){
        //invalid input case
    }
    cout << "\nInvalid input. Please, enter the dates as numbers. \n";
    DateInput();
}

// -----------------------------------------------------------
// Expense and Income structures
// -----------------------------------------------------------

// Expense represents an expenditure with amount, category, and
// optional recurrence or notes. It bundles up methods to record
// itself in the budget files and handle user confirmation.
struct Expense {
    double amount;
    string category;
    bool isRecurring = false;
    string notes = "";

    // WriteBudget appends this expense to the budget CSV and adds
    // an entry to the recurring file if applicable.
    int WriteBudget() {
        ofstream budget(BFile, ios::app);
        budget << date << "," << "expense" << "," << amount << "," << category << "," << isRecurring << "," << balance << "\n";
        budget.close();
        if (isRecurring){
            ofstream recur(RFile);
            recur << date << "," << "expense" << "," << -amount;
            recur.close();
        }
        return 0;
    }

    // ExpHandling updates the global balance and writes to file,
    // prompting the user if funds are insufficient.
    int ExpHandling(){
        if (balance>=amount){
            balance -= amount;
            WriteBudget();
            return 1;
        }
        cout << "Looks like you might not have enough money. Do you still want to add this expense? (Y/N)\n";
        if (YesNoInput() == "Y") {
            balance -= amount; 
            WriteBudget();
            return 1;
        }
        else {
            cout << "Expense cancelled\n";
            return 0;
        }
    }

    // Default constructor interacts with the user to populate the
    // fields and then calls ExpHandling to persist the entry.
    Expense(){
        string temp;
        bool flag;
        do{
            flag = true;
            cout << "How much did you spend?\n";
            amount = ValidatedInput();
            cout << "What was the category of the expenditure?\n";
            category = ExpenseInput();
            cout << "Is your expense monthly recurring? (Y/N)\n";
            if (YesNoInput() == "Y") isRecurring = true;
            cout << "Confirm the new expense of $" << amount<< " on " << category << ". (Y/N)\n";
            if (!(YesNoInput() == "Y")) {
                flag = false; 
                cout << "Then let's try again!\n";
            }
        } while (!(flag));
        cout << "Do you want to add any note? If not, input '0'.\nYour note: ";
        cin >> notes;
        ExpHandling();
    }

    // Convenience constructor used by ReadBudgetE for parsing records.
    Expense(int am, string cat, bool rec = false){
        amount = am;
        category = cat;
        isRecurring = rec;
    }
};

// Income is structurally similar to Expense but for incoming funds.
struct Income {
    double amount;
    string category;
    bool isRecurring = false;
    string notes = "";

    // Append this income to the budget file and recurring file if
    // needed.
    int WriteBudget() {
        ofstream budget(BFile, ios::app);
        budget << date << "," << "income" << "," << amount << "," << category << "," << isRecurring << "," << balance << "\n";
        budget.close();
        if (isRecurring){
            ofstream recur(RFile);
            recur << date << "," << "income" << "," << amount;
            recur.close();
        }
        return 0;
    }

    // IncHandling simply changes the balance and writes the record.
    int IncHandling() {
        balance += amount;
        WriteBudget();
        return 1;
    }

    // Interactive constructor for a new income record.
    Income(){
        string temp;
        bool flag;
        do{
            flag = true;
            cout << "How much did you earn?\n";
            amount = ValidatedInput();
            cout << "What was the source of the income?\n";
            category = IncomeInput();
            cout << "Is your income monthly recurring? (Y/N)\n";
            if (YesNoInput() == "Y") isRecurring = true;
            cout << "Confirm the new income of $" << amount<< " from " << category << ". (Y/N)\n";
            if (!(YesNoInput() == "Y")) {
                flag = false; 
                cout << "Then let's try again!\n";
            }
        } while (!(flag));
        cout << "Do you want to add any note? If not, input '0'.\nYour note: ";
        cin >> notes;
        IncHandling();
    }
};

// -----------------------------------------------------------
// File-reading utilities that return the most recent income or
// expense record and update the balance to the last read value.
// These are used for simple lookups rather than full traversals.
// -----------------------------------------------------------
Income ReadBudgetI() {
    ifstream budget(BFile);
    string line;
    string date, type, amount, category, isRecurring, myBalance;
    getline(budget, line); // header
    while (getline(budget, line)){
        stringstream ss(line);
        getline(ss, date, ',');
        getline(ss, type, ',');
        getline(ss, amount, ',');
        getline(ss, category, ',');
        getline(ss, isRecurring, ',');
        getline(ss, myBalance, ',');
        if (type == "income") {
            Income record;
            record.amount = stod(amount);
            record.category = category;
            record.isRecurring = (isRecurring == "1") ? true : false;
            balance = stod(myBalance);
            return record;
        }
        else{
            Income record;
            record.amount = 0;
            record.category = "0";
            record.isRecurring = false;
            balance = stod(myBalance);
            return record;
        }
    }
    budget.close();
}

Expense ReadBudgetE() {
    ifstream budget(BFile);
    string line;
    string myDate, type, amount, category, isRecurring, myBalance;
    getline(budget, line);
    while (getline(budget, line)){
        stringstream ss(line);
        getline(ss, myDate, ',');
        getline(ss, type, ',');
        getline(ss, amount, ',');
        getline(ss, category, ',');
        getline(ss, isRecurring, ',');
        getline(ss, myBalance, ',');
        if (type == "expense") {
            Expense record;
            record.amount = stod(amount);
            record.category = category;
            record.isRecurring = (isRecurring == "1") ? true : false;
            balance = stod(myBalance);
            return record;
        }
    }
    budget.close();
}

// -----------------------------------------------------------
// Convenience functions to reset the data files; used by the
// hidden debug menu (option 101) to clear all records.
// -----------------------------------------------------------
int clearBudget(){
    ofstream budget(BFile);
    budget << "date,type,amount,category,SEPA,balance\n";
    budget.close();
    return 1;
}

int clearRecur(){
    ofstream recur(RFile);
    recur << "date,type,amount\n";
    recur.close();
    return 1;
}

int clearTech(){
    ofstream tech(TFile);
    tech << "date,balance,goal\n";
    tech << "0,0.0,0.0\n";
    tech.close();
    return 1;
}

// Show the current global balance/date/goal information to the user.
void showBalance(){
    cout << "==============================\n";
    cout << "It is day " << date << " today.\n";
    cout << "Your current balance is: $" << balance << ".\n";
    cout << "Your goal is: $" << goal << ".\n";
    cout << "==============================\n";
}

// Read and display the recurring transactions file with a simple
// calculation of days until next payment.
void showRecurring(){
    ifstream recur(RFile);
    string line;
    string myDate, type, amount;
    double local_balance = 0.0;
    cout << "Your recurring payments: \n";
    cout << "==============================\n";
    cout << "DATE | TYPE | AMOUNT | DAYS TO PAYMENT\n";
    getline(recur, line);
    while (getline(recur, line)){
        stringstream ss(line);
        getline(ss, myDate, ',');
        getline(ss, type, ',');
        getline(ss, amount, ',');
        int days_to_payment = (date/30)*30+(stoi(myDate)%30);
        local_balance += stod(amount);
        cout << "Day " << myDate << " | " << type << " | " << amount << " | " << days_to_payment << endl;
    }
    cout << "==============================\n";
    cout << "Your recurring payments balance is $" << local_balance << endl;
    recur.close();
}

// getStatement prints all transactions within a user-specified
// range and computes the local change in balance over that period.
void getStatement(){
    auto desired_date = DateInput();
    int desired_start = desired_date.first;
    int desired_end = desired_date.second;
    cout << "==============================\n";
    cout << "DATE | TYPE | AMOUNT | CATEGORY | BALANCE\n";
    double local_balance = 0.0;
    ifstream budget(BFile);
    string line;
    string myDate, type, amount, category, isRecurring, myBalance;
    getline(budget, line);
    while (getline(budget, line)){
        stringstream ss(line);
        getline(ss, myDate, ',');
        getline(ss, type, ',');
        getline(ss, amount, ',');
        getline(ss, category, ',');
        getline(ss, isRecurring, ',');
        getline(ss, myBalance, ',');
        if (stoi(myDate) >= desired_start && stoi(myDate) <= desired_end){
            if (type == "income") {
                local_balance += stod(amount);
            }
            else{
                local_balance -= stod(amount);
            }
            cout << "Day " << myDate << " | " << type << " | " << amount << " | " << category << " | " << myBalance << endl;
        }
    }
    cout << "==============================\n";
    cout << "Local change of balance is $" << local_balance << endl;
    budget.close();
}

// -----------------------------------------------------------
// Main user interface and program loop
// -----------------------------------------------------------
int main()
{
    StartUp();              // load technical state
    cout << "Hello!\n";
    showBalance();
    showRecurring();
    cout << "Enter a new budget goal: ";
    cin >> goal;
    do {
        system("CLS");
        cout << "Choose an option:\n1. Add expense\n2. Add income\n3. Show balance\n4. Get a budget statement\n5. Show recurring payments\n6. Exit\n";
        switch (UserChoiceInput()){
            case 1:
                system("CLS"); 
                Expense();          // interactively add expense
                system("pause");
                break;
            case 2:
                system("CLS");
                Income();           // interactively add income
                system("pause");
                break;
            case 3:
                system("CLS");
                showBalance();
                system("pause");
                break;
            case 4: 
                system("CLS");
                getStatement();
                system("pause");
                break;
            case 5: 
                system("CLS");
                showRecurring();
                system("pause");
                break;
            case 6:
                system("CLS");
                SaveTech();
                exit(0);
            case 101:
                // hidden reset command for development/testing
                clearRecur();
                clearBudget();
                clearTech();
                exit(0);
            default: 
                system("CLS");
                continue;
        }
    } while (true);
}
