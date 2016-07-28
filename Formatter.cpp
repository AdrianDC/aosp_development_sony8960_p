#include "Formatter.h"

#include <assert.h>

using std::string;

namespace android {

Formatter::Formatter()
    : mIndentDepth(0),
      mAtStartOfLine(true) {
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
                printf("%*s", (int)(2 * mIndentDepth), "");
                mAtStartOfLine = false;
            }

            printf("%s", out.substr(start).c_str());
            break;
        }

        if (pos == start) {
            printf("\n");
            mAtStartOfLine = true;
        } else if (pos > start) {
            if (mAtStartOfLine) {
                printf("%*s", (int)(2 * mIndentDepth), "");
            }

            printf("%s", out.substr(start, pos - start + 1).c_str());

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
