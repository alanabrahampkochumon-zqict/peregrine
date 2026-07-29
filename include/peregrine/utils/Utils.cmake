include_guard()

set(UtilsDir "utils/")
set(UtilHeaders
        Constants.h
        Preprocessors.h
        Helpers.h
        Helpers.tpp
)

list(TRANSFORM UtilHeaders PREPEND ${UtilsDir})