%module RSupport

%{
		#define SWIG_FILE_WITH_INIT
    #include <RRegistry/Entries.hpp>
    #include <RRegistry/TypeConverter.hpp>
    #include <RComm/LiteComm.hpp>
    #include <RComm/LiteCommAdapter.hpp>
    #include <RRegistry/Registry.hpp>
    #include <RSupport/RSupport.h>
    #include <RSupport/PipeAdapter.hpp>
    using namespace lrt;
    using namespace lrt::rregistry;
    using namespace lrt::rsupport;
%}

%include "shared.i"

%shared_ptr(rsupport::PipeAdapter)

%include "../RSupport/include/RSupport/RSupport.h"
%include "../RSupport/include/RSupport/PipeAdapter.hpp"
