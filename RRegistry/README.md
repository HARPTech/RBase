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

Working with the Registry
-------------------------

The repository is defined through its main list of properties in `Entries.hpp`.
This file contains all available properties in enumerations, one enumeration for
each provided datatype. To add new entries, append them to the enumeration.

Please provide **extra care** when adding new properties! Moved property
identifiers could have a very negative effect on the system and should be
handled very carefully. As soon as properties are fixed, assign a number
to them. Critical Properties should always be fixed to static numeric IDs. Without
a fixed numbering, a change of the property ordering can lead to transmission errors.
