#include "Formatter.h"

#include <assert.h>

using std::string;

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

Formatter &Formatter::operator<<(const string &out) {
    const size_t len = out.length();
    size_t start = 0;
    while (start < len) {
        size_t pos = out.find("\n", start);

        if (pos == string::npos) {
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
