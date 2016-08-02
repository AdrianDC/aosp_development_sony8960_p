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

            fprintf(mFile, "%s", out.substr(start).c_str());
            break;
        }

        if (pos == start) {
            fprintf(mFile, "\n");
            mAtStartOfLine = true;
        } else if (pos > start) {
            if (mAtStartOfLine) {
                fprintf(mFile, "%*s", (int)(2 * mIndentDepth), "");
            }

            fprintf(mFile, "%s", out.substr(start, pos - start + 1).c_str());

            mAtStartOfLine = true;
        }

        start = pos + 1;
    }

    return *this;
}

Formatter &Formatter::operator<<(size_t n) {
    return (*this) << std::to_string(n);
}

}  // namespace android
