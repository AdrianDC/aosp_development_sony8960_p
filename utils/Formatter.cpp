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

#include "Formatter.h"

#include <assert.h>

namespace android {

Formatter::Formatter(FILE *file)
    : mFile(file == NULL ? stdout : file),
      mIndentDepth(0),
      mAtStartOfLine(true) {
}

Formatter::~Formatter() {
    if (mFile != stdout) {
        fclose(mFile);
    }
    mFile = NULL;
}

void Formatter::indent() {
    ++mIndentDepth;
}

void Formatter::unindent() {
    assert(mIndentDepth > 0);
    --mIndentDepth;
}

Formatter &Formatter::operator<<(const std::string &out) {
    const size_t len = out.length();
    size_t start = 0;
    while (start < len) {
        size_t pos = out.find("\n", start);

        if (pos == std::string::npos) {
            if (mAtStartOfLine) {
                fprintf(mFile, "%*s", (int)(2 * mIndentDepth), "");
                mAtStartOfLine = false;
            }

            output(out.substr(start));
            break;
        }

        if (pos == start) {
            fprintf(mFile, "\n");
            mAtStartOfLine = true;
        } else if (pos > start) {
            if (mAtStartOfLine) {
                fprintf(mFile, "%*s", (int)(2 * mIndentDepth), "");
            }

            output(out.substr(start, pos - start + 1));

            mAtStartOfLine = true;
        }

        start = pos + 1;
    }

    return *this;
}

Formatter &Formatter::operator<<(size_t n) {
    return (*this) << std::to_string(n);
}

void Formatter::setNamespace(const std::string &space) {
    mSpace = space;
}

void Formatter::output(const std::string &text) const {
    const size_t spaceLength = mSpace.size();
    if (spaceLength > 0) {
        // Remove all occurences of "mSpace" and output the filtered result.
        size_t matchPos = text.find(mSpace);
        if (matchPos != std::string::npos) {
            std::string newText = text.substr(0, matchPos);
            size_t startPos = matchPos + spaceLength;
            while ((matchPos = text.find(mSpace, startPos))
                    != std::string::npos) {
                newText.append(text.substr(startPos, matchPos - startPos));
                startPos = matchPos + spaceLength;
            }
            newText.append(text.substr(startPos));
            fprintf(mFile, "%s", newText.c_str());
            return;
        }
    }

    fprintf(mFile, "%s", text.c_str());
}

}  // namespace android
