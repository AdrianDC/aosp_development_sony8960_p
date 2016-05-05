#ifndef I_HIDL_BINDER_MappableRegion_H
#define I_HIDL_BINDER_MappableRegion_H

#include "binder/Parcel.h"
#include "binder/Parcelable.h"

namespace android {
    namespace service {
        namespace media {
            namespace rtstreams {

                class MappableRegion : public Parcelable {

                public:
                    enum class MappingType : int32_t {
                      UNDEFINED_MAPPING, ASHMEM_MAPPING, FILE_MAPPING
                    };

                    MappableRegion() {}


                    MappableRegion(int fd, MappingType regionType, off64_t offset, size_t size) : mFd(
                            fd), mRegionType(regionType), mOffset(offset), mSize(size) { }

                    int getFd() const {
                        return mFd;
                    }

                    MappingType getRegionType() const {
                        return mRegionType;
                    }

                    off64_t getOffset() const {
                        return mOffset;
                    }

                    size_t getSize() const {
                        return mSize;
                    }

                    bool operator == (const MappableRegion& other) const {
                        return (mFd == other.mFd
                                && mRegionType == other.mRegionType
                                && mOffset == other.mOffset
                                && mSize == other.mSize);
                    }

                    bool isValid() {
                        return mRegionType != MappingType::UNDEFINED_MAPPING;
                    }


                    virtual status_t writeToParcel(Parcel* parcel) const override {
                        parcel->writeFileDescriptor(mFd, false);
                        parcel->writeInt32((int32_t) mRegionType);
                        parcel->writeInt64(mOffset);
                        parcel->writeUint64(mSize);
                        return parcel->errorCheck();
                    }

                    virtual status_t readFromParcel(const Parcel* parcel) override {
                        mFd = parcel->readFileDescriptor();
                        mRegionType = (MappingType) parcel->readInt32();
                        mOffset = parcel->readInt64();
                        mSize = parcel->readUint64();

                        return parcel->errorCheck();
                    }

                private:
                    int mFd = -1;
                    MappingType mRegionType = MappingType::UNDEFINED_MAPPING;
                    off64_t mOffset;
                    size_t mSize = 0;
                };

            }
        }
    }
}
#endif // I_HIDL_BINDER_MappableRegion_H
