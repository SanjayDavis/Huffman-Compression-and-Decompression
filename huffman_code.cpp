#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

class node {
public:
    int value;
    char letter;
    node* left;
    node* right;

    node(char letter) {
        this->value = 1;
        this->letter = letter;
        this->left = NULL;
        this->right = NULL;
    }

    node(int value) {
        this->value = value;
        this->letter = ' ';
        this->left = NULL;
        this->right = NULL;
    }
};

class table {
public:
    char letter;
    string output;

    table(char letter, string temp) {
        this->letter = letter;
        this->output = temp;
    }
};

vector<node*> charecter;
vector<table> chart;

void swap_node(node*& a, node*& b) {
    node* temp = a;
    a = b;
    b = temp;
}

void sort_nodes() {
    int l = charecter.size();
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < l - i - 1; j++) {
            if (charecter[j]->value > charecter[j + 1]->value) {
                swap_node(charecter[j], charecter[j + 1]);
            }
        }
    }
}

void single_sort() {
    int l = charecter.size();
    for (int i = l - 2; i >= 0; i--) {
        if (charecter[i]->value > charecter[i + 1]->value) {
            swap_node(charecter[i], charecter[i + 1]);
        } else break;
    }
}

void print() {
    for (auto it : charecter) {
        cout << it->letter << "  " << it->value << endl;
    }
}

node* tree() {
    sort_nodes();
    while (charecter.size() > 1) {
        print();
        node* new_node = new node(charecter[0]->value + charecter[1]->value);
        new_node->left = charecter[0];
        new_node->right = charecter[1];
        charecter.erase(charecter.begin());
        charecter.erase(charecter.begin());
        charecter.push_back(new_node);
        single_sort();
    }
    return charecter[0];
}

void assign_string(node* root, string temp, vector<table>& chart) {
    if (root == NULL) return;
    if (root->left == NULL && root->right == NULL) {
        chart.push_back(table(root->letter, temp));
        cout << "Assigned code to character [" << root->letter << "]: " << temp << endl;
    } else {
        assign_string(root->left, temp + '0', chart);
        assign_string(root->right, temp + '1', chart);
    }
}

void gather_string(string str) {
    for (char it : str) {
        bool found = false;
        for (int i = 0; i < charecter.size(); i++) {
            if (charecter[i]->letter == it) {
                charecter[i]->value++;
                found = true;
                break;
            }
        }
        if (!found) {
            charecter.push_back(new node(it));
        }
    }
}

node* start() {
    sort_nodes();
    node* root = tree();
    assign_string(root, "", chart);
    return root;
}

stringstream print_strings(string file_name) {
    fstream initial(file_name, ios::in);
    stringstream output;
    if (!initial.is_open()) {
        cerr << "Error opening file!" << endl;
        return output;
    }
    char l;
    while (initial.get(l)) {
        for (auto& it : chart) {
            if (it.letter == l) {
                output << it.output;
                break;
            }
        }
    }
    initial.close();
    return output;
}

void store_metadata() {
    string name = "metadata.txt";
    fstream file(name, ios::out);
    for (auto it : chart) {
        file << it.letter << ":" << it.output << endl;
    }
    file.close();
}

stringstream convert_binary(stringstream& output) {
    stringstream total;
    char l;
    while (output.get(l)) {
        total << static_cast<char>(l);
    }
    return total;
}

void send_text(stringstream& str, string file_name) {
    fstream file(file_name, ios::out);
    char l;
    while (str.get(l)) {
        file << l;
    }
    file.close();
}

void help() {
    cout << "Usage:\n";
    cout << "  -p <file_path> : Compress a file\n";
    cout << "  -t <text>      : Compress input text\n";
    cout << "Extracted file will be uploaded to zipped.bin\n";
}

void with_file(string file_name) {
    fstream file(file_name, ios::in);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << file_name << endl;
        return;
    }
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    gather_string(buffer.str());
    start();
    stringstream output = print_strings(file_name);
    stringstream str = convert_binary(output);
    send_text(str, "zipped.bin");
    store_metadata();
}

void with_text(string text) {
    gather_string(text);
    start();
    stringstream output;
    for (char c : text) {
        for (auto& it : chart) {
            if (it.letter == c) {
                output << it.output;
                break;
            }
        }
    }
    stringstream str = convert_binary(output);
    send_text(str, "zipped.bin");
    store_metadata();
}

int main(int argc, char** argv) {
    if (argc != 3) {
        help();
        return 0;
    }
    string option = argv[1];
    if (option == "-p") with_file(argv[2]);
    else if (option == "-t") with_text(argv[2]);
    else help();
    return 0;
    
}
