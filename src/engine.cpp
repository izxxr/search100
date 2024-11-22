/* Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025 */

#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <fstream>
#include <set>
#include <tuple>
#include <json.hpp>
#include <stemming.cpp>

#ifndef _SEARCH100_ENGINE
#define _SEARCH100_ENGINE


/**
 * @brief Describes search result for a specific term in query.
 * 
 */
class SearchResult
{
    public:

    /**
     * @brief The term in search query that this result refers to.
     */
    Stem query_term;

    /**
     * @brief The ID of document that this result refers to.
     */
    int document_id;

    /**
     * @brief The relevance (TF-IDF) score of the result.
     */
    double relevance_score;
    
    /**
     * @brief The occurrences of the searched term in the given document.
     */
    std::vector<Occurrence> occurrences;
};


/**
 * @brief The core search engine class.
 * 
 * This class manages all the searching and indexing processes and
 * keeps the indices cache.
 */
class SearchEngine
{
    /* The path pointing to directory containing the documents (or text files) to be searched. */
    std::filesystem::path corpus_directory_path;

    /* Maps document ID to path of that document. */
    std::map<int, std::filesystem::path> documents;

    /**
     * @brief Maps document ID to a map of all terms in that document.
     * 
     * If term1 and term2 occurs in document with document_id, then the mapping
     * looks like the following:
     * 
     * { document_id: {term1: [PositionAwareStem], term2: [PositionAwareStem] } }
     * 
     * */
    std::map<int, std::map<std::string, std::vector<Occurrence>>> term_occurrences;

    /* Maps a term to vector of document IDs in which it occurs. */
    std::map<std::string, std::set<int>> term_documents;

    /* Used to track largest document IDs */
    int doc_id_tracker = -1;

    /**
     * @brief Loads indexes from data on disk.
     */
    void loadFromFiles() {
        nlohmann::json documents_json = readJSON("documents.json");
        nlohmann::json term_occurrences_json = readJSON("term_occurrences.json");
        nlohmann::json term_documents_json = readJSON("term_documents.json");

        for (nlohmann::json::iterator iter = documents_json.begin(); iter != documents_json.end(); ++iter) {
            int document_id = iter.value();
            documents[document_id] = std::filesystem::path(iter.key());

            for (auto &[term, occurrences] : term_occurrences_json[std::to_string(document_id)].items())
            {
                for (auto &occurrence : occurrences)
                {
                    Occurrence parsed;
                    parsed.document_id = document_id;
                    parsed.stemmed = term;
                    parsed.original = occurrence["original"];
                    parsed.index = occurrence["index"];
                    parsed.line = occurrence["line"];
                    term_occurrences[document_id][term].push_back(parsed);
                }
            }
        }

        term_documents = term_documents_json.get<std::map<std::string, std::set<int>>>();
    }

    /**
     * @brief Indexes the given file.
     * 
     * @param file: The path object for file to index.
     * @param documents_json: The JSON object to add document data to.
     * @param term_occurrences_json: The JSON object to add terms data to.
     * @param term_documents_json: The JSON object to add occurrence documents data to.
     */
    void indexDocument(
        const std::filesystem::directory_entry &file,
        nlohmann::json &documents_json,
        nlohmann::json &term_occurrences_json,
        nlohmann::json &term_documents_json
    )
    {
        std::filesystem::path path(file.path());
        std::ifstream fs(path);
        std::string line;
        PorterStemmer stemmer;

        int lineno = 0;
        int document_id = ++doc_id_tracker;
        std::string document_id_str = std::to_string(document_id);  // for JSON

        documents[document_id] = path;
        term_occurrences[document_id] = {};

        documents_json[path.string()] = document_id;
        term_occurrences_json[document_id_str] = std::map<std::string, std::vector<nlohmann::json>>();
        auto &doc_term_occurrences = term_occurrences_json[document_id_str];

        while (getline(fs, line))
        {
            std::vector<Stem> stems = stemmer.stemLine(line);
            for (Stem stem : stems)
            {
                Occurrence occ = Occurrence::fromStem(stem, document_id, lineno);
                term_occurrences[document_id][stem.stemmed].push_back(occ);
                term_documents[stem.stemmed].insert(document_id);

                if (!doc_term_occurrences.count(stem.stemmed))
                    doc_term_occurrences[stem.stemmed] = std::vector<nlohmann::json>{};

                doc_term_occurrences[stem.stemmed].push_back(occ.toJSON());
                term_documents_json[stem.stemmed] = term_documents[stem.stemmed];
            }
            lineno++;
        }
    }


    /**
     * @brief Writes the given JSON object to file at given path.
     * 
     * @param filename: The name of file to write into.
     * @param obj: The JSON object to write.
     * 
     * @returns `nlohmann::json` - the parsed JSON.
     */
    void writeJSON(const std::string filename, const nlohmann::json &obj)
    {
        std::ofstream fs(filename);
        fs << obj << std::endl;
        fs.close();
    }

    /**
     * @brief Reads the given JSON file.
     * 
     * @param filename: The name of file to read from.
     * 
     * @returns `nlohmann::json` - the parsed JSON.
     */
    nlohmann::json readJSON(const std::string filename)
    {
        std::ifstream fs(filename);
        return nlohmann::json::parse(fs);
    }

    /**
     * @brief Computes the term frequency (TF) of a term in a document.
     * 
     * TF(t, d) = (number of times t occurs in d) / (total number of terms in d)
     * 
     * t: the targeted term
     * d: the targeted document
     * 
     * TF is the measure of how frequent a term occurs in a document. The
     * value ranges between 0-1 with higher values indicating higher frequency.
     * 
     * https://en.wikipedia.org/wiki/Tf%E2%80%93idf#Term_frequency
     * 
     * @param term: The stemmed term to find TF for.
     * @param document_id: The ID of document to find TF of term in.
     * 
     * @returns double - TF value.
     */
    double computeTF(std::string term, int document_id)
    {
        double term_freq = (double)(term_occurrences[document_id][term].size());
        double total_terms = (double)(term_occurrences[document_id].size());

        return term_freq / total_terms;
    }

    /**
     * @brief Computes the inverse document frequency (IDF) value for given term.
     * 
     * IDF(t) = (number of documents in corpus) / (number of documents containing t)
     * 
     * t: the targeted term
     * 
     * IDF is measure of how rare the term is in corpus documents. Higher IDF value
     * for a term indicates that the term is rare and lower value indicates less rare
     * or common terms.
     * 
     * https://en.wikipedia.org/wiki/Tf%E2%80%93idf#Inverse_document_frequency
     * 
     * @param term: The stemmed term to find IDF for.
     * 
     * @returns double - IDF value.
     */
    double computeIDF(std::string term)
    {
        double total_docs = (double)term_documents.size();
        double df;

        if (!term_documents.count(term))
            df = 0.0;
        else
            df = (double)term_documents[term].size();

        // The increment is done to prevent zero division.
        return std::log((total_docs) / (df + 1));
    }

    /**
     * @brief Computes the TF-IDF value for given term in given document.
     * 
     * TF-IDF is measure for how relevant a term is in a document. That is,
     * terms with higher TF-IDF values are ranked higher in search results
     * as such terms are more relevant. TF-IDF is therefore referred to as
     * relevance or importance score.
     * 
     * https://en.wikipedia.org/wiki/Tf%E2%80%93idf#Term_frequency%E2%80%93inverse_document_frequency
     * 
     * @param term: The stemmed term to find IDF for.
     * @param document_id: The ID of document to find IDF of term in.
     * 
     * @returns double - TF-IDF value.
     */
    double computeTfIdf(std::string term, int document_id)
    {
        double tf = computeTF(term, document_id);
        double idf = computeIDF(term);

        return (idf * tf);
    }

    /**
     * @brief Finds the common documents in which all searched terms occur.
     * 
     * This method is used when searching is performed using 'AND' strategy, that
     * is, only documents that have all of the searched terms are returned.
     * 
     * @param query_terms: The searched terms.
     * 
     * @returns set<int> - the document IDs.
     */
    std::set<int> findCommonDocuments(std::vector<Stem> &query_terms)
    {
        std::set<int> common_document_ids;
        std::set<int> inserter_set;

        for (auto &term : query_terms)
        {
            auto &term_document_ids = term_documents[term.stemmed];

            if (common_document_ids.empty())
            {
                std::set_union(
                    common_document_ids.begin(),
                    common_document_ids.end(),
                    term_document_ids.begin(),
                    term_document_ids.end(),
                    std::inserter(inserter_set, inserter_set.begin())
                );
            }
            else
            {
                std::set_intersection(
                    common_document_ids.begin(),
                    common_document_ids.end(),
                    term_document_ids.begin(),
                    term_document_ids.end(),
                    std::inserter(inserter_set, inserter_set.begin())
                );
            }

            common_document_ids = inserter_set;
            inserter_set.clear();
        }

        return common_document_ids;
    }

    /**
     * @brief Gets relevance scores for each document in which the searched term occurs.
     * 
     * If `search_strategy_and` is true, only the documents in which all searched terms
     * occur are returned. In contrary case, the documents that have any of searched terms
     * are returned.
     * 
     * @param query_terms: Vector of searched terms.
     * @param search_strategy_and: Whether to use 'AND' strategy. If false, uses 'OR' strategy.
     * 
     * @returns vector<tuple<Stem, int, double>> - vector of 3-tuples each value representing
     * searched term, its document ID, and relevance score respectively.
     */
    std::vector<std::tuple<Stem, int, double>> getRelevantScores(std::vector<Stem> &query_terms, bool search_strategy_and = true)
    {
        std::vector<std::tuple<Stem, int, double>> relevance_scores;
        std::set<int> document_ids;

        if (search_strategy_and)
            document_ids = findCommonDocuments(query_terms);

        for (auto &term : query_terms)
        {
            if (!search_strategy_and)
                document_ids = term_documents[term.stemmed];

            for (int document_id : document_ids)
            {
                auto tup = std::make_tuple(term, document_id, computeTfIdf(term.stemmed, document_id));
                relevance_scores.push_back(tup);
            }
        }

        std::sort(
            relevance_scores.begin(),
            relevance_scores.end(),
            [](const std::tuple<Stem, int, double> &a, std::tuple<Stem, int, double> &b)
            {
                return std::get<2>(a) > std::get<2>(b);
            }
        );

        return relevance_scores;
    }

    public:

    /**
     * @brief Search engine constructor
     * 
     * @param corpus_directory_path_str: The path of corpus directory.
     */
    SearchEngine(std::string corpus_directory_path_str)
    {
        corpus_directory_path = std::filesystem::path(corpus_directory_path_str);
        if (corpus_directory_path.has_filename())
            throw "corpus_directory_path_str must be a directory, not a file.";
    }

    /**
     * @brief Index the documents in corpus directory.
     * 
     * This method will first check whether local data of indexes is
     * available. If so, the data will be loaded. In case data is not
     * available, the files are indexed and indexes are stored locally.
     * 
     * @param useData: If true (default), the local indexes data is used
     * to load indexes in memory if available. If false, even if data is
     * available, the indexes are regenerated from corpus.
     * 
     */
    void indexCorpusDirectory(bool useData = true)
    {
        log("Finding local documents index...");

        if (checkFileExists("term_occurrences.json") && checkFileExists("term_documents.json")
            && checkFileExists("documents.json") && useData)
        {
            log("Loading local indexes...");
            loadFromFiles();

            log("Successfully loaded indexes for " + std::to_string(getIndexSize()) + " documents.");
            return;
        }

        log("No local indexes found.");
        log("Indexing corpus directory...");

        nlohmann::json documents_json;
        nlohmann::json term_occurrences_json;
        nlohmann::json term_documents_json;

        for (auto &file : std::filesystem::recursive_directory_iterator(corpus_directory_path))
        {
            std::filesystem::path fp = file.path();
            if (fp.extension().string() != ".txt")
                continue;

            indexDocument(file, documents_json, term_occurrences_json, term_documents_json);
            log(fp.string() + " - DONE", "", false, 1);
        }

        if (!getIndexSize())
        {
            log(
                "No searchable text documents. Place text files to be searched in "
                + corpus_directory_path.string() + " directory and restart Search100!",
                "WARNING"
            );
            return;
        }

        log("Writing index data to disk...");
        writeJSON("documents.json", documents_json);
        writeJSON("term_occurrences.json", term_occurrences_json);
        writeJSON("term_documents.json", term_documents_json);
        log("Successfully indexed " + std::to_string(getIndexSize()) + " documents...");
    }

    /**
     * @brief The number of documents stored in loaded indexes.
     * 
     * @returns int - the index size.
     */
    int getIndexSize()
    {
        return documents.size();
    }

    /**
     * @brief Get a document's path by its ID.
     * 
     * If the document ID is invalid and no such document for that ID
     * exists then an error is thrown with integer value -1.
     * 
     * @param document_id: The ID of document to get path of.
     * 
     * @returns filesystem::path - the path object.
     */
    std::filesystem::path getDocumentPath(int document_id)
    {
        if (!documents.count(document_id))
            throw -1;

        return documents[document_id];
    }

    /**
     * @brief Performs a search query.
     * 
     * If `search_strategy_and` is true, only the documents in which all searched terms
     * occur are returned. In contrary case, the documents that have any of searched terms
     * are returned.
     * 
     * @param query: The search query as string.
     * @param search_strategy_and: Whether to use 'AND' strategy. If false, uses 'OR' strategy.
     * 
     * @returns vector<SearchResult> - sequence of search results, sorted in descending order
     * of relevance.
     */
    std::vector<SearchResult> search(std::string query, bool search_strategy_and = true)
    {
        PorterStemmer stemmer;
        auto terms = stemmer.stemLine(query);

        if (terms.empty())
        {
            log("Terms are not enough for query.");
            return std::vector<SearchResult>{};
        }

        auto relevance_scores = getRelevantScores(terms, search_strategy_and);

        std::vector<SearchResult> results;

        for (auto &[stem, document_id, score] : relevance_scores)
        {
            auto &occurrences = term_occurrences[document_id][stem.stemmed];
            SearchResult result;

            result.document_id = document_id;
            result.query_term = stem;
            result.relevance_score = score;
            result.occurrences = occurrences;

            results.push_back(result);
        }

        return results;
    }
};

#endif
