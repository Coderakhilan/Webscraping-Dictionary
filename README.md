# Webscraping-Dictionary
This GitHub repository contains a C++ implementation for web scraping and sorting synonyms. The code utilizes the libcurl library to fetch data from web pages and performs sorting operations on a linked list.
# Basic Functionality
The code takes the word input from the user and uses it to navigate to the Merriam Webster dictionary where it webscrapes the HTML contents and locates the class holding the description and synonym of the word. It displays locally in the terminal after code execution.
# Working in detail
The following below is the function that holds the scraped data.
```
struct ScrapedData {
    string description;
    string synonyms;
};
```
