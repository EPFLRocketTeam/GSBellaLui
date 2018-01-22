
#ifndef GS_MATTERHORN_EXCEPTIONUTILS_H
#define GS_MATTERHORN_EXCEPTIONUTILS_H

#include <stdexcept>

template<class T>
static void requireNonNull(T *ptr) {
    if (ptr == nullptr) {
        throw std::invalid_argument("Pointer was null");
    }
}

#endif //GS_MATTERHORN_EXCEPTIONUTILS_H