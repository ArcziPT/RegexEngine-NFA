# RegexEngine-NFA (C++17)
Standalone library (using only standard c++17 library) for parsing and exceuting regular expressions by simulating NFA.
Supported:
* \+ - once or more
* \. - any
* \* - zero or more
* [a-zABC] [^A-Z]- sets and negations

### Usage
```c++
#include "Regex.hpp"

Regex regex{"(a|b)+[d-g]x*"}; //create regex object
regex.check("abadaf"); //check if whole string matches regex
regex.one_match("zaadxzzzadx"); //find first match in string
regex.all_matches("zaadxzzzadx"); //find all matches is string
```