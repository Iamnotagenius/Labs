#include <curl/curl.h>
#include <curl/options.h>
#include <json/json.h>
#include <iostream>
#include <iomanip>
#include <map>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "tableprinter.hpp"
#include "currency.hpp"

#define URL "https://www.cbr-xml-daily.ru/daily_json.js"
#define CODE_LEN 3


static struct {
    const std::map<std::string, int> duration_ratios = {
        {"s", 1},
        {"m", 60},
        {"h", 3600},
        {"d", 86400},
        {"w", 604800},
        {"M", 2629746},
        {"y", 31556952}
    };
    std::chrono::seconds interval = std::chrono::seconds(10);
    int precision = 2;
    enum { NORMAL, LIST, ALL } mode = NORMAL;
    std::vector<std::string> currencies = std::vector<std::string>({"USD", "EUR"});
} OPTIONS;

int WriteData(char *data, size_t size, size_t nmemb,
              std::string *buffer)
{
    if(buffer == NULL)
        return 0;

    buffer->append(data, size*nmemb);
    return size * nmemb;
}

static bool __is_exit = false;

void listen_for_quit_key() {
    while (true) {
        if (std::getchar())
            __is_exit = true;
    }
}

int main(int argc, char *argv[]) {
    using namespace Currency;
    // parse arguments

    if (argc > 1) {
        std::vector<std::string> args(argv + 1, argv + argc);
        if (args[0] == "-h" || args[0] == "--help") {
            std::cout << "Usage: " << argv[0] << " [CUR1,CUR2,CUR3,...,CURN | --list | --all]" << std::endl
            << "Use --list option to check for a list of available currencies." << std::endl
            << "Also there's --all option if you're an 80lvl financist." << std::endl
            << "With no option, it's equivalent to '" << argv[0] << " USD,EUR'" << std::endl
            << "You can set desired precision with '--precision VALUE' option" << std::endl
            << "Specify --interval TIME to set update period" << std::endl
            << "Format is <val>s, where <val> is an integer, and 's' could be one of the following:" << std::endl
            << "<No suffix>, s -- seconds" << std::endl
            << "             m -- minutes" << std::endl
            << "             h -- hours" << std::endl
            << "             d -- days" << std::endl
            << "             w -- weeks" << std::endl
            << "             M -- months" << std::endl
            << "             y -- years" << std::endl;
            return EXIT_SUCCESS;
        }

        auto precision_arg = std::find(args.begin(), args.end(), "--precision");
        if (precision_arg != args.end()) {
            if (precision_arg + 1 == args.end()) {
                std::cerr << "No precision.\n";
                return EXIT_FAILURE;
            }
            OPTIONS.precision = std::stoi(*(precision_arg + 1));
        }

        auto interval_arg = std::find(args.begin(), args.end(), "--interval");
        if (interval_arg != args.end()) {
            if (interval_arg + 1 == args.end()) {
                std::cerr << "No interval.\n";
                return EXIT_FAILURE;
            }
            std::istringstream is(*(interval_arg + 1));
            std::string suffix;
            int val;
            is >> val >> suffix;
            if (is.fail() || !OPTIONS.duration_ratios.contains(suffix)) {
                std::cerr << "Bad format for interval.\n";\
                return EXIT_FAILURE;
            }

            OPTIONS.interval = std::chrono::seconds(val * OPTIONS.duration_ratios.at(suffix));

        }

        if (args[0] == "--list") {
            OPTIONS.mode = OPTIONS.LIST;
        }
        if (args[0] == "--all") {
            OPTIONS.mode = OPTIONS.ALL;
        }
        if (OPTIONS.mode == OPTIONS.NORMAL && args[0][0] != '-') {
            OPTIONS.currencies.clear();
            std::istringstream str(args[0]);
            for (std::array<char, 4> cur; str.getline(&cur[0], 4, ','); ) {
                OPTIONS.currencies.push_back(&cur[0]);
            }
        }

    }
    auto info = CurrencyInfo(OPTIONS.currencies);
    std::cout.setf(std::ios::left, std::ios::adjustfield);
    CURL *curl;
    CURLcode result;
    std::string *buffer;
    std::thread keypress(listen_for_quit_key);
    keypress.detach();
    std::string *revert = nullptr;
    unsigned long max_value_len;
    if (OPTIONS.mode == OPTIONS.NORMAL || OPTIONS.mode == OPTIONS.ALL)
        std::cout << "Press Enter to stop the program.\n";

    std::cout << std::fixed << std::setprecision(OPTIONS.precision);


    while (true) {
        // do get request
        buffer = new std::string();
        curl = curl_easy_init();
        if(!curl) {
            std::cerr << "CURL Connection failed.\n";
            return EXIT_FAILURE;
        }

        curl_easy_setopt(curl, CURLOPT_URL, URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

        result = curl_easy_perform(curl);
        if(result != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(result));
        }
        curl_easy_cleanup(curl);

        //parse a json
        Json::Value root;
        Json::CharReaderBuilder builder;
        JSONCPP_STRING err;
        const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

        if (!reader->parse(buffer->c_str(), buffer->c_str() + buffer->length(), &root, &err)) {
            std::cerr << "Error parsing JSON.\n";
            return EXIT_FAILURE;
        }
        delete buffer;

        // Output data
        root = root["Valute"];

        if (OPTIONS.mode == OPTIONS.LIST) {
            auto currencies = root.getMemberNames();
            for (auto cur : currencies) {
                std::cout << root[cur]["Name"].asString() << " - " << cur << std::endl;
            }
            return EXIT_SUCCESS;
        }
        max_value_len = [&root]() -> unsigned long {
            auto lengths = std::vector<int>();
            std::transform(OPTIONS.currencies.begin(), OPTIONS.currencies.end(),
                std::back_inserter(lengths),
                [&root](std::string cur){
                    std::stringstream str;
                    str << std::fixed << std::setprecision(OPTIONS.precision) << root[cur]["Value"].asFloat();
                    return str.str().length();
                });
            return *std::max_element(lengths.begin(), lengths.end());
        }();

        auto printer = TablePrinter<2>(CODE_LEN, max_value_len);
        if (OPTIONS.mode == OPTIONS.ALL) {
            OPTIONS.currencies.clear();
            OPTIONS.currencies = root.getMemberNames();
        }

        auto something_wrong = std::vector<std::string>();
        printer();
        for (auto currency : OPTIONS.currencies) {
            auto value = root[currency]["Value"].asFloat();
            if (value != 0) {
                printer(currency, value);
                printer();
                info.Push(currency, value);
            }
            else {
                something_wrong.push_back(currency);
            }
        }
        if (!something_wrong.empty()) {
            std::cout << "There are some currencies (";
            for (auto cur = something_wrong.begin(); cur != something_wrong.end() - 1; ++cur) {
                std::cout << *cur << ", ";
            }
            std::cout << something_wrong[something_wrong.size() - 1] << ") which has the value of 0" << std::endl;
            std::cout << "You probably mistyped it or it is not tracked by russian central bank (unless ruble has gone too high that there's not enough precision but it's waaaay too unlikely or you are looking for currency no one cares about)." << std::endl <<
                "Use --list option to check for a list of available currencies." << std::endl <<
                "Also there's --all option if you're an 80lvl financist." << std::endl;
            return EXIT_FAILURE;
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000ms);

        if (__is_exit)
            break;

        if (revert == nullptr) {
            revert = new std::string();
            for (size_t i = 0; i < OPTIONS.currencies.size() * 2 + 1; ++i) {
                revert->append("\033[A");
            }
            revert->append("\r");
        }
        std::cout << *revert;
    }

    // Print summary
    auto median_width = std::max(6UL, max_value_len);
    auto average_width = std::max(7UL, max_value_len);
    auto summary = TablePrinter<3>(6, average_width, median_width);
    summary();
    summary("Valute", "Average", "Median");
    summary();
    for (auto stats : info) {
        summary(stats.currency, stats.average, stats.median);
    }
    summary();
    return EXIT_SUCCESS;
}
