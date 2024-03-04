# Webscraping-Dictionary
This GitHub repository contains a C++ implementation for web scraping and sorting synonyms. The code utilizes the libcurl library to fetch data from web pages and performs sorting operations on a linked list.
# Basic Functionality
The code takes the word input from the user and uses it to navigate to the Merriam Webster dictionary where it webscrapes the HTML contents and locates the class holding the description and synonym of the word. It displays locally in the terminal after code execution.
# Motivation
Needed a quick project that will use C++ and concepts learned from my Data Structures & Algorithms Course at my University whilst exploring a new area of interest web data extraction and HTML manipulation. Decided to make life hard and perhaps looking at it positively - interesting? by using C++ which only has two C libraries gumbo and libcurl for webscraping. Finally landed on libcurl API as gumbo parser has been discontinued since 2016.
# Pre-Requisites
You need to install curl for windows and the libcurl API. You also need Microsoft Visual Studio and CMake as making libcurl work alongside C++ in windows is extremely frustrating.
# Working in detail
First we build a function that holds the scraped data.
```
struct ScrapedData {
    string description;
    string synonyms;
};
```
Then we make a callback function that is used to handle the response data received from a HTTP request using libcurl.
```
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* buffer) {
    size_t totalSize = size * nmemb;
    buffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}
```
Next, we create a function prototype which is basically early declaring a function and telling the compiler how it will be used before actually defining the contents.
```
string extractContent(const string& html);
```
Now, we build the scraping function for using libcurl to perform a HTTP request to the website and retrieve the description content from the response.
```
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
```
We are going to extract content till the first full stop which is where the description of the word stops.
```
string content = extractContent(description);
    size_t pos = content.find('.');
    if (pos != string::npos) {
        description = content.substr(0, pos);
    }
    return description;
}
```
Now, we are gonna extract content at <meta name="description"...> in the HTML as that's where the description content of the word is stored. We will also remove the unwanted substring in the way.
```
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
```
Now, defining a structure for linked list node containing string 'data' and a pointer to the next node.
```
struct Node {
    string data;
    Node* next;

    Node(const string& value) : data(value), next(nullptr) {}
};
```
It's time to create the linked list functions and implement the quick sort algorithm.
```
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
```
Here is the part of the main function which will take input from the user for a word. Scrape the description of the word from Merriam-Webster's dictionary website using "scrapedescription".
```
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
```
After that, we scrape synonyms from the thesaurus page by the same method and construct a linked list holding the synonyms. Finally displaying the sorted synonyms using quick sort algorithm.
```
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
```
