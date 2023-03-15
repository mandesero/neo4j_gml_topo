#include <iostream>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>

int main()
{
    // Initialize CURL
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    // Set CURL options
    std::string url = "neo4j://localhost:7687";
    std::string query = "CREATE {:Switch {dpid: 3}};";
    std::string data = "{\"statements\":[{\"statement\":\"" + query + "\"}]}";
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    // Send request and receive response
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](char* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
        ((std::string*)userdata)->append(ptr, size * nmemb);
        return size * nmemb;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);

    // Parse JSON response
    Json::Value root;
    Json::Reader reader;
    bool success = reader.parse(response, root);
    if (!success) {
        std::cerr << "Failed to parse JSON response" << std::endl;
        return 1;
    }
    Json::Value results = root["results"][0]["data"];
    for (const auto& result : results) {
        Json::Value node = result["row"][0];
        std::cout << node << std::endl;
    }

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}