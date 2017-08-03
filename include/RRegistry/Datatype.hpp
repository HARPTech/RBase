#ifndef LRT_RREGISTRY_DATATYPE_HPP
#define LRT_RREGISTRY_DATATYPE_HPP

#include <stdint.h>
#include <string.h>

#define LRT_RREGISTRY_DATATYPE_NAME(ID, NAME)	\
    case static_cast<int>(ID):			\
	return NAME;

#define LRT_RREGISTRY_DATATYPE_VOID "Void"
#define LRT_RREGISTRY_DATATYPE_INT_8 "Int 8"
#define LRT_RREGISTRY_DATATYPE_INT_16 "Int 16"
#define LRT_RREGISTRY_DATATYPE_INT_32 "Int 32"
#define LRT_RREGISTRY_DATATYPE_INT_64 "Int 64"
#define LRT_RREGISTRY_DATATYPE_UINT_8 "UInt 8"
#define LRT_RREGISTRY_DATATYPE_UINT_16 "UInt 16"
#define LRT_RREGISTRY_DATATYPE_UINT_32 "UInt 32"
#define LRT_RREGISTRY_DATATYPE_UINT_64 "UInt 64"
#define LRT_RREGISTRY_DATATYPE_FLOAT "Float"
#define LRT_RREGISTRY_DATATYPE_DOUBLE "Double"
#define LRT_RREGISTRY_DATATYPE_STRING "String"
#define LRT_RREGISTRY_DATATYPE___COUNT "[INTERNAL] Counter"

namespace lrt
{
namespace rregistry
{
typedef uint16_t category_type_t;
typedef uint16_t property_type_t;
    
enum class Datatype
{
    VOID,
    
    INT_8,
    INT_16,
    INT_32,
    INT_64,
    UINT_8,
    UINT_16,
    UINT_32,
    UINT_64,

    FLOAT,
    DOUBLE,

    STRING,

    __COUNT
};

const char* GetDatatypeName(Datatype type)
{
    switch(type) {
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::VOID,
				    LRT_RREGISTRY_DATATYPE_VOID)
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::INT_8,
				    LRT_RREGISTRY_DATATYPE_INT_8)
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::INT_16,
				    LRT_RREGISTRY_DATATYPE_INT_16)
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::INT_32,
				    LRT_RREGISTRY_DATATYPE_INT_32)
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::INT_64,
				    LRT_RREGISTRY_DATATYPE_INT_64)
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::UINT_8,
				    LRT_RREGISTRY_DATATYPE_UINT_8)
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::UINT_16,
				    LRT_RREGISTRY_DATATYPE_UINT_16)
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::UINT_32,
				    LRT_RREGISTRY_DATATYPE_UINT_32)
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::UINT_64,
				    LRT_RREGISTRY_DATATYPE_UINT_64)

	LRT_RREGISTRY_DATATYPE_NAME(Datatype::FLOAT,
				    LRT_RREGISTRY_DATATYPE_FLOAT)
	LRT_RREGISTRY_DATATYPE_NAME(Datatype::DOUBLE,
				    LRT_RREGISTRY_DATATYPE_DOUBLE)

	LRT_RREGISTRY_DATATYPE_NAME(Datatype::STRING,
				    LRT_RREGISTRY_DATATYPE_STRING)

	LRT_RREGISTRY_DATATYPE_NAME(Datatype::__COUNT,
				    LRT_RREGISTRY_DATATYPE___COUNT)
    }
    return "[ERROR] Not found.";
}

}
}

#endif
