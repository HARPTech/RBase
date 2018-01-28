%module RRegistry

%{
    #define SWIG_FILE_WITH_INIT
    #include <RRegistry/Entries.hpp>
    #include <RRegistry/TypeConverter.hpp>
    #include <RComm/LiteComm.hpp>
    #include <RComm/LiteCommAdapter.hpp>
    #include <RRegistry/Registry.hpp>
    using namespace lrt;
    using namespace lrt::rregistry;
%}

%include "shared.i"

%init %{
    import_array();
%}

%shared_ptr(rregistry::Registry)

%include "../RRegistry/include/RRegistry/Entries.hpp"
%include "../RRegistry/include/RRegistry/TypeConverter.hpp"
%include "../RRegistry/include/RRegistry/Registry.hpp"

 // Special template functions.
     %template(setInt8) lrt::rregistry::Registry::set<lrt::rregistry::Int8, signed char>;
     %template(setInt16) lrt::rregistry::Registry::set<lrt::rregistry::Int16, short>;
     %template(setInt32) lrt::rregistry::Registry::set<lrt::rregistry::Int32, int>;
     %template(setInt64) lrt::rregistry::Registry::set<lrt::rregistry::Int64, long>;
     %template(setUint8) lrt::rregistry::Registry::set<lrt::rregistry::Uint8, unsigned char>;
     %template(setUint16) lrt::rregistry::Registry::set<lrt::rregistry::Uint16, unsigned short>;
     %template(setUint32) lrt::rregistry::Registry::set<lrt::rregistry::Uint32, unsigned int>;
     %template(setUint64) lrt::rregistry::Registry::set<lrt::rregistry::Uint64, unsigned long>;
     %template(setFloat) lrt::rregistry::Registry::set<lrt::rregistry::Float, float>;
     %template(setDouble) lrt::rregistry::Registry::set<lrt::rregistry::Double, double>;
     %template(setBool) lrt::rregistry::Registry::set<lrt::rregistry::Bool, bool>;
     %template(setString) lrt::rregistry::Registry::set<lrt::rregistry::String, std::string>;

     %template(getInt8) lrt::rregistry::Registry::get<lrt::rregistry::Int8, signed char>;
     %template(getInt16) lrt::rregistry::Registry::get<lrt::rregistry::Int16, short>;
     %template(getInt32) lrt::rregistry::Registry::get<lrt::rregistry::Int32, int>;
     %template(getInt64) lrt::rregistry::Registry::get<lrt::rregistry::Int64, long>;
     %template(getUint8) lrt::rregistry::Registry::get<lrt::rregistry::Uint8, unsigned char>;
     %template(getUint16) lrt::rregistry::Registry::get<lrt::rregistry::Uint16, unsigned short>;
     %template(getUint32) lrt::rregistry::Registry::get<lrt::rregistry::Uint32, unsigned int>;
     %template(getUint64) lrt::rregistry::Registry::get<lrt::rregistry::Uint64, unsigned long>;
     %template(getFloat) lrt::rregistry::Registry::get<lrt::rregistry::Float, float>;
     %template(getDouble) lrt::rregistry::Registry::get<lrt::rregistry::Double, double>;
     %template(getBool) lrt::rregistry::Registry::get<lrt::rregistry::Bool, bool>;
     %template(getString) lrt::rregistry::Registry::get<lrt::rregistry::String, std::string>;
