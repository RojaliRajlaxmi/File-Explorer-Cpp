#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

void listFiles(const string &path) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;

    dir = opendir(path.c_str());
    if (!dir) {
        cout << "❌ Cannot open directory: " << path << endl;
        return;
    }

    cout << "\n📁 Contents of: " << path << "\n";
    cout << "---------------------------------\n";
    while ((entry = readdir(dir)) != NULL) {
        string fullPath = path + "/" + entry->d_name;
        stat(fullPath.c_str(), &info);

        if (S_ISDIR(info.st_mode))
            cout << "\033[1;34m[DIR]\033[0m  " << entry->d_name << endl; // Blue for folders
        else
            cout << "       " << entry->d_name << endl;
    }
    closedir(dir);
}

void changeDirectory(string &currentPath, const string &folder) {
    string newPath = currentPath + "/" + folder;
    if (chdir(newPath.c_str()) == 0) {
        char buffer[1024];
        getcwd(buffer, sizeof(buffer));
        currentPath = buffer;
    } else {
        cout << "❌ Folder not found.\n";
    }
}

void copyFile(const string &src, const string &dest) {
    ifstream in(src, ios::binary);
    ofstream out(dest, ios::binary);
    if (!in || !out) {
        cout << "❌ Error copying file.\n";
        return;
    }
    out << in.rdbuf();
    cout << "✅ File copied successfully.\n";
}

void deleteFile(const string &file) {
    if (remove(file.c_str()) == 0)
        cout << "🗑️ File deleted.\n";
    else
        cout << "❌ Error deleting file.\n";
}

void searchFile(const string &path, const string &filename) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;

    if (!(dir = opendir(path.c_str())))
        return;

    while ((entry = readdir(dir)) != NULL) {
        string name = entry->d_name;
        if (name == "." || name == "..")
            continue;

        string fullPath = path + "/" + name;
        stat(fullPath.c_str(), &info);

        if (name.find(filename) != string::npos)
            cout << "🔍 Found: " << fullPath << endl;

        if (S_ISDIR(info.st_mode))
            searchFile(fullPath, filename);
    }
    closedir(dir);
}

void showHelp() {
    cout << "\n🆘 Available Commands:\n";
    cout << "---------------------------------\n";
    cout << "ls                     - List files in current directory\n";
    cout << "cd <folder>            - Change directory\n";
    cout << "copy <src> <dest>      - Copy a file\n";
    cout << "del <file>             - Delete a file\n";
    cout << "search <filename>      - Search for a file\n";
    cout << "help                   - Show this help message\n";
    cout << "exit                   - Exit program\n";
}

int main() {
    string currentPath;
    char buffer[1024];
    getcwd(buffer, sizeof(buffer));
    currentPath = buffer;

    cout << "🧭 Simple File Explorer (C++)\n";
    showHelp();

    string cmd;
    while (true) {
        cout << "\n[" << currentPath << "]$ ";
        getline(cin, cmd);

        if (cmd == "exit") break;
        else if (cmd == "ls") listFiles(currentPath);
        else if (cmd.rfind("cd ", 0) == 0) changeDirectory(currentPath, cmd.substr(3));
        else if (cmd.rfind("copy ", 0) == 0) {
            string src, dest;
            stringstream ss(cmd.substr(5));
            ss >> src >> dest;
            copyFile(src, dest);
        }
        else if (cmd.rfind("del ", 0) == 0) deleteFile(cmd.substr(4));
        else if (cmd.rfind("search ", 0) == 0) searchFile(currentPath, cmd.substr(7));
        else if (cmd == "help") showHelp();
        else cout << "❓ Unknown command. Type 'help' for options.\n";
    }

    cout << "👋 Exiting File Explorer.\n";
    return 0;
}
