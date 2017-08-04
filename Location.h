/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LOCATION_H_
#define LOCATION_H_

#include <stdint.h>
#include <iostream>
#include <string>

// Mimics for yy::location and yy::position
namespace android {

struct Position {
    Position() = default;
    Position(std::string f, size_t l, size_t c)
            : mFilename(f), mLine(l), mColumn(c) {}
    inline const std::string &filename() const { return mFilename; }
    inline size_t line() const { return mLine; }
    inline size_t column() const { return mColumn; }

   private:
    // File name to which this position refers.
    std::string mFilename;
    // Current line number.
    size_t mLine;
    // Current column number.
    size_t mColumn;
};

inline std::ostream& operator<< (std::ostream& ostr, const Position& pos) {
    if (!pos.filename().empty()) {
        ostr << pos.filename() << ":";
    }
    return ostr << pos.line() << "." << pos.column();
}

struct Location {
    Location() = default;
    Location(const Position& begin, const Position& end) { setLocation(begin, end); }

    void setLocation(const Position& begin, const Position& end) {
        mIsValid = true;
        mBegin = begin;
        mEnd = end;
    }

    bool isValid() const { return mIsValid; }
    const Position& begin() const { return mBegin; }
    const Position& end() const { return mEnd; }

    static Location startOf(const std::string& path) {
        return Location(Position(path, 1, 1), Position(path, 1, 1));
    }

   private:
    bool mIsValid = false;

    // Beginning of the located region.
    Position mBegin;
    // End of the located region.
    Position mEnd;
};

inline std::ostream& operator<< (std::ostream& ostr, const Location& loc) {
    Position last = Position(loc.end().filename(),
            loc.end().line(),
            std::max<size_t>(1u, loc.end().column() - 1));
    ostr << loc.begin();
    if (loc.begin().filename() != last.filename()) {
        ostr << "-" << last;
    } else if (loc.begin().line() != last.line()) {
        ostr << "-" << last.line()  << "." << last.column();
    } else if (loc.begin().column() != last.column()) {
        ostr << "-" << last.column();
    }
    return ostr;
}

} // namespace android

#endif
