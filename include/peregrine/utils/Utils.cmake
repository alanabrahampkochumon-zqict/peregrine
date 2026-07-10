include_guard()

set(UtilsDir "utils/")
set(UtilHeaders
        Constants.h
        Utilities.h
)

list(TRANSFORM UtilHeaders PREPEND ${UtilsDir})