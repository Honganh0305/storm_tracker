#include "utlis/HTTPClient.h"
#include <iostream>
#include <string>
#include <curl/curl.h>

static bool curlInitialized = false;

size_t HTTPClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    ((std::string*)userp)->append((char*)contents, total);
    return total;
}

std::string HTTPClient::GET(const std::string& url) {
    return GET(url, {});
}

std::string HTTPClient::GET(const std::string& url, const std::vector<std::string>& headers) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headerList = nullptr;
    for (const auto& h : headers) headerList = curl_slist_append(headerList, h.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headerList);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return "";
    return response;
}