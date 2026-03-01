# BudgetTracker

A simple command-line personal finance tracker written in C++. It allows users to record expenses and income, maintain a running balance, set financial goals, and view statements or recurring transactions. Data is stored in CSV files (`budget.csv`, `technical.csv`, `recurring.csv`).

## Features

- Add expenses with categories and optional monthly recurrence
- Add income entries with categories and optional monthly recurrence
- Track current balance and financial goal
- View account statements over a specified date range
- List recurring payments and calculate upcoming due dates
- Persist data between runs using CSV files
- 👍 Simple menu-driven CLI interface

## Project Structure

```
BudgetTracker/
├─ budget.csv         # transaction log created/updated by program
├─ recurring.csv      # recurring income/expense definitions
├─ technical.csv      # stores balance, current date, and goal
└─ BudgetTracker.cpp  # main source code with all logic and comments
```

## Getting Started

### Requirements

- Windows OS (tested with Visual Studio `cl.exe`) or any environment with a C++17-compliant compiler
- Standard library headers (`<iostream>`, `<fstream>`, etc.)

### Building

1. Open a Developer Command Prompt for Visual Studio or similar terminal.
2. Change into the project directory:
   ```powershell
   cd c:\Users\Konstantin\Desktop\BudgetTracker\BudgetTracker
   ```
3. Compile using `cl` (or adapt to your compiler):
   ```powershell
   cl.exe /Zi /EHsc /nologo /FeBudgetTracker.exe BudgetTracker.cpp
   ```
   This will produce `BudgetTracker.exe` in the same directory.

### Running

1. Make sure the CSV files (`budget.csv`, `technical.csv`, `recurring.csv`) are in the same directory as the executable. If they don't exist, the program will create them automatically when entries are added.
2. Run the executable:
   ```powershell
   .\BudgetTracker.exe
   ```
3. Follow the on-screen menu to add expenses/income, view balance, statements, or recurring entries. Enter `6` to exit; the technical state is saved automatically.

> **Tip:** option `101` is a hidden developer reset that clears all CSV data (useful for debugging).

## CSV File Format

- **budget.csv**: `date,type,amount,category,SEPA,balance`
- **technical.csv**: `balance,date,goal` (single data row)
- **recurring.csv**: `date,type,amount` (one entry per recurrence)

## Extending the Project

- Add more categories or internationalization
- Improve date handling with calendar libraries
- Replace CSV storage with a database
- Add unit tests and error handling improvements

## License

This project is provided as-is for educational purposes. Feel free to modify and distribute.

---

Created by a simple C++ budgeting tool. Use responsibly! 😊
