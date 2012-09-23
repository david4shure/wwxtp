

typedef struct Test_Entry {
    const char* id;
    const char* city;
    const char* name;
    const char* where;
} Test_Entry;

Test_Entry test_database [9] = {
    {"IZA", "SANTA YNEZ, CA",      "SANTA YNEZ AIRPORT",               "-"},
    {"SBA", "SANTA BARBARA, CA",   "SANTA BARBARA MUNICIPAL AIRPORT",  "15.9 nm SE"},
    {"LPC", "LOMPOC, CA",          "LOMPOC AIRPORT",                   "19.7 nm W"},
    {"SMX", "SANTA MARIA, CA",     "SANTA MARIA PUB/CAPT G ALLAN HANCOCK FIELD AIRPORT", "25.8 nm NW"},
    {"L88", "NEW CUYAMA, CA",      "NEW CUYAMA AIRPORT",               "27.7 nm NE"},
    {"L52", "OCEANO, CA",          "OCEANO COUNTY AIRPORT",            "40.1 nm NW"},
    {"L17", "TAFT, CA",            "TAFT-KERN COUNTY AIRPORT",         "44.9 nm NE"},
    {"SBP", "SAN LUIS OBISPO, CA", "SAN LUIS COUNTY REGIONAL AIRPORT", "47.0 nm NW"},
    {"OXR", "OXNARD, CA",          "OXNARD AIRPORT",                   "49.4 nm ESE"}
};



