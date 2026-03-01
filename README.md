BUDGET TRACKER

The console app serves as a tool of budget monitoring and planning. This app was designed as an examination project for the Programming course at the SRH University of Stuttgart. The objectives of the app should have been:
1. Expense and income data structures using structs; 
2. Add new income and expense records;
3. Budget calculation and balance logic; 
4. File handling (save and load financial records);
5. Input validation and error handling;
6. Menu-based CLI system.

All of the tasks have been completed successfully. The app makes use of the CSV files for the transaction storage and structs for their creation.
Use of the structs turned out to be not the most efficient decsision - classes could be a better one - however this topic has not been covered yet by the time structures were already implemented. Anyways, structures were as good as classes for representing transacions in this vry case.
One more issue that was faced are the dates. The date-time libraries couldn't work well with the format of data I used, so, I just created an integer variable measuring the number of days since the start of the programm incrementing the register with every new run. This stays a partial solution.

To use the project you can simply download the folder with all the CSV files in there. CPP will find them nd work with them properly. In case you wish to restart the progress of the budget tracker you can use the special function, you can find inside the code. 
