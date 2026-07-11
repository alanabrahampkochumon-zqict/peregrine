include_guard()

set(UtilsDir "utils/")
set(UtilHeaders
        Constants.h
        Preprocessors.h
        Utilities.h
)

list(TRANSFORM UtilHeaders PREPEND ${UtilsDir})