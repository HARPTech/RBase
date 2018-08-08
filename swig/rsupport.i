%module RSupport

%{
		#define SWIG_FILE_WITH_INIT
    #include <RRegistry/Entries.hpp>
    #include <RRegistry/TypeConverter.hpp>
    #include <RComm/LiteComm.hpp>
    #include <RComm/LiteCommAdapter.hpp>
    #include <RRegistry/Registry.hpp>
    #include <RSupport/RSupport.hpp>
    #include <RSupport/SocketClientAdapter.hpp>
    using namespace lrt;
    using namespace lrt::rregistry;
    using namespace lrt::rsupport;
%}

%include "shared.i"

%shared_ptr(rsupport::SocketClientAdapter)

%include "../RSupport/include/RSupport/RSupport.hpp"
%include "../RSupport/include/RSupport/SocketClientAdapter.hpp"
