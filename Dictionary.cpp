#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <tuple>
#include <queue>
#include <string>
#include <iomanip>
#include <cctype>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;
//Typedef Aliases
using WORD = string;  // Define WORD as a string type, representing a word in the dictionary.

using WORD_LIST = set<WORD>;  // Set used here
// WORD_LIST is a set that holds unique words (of type string). Duplicates are automatically removed.

using WORD_FREQ_LIST = map<char, int>;//map used here
// WORD_FREQ_LIST is a map where the key is a character (char), and the value is the frequency of that character (int). It stores letter frequencies.

using FREQ_WORD_LIST = multimap<int, WORD_LIST>; // multimap for storing words by frequency
// FREQ_WORD_LIST is a map where the key is an integer (int) representing the frequency, and the value is a WORD_LIST (set of words) that have that frequency.

using COUNTER = tuple<int, int, int>;  // Tuple used here
// COUNTER is a tuple holding three integers:
// - First: the total number of letters (int)
// - Second: the total number of words (int)
// - Third: the total number of lines (int)

using LONGEST = priority_queue<WORD>;
// LONGEST is a priority queue that holds WORDs (strings), with the longest words having the highest priority (longer words come first).

// Structs for holding all data
struct DICTION {
	COUNTER stats;                  // Tuple: (letters, words, lines)
	WORD_LIST words;                // Set of unique words
	WORD_FREQ_LIST wordFreq;        // Map of letter frequencies
	map<WORD, int> wordFrequency;   // Map of word frequencies
	LONGEST longWord;               // Priority queue for longest words
};

// Function to get the next word from the input stream
WORD getNextWord(istream& in) {
	WORD w;  // Declare a WORD (string) to hold the next word.
	char ch;  // Declare a char variable to read individual characters from the input stream.
	bool inWord = false;  // Boolean flag to indicate if we are currently inside a word.

	while (in.get(ch)) {  // Read characters one by one from the input stream.
		if (isalpha(ch)) {  // Check if the character is alphabetic (part of a word).
			w.push_back(ch);  // Add the character to the word.
			inWord = true;  // Set the inWord flag to true, indicating we are inside a word.
		}
		else if (inWord) {  // If we encounter a non-alphabetic character and are inside a word...
			return w;  // Return the word we have built so far.
		}
	}

	return w;  // Return the last word (could be empty if no word is found).
}

// Function to process the text file
void processText(DICTION& d, istream& in) {
	// Initialize frequencies for all letters
	for (char ch = 'a'; ch <= 'z'; ++ch) {
		d.wordFreq[ch] = 0;                     // Initialize lowercase letter frequency
		d.wordFreq[char(toupper(ch))] = 0;     // Initialize uppercase letter frequency
	}

	string line;                                // Holds each line from the file
	while (getline(in, line)) {                // Read file line by line
		get<2>(d.stats)++;                     // Increment line count (tuple used)
		istringstream lineStream(line);        // Stream to parse words in the line
		WORD word;                             // Holds each word
		while ((word = getNextWord(lineStream)) != "") { // Get words from line
			get<1>(d.stats)++;                 // Increment word count (tuple used)
			d.words.insert(word);              // Insert word into set (set used)
			d.wordFrequency[word]++;           // Increment word frequency
			d.longWord.push(word);             // Add word to priority queue
			for (char c : word) {              // Process each character in the word
				d.wordFreq[c]++;               // Increment character frequency (map used)
				get<0>(d.stats)++;             // Increment letter count (tuple used)
			}
		}
	}
}

// Function to display results
void displayResults(const DICTION& d) {
	cout << "/--------------\\\n";
	cout << "| Text Stats   |\n";
	cout << "\\--------------/\n";
	cout << "Number of Letters: " << get<0>(d.stats) << endl;  // Print letter count (tuple used)
	cout << "Number of Words: " << get<1>(d.stats) << endl;  // Print word count (tuple used)
	cout << "Number of Lines: " << get<2>(d.stats) << endl;  // Print line count (tuple used)

	cout << "\n/--------------\\\n";
	cout << "| Letter Freq  |\n";
	cout << "\\--------------/\n";

	for (char ch = 'a'; ch <= 'z'; ++ch) {      // Display frequencies for uppercase letters
		char upper = toupper(ch);
		cout << "[" << upper << "]" << " | ";
		for (int i = 0; i < min(d.wordFreq.at(upper), 10); ++i) cout << '*'; // Print stars for frequency
		if (d.wordFreq.at(upper) > 10) cout << " (" << (d.wordFreq.at(upper) - 10) << ")"; // Overflow count
		cout << endl;
	}

	for (char ch = 'a'; ch <= 'z'; ++ch) {      // Display frequencies for lowercase letters
		cout << "[" << ch << "]" << " | ";
		for (int i = 0; i < min(d.wordFreq.at(ch), 10); ++i) cout << '*'; // Print stars for frequency
		if (d.wordFreq.at(ch) > 10) cout << " (" << (d.wordFreq.at(ch) - 10) << ")"; // Overflow count
		cout << endl;
	}


	cout << "\n/--------------\\\n";
	cout << "| Dictionary   |\n";
	cout << "\\--------------/\n";
	cout << "Words in dictionary:\n";
	for (const auto& word : d.words) {          // Display all unique words (set used)
		cout << word << " ";
	}

	cout << "\n\nWord                               Frequencies:\n";
	cout << "----------------------------------------------------\n";
	vector<pair<int, WORD>> sortedWords;        // Vector of pairs to sort words
	for (const auto& [word, freq] : d.wordFrequency) {
		sortedWords.push_back({ freq, word });    // Add frequency-word pairs to vector
	}

	sort(sortedWords.begin(), sortedWords.end(), [](const pair<int, WORD>& a, const pair<int, WORD>& b) {
		if (a.second.length() == b.second.length())
			return a.second < b.second;        // Sort alphabetically if lengths are equal
		return a.second.length() < b.second.length(); // Sort by word length
		});

	for (const auto& [freq, word] : sortedWords) { // Display sorted words with frequencies
		cout << std::setw(35) << std::left << word << ": " << freq << '\n';
	}

	cout << "\n/--------------\\\n";
	cout << "| Histogram    |\n";
	cout << "\\--------------/\n";

	// Collect the maximum frequency for height calculation
	int maxFreq = 0;
	for (const auto& [freq, word] : sortedWords) {
		maxFreq = max(maxFreq, freq);
	}

	// Display the histogram upwards
	for (int level = maxFreq; level > 0; --level) {
		for (const auto& [freq, word] : sortedWords) {
			if (freq >= level)
				cout << " " << std::right << "*";
			else
				cout << " " << std::right << " ";
		}
		cout << endl;

	}
	cout << " ----------------------------------------\n\n";

	// Display the words 
	for (auto& [freq, word] : sortedWords) {
		cout << std::left << word << "------/";  // Left-align the words

		cout << endl;
	}


}

int main() {
	DICTION dict;                              // Holds all text analysis data
	ifstream file("data.txt");                // Open input file
	if (!file.is_open()) {                    // Check for file error
		cerr << "Error opening file!" << endl;
		return 1;
	}

	processText(dict, file);                  // Process the text file
	displayResults(dict);                     // Display the results


	return 0;
}
