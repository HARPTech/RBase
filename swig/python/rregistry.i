%{
		#define SWIG_FILE_WITH_INIT
%}
%include "numpy.i"
%init %{
		import_array();
%}

%include "pyshared.i"
%include "../rregistry.i"
