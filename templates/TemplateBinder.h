// SECTION binder
// START file
#ifndef I_HIDL_BINDER_header_guard_H
#define I_HIDL_BINDER_header_guard_H

#include "binder/Parcel.h"
#include "binder/Parcelable.h"

namespace android {
    namespace service {
        namespace media {
            namespace rtstreams {

                class package_name : public Parcelable {

                public:
                  // START declarations
                  // START declare_enum_decl
                    enum class enum_name : enum_base_type {
                      enum_fields
                    };
                  // END declare_enum_decl
                  function_name(param_list) function_code // ALL declare_function

                  // END declarations

                    virtual status_t writeToParcel(Parcel* parcel) const override {
                    //START vars_writer
                        parcel->writeFileDescriptor(mFd, false); // ALL param_write_ref
                        parcel->writeInt32((int32_t) mRegionType); // ALL param_write_named_type_enum
                        parcel->writeInt64(mOffset); // ALL param_write_scalar_int64_t
                        parcel->writeUint64(mSize); // ALL param_write_scalar_uint64_t
                    // END vars_writer
                        return parcel->errorCheck();
                    }

                    virtual status_t readFromParcel(const Parcel* parcel) override {
                      // START vars_reader
                        param_name = parcel->readFileDescriptor(); // ALL param_read_ref
                        param_name = (enum_name) parcel->readInt32(); // ALL param_read_named_type_enum
                        param_name = parcel->readInt64(); // ALL param_read_scalar_int64_t
                        param_name = parcel->readUint64(); // ALL param_read_scalar_uint64_t
                        // END vars_reader
                        return parcel->errorCheck();
                    }

                private:
                  // START vars_decl
                  int mFd = -1;
                    MappingType mRegionType = MappingType::UNDEFINED_MAPPING;
                    off64_t mOffset;
                    size_t mSize = 0;
                  // END vars_decl
                };

            }
        }
    }
}
#endif // I_HIDL_BINDER_header_guard_H
// END file
