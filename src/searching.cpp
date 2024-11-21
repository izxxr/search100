#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <vector>

using namespace std;

vector<string> prepocessQuery(const string& query)
{
    vector<string> tokens;
    string word = "";
    for (char ch : query)
    {
        if(isalnum(ch))
        {
            word += tolower(ch); // normalize whole string to lowercase
        }
        else if(!word.empty())
        {
            tokens.push_back(word);
            word = "";
        }
    }
    if (!word.empty()){
        tokens.push_back(word);
    }
    return tokens;
}

double computeIDF(const string& term) {
    int docCount = invertedIndex[term].size(); // Number of documents containing the term
    if (docCount == 0) return 0; // Avoid division by zero
    return log((double)totalDocuments / (1 + docCount)); // Add 1 to avoid log(0)
}
map<string, double> scoreDocuments(const vector<string>& queryTerms) {
    map<string, double> scores; // Store scores for each document

    for (const string& term : queryTerms) {
        double idf = computeIDF(term); // Compute IDF for the term
        if (invertedIndex.count(term) == 0) continue; // Skip if term is not in index

        for (const auto& [docID, tf] : invertedIndex[term]) {
            scores[docID] += tf * idf; // Add TF-IDF score to the document
        }
    }

    return scores;
}
vector<pair<string, double>> rankResults(const map<string, double>& scores) {
    vector<pair<string, double>> rankedResults(scores.begin(), scores.end());

    // Sort by score in descending order
    sort(rankedResults.begin(), rankedResults.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    return rankedResults;
}
void search(const string& query) {
    vector<string> queryTerms = preprocessQuery(query); // Tokenize and normalize query
    map<string, double> scores = scoreDocuments(queryTerms); // Compute scores
    vector<pair<string, double>> results = rankResults(scores); // Rank results

    cout << "Search Results for \"" << query << "\":" << endl;
    for (const auto& [docID, score] : results) {
        cout << docID << " (score: " << score << ")" << endl; // Display results
    }
}
