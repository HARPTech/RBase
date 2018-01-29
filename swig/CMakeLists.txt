# Swig Bindings for multiple languages

find_package(SWIG)

if(${SWIG_FOUND})
		include(UseSWIG)
		message(STATUS "SWIG found, generating bindings for other languages.")

		# Python
		# ======
		add_subdirectory(python)

endif()
