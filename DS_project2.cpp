#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>
#include <fstream>

using namespace std;


// mm

class MinHeap
{
private:
    std::vector<long long> heap;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    void swap(int i, int j)
    {
        long long temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    void heapifyUp(int index)
    {
        while (index > 0 && heap[index] < heap[parent(index)])
        {
            swap(index, parent(index));
            index = parent(index);
        }
    }

    void heapifyDown(int index)
    {
        int size = heap.size();
        while (true)
        {
            int left = leftChild(index);
            int right = rightChild(index);
            int smallest = index;

            if (left < size && heap[left] < heap[smallest])
            {
                smallest = left;
            }
            if (right < size && heap[right] < heap[smallest])
            {
                smallest = right;
            }

            if (smallest == index)
            {
                break;
            }

            swap(index, smallest);
            index = smallest;
        }
    }

public:
    void push(long long value)
    {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    void pop()
    {
        if (heap.empty())
        {
            std::cerr << "Error: Cannot pop from an empty heap." << std::endl;
            return;
        }
        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty())
        {
            heapifyDown(0);
        }
    }

    long long top()
    {
        if (heap.empty())
        {
            std::cerr << "Error: Heap is empty." << std::endl;
            return -1;
        }
        return heap[0];
    }

    bool isEmpty()
    {
        return heap.empty();
    }

    int size()
    {
        return heap.size();
    }
};
struct student
{
    string firstname;
    string lastname;
    string ID;
    float GPA;
    long long STU_ID;
};

enum Color
{
    RED,
    BLACK
};

struct Node
{
    student data;
    Color color;
    Node *left;
    Node *right;
    Node *parent;
    Node(student std) : data(std), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBTree
{
private:
    Node *root;
    long long nextAvailableID = 1000001;
    unordered_set<string> nationalID_lookup;

    MinHeap freedIDsHeap;

    void RotateLeft(Node *&);
    void RotateRight(Node *&);
    void insertFixup(Node *&);
    void inorderHelper(Node *node);
    Node *searchHelper(Node *node, long long id);
    void findTopGpaHelper(Node *node, vector<Node *> &bestNodes);
    void findGeGpaHelper(Node *node, float minGpa, bool &found);
    void deleteFixup(Node *x);
    void rbTransplant(Node *u, Node *v);
    Node *minimum(Node *node);
    void collectSuspendedStudents(Node *node, vector<student> &studentsToDelete);
    void saveHelper(Node *node, ofstream &file);
    void insertLoadedStudent(const student &std_data);

public:
    RBTree() : root(nullptr) {}
    long long insert(student &std_data);
    void findStudent(long long stu_id);
    bool search(long long id);
    void printInorder();
    void remove(int);
    void findBestGpa();
    void findGeGpa(float minGpa);
    void deleteNode(long long stu_id, bool silent = false);
    void deleteSuspended();
    void changeStudentInfo(long long stu_id, const string &attribute, const string &value);
    void saveToFile(const string &filename);
    void loadFromFile(const string &filename);
};

void RBTree::RotateLeft(Node *&r)
{
    Node *new_r = r->right;
    r->right = new_r->left;
    if (new_r->left != nullptr)
        new_r->left->parent = r;
    new_r->parent = r->parent;
    if (r->parent == nullptr)
        this->root = new_r;
    else if (r == r->parent->left)
        r->parent->left = new_r;
    else
        r->parent->right = new_r;
    new_r->left = r;
    r->parent = new_r;
}
void RBTree::RotateRight(Node *&r)
{
    Node *new_r = r->left;
    r->left = new_r->right;
    if (new_r->right != nullptr)
        new_r->right->parent = r;
    new_r->parent = r->parent;
    if (r->parent == nullptr)
        this->root = new_r;
    else if (r == r->parent->right)
        r->parent->right = new_r;
    else
        r->parent->left = new_r;
    new_r->right = r;
    r->parent = new_r;
}
void RBTree::insertFixup(Node *&pt)
{
    Node *parent = nullptr, *grandparent = nullptr;
    while ((pt != root) && (pt->color != BLACK) && (pt->parent->color == RED))
    {
        parent = pt->parent;
        grandparent = pt->parent->parent;
        if (parent == grandparent->left)
        {
            Node *uncle = grandparent->right;
            if (uncle != nullptr && uncle->color == RED)
            {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                pt = grandparent;
            }
            else
            {
                if (pt == parent->right)
                {
                    RotateLeft(parent);
                    pt = parent;
                    parent = pt->parent;
                }
                parent->color = BLACK;
                grandparent->color = RED;
                RotateRight(grandparent);
                pt = parent;
            }
        }
        else
        {
            Node *uncle = grandparent->left;
            if (uncle != nullptr && uncle->color == RED)
            {
                grandparent->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                pt = grandparent;
            }
            else
            {
                if (pt == parent->left)
                {
                    RotateRight(parent);
                    pt = parent;
                    parent = pt->parent;
                }
                parent->color = BLACK;
                grandparent->color = RED;
                RotateLeft(grandparent);
                pt = parent;
            }
        }
    }
    root->color = BLACK;
}

Node *RBTree::searchHelper(Node *node, long long id)
{
    if (node == nullptr || node->data.STU_ID == id)
        return node;
    if (id < node->data.STU_ID)
        return searchHelper(node->left, id);
    return searchHelper(node->right, id);
}

bool RBTree::search(long long id)
{
    return searchHelper(root, id) != nullptr;
}

long long RBTree::insert(student &std_data)
{
    if (nationalID_lookup.count(std_data.ID))
    {
        cout << "FAILED => RECORD EXISTS" << endl;
        return -1;
    }

    long long generatedID;

    if (!freedIDsHeap.isEmpty())
    {
        generatedID = freedIDsHeap.top();
        freedIDsHeap.pop();
    }
    else
    {
        generatedID = nextAvailableID;
        nextAvailableID++;
    }

    std_data.STU_ID = generatedID;

    Node *newNode = new Node(std_data);
    if (root == nullptr)
    {
        newNode->color = BLACK;
        root = newNode;
    }
    else
    {
        Node *current = root;
        Node *parent = nullptr;
        while (current != nullptr)
        {
            parent = current;
            if (newNode->data.STU_ID < current->data.STU_ID)
                current = current->left;
            else
                current = current->right;
        }
        newNode->parent = parent;
        if (newNode->data.STU_ID < parent->data.STU_ID)
            parent->left = newNode;
        else
            parent->right = newNode;
        insertFixup(newNode);
    }

    nationalID_lookup.insert(std_data.ID);
    return generatedID;
}

student parseInput(const string &input)
{
    student s;
    s.STU_ID = 0;
    string data = input.substr(input.find(" ") + 1);
    stringstream ss(data);
    string segment;
    while (getline(ss, segment, ','))
    {
        if (segment[0] == ' ')
            segment.erase(0, 1);
        size_t colon_pos = segment.find(':');
        if (colon_pos != string::npos)
        {
            string key = segment.substr(0, colon_pos);
            string value = segment.substr(colon_pos + 1);
            if (key == "NAME")
                s.firstname = value;
            else if (key == "FAMILY")
                s.lastname = value;
            else if (key == "ID")
                s.ID = value;
            else if (key == "GPA")
                s.GPA = stof(value);
        }
    }
    return s;
}

void RBTree::findStudent(long long stu_id)
{
    Node *resultNode = searchHelper(root, stu_id);
    if (resultNode != nullptr)
    {
        cout << "ID: " << resultNode->data.ID << "," << endl;
        cout << "NAME: " << resultNode->data.firstname << "," << endl;
        cout << "FAMILY: " << resultNode->data.lastname << "," << endl;
        cout << "STU_ID: " << resultNode->data.STU_ID << "," << endl;
        cout << "GPA: " << resultNode->data.GPA << endl;
    }
    else
    {
        cout << "NOT FOUND" << endl;
    }
}

void RBTree::findTopGpaHelper(Node *node, vector<Node *> &bestNodes)
{
    if (node == nullptr)
    {
        return;
    }

    if (bestNodes.empty() || node->data.GPA > bestNodes.front()->data.GPA)
    {
        bestNodes.clear(); 
        bestNodes.push_back(node); 
    }

    else if (node->data.GPA == bestNodes.front()->data.GPA)
    {
        bestNodes.push_back(node); 
    }


    findTopGpaHelper(node->left, bestNodes);
    findTopGpaHelper(node->right, bestNodes);
}

void RBTree::findBestGpa()
{
    if (root == nullptr)
    {
        cout << "FAILED => DATABASE IS EMPTY" << endl;
        return;
    }

    vector<Node *> bestStudentNodes;
    findTopGpaHelper(root, bestStudentNodes); 

    if (!bestStudentNodes.empty())
    {
        cout << "Found " << bestStudentNodes.size() << " student(s) with the top GPA of " << bestStudentNodes.front()->data.GPA << ":" << endl;
        cout << "-----------------------------------------------" << endl;

        for (const auto &studentNode : bestStudentNodes)
        {
            cout << "ID: " << studentNode->data.ID << "," << endl;
            cout << "NAME: " << studentNode->data.firstname << "," << endl;
            cout << "FAMILY: " << studentNode->data.lastname << "," << endl;
            cout << "STU_ID: " << studentNode->data.STU_ID << "," << endl;
            cout << "GPA: " << studentNode->data.GPA << endl;
            cout << "-----------------------------------------------" << endl;
        }
    }
}

void RBTree::findGeGpaHelper(Node *node, float minGpa, bool &found)
{
    if (node == nullptr)
    {
        return;
    }
    if (node->data.GPA >= minGpa)
    {
        cout << "ID: " << node->data.ID << ", "
             << "NAME: " << node->data.firstname << ", "
             << "FAMILY: " << node->data.lastname << ", "
             << "STU_ID: " << node->data.STU_ID << ", "
             << "GPA: " << node->data.GPA << endl;
        found = true;
    }
    findGeGpaHelper(node->left, minGpa, found);
    findGeGpaHelper(node->right, minGpa, found);
}

void RBTree::findGeGpa(float minGpa)
{
    if (root == nullptr)
    {
        cout << "FAILED => DATABASE IS EMPTY" << endl;
        return;
    }
    bool studentFound = false;
    findGeGpaHelper(root, minGpa, studentFound);
    if (!studentFound)
    {
        cout << "FAILED => NOT FOUND" << endl;
    }
}

Node *RBTree::minimum(Node *node)
{
    while (node->left != nullptr)
    {
        node = node->left;
    }
    return node;
}

void RBTree::rbTransplant(Node *u, Node *v)
{
    if (u->parent == nullptr)
    {
        root = v;
    }
    else if (u == u->parent->left)
    {
        u->parent->left = v;
    }
    else
    {
        u->parent->right = v;
    }
    if (v != nullptr)
    {
        v->parent = u->parent;
    }
}

void RBTree::deleteFixup(Node *x)
{
    Node *s;
    while (x != root && (x == nullptr || x->color == BLACK))
    {
        if (x == x->parent->left)
        {
            s = x->parent->right;
            if (s->color == RED)
            {
                s->color = BLACK;
                x->parent->color = RED;
                RotateLeft(x->parent);
                s = x->parent->right;
            }
            if ((s->left == nullptr || s->left->color == BLACK) && (s->right == nullptr || s->right->color == BLACK))
            {
                s->color = RED;
                x = x->parent;
            }
            else
            {
                if (s->right == nullptr || s->right->color == BLACK)
                {
                    if (s->left != nullptr)
                        s->left->color = BLACK;
                    s->color = RED;
                    RotateRight(s);
                    s = x->parent->right;
                }
                s->color = x->parent->color;
                x->parent->color = BLACK;
                if (s->right != nullptr)
                    s->right->color = BLACK;
                RotateLeft(x->parent);
                x = root;
            }
        }
        else
        {
            s = x->parent->left;
            if (s->color == RED)
            {
                s->color = BLACK;
                x->parent->color = RED;
                RotateRight(x->parent);
                s = x->parent->left;
            }
            if ((s->right == nullptr || s->right->color == BLACK) && (s->left == nullptr || s->left->color == BLACK))
            {
                s->color = RED;
                x = x->parent;
            }
            else
            {
                if (s->left == nullptr || s->left->color == BLACK)
                {
                    if (s->right != nullptr)
                        s->right->color = BLACK;
                    s->color = RED;
                    RotateLeft(s);
                    s = x->parent->left;
                }
                s->color = x->parent->color;
                x->parent->color = BLACK;
                if (s->left != nullptr)
                    s->left->color = BLACK;
                RotateRight(x->parent);
                x = root;
            }
        }
    }
    if (x != nullptr)
        x->color = BLACK;
}

void RBTree::deleteNode(long long stu_id, bool silent)
{
    Node *z = searchHelper(root, stu_id);
    if (z == nullptr)
    {
        if (!silent)
        {
            cout << "FAILED => NOT FOUND" << endl;
        }
        return;
    }

    Node *x, *y = z;
    Color y_original_color = y->color;

    if (z->left == nullptr)
    {
        x = z->right;
        rbTransplant(z, z->right);
    }
    else if (z->right == nullptr)
    {
        x = z->left;
        rbTransplant(z, z->left);
    }
    else
    {
        y = minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z)
        {
            if (x != nullptr)
                x->parent = y;
        }
        else
        {
            rbTransplant(y, y->right);
            y->right = z->right;
            if (y->right != nullptr)
                y->right->parent = y;
        }
        rbTransplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    nationalID_lookup.erase(z->data.ID);

    freedIDsHeap.push(z->data.STU_ID);

    delete z;

    if (y_original_color == BLACK)
    {
        if (x != nullptr)
            deleteFixup(x);
    }

    if (!silent)
    {
        cout << "SUCCESS" << endl;
    }
}

void RBTree::collectSuspendedStudents(Node *node, vector<student> &studentsToDelete)
{
    if (node == nullptr)
    {
        return;
    }

    if (node->data.GPA < 12.0)
    {
        studentsToDelete.push_back(node->data);
    }
    collectSuspendedStudents(node->left, studentsToDelete);
    collectSuspendedStudents(node->right, studentsToDelete);
}

void RBTree::deleteSuspended()
{
    if (root == nullptr)
    {
        cout << "FAILED => DATABASE IS EMPTY" << endl;
        return;
    }

    vector<student> studentsToDelete;

    collectSuspendedStudents(root, studentsToDelete);

    if (studentsToDelete.empty())
    {
        cout << "SUCCESS, No suspended students found to delete." << endl;
        return;
    }

    cout << "SUCCESS, DELETED RECORDS:" << endl;

    for (const auto &stu : studentsToDelete)
    {
        cout << "NAME: " << stu.firstname
             << ", FAMILY: " << stu.lastname
             << ", ID: " << stu.ID
             << ", GPA:" << stu.GPA << endl;

        deleteNode(stu.STU_ID, true);
    }
}

void RBTree::changeStudentInfo(long long stu_id, const string &attribute, const string &value)
{
    Node *targetNode = searchHelper(root, stu_id);

    if (targetNode == nullptr)
    {
        cout << "FAILED => NOT FOUND" << endl;
        return;
    }

    if (attribute == "NAME")
    {
        targetNode->data.firstname = value;
    }
    else if (attribute == "FAMILY")
    {
        targetNode->data.lastname = value;
    }
    else if (attribute == "GPA")
    {
        try
        {
            targetNode->data.GPA = stof(value);
        }
        catch (...)
        {
            cout << "FAILED => INVALID GPA FORMAT" << endl;
            return;
        }
    }
    else if (attribute == "ID")
    {
        if (nationalID_lookup.count(value))
        {
            cout << "FAILED => DUPLICATE NATIONAL ID" << endl;
            return;
        }
        nationalID_lookup.erase(targetNode->data.ID);
        targetNode->data.ID = value;
        nationalID_lookup.insert(value);
    }
    else if (attribute == "STU_ID")
    {
        long long new_stu_id;
        try
        {
            new_stu_id = stoll(value);
        }
        catch (...)
        {
            cout << "FAILED => INVALID STU_ID FORMAT" << endl;
            return;
        }

        if (search(new_stu_id))
        {
            cout << "FAILED => DESTINATION STU_ID ALREADY EXISTS" << endl;
            return;
        }

        student temp_data = targetNode->data;

        deleteNode(stu_id, true);

        temp_data.STU_ID = new_stu_id;

        insertLoadedStudent(temp_data);

        if (new_stu_id >= nextAvailableID)
        {
            nextAvailableID = new_stu_id + 1;
        }
    }
    else
    {
        cout << "FAILED => UNKNOWN ATTRIBUTE" << endl;
        return;
    }

    cout << "SUCCESS" << endl;
}

void RBTree::saveHelper(Node *node, ofstream &file)
{
    if (node == nullptr)
    {
        return;
    }

    file << node->data.STU_ID << ","
         << node->data.firstname << ","
         << node->data.lastname << ","
         << node->data.ID << ","
         << node->data.GPA << "\n";

    saveHelper(node->left, file);
    saveHelper(node->right, file);
}

void RBTree::saveToFile(const string &filename)
{
    ofstream outFile(filename);
    if (!outFile.is_open())
    {
        cerr << "Error: Could not open file for writing." << endl;
        return;
    }
    if (root != nullptr)
    {
        saveHelper(root, outFile);
    }
    outFile.close();
}

void RBTree::insertLoadedStudent(const student &std_data)
{
    Node *newNode = new Node(std_data);
    if (root == nullptr)
    {
        newNode->color = BLACK;
        root = newNode;
    }
    else
    {
        Node *current = root;
        Node *parent = nullptr;
        while (current != nullptr)
        {
            parent = current;
            if (newNode->data.STU_ID < current->data.STU_ID)
                current = current->left;
            else
                current = current->right;
        }
        newNode->parent = parent;
        if (newNode->data.STU_ID < parent->data.STU_ID)
            parent->left = newNode;
        else
            parent->right = newNode;
        insertFixup(newNode);
    }

    nationalID_lookup.insert(std_data.ID);
}

void RBTree::loadFromFile(const string &filename)
{
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        cout << "No saved data found. Starting a new database." << endl;
        return;
    }

    string line;
    long long max_stu_id = 1000000;

    unordered_set<long long> loaded_ids;

    while (getline(inFile, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);
        string item;
        student s;

        getline(ss, item, ',');
        s.STU_ID = stoll(item);

        getline(ss, item, ',');
        s.firstname = item;

        getline(ss, item, ',');
        s.lastname = item;

        getline(ss, item, ',');
        s.ID = item;

        getline(ss, item, ',');
        s.GPA = stof(item);

        insertLoadedStudent(s);

        loaded_ids.insert(s.STU_ID);

        if (s.STU_ID > max_stu_id)
        {
            max_stu_id = s.STU_ID;
        }
    }

    nextAvailableID = max_stu_id + 1;

    for (long long i = 1000001; i <= max_stu_id; ++i)
    {
        if (loaded_ids.find(i) == loaded_ids.end())
        {
            freedIDsHeap.push(i);
        }
    }

    inFile.close();
}

int main()
{
    RBTree student_tree;
    string db_filename = "student_database.txt";
    student_tree.loadFromFile(db_filename);
    string line;
    cout << "Student Management System (RBTree + Hash Table)" << endl;
    cout << "-----------------------------------------------" << endl;
    cout << "Usage:" << endl;
    cout << "1. Register: REGISTER NAME:..., FAMILY:..., ID:..., GPA:..." << endl;
    cout << "2. Find with student id : FIND ..." << endl;
    cout << "3. Find best GPA : FINDBESTGPA" << endl;
    cout << "4. Find GPA greater than a number : FIND_GE" << endl;
    cout << "5. Delete an student : DELETE ..." << endl;
    cout << "6. Delete suspend students : DELETE_SUSPENDED" << endl;
    cout << "7. Change information of an student : CHANGE <STU_ID> <ATTRIBUTE> <DESTINATION>" << endl;
    cout << "   (Attributes: NAME, FAMILY, ID, GPA, STU_ID)" << endl;
    cout << "8. Exit: EXIT" << endl;
    cout << "-----------------------------------------------" << endl;
    while (true)
    {
        cout << "> ";
        getline(cin, line);
        if (line.rfind("REGISTER", 0) == 0)
        {
            try
            {
                student newStudent = parseInput(line);
                long long newID = student_tree.insert(newStudent);

                if (newID != -1)
                {
                    cout << "  SUCCESS => STU_ID:" << newID << endl;
                }
            }
            catch (const std::invalid_argument &e)
            {
                cout << "  ERROR: Invalid GPA format. Please provide a number." << endl;
            }
            catch (...)
            {
                cout << "  ERROR: Could not parse the input string." << endl;
            }
        }
        else if (line.rfind("FIND ", 0) == 0)
        {
            try
            {
                string stu_id_str = line.substr(5);
                long long stu_id_to_find = stoll(stu_id_str);

                student_tree.findStudent(stu_id_to_find);
            }
            catch (...)
            {
                cout << "  ERROR: Invalid STU_ID format." << endl;
            }
        }
        else if (line == "FINDBESTGPA")
        {
            student_tree.findBestGpa();
        }
        else if (line.rfind("FIND_GE ", 0) == 0 || line.rfind("FIND GE ", 0) == 0)
        {
            try
            {
                size_t last_space_pos = line.rfind(' ');
                string gpa_str = line.substr(last_space_pos + 1);
                float minGpa = stof(gpa_str);
                student_tree.findGeGpa(minGpa);
            }
            catch (const std::invalid_argument &e)
            {
                cout << " ERROR: Invalid GPA format. Please provide a number." << endl;
            }
            catch (...)
            {
                cout << " ERROR: Could not parse the command." << endl;
            }
        }
        else if (line.rfind("DELETE ", 0) == 0)
        {
            try
            {
                string stu_id_str = line.substr(7);
                long long stu_id_to_delete = stoll(stu_id_str);
                student_tree.deleteNode(stu_id_to_delete);
            }
            catch (...)
            {
                cout << " ERROR: Invalid STU_ID format." << endl;
            }
        }
        else if (line == "DELETE_SUSPENDED")
        {
            student_tree.deleteSuspended();
        }
        else if (line.rfind("CHANGE ", 0) == 0)
        {
            try
            {
                stringstream ss(line);
                string command, stu_id_str, attribute, value;

                ss >> command >> stu_id_str >> attribute >> value;

                if (command.empty() || stu_id_str.empty() || attribute.empty() || value.empty())
                {
                    cout << " ERROR: Invalid CHANGE command format." << endl;
                }
                else
                {
                    long long stu_id = stoll(stu_id_str);
                    student_tree.changeStudentInfo(stu_id, attribute, value);
                }
            }
            catch (...)
            {
                cout << " ERROR: Invalid command or STU_ID format." << endl;
            }
        }
        else if (line == "EXIT")
        {
            cout << "Saving data and exiting program." << endl;
            student_tree.saveToFile(db_filename);
            break;
        }
        else
        {
            cout << "  Invalid command." << endl;
        }
    }
    return 0;
}