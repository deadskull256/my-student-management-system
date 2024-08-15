#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

const string prefix = "uca";
string username, password, un, pw;
const fs::path dir = "user_data";
fs::path filePath;
bool loggedIn = false;
string assignments;
string tests;
vector<int> testScores; 

// Function prototypes
void handleLogin();
void handleRegistration();
void handleMainMenu();
bool teacherAccess(const string &un);
void handleStudent();
void handleTeacher();
void updateCredentials(const string& newCredential, const string& oldCredential);

void ensureDirectoryExists() {
    if (!fs::exists(dir)) {
        fs::create_directory(dir);
    }
}

void handleLogin() {
    // Request username and password
    cout << "Please Enter Your Username: ";
    cin >> username;
    cout << "Please Enter Your Password: ";
    cin >> password;

    // Creates Variables Used to Write Registration in Folder
    filePath = dir / (username + ".txt");

    // Attempt to Open the File
    ifstream read(filePath);

    // Check if the File was Opened Successfully
    if (read) {
        getline(read, un);
        getline(read, pw);
        if (un == username && pw == password) {
            loggedIn = true;
            if (teacherAccess(un)) {
                handleTeacher();
            } else {
                handleStudent();
            }
        } else {
            cout << "Your Username or Password is incorrect! Please check your spelling!" << endl;
            handleMainMenu();
        }
        read.close();
    } else {
        cout << "No Account Found with This Username." << endl;
        handleMainMenu();
    }
}

void handleRegistration() {
    cout << "Please Enter the Username You Want to Use: ";
    cin >> username;
    cout << "Please Enter Your Password: ";
    cin >> password;

    filePath = dir / (username + ".txt");
    ofstream outputFile(filePath);
    if (outputFile) {
        outputFile << username << endl << password << endl;
        outputFile.close();
        cout << "Successfully Created Your Account, Please Login!" << endl;
        handleMainMenu();
    } else {
        cout << "Error: Could not create account. Please try again. Code: 11" << endl;
    }
}

void handleMainMenu() {
    string loginOrRegister;

    cout << "\t\t\t_________________________________________________\n\n\n";
    cout << "\t\t\t     Welcome to The Student Management System!    \n\n\n";
    cout << "Login or Register: ";
    cin >> loginOrRegister;

    if (loginOrRegister == "Login" || loginOrRegister == "LOGIN" || loginOrRegister == "login") {
        handleLogin();
    } else if (loginOrRegister == "Register" || loginOrRegister == "REGISTER" || loginOrRegister == "register") {
        handleRegistration();
    } else {
        cout << "Invalid Choice: Please Enter 'Login' or 'Register' as Your Choices!" << endl;
        handleMainMenu(); // Restart the menu
    }
}

bool teacherAccess(const string &un) { 
    return un.find(prefix) == 0; 
}

void handleStudent() {
    if (!loggedIn) {
        handleMainMenu();
    }
    
    int choice;
    cout << "Welcome Student!" << endl;
    cout << "1. Edit your details" << endl;
    cout << "2. Submit your assignment" << endl;
    cout << "3. Look at your completed tests" << endl;
    cout << "4. Logout" << endl;
    cout << "What would you like to do? ";
    cin >> choice;

    switch (choice) {
        case 1: {
            int change;
            string fileContent;
            string line;
            
            ifstream inFile(filePath);
            
            if (!inFile) {
                cerr << "Error: Could not open the file. Please contact your local Administrator! Code: 12" << endl;
                return;
            }
            
            while (getline(inFile, line)) {
                fileContent += line + "\n";
            }
            inFile.close();

            cout << "1. Username: " << un << endl;
            cout << "2. Password: " << pw << endl;
            cout << "3. Exit" << endl;
            cout << "Please type 1 to change your Username or 2 to change your Password!" << endl;
            cin >> change;

            if (change == 1) {
                string newUsername;
                cout << "What Would You Like to Replace Your Username With: ";
                cin >> newUsername;

                size_t pos;
                if ((pos = fileContent.find(un)) != string::npos) {
                    fileContent.replace(pos, un.length(), newUsername);
                    un = newUsername;
                } else {
                    cerr << "Error: Old username not found in the file content. Code: 13" << endl;
                }
                
                ofstream outFile(filePath);
                if (outFile) {
                    outFile << fileContent;
                    outFile.close();
                    cout << "Your Username Has Been Changed!" << endl;
                } else {
                    cerr << "Error: Could not write to the file. Please contact your local Administrator! Code: 14" << endl;
                }
                handleMainMenu();
                
            } else if (change == 2) {
                string newPassword;
                cout << "What Would You Like to Replace Your Password With: ";
                cin >> newPassword;

                size_t pos;
                if ((pos = fileContent.find(pw)) != string::npos) {
                    fileContent.replace(pos, pw.length(), newPassword);
                    pw = newPassword;
                } else {
                    cerr << "Error: Old password not found in the file content. Code: 15" << endl;
                }
                ofstream outFile(filePath);
                if (outFile) {
                    outFile << fileContent;
                    outFile.close();
                    cout << "Your Password Has Been Changed!" << endl;
                } else {
                    cerr << "Error: Could not write to the file. Please contact your local Administrator! Code: 16" << endl;
                }
                handleMainMenu();
                
            } else {
                cout << "Invalid choice. Please select a valid option." << endl;
                handleStudent(); // Restart student menu
            }
            break;
        }
        case 2:
            cout << "Loading..." << endl;
            if (assignments.empty()) {
                cout << "There Are Currently No Assignments Assigned!" << endl;
            } else {
                cout << assignments << endl;
            }
            handleStudent();
            break;
        case 3:
            cout << "Loading completed tests..." << endl;
            if (tests.empty() && testScores.empty()) {
                cout << "No completed tests available." << endl;
            } else {
                cout << "Tests: " << tests << endl;
                cout << "Scores: ";
                for (const int& score : testScores) {
                    cout << score << ' ';
                }
                cout << endl;
            }
            handleStudent();
            break;
        case 4:
            cout << "Logging out..." << endl;
            loggedIn = false;
            handleMainMenu(); // Return to main menu
            break;
        default:
            cout << "Invalid choice. Please select a valid option." << endl;
            handleStudent(); // Restart student menu
            break;
    }
}

void updateCredentials(const string& newCredential, const string& oldCredential) {
    ifstream inFile(filePath);
    if (!inFile) {
        cerr << "Error: Could not open the file for reading. Code: 30" << endl;
        return;
    }

    string fileContent;
    string line;
    while (getline(inFile, line)) {
        fileContent += line + "\n";
    }
    inFile.close();

    size_t pos;
    if ((pos = fileContent.find(oldCredential)) != string::npos) {
        fileContent.replace(pos, oldCredential.length(), newCredential);
    } else {
        cerr << "Error: Old credential not found in the file content. Code: 31" << endl;
        return;
    }

    ofstream outFile(filePath);
    if (outFile) {
        outFile << fileContent;
        outFile.close();
    } else {
        cerr << "Error: Could not write to the file. Code: 32" << endl;
    }
}

void handleTeacher() {
    if (!loggedIn) {
        handleMainMenu();
    }

    int response;
    cout << "Welcome, Teacher!" << endl;
    cout << "1. Handle Assignments" << endl;
    cout << "2. Handle Scores" << endl;
    cout << "3. Change Login Details" << endl;
    cout << "4. Logout" << endl;
    cout << "What would you like to do? ";
    cin >> response;
    cin.ignore();  // Clear the newline character from the input buffer

    switch (response) {
        case 1: {
            string studentUsername, assignment;
            cout << "Enter Student Username: ";
            getline(cin, studentUsername);
            cout << "Enter assignment name to add: ";
            getline(cin, assignment);

            fs::path studentFilePath = dir / (studentUsername + ".txt");
            ofstream studentFile(studentFilePath, ios::app);
            if (studentFile) {
                studentFile << assignment << endl;
                cout << "Assigned \"" << assignment << "\" to " << studentUsername << "." << endl;
            } else {
                cerr << "Error: Could Not Open Student File. Code: 21" << endl;
            }
            break;
        }
        case 2: {
            string studentUsername;
            cout << "Enter Student Username: ";
            getline(cin, studentUsername);

            fs::path studentFilePath = dir / (studentUsername + ".txt");
            ifstream studentFile(studentFilePath);
            if (!studentFile) {
                cerr << "Error: Could Not Open Student File. Code: 22" << endl;
                return;
            }

            cout << "Current scores for " << studentUsername << ":\n";
            for (size_t i = 0; i < tests.size(); ++i) {
                cout << tests[i] << ": " << testScores[i] << endl;
            }

            cout << "Enter the Number of the Test to Update (0 to " << tests.size() - 1 << "): ";
            int testIndex;
            cin >> testIndex;
            cin.ignore();

            if (testIndex >= 0 && testIndex < tests.size()) {
                int newScore;
                cout << "Enter New Score for " << tests[testIndex] << ": ";
                cin >> newScore;
                cin.ignore();

                testScores[testIndex] = newScore;
                cout << "Score Updated!" << endl;
            } else {
                cout << "Invalid Test Number." << endl;
            }
            break;
        }
        case 3: {
            string newUsername, newPassword;
            cout << "1. Change Username" << endl;
            cout << "2. Change Password" << endl;
            cout << "What would you like to change? ";
            int change;
            cin >> change;
            cin.ignore();

            if (change == 1) {
                cout << "Enter New Username: ";
                getline(cin, newUsername);
                updateCredentials(newUsername, un);
                cout << "Your Username Has Been Changed!" << endl;
            } else if (change == 2) {
                cout << "Enter New Password: ";
                getline(cin, newPassword);
                updateCredentials(newPassword, pw);
                cout << "Your Password Has Been Changed!" << endl;
            } else {
                cout << "Invalid choice. Please select a valid option." << endl;
            }
            break;
        }
        case 4:
            cout << "Logging you out...." << endl;
            loggedIn = false;
            handleMainMenu();
            break;
        default:
            cout << "Invalid choice. Please select a valid option." << endl;
            handleTeacher();
            break;
    }
}

int main() {
    ensureDirectoryExists();
    handleMainMenu();
    return 0;
}
