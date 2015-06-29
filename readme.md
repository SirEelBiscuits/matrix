Matrix.h
========

A very special generic matrix class

Should work with any type specified for BaseType, so long as it has basic overloads. Types which have 'interesting' mutations multiplication and division _should_ work I am not currently supporting mutation under addition because wtff. DownCastType is for intermediate operations where types might be accumulating type information. this isn't permissable in C++ usually, but the matrix will maintain return type validity, and just use this value internally. If it compiles with the default, the default is fine!

Example for when DownCastType is needed:

BaseType = T<int m_pow> where m_pow is the exponent of the meters unit. Multiplying two together will result in summing their m_pow to maintain the unit validity. If these use float as a storage type, and can be cast to and from, then DownCastType should be float. If there is no such type, make one as a helper type to use here. The DownCastType must have its operators return the same type, always.

To Do
-----

Fill out this damn file.
