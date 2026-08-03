include_guard()

set(TelemetryDir "telemetry/")
set(TelemetryHeaders
        ArenaTelemetry.h
        ArenaTelemetry.tpp
        StackTelemetry.h
        StackTelemetry.tpp
        PoolTelemetry.h
        PoolTelemetry.tpp
)

list(TRANSFORM TelemetryHeaders PREPEND ${TelemetryDir})