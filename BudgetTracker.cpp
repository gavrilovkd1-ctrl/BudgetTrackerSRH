#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <chrono>
#include <limits>
using namespace std;

string BFile = "budget.csv";
string TFile = "technical.csv";
string RFile = "recurring.csv";
double balance = 0.0;
int date = 0;
double goal = 0.0;

//text formatting functions
string green(string input){
    return "\033[32m" + input + "\033[0m";
}

string red(string input){
    return "\033[31m" + input + "\033[0m";
}

//technical data functions
void StartUp(){
    ifstream tech(TFile);
    string line, stb, std, stg; // stb = string balance, std = string data, stg = string goal
    getline(tech, line);
    getline(tech, line);
    stringstream ss(line);
    getline(ss, stb, ',');
    getline(ss, std, ',');
    getline(ss, stg, ',');
    balance = stod(stb);
    date = stoi(std)+1; //date incremented
    goal = stod(stg);
    tech.close();

    ifstream recur(RFile);
    string myDate, amount;
    getline(recur, line);
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

void SaveTech(){
    ofstream tech("technical.csv");
    tech << "balance,date,goal\n";
    tech << balance << ',' << date << ',' << goal;
    tech.close();
}

//input functions
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

//structure for expenses
struct Expense {
    double amount;
    string category;
    bool isRecurring = false;
    string notes = "";

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

    //function for expense
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

    //constructor
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

    Expense(int am, string cat, bool rec = false){
        amount = am;
        category = cat;
        isRecurring = rec;
    }
};

//structure for income
struct Income {
    double amount;
    string category;
    bool isRecurring = false;
    string notes = "";

    //functions to add new lines of income and expenses to the budget csv file
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

    //function for income
    int IncHandling() {
        balance += amount;
        WriteBudget();
        return 1;
    }
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

//functions to read the budget csv file
Income ReadBudgetI() {
    ifstream budget(BFile);
    string line;
    string date, type, amount, category, isRecurring, myBalance;
    getline(budget, line);
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

//function to clear the budget file
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

void showBalance(){
    cout << "==============================\n";
    cout << "It is day " << date << " today.\n";
    cout << "Your current balance is: $" << balance << ".\n";
    cout << "Your goal is: $" << goal << ".\n";
    cout << "==============================\n";
}

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

//function for menu and general operations
int main()
{
    StartUp();
    cout << "Hello!\n";
    showBalance();
    showRecurring();
    cout << "Enter a new budget goal: ";
    cin >> goal;
    do {
        system("CLS");
        cout << "Choose an option:\n1. Add expense\n2. Add income\n3. Show balance\n4. Get a budget statement\n5. Show recurring payments\n6. Exit\n";;
        switch (UserChoiceInput()){
            case 1:
                system("CLS"); 
                Expense();
                system("pause");
                break;
            case 2:
                system("CLS");
                Income();
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

