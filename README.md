RRegistry
=========

The registry contains all important data points, identifiers and other 
synchronization-relevant code. This repository is central in providing
a uniform development experience across platforms.

All properties should be available in the whole system. Data transfers
should be completely transparent to the programmer. The only exception 
is on the Arduino (RBreakout), which is streamlined to only receive what
it needs using a custom implementation of the variable store.

Including in other Repositories
-------------------------------

The registry has no dependencies and can be included directly without
any modification of the environment. It thereby is a good fit for a
git submodule.

Categories
----------

The categories in this codebase define the general data categories available
in the system. When new entries are added, new categories have to be added
too. 

Each defined category has a respective header file, which contains the exact
definitions of the properties within. Each property has an assigned type, which
defines how it is handled. Using this concept, types do not have to be transferred
over the wire, but instead one can directly send a value, its respective category and
the desired property.

Adding new Categories
---------------------

To add a new category, first add it to include/RRegistry/Categories.hpp. Then
add an equally named header file into the include/RRegistry folder which looks
like the other category header files. After that, push the changes and update
the reference to the registry in all other dependent repositories.
