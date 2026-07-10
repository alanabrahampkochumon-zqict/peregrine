include_guard()

set(TelemetryDir "telemetry/")
set(TelemetryHeaders
        ArenaTelemetry.h
        ArenaTelemetry.tpp
)

list(TRANSFORM TelemetryHeaders PREPEND ${TelemetryDir})