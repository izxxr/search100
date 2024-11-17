/* Copyright (C) Izhar Ahmad & Mustafa Hussain Qizilbash, 2024-2025 */

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <json.hpp>
#include <stemming.cpp>

#ifndef _SEARCH100_ENGINE
#define _SEARCH100_ENGINE


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
    std::map<int, std::map<std::string, std::vector<PositionAwareStem>>> term_occurences;

    /* Maps a term to vector of document IDs in which it occurs. */
    std::map<std::string, std::vector<int>> term_documents;

    /* Used to track largest document IDs */
    int doc_id_tracker = -1;

    void loadFromFiles() {
        nlohmann::json documents_json = readJSON("documents.json");
        nlohmann::json term_occurences_json = readJSON("term_occurences.json");
        nlohmann::json term_documents_json = readJSON("term_documents.json");

        for (nlohmann::json::iterator iter = documents_json.begin(); iter != documents_json.end(); ++iter) {
            int document_id = iter.value();
            documents[document_id] = std::filesystem::path(iter.key());

            for (auto &[term, occurences] : term_occurences_json[std::to_string(document_id)].items())
            {
                for (auto &occurence : occurences)
                {
                    PositionAwareStem stem;
                    stem.document_id = document_id;
                    stem.stemmed = term;
                    stem.original = occurence["original"];
                    stem.index = occurence["index"];
                    stem.row = occurence["row"];
                    term_occurences[document_id][term].push_back(stem);
                }
            }
        }

        term_documents = term_documents_json.get<std::map<std::string, std::vector<int>>>();
    }

    void indexDocument(
        const std::filesystem::directory_entry &file,
        nlohmann::json &documents_json,
        nlohmann::json &term_occurences_json,
        nlohmann::json &term_documents_json
    )
    {
        std::filesystem::path path(file.path());
        std::ifstream fs(path);
        std::string line;
        PorterStemmer stemmer;

        int row = 0;
        int document_id = ++doc_id_tracker;
        std::string document_id_str = std::to_string(document_id);  // for JSON

        documents[document_id] = path;
        term_occurences[document_id] = {};

        documents_json[path.string()] = document_id;
        term_occurences_json[document_id_str] = std::map<std::string, std::vector<nlohmann::json>>();
        auto &doc_term_occurences = term_occurences_json[document_id_str];

        while (getline(fs, line))
        {
            std::vector<PositionAwareStem> stems = stemmer.stemSentence(line, row, document_id);
            for (PositionAwareStem stem : stems)
            {
                term_occurences[document_id][stem.stemmed].push_back(stem);
                term_documents[stem.stemmed].push_back(document_id);

                if (!term_documents_json.count(stem.stemmed))
                    term_documents_json[stem.stemmed] = std::vector<int>{};
                if (!doc_term_occurences.count(stem.stemmed))
                    doc_term_occurences[stem.stemmed] = std::vector<nlohmann::json>{};

                doc_term_occurences[stem.stemmed].push_back(stem.toJSON());
                term_documents_json[stem.stemmed].push_back(document_id);
            }
            row++;
        }
    }

    void writeJSON(const std::string &filename, const nlohmann::json &obj)
    {
        std::ofstream fs(filename);
        fs << obj << std::endl;
        fs.close();
    }

    nlohmann::json readJSON(const std::string &filename)
    {
        std::ifstream fs(filename);
        return nlohmann::json::parse(fs);
    }

    public:

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
     * @param useData: if true (default), the local indexes data is used
     * to load indexes in memory if available.
     * 
     */
    void indexCorpusDirectory(bool useData = true)
    {
        log("Finding local documents index...");

        if (checkFileExists("term_occurences.json") && checkFileExists("term_documents.json")
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
        nlohmann::json term_occurences_json;
        nlohmann::json term_documents_json;

        for (auto &file : std::filesystem::recursive_directory_iterator(corpus_directory_path))
        {
            std::filesystem::path fp = file.path();
            if (fp.extension().string() != ".txt")
                continue;

            indexDocument(file, documents_json, term_occurences_json, term_documents_json);
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
        writeJSON("term_occurences.json", term_occurences_json);
        writeJSON("term_documents.json", term_documents_json);
        log("Successfully indexed " + std::to_string(getIndexSize()) + " documents...");
    }

    /**
     * @brief The number of documents stored in loaded indexes.
     */
    int getIndexSize()
    {
        return documents.size();
    }
};

#endif
