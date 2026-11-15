# Tests del motor KomaEngine.
# Todavia no hay pruebas implementadas ni frameworks de testing configurados.
# Cuando se anadan pruebas, se activaran con:
#   cmake -S . -B build -DKOMA_BUILD_TESTS=ON
option(KOMA_BUILD_TESTS "Compilar las pruebas del motor" OFF)

if(KOMA_BUILD_TESTS)
    message(STATUS "KOMA_BUILD_TESTS=ON: aun no hay pruebas que compilar")
endif()
