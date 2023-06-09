#include <iostream>
#include <string>
#include <curl/curl.h>
using namespace std;
//STRUCTURE TO HOLD SCRAPED DATA
struct ScrapedData {
    string description;
    string synonyms;
};
//CALLBACK FUNCTION TO RECEIVE RESPONSE DATA
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* buffer) {
    size_t totalSize = size * nmemb;
    buffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}
//FUNCTION PROTO FOR EXTRACTCONTENT
string extractContent(const string& html);
//SCRAPING FUNCTION
string scrapeDescription(const string& url) {
    string description;
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        //REDIRECT
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        //WRITE CALLBACK FUNCTION TO RECEIVE RESPONSE
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &description);
        //HTTP REQUEST
        CURLcode res = curl_easy_perform(curl);
        //MEMORY CLEAN UP
        curl_easy_cleanup(curl);
        if (res != CURLE_OK) {
            cerr << "Error: Failed to fetch URL " << url << " - " << curl_easy_strerror(res) << endl;
            return "";
        }
    }
    //EXTRACTING CONTENT UNTIL FIRST FULL STOP.
    string content = extractContent(description);
    size_t pos = content.find('.');
    if (pos != string::npos) {
        description = content.substr(0, pos);
    }
    return description;
}
//FUNCTION TO SCRAPE CONTENT NEAR <META NAME = DESCRIPTION...
string extractContent(const string& html) {
    string startTag = "<meta name=\"description\"";
    string endTag = "\">";
    size_t startPos = html.find(startTag);
    if (startPos != string::npos) {
        startPos = html.find("content=", startPos);
        if (startPos != string::npos) {
            startPos = html.find('\"', startPos) + 1;
            size_t endPos = html.find('.', startPos);
            if (endPos != string::npos) {
                string content = html.substr(startPos, endPos - startPos);
                //REMOVE UNWANTED SUBSTRING
                string unwantedSubstring = "<link rel=\"canonical\" href=\"https://www";
                size_t unwantedPos = content.find(unwantedSubstring);
                if (unwantedPos != string::npos) {
                    content.erase(unwantedPos, unwantedSubstring.length());
                }

                return content;
            }
        }
    }
    return "";
}
//STRUCTURE TO HOLD LINKED LIST NODE
struct Node {
    string data;
    Node* next;

    Node(const string& value) : data(value), next(nullptr) {}
};
//INSERT NODE AT END OF LINKED LIST
void insertNode(Node** head, const string& value) {
    static bool skipFirst = true; //FLAG FOR SKIPPING FIRST WORD

    if (skipFirst) {
        skipFirst = false;
        return;
    }
    Node* newNode = new Node(value);
    if (*head == nullptr)
        *head = newNode;
    else {
        Node* current = *head;
        while (current->next != nullptr)
            current = current->next;
        current->next = newNode;
    }
}
//SWAP TWO NODES
void swapNodesData(Node* node1, Node* node2) {
    string temp = node1->data;
    node1->data = node2->data;
    node2->data = temp;
}
//PARTITION OF LINKED LIST AND RETURN PIVOT
Node* partition(Node* head, Node* end, Node** newHead, Node** newEnd) {
    Node* pivot = end;
    Node* prev = nullptr;
    Node* current = head;
    Node* tail = pivot;
    while (current != pivot) {
        if (current->data < pivot->data) {
            if (*newHead == nullptr)
                *newHead = current;
            prev = current;
            current = current->next;
        }
        else {
            if (prev)
                prev->next = current->next;
            Node* temp = current->next;
            current->next = nullptr;
            tail->next = current;
            tail = current;
            current = temp;
        }
    }
    if (*newHead == nullptr)
        *newHead = pivot;
    *newEnd = tail;
    return pivot;
}
//GET TAIL FUNCTION
Node* getTail(Node* head) {
    if (head == nullptr)
        return nullptr;
    while (head->next != nullptr)
        head = head->next;
    return head;
}
//QUICKSORT RECURSIVE FUNCTION
Node* quickSortRecur(Node* head, Node* end) {
    if (head == nullptr || head == end)
        return head;
    Node* newHead = nullptr;
    Node* newEnd = nullptr;
    Node* pivot = partition(head, end, &newHead, &newEnd);
    if (newHead != pivot) {
        Node* temp = newHead;
        while (temp->next != pivot)
            temp = temp->next;
        temp->next = nullptr;
        newHead = quickSortRecur(newHead, temp);
        temp = getTail(newHead);
        temp->next = pivot;
    }
    pivot->next = quickSortRecur(pivot->next, newEnd);
    return newHead;
}
//QUICKSORTER
void quickSort(Node** headRef) {
    *headRef = quickSortRecur(*headRef, getTail(*headRef));
}
//DISPLAY LINKED LIST
void displayList(Node* head) {
    int counter = 0;  
    //SYNONYM COUNTER
    while (head != nullptr && counter < 3) {
        cout << head->data << endl;
        head = head->next;
        counter++;
    }
}
int main() {
    string word;
    cout << "Enter a word: ";
    cin >> word;
    //FIRST URL SCRAPE
    string url1 = "https://www.merriam-webster.com/dictionary/" + word;
    string description1 = scrapeDescription(url1);
    if (!description1.empty()) {
        string content1 = extractContent(description1);
        cout << "Description: " << content1 << endl;
    }
    else {
        cout << "Word doesn't exist." << endl;
        return 0;
    }
    //SECOND URL SCRAPE
    string url2 = "https://www.merriam-webster.com/thesaurus/" + word;
    string description2 = scrapeDescription(url2);
    if (!description2.empty()) {
        string content2 = extractContent(description2);
    //CONSTRUCTING LINKED LIST FROM COMMA SEPARATED VALUES
    Node* head = nullptr;
    string synonym;
    size_t pos = content2.find(',');
    while (pos != string::npos) {
        content2.erase(0, pos + 1); //FLAG FOR SKIPPING FIRST WORD AND COMMA
        pos = content2.find(',');
        synonym = content2.substr(0, pos); 
        insertNode(&head, synonym);
    }
        //INSERT LAST SYNONYM
        insertNode(&head, content2);
        //CALLING QUICKSORT
        quickSort(&head);
        //DISPLAY SORTED SYNONYMS
        cout << "Synonyms: " << endl;
        displayList(head);
    }
    else {
        cout << "No synonyms found." << endl;
    }
    return 0;
}